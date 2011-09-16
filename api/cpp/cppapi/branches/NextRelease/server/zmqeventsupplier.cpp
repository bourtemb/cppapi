static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
//
//  file 	zmqeventsupplier.cpp
//
//		C++ classes for implementing the event server and client
//		singleton classes - ZmqEventSupplier.
//		This class is used to send events from the server
//		to the client(s) when zmq is used to transport the events
//
//  	author(s) : E.Taurel (taurel@esrf.fr)
//
//		original : August 2011
//
// Copyright (C) :      2011
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//		$Revision: 17240 $
//
//
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>
#include <eventsupplier.h>

#include <omniORB4/internal/giopStream.h>

#include <iterator>

using namespace CORBA;

namespace Tango {

ZmqEventSupplier *ZmqEventSupplier::_instance = NULL;


/************************************************************************/
/*		       															*/
/* 			ZmqEventSupplier class 					    				*/
/*			----------------											*/
/*		       															*/
/************************************************************************/


ZmqEventSupplier::ZmqEventSupplier(Database *db,string &host_name,string &specified_ip):EventSupplier(db,host_name),zmq_context(1),event_pub_sock(NULL)
{
cout << "Entering ZmqEventSupplier ctor" << endl;
	_instance = this;

//
// Create the Publisher socket for heartbeat event and bind it
// If the user has specified one IP address on the command line,
// re-use it in the endpoint
//

    heartbeat_pub_sock = new zmq::socket_t(zmq_context,ZMQ_PUB);

    heartbeat_endpoint = "tcp://";

    if (specified_ip.empty() == false)
    {
        heartbeat_endpoint = heartbeat_endpoint + specified_ip + ':';
        ip_specified = true;
        user_ip = specified_ip;
    }
    else
    {
        heartbeat_endpoint = heartbeat_endpoint + "*:";
        ip_specified = false;
    }

//
// Bind the publisher socket to one ephemeral port
//

    tango_bind(heartbeat_pub_sock,heartbeat_endpoint);
cout << "Heartbeat publisher socket binded to " << heartbeat_endpoint << endl;

//
// If needed, replace * by host IP address in enpoint string
//

    if (specified_ip.empty() == true)
    {
        ApiUtil *au = ApiUtil::instance();
        vector<string> adrs;

        au->get_ip_from_if(adrs);
//copy(adrs.begin(),adrs.end(),ostream_iterator<string>(cout,"\n"));

        string::size_type pos = heartbeat_endpoint.find('*');
        if (adrs.size() > 1)
        {
            for (unsigned int i = 0;i < adrs.size();++i)
            {
                string::size_type start;
                if ((start = adrs[i].find("127.")) == 0)
                    continue;
                heartbeat_endpoint.replace(pos,1,adrs[i]);
                host_ip = adrs[i];
                break;
            }
        }
        else
        {
           heartbeat_endpoint.replace(pos,1,adrs[0]);
           host_ip = adrs[0];
        }
    }

//
// Find out the host endianness
//

    host_endian = test_endian();

//
// Init heartbeat and event call info (both ok and nok)
// Leave the OID un-initialized
// Marshall the structure into CORBA CDR
//

    heartbeat_call.version = ZMQ_EVENT_PROT_VERSION;
    heartbeat_call.method_name = CORBA::string_dup(HEARTBEAT_METHOD_NAME);
    heartbeat_call.call_is_except = false;

    heartbeat_call >>= heartbeat_call_cdr;

    event_call_ok.version = ZMQ_EVENT_PROT_VERSION;
    event_call_ok.method_name = CORBA::string_dup(EVENT_METHOD_NAME);
    event_call_ok.call_is_except = false;

    event_call_ok >>= event_call_ok_cdr;

    event_call_nok.version = ZMQ_EVENT_PROT_VERSION;
    event_call_nok.method_name = CORBA::string_dup(EVENT_METHOD_NAME);
    event_call_nok.call_is_except = true;

    event_call_nok >>= event_call_nok_cdr;

//
// Start to init the event name used for the DS heartbeat event
//

    heartbeat_event_name = fqdn_prefix;
    heartbeat_event_name = heartbeat_event_name + "dserver/";
    heartbeat_name_init = false;
}


ZmqEventSupplier *ZmqEventSupplier::create(Database *db,string &host_name,string &specified_ip)
{
	cout4 << "calling Tango::ZmqEventSupplier::create() \n";

//
// Does the ZmqEventSupplier singleton exist already ? if so simply return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// ZmqEventSupplier singleton does not exist, create it
//

	ZmqEventSupplier *_event_supplier = new ZmqEventSupplier(db,host_name,specified_ip);

	return _event_supplier;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::tango_bind()
//
// description : 	Choose a free port to bind ZMQ socket
//
// argument : in : - sock : The ZMQ socket
//                 - endpoint : The beginning of the ZMQ endpoint
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::tango_bind(zmq::socket_t *sock,string &endpoint)
{
    stringstream ss;
    string base_endpoint(endpoint);
    string tmp_endpoint;

    int port;
    for (port = EPHEMERAL_PORT_BEGIN; port < EPHEMERAL_PORT_END; port++)
    {
        ss << port;
        tmp_endpoint = base_endpoint + ss.str();

cout << "Trying to bind to endpoint " << tmp_endpoint << endl;

        if (zmq_bind(*sock, tmp_endpoint.c_str()) == 0)
        {
            break;
        }
        ss.str("");
    }

    if (port == EPHEMERAL_PORT_END)
    {
        EventSystemExcept::throw_exception((const char*)"API_ZmqInitFailed",
                        (const char*)"Can't bind the ZMQ socket. All port used!",
                        (const char*)"ZmqEventSupplier::tango_bind()");
    }
    endpoint = tmp_endpoint;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::test_endian()
//
// description : 	Get the host endianness
//
// This method returns the host endianness
//      0 -> Big endian
//      1 -> Little endian
//
//-----------------------------------------------------------------------------

unsigned char ZmqEventSupplier::test_endian()
{
    int test_var = 1;
	unsigned char *cptr = (unsigned char*)&test_var;
    return (!(cptr[0] == 0));
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::create_event_socket()
//
// description : 	Create and bind the publisher socket used to publish the
//                  real events
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::create_event_socket()
{

    if (event_pub_sock == NULL)
    {

//
// Create the Publisher socket for real events and bind it
// If the user has specified one IP address on the command line,
// re-use it in the endpoint
//

        event_pub_sock = new zmq::socket_t(zmq_context,ZMQ_PUB);

        event_endpoint = "tcp://";

        if (ip_specified == true)
        {
            event_endpoint = event_endpoint + user_ip + ':';
        }
        else
        {
            event_endpoint = event_endpoint + "*:";
        }

//
// Bind the publisher socket to one ephemeral port
//

        tango_bind(event_pub_sock,event_endpoint);
cout << "Event publisher socket binded to " << event_endpoint << endl;

//
// If needed, replace * by host IP address in enpoint string
//

        if (ip_specified == false)
        {
            string::size_type pos = event_endpoint.find('*');
            event_endpoint.replace(pos,1,host_ip);
        }
    }

}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::push_heartbeat_event()
//
// description : 	Method to push the hearbeat event
//
//-----------------------------------------------------------------------------

void tg_free(void *data,void *hint)
{
}

void ZmqEventSupplier::push_heartbeat_event()
{
cout << "Entering ZmqEventSupplier::push_heartbeat_event" << endl;

	time_t delta_time;
	time_t now_time;

//
// Heartbeat - check wether a heartbeat event has been sent recently
// if not then send it. A heartbeat contains no data, it is used by the
// consumer to know that the supplier is still alive.
//

	Tango::Util *tg = Tango::Util::instance();
	DServer *adm_dev = tg->get_dserver_device();
	now_time = time(NULL);
	delta_time = now_time - adm_dev->last_heartbeat;
	cout3 << "ZmqEventSupplier::push_heartbeat_event(): delta time since last heartbeat " << delta_time << endl;

	if (heartbeat_name_init == false)
	{
        heartbeat_event_name = heartbeat_event_name + adm_dev->get_full_name() + ".heartbeat";
	    heartbeat_name_init = true;
	}

//
// We here compare delta_time to 9 and not to 10.
// This is necessary because, sometimes the polling thread is some
// milli second in advance. The computation here is done in seconds
// So, if the polling thread is in advance, delta_time computed in
// seconds will be 9 even if in reality it is 9,9
//

	if (delta_time >= 9)
	{
		cout3 << "ZmqEventSupplier::push_heartbeat_event(): detected heartbeat event for " << heartbeat_event_name << endl;
		cout3 << "ZmqEventSupplier::push_heartbeat_event(): delta _time " << delta_time << endl;

//
// Create zmq messages using zero copy messages
//

        zmq::message_t name_mess((void *)heartbeat_event_name.data(),heartbeat_event_name.size(),tg_free);
        zmq::message_t endian_mess(&host_endian,1,tg_free);
        zmq::message_t call_mess(heartbeat_call_cdr.bufPtr(),heartbeat_call_cdr.bufSize(),tg_free);

		bool fail = false;
		try
		{
//
// For debug and logging purposes
//

            if (omniORB::trace(20))
            {
                omniORB::logger log;
                log << "ZMQ: Pushing some data" << '\n';
            }
            if (omniORB::trace(30))
            {
                {
                    omniORB::logger log;
                    log << "ZMQ: Event name" << '\n';
                }
                omni::giopStream::dumpbuf((unsigned char *)name_mess.data(),name_mess.size());

                {
                    omniORB::logger log;
                    log << "ZMQ: Endianess" << '\n';
                }
                omni::giopStream::dumpbuf((unsigned char *)endian_mess.data(),endian_mess.size());

                {
                    omniORB::logger log;
                    log << "ZMQ: Call info" << '\n';
                }
                omni::giopStream::dumpbuf((unsigned char *)call_mess.data(),call_mess.size());
            }

//
// Push the event
//

cout << "Pushing heartbeat for " << heartbeat_event_name << endl;
            heartbeat_pub_sock->send(name_mess,ZMQ_SNDMORE);
			heartbeat_pub_sock->send(endian_mess,ZMQ_SNDMORE);
			heartbeat_pub_sock->send(call_mess,0);
		}
		catch(...)
		{
			cout3 << "ZmqEventSupplier::push_heartbeat_event() failed !\n";
			fail = true;
		}

//
// If it was not possible to communicate with notifd,
// try a reconnection
//

//		if (fail == true)
//		{
//			try
//			{
//				reconnect_notifd();
//			}
//			catch (...) {}
//		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::push_event()
//
// description : 	Method to send the event to the event channel
//
// argument : in :	device_impl : The device
//			        event_type : The event type (change, periodic....)
//			        filterable_names :
//			        filterable_data :
//			        attr_value : The attribute value
//			        except : The exception thrown during the last
//				             attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void tg_unlock(void *data,void *hint)
{
    EventSupplier *ev = (EventSupplier *)hint;
    omni_mutex &the_mutex = ev->get_push_mutex();
    the_mutex.unlock();
cout << "Unlock.................." << endl;
}

void ZmqEventSupplier::push_event(DeviceImpl *device_impl,string event_type,
            vector<string> &filterable_names,vector<double> &filterable_data,vector<string> &filterable_names_lg,vector<long> &filterable_data_lg,
            struct AttributeData &attr_value,string &attr_name,DevFailed *except)
{
	cout3 << "ZmqEventSupplier::push_event(): called for attribute " << attr_name << endl;

//
// Get the mutex to synchronize the sending of events
//

//	omni_mutex_lock l(push_mutex);
    cout << "Lock............" << endl;
    push_mutex.lock();

//
// Create full event name
//

	string loc_attr_name(attr_name);
	transform(loc_attr_name.begin(),loc_attr_name.end(),loc_attr_name.begin(),::tolower);
	event_name = fqdn_prefix + device_impl->get_name_lower() + "/" + loc_attr_name + "." + event_type;
cout << "event_name = " << event_name << endl;

//
// Create zmq messages
//

    zmq::message_t name_mess((void *)event_name.data(),event_name.size(),tg_free);
    zmq::message_t endian_mess(&host_endian,1,tg_free);

    size_t mess_size;
    void *mess_ptr;
    if (except == NULL)
    {
        mess_size = event_call_ok_cdr.bufSize();
        mess_ptr = event_call_ok_cdr.bufPtr();
    }
    else
    {
        mess_size = event_call_nok_cdr.bufSize();
        mess_ptr = event_call_nok_cdr.bufPtr();
    }

    zmq::message_t call_mess(mess_ptr,mess_size,tg_unlock,(void *)this);

//
// Marshall the event data
//

//	cdrMemoryStream data_call_cdr;
	CORBA::Long padding = 0XDEC0DEC0;
	data_call_cdr.rewindPtrs();
	padding >>= data_call_cdr;
	padding >>= data_call_cdr;

    if (except == NULL)
    {
        if (attr_value.attr_val != NULL)
        {
            *(attr_value.attr_val) >>= data_call_cdr;
        }
        else if (attr_value.attr_val_3 != NULL)
        {
            *(attr_value.attr_val_3) >>= data_call_cdr;
        }
        else if (attr_value.attr_val_4 != NULL)
        {
            *(attr_value.attr_val_4) >>= data_call_cdr;
        }
        else if (attr_value.attr_conf_2 != NULL)
        {
            *(attr_value.attr_conf_2) >>= data_call_cdr;
        }
        else if (attr_value.attr_conf_3 != NULL)
        {
            *(attr_value.attr_conf_3) >>= data_call_cdr;
        }
        else
        {
            *(attr_value.attr_dat_ready) >>= data_call_cdr;
        }
    }
    else
    {
        except->errors >>= data_call_cdr;
    }

    mess_size = data_call_cdr.bufSize() - sizeof(CORBA::Long);
    mess_ptr = data_call_cdr.bufPtr() + sizeof(CORBA::Long);
    zmq::message_t data_mess(mess_ptr,mess_size,tg_free);

//
// Send the data
//

    bool fail = false;
    try
    {

//
// For debug and logging purposes
//

        if (omniORB::trace(20))
        {
            omniORB::logger log;
            log << "ZMQ: Pushing some data" << '\n';
        }
        if (omniORB::trace(30))
        {
            {
                omniORB::logger log;
                log << "ZMQ: Event name" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)name_mess.data(),name_mess.size());

            {
                omniORB::logger log;
                log << "ZMQ: Endianess" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)endian_mess.data(),endian_mess.size());

            {
                omniORB::logger log;
                log << "ZMQ: Call info" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)call_mess.data(),call_mess.size());

            {
                omniORB::logger log;
                log << "ZMQ: Event data" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)data_mess.data(),data_mess.size());
        }

//
// Push the event
//

cout << "Pushing event for " << event_name << endl;
        event_pub_sock->send(name_mess,ZMQ_SNDMORE);
        event_pub_sock->send(endian_mess,ZMQ_SNDMORE);
        event_pub_sock->send(call_mess,ZMQ_SNDMORE);
        event_pub_sock->send(data_mess,0);
    }
    catch(...)
    {
        cout3 << "ZmqEventSupplier::push_event() failed !\n";
        push_mutex.unlock();
cout <<"Unlock in catch block.........." << endl;
        fail = true;
    }
}


} /* End of Tango namespace */
