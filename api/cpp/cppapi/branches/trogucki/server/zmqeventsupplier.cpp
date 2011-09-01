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

using namespace CORBA;

namespace Tango {

ZmqEventSupplier *ZmqEventSupplier::_instance = NULL;


/************************************************************************/
/*		       															*/
/* 			ZmqEventSupplier class 					    				*/
/*			----------------											*/
/*		       															*/
/************************************************************************/


//ZmqEventSupplier::ZmqEventSupplier(Database *db,string &host_name,string &specified_ip):EventSupplier(db,host_name),zmq_context(1)
ZmqEventSupplier::ZmqEventSupplier(Database *db,string &host_name,string &specified_ip):EventSupplier(db,host_name)
{
cout << "Entering ZmqEventSupplier ctor" << endl;
	_instance = this;

//
// Create the Publisher socket for heartbeat event and bind it
// If the user has specified one IP address on the command line,
// re-use it in the endpoint
//

//    heartbeat_pub_sock = new zmq::socket_t(zmq_context,ZMQ_PUB);

    heartbeat_endpoint = "tcp://";

    if (specified_ip.empty() == false)
    {
        heartbeat_endpoint = heartbeat_endpoint + specified_ip + ':';
    }
    else
    {
        heartbeat_endpoint = heartbeat_endpoint + "*:";
    }
//    tango_bind(heartbeat_pub_sock,heartbeat_endpoint);
cout << "Heartbeat publisher socket binded to " << heartbeat_endpoint << endl;
}


ZmqEventSupplier *ZmqEventSupplier::create(Database *db,string &host_name,string &specified_ip)
{
	cout4 << "calling Tango::ZmqEventSupplier::create() \n";

//
// does the ZmqEventSupplier singleton exist already ? if so simply return it
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

//void ZmqEventSupplier::tango_bind(zmq::socket_t *sock,string &endpoint)
//{
//    stringstream ss;
//    string base_endpoint(endpoint);
//    string tmp_endpoint;
//
//    for (int port = 42000; port < 43000; port++)
//    {
//        ss << port;
//        tmp_endpoint = base_endpoint + ss.str();
//
//cout << "Trying to bind to endpoint " << tmp_endpoint << endl;
//
//        if (zmq_bind(*sock, tmp_endpoint.c_str()) == 0)
//        {
//            break;
//        }
//        ss.str("");
//    }
//    endpoint = tmp_endpoint;
//}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::push_heartbeat_event()
//
// description : 	Method to send the hearbeat event
//
// argument : in :
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::push_heartbeat_event()
{
cout << "Entering ZmqEventSupplier::push_heartbeat_event" << endl;
	string event, domain_name;
	time_t delta_time;
	time_t now_time;
	static int heartbeat_counter=0;

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

//
// We here compare delta_time to 9 and not to 10.
// This is necessary because, sometimes the polling thread is some
// milli second in advance. The computation here is done in seconds
// So, if the polling thread is in advance, delta_time computed in
// seconds will be 9 even if in reality it is 9,9
//

	if (delta_time >= 9)
	{
		domain_name = "dserver/" + adm_dev->get_full_name();

//		struct_event.header.fixed_header.event_type.domain_name = CORBA::string_dup(domain_name.c_str());
// 		struct_event.header.fixed_header.event_type.type_name   = CORBA::string_dup(fqdn_prefix.c_str());
//  	struct_event.header.variable_header.length( 0 );

		cout3 << "ZmqEventSupplier::push_heartbeat_event(): detected heartbeat event for " << domain_name << endl;
		cout3 << "ZmqEventSupplier::push_heartbeat_event(): delta _time " << delta_time << endl;
//  		struct_event.header.fixed_header.event_name  = CORBA::string_dup("heartbeat");
//  		struct_event.filterable_data.length(1);
//  		struct_event.filterable_data[0].name = CORBA::string_dup("heartbeat_counter");
//  		struct_event.filterable_data[0].value <<= (CORBA::Long) heartbeat_counter++;
//		adm_dev->last_heartbeat = now_time;
//
//		struct_event.remainder_of_body <<= (CORBA::Long)adm_dev->last_heartbeat;

//
// Push the event
//

//		bool fail = false;
//		try
//		{
//			structuredProxyPushConsumer -> push_structured_event(struct_event);
//		}
//		catch(const CosEventComm::Disconnected&)
//		{
//			cout3 << "NotifdEventSupplier::push_heartbeat_event() event channel disconnected !\n";
//			fail = true;
//		}
//       	catch(const CORBA::TRANSIENT &)
//       	{
//			cout3 << "NotifdEventSupplier::push_heartbeat_event() caught a CORBA::TRANSIENT ! " << endl;
//			fail = true;
//		}
//		catch(const CORBA::COMM_FAILURE &)
//		{
//			cout3 << "NotifdEventSupplier::push_heartbeat_event() caught a CORBA::COMM_FAILURE ! " << endl;
//			fail = true;
//		}
//		catch(const CORBA::SystemException &)
//		{
//			cout3 << "NotifdEventSupplier::push_heartbeat_event() caught a CORBA::SystemException ! " << endl;
//			fail = true;
//		}

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

void ZmqEventSupplier::push_event(DeviceImpl *device_impl,string event_type,
            vector<string> &filterable_names,vector<double> &filterable_data,vector<string> &filterable_names_lg,vector<long> &filterable_data_lg,
            struct AttributeData &attr_value,string &attr_name,DevFailed *except)
{
	cout3 << "ZmqEventSupplier::push_event(): called for attribute " << attr_name << endl;

	// get the mutex to synchronize the sending of events
	omni_mutex_lock l(push_mutex);
}


} /* End of Tango namespace */
