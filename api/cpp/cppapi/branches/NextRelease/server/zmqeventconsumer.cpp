static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
///  file 	zmqeventconsumer.cpp
///
///		C++ classes for implementing the event consumer
///		singleton class when used with zmq
///
///		author(s) : E.Taurel
///
///		original : 16 August 2011
///
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
///
///		$Revision$
///
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>

#include <stdio.h>
#include <assert.h>

#include <omniORB4/internal/giopStream.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace CORBA;

namespace Tango {


ZmqEventConsumer *ZmqEventConsumer::_instance = NULL;
//omni_mutex EventConsumer::ev_consumer_inst_mutex;


/************************************************************************/
/*		       															*/
/* 			ZmqEventConsumer class 					    				*/
/*			----------------											*/
/*		       															*/
/************************************************************************/

ZmqEventConsumer::ZmqEventConsumer(ApiUtil *ptr) : EventConsumer(ptr),omni_thread((void *)ptr),zmq_context(1)
{
	cout3 << "calling Tango::ZmqEventConsumer::ZmqEventConsumer() \n";

	_instance = this;

//
// Initialize the var references
//

    av = new AttributeValue();
    av3 = new AttributeValue_3();
    av4 = new AttributeValue_4();
    ac2 = new AttributeConfig_2();
    ac3 = new AttributeConfig_3();
    adr = new AttDataReady();
    del = new DevErrorList();

	start_undetached();
}

ZmqEventConsumer *ZmqEventConsumer::create()
{
	omni_mutex_lock guard(ev_consumer_inst_mutex);

//
// check if the ZmqEventConsumer singleton exists, if so return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// ZmqEventConsumer singleton does not exist, create it
//

	ApiUtil *ptr =  ApiUtil::instance();
	return new ZmqEventConsumer(ptr);
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::run_undetached()
//
// description :    Main method for the ZMQ event system reciving thread
//
//-----------------------------------------------------------------------------

void *ZmqEventConsumer::run_undetached(void *arg)
{

    int linger = 0;

//
// Create the subscriber socket used to receive heartbeats coming from different DS
// This socket subscribe to everything because dedicated publishers are used to
// send the heartbeat events. This socket will be connected to all needed publishers
//

    heartbeat_sub_sock = new zmq::socket_t(zmq_context,ZMQ_SUB);
    heartbeat_sub_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

//
// Create the subscriber socket used to receive events coming from different DS
// This socket subscribe to everything because dedicated publishers are used to
// send the heartbeat events. This socket will be connected to all needed publishers
//

    event_sub_sock = new zmq::socket_t(zmq_context,ZMQ_SUB);
    event_sub_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

//
// Create the control socket (REQ/REP pattern) and binds it
//

    control_sock = new zmq::socket_t(zmq_context,ZMQ_REP);
    control_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));
    control_sock->bind(CTRL_SOCK_ENDPOINT);

//
// Initialize poll set
//

    zmq::pollitem_t items [] = {
        { *heartbeat_sub_sock, 0, ZMQ_POLLIN, 0 },
        { *control_sock, 0, ZMQ_POLLIN, 0 },
        { *event_sub_sock, 0, ZMQ_POLLIN, 0},
    };

//
// Enter the infinite loop
//

    while(1)
    {
        zmq::message_t received_event_name,received_endian;
        zmq::message_t received_call,received_event_data;
        zmq::message_t received_ctrl;

//
// Wait for message
//

        zmq::poll(&items[0],3,-1);
//cout << "Awaken !!!!!!!!" << endl;

//
// Something received by the heartbeat socket ?
//

        if (items [0].revents & ZMQ_POLLIN)
        {
//cout << "For the heartbeat socket" << endl;
            heartbeat_sub_sock->recv(&received_event_name);
            heartbeat_sub_sock->recv(&received_endian);
            heartbeat_sub_sock->recv(&received_call);

            process_heartbeat(received_event_name,received_endian,received_call);
        }

//
// Something received by the control socket?
//

        if (items [1].revents & ZMQ_POLLIN)
        {
//cout << "For the control socket" << endl;
            control_sock->recv(&received_ctrl);

            string ret_str;
            bool ret;

            try
            {
                ret = process_ctrl(received_ctrl);
                ret_str = "OK";
            }
            catch (zmq::error_t &e)
            {
                ret_str = e.what();
            }

            zmq::message_t reply(ret_str.size());
            ::memcpy((void *)reply.data(),ret_str.data(),ret_str.size());
            control_sock->send(reply);

            if (ret == true)
            {
                delete heartbeat_sub_sock;
                delete control_sock;

                break;
            }
        }

//
// Something received by the event socket ?
//

        if (items [2].revents & ZMQ_POLLIN)
        {
//cout << "For the event socket" << endl;
            event_sub_sock->recv(&received_event_name);
            event_sub_sock->recv(&received_endian);
            event_sub_sock->recv(&received_call);
            event_sub_sock->recv(&received_event_data);

            process_event(received_event_name,received_endian,received_call,received_event_data);
        }

    }

	return (void *)NULL;
}


//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::process_heartbeat()
//
// description : 	Process execution when a message has been received by the
//                  heartbeat socket
//
// args: in : - received_event_name : The full event name
//            - received_endian : The sender endianess
//            - received_call : The call informations (oid - method name...)
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::process_heartbeat(zmq::message_t &received_event_name,zmq::message_t &received_endian,zmq::message_t &received_call)
{
//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A heartbeat message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Event name" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_event_name.data(),received_event_name.size());

        {
            omniORB::logger log;
            log << "ZMQ: Endianess" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_endian.data(),received_endian.size());

        {
            omniORB::logger log;
            log << "ZMQ: Call info" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_call.data(),received_call.size());
    }

//
// Extract data from messages
//

    const ZmqCallInfo *receiv_call;

    unsigned char endian = ((char *)received_endian.data())[0];
    string event_name((char *)received_event_name.data(),(size_t)received_event_name.size());

    cdrMemoryStream call_info((char *)received_call.data(),(size_t)received_call.size());
    call_info.setByteSwapFlag(endian);

    ZmqCallInfo_var c_info_var = new ZmqCallInfo;
    (ZmqCallInfo &)c_info_var <<= call_info;
    receiv_call = &c_info_var.in();

//
// Call the required method
//

    if (::strcmp(receiv_call->method_name,HEARTBEAT_METHOD_NAME) == 0)
    {
        push_heartbeat_event(event_name);
    }
    else
    {

    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::process_event()
//
// description : 	Process execution when a message has been received by the
//                  event socket
//
// args: in : - received_event_name : The full event name
//            - received_endian : The sender endianess
//            - received_call : The call informations (oid - method name...)
//            - event_data : The event data !
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::process_event(zmq::message_t &received_event_name,zmq::message_t &received_endian,zmq::message_t &received_call,zmq::message_t &event_data)
{
//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A event message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Event name" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_event_name.data(),received_event_name.size());

        {
            omniORB::logger log;
            log << "ZMQ: Endianess" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_endian.data(),received_endian.size());

        {
            omniORB::logger log;
            log << "ZMQ: Call info" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_call.data(),received_call.size());

        {
            omniORB::logger log;
            log << "ZMQ: Event data" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)event_data.data(),event_data.size());
    }

//
// Extract data from messages
//

    const ZmqCallInfo *receiv_call;

    unsigned char endian = ((char *)received_endian.data())[0];
    string event_name((char *)received_event_name.data(),(size_t)received_event_name.size());

    cdrMemoryStream call_info((char *)received_call.data(),(size_t)received_call.size());
    call_info.setByteSwapFlag(endian);

    ZmqCallInfo_var c_info_var = new ZmqCallInfo;
    (ZmqCallInfo &)c_info_var <<= call_info;
    receiv_call = &c_info_var.in();

//
// Call the required method
//

    if (::strcmp(receiv_call->method_name,EVENT_METHOD_NAME) == 0)
    {
        push_zmq_event(event_name,endian,event_data,receiv_call->call_is_except);
    }
    else
    {

    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::process_ctrl()
//
// description : 	Process task when something has been received by the control
//                  socket
//
// args: - in: - received_ctrl : The received data
//
// This method returns true if the calling thread has to exit (because a
// ZMQ_END command has been received). Otherwise, it returns false
//
//-----------------------------------------------------------------------------

bool ZmqEventConsumer::process_ctrl(zmq::message_t &received_ctrl)
{
    bool ret = false;

//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A control message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Control data " << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_ctrl.data(),received_ctrl.size());
    }

//
// Extract cmd code from messages
//

    const char *tmp_ptr = (const char *)received_ctrl.data();
    char cmd_code = tmp_ptr[0];

//
// Process each command
//

    int ind = 1;

    switch (cmd_code)
    {
        case ZMQ_END:
        {
            ret = true;
        }
        break;

        case ZMQ_CONNECT_HEARTBEAT:
        {
//
// First extract the endpoint and the event name from received buffer
//

            const char *endpoint = &(tmp_ptr[1]);
            int start = ::strlen(endpoint) + 2;
            const char *event_name = &(tmp_ptr[start]);

//
// Connect the heartbeat socket to the new publisher
//

            bool connect_heart = false;

            if (connected_heartbeat.empty() == false)
            {
                vector<string>::iterator pos;
                pos = find(connected_heartbeat.begin(),connected_heartbeat.end(),endpoint);
                if (pos == connected_heartbeat.end())
                    connect_heart = true;
            }
            else
                connect_heart = true;

            if (connect_heart == true)
            {
                heartbeat_sub_sock->connect(endpoint);
                connected_heartbeat.push_back(endpoint);
            }


//
// Subscribe to the new heartbeat event
//

            heartbeat_sub_sock->setsockopt(ZMQ_SUBSCRIBE,event_name,::strlen(event_name));
        }
        break;

        case ZMQ_DISCONNECT_HEARTBEAT:
        {
//
// Get event name
//

            const char *event_name = &(tmp_ptr[1]);

//
// Unsubscribe this event from the heartbeat socket
//

            heartbeat_sub_sock->setsockopt(ZMQ_UNSUBSCRIBE,event_name,::strlen(event_name));
        }
        break;

        case ZMQ_CONNECT_EVENT:
        {
//
// First extract the endpoint and the event name from received buffer
//

            const char *endpoint = &(tmp_ptr[1]);
            int start = ::strlen(endpoint) + 2;
            const char *event_name = &(tmp_ptr[start]);
cout << "Connect subscriber to endpoint " << endpoint << " for event " << event_name << endl;

//
// Connect the socket to the publisher
//

            bool connect_pub = false;

            if (connected_pub.empty() == false)
            {
                vector<string>::iterator pos;
                pos = find(connected_pub.begin(),connected_pub.end(),endpoint);
                if (pos == connected_pub.end())
                    connect_pub = true;
            }
            else
                connect_pub = true;

            if (connect_pub == true)
            {
cout << "Connect socket with endpoint: " << endpoint << endl;
                event_sub_sock->connect(endpoint);
                connected_pub.push_back(endpoint);
            }


//
// Subscribe to the new event
//

cout << "Zmq subscribe with string: " << event_name << endl;
            event_sub_sock->setsockopt(ZMQ_SUBSCRIBE,event_name,::strlen(event_name));
        }
        break;

        case ZMQ_DISCONNECT_EVENT:
        {

//
// Get event name
//

            const char *event_name = &(tmp_ptr[1]);
            string ev_name(event_name);

//
// Check if it is a multicast event
//

            bool mcast = false;

            map<string,zmq::socket_t *>::iterator pos;
            if (event_mcast.empty() != true)
            {
                pos = event_mcast.find(ev_name);
                if (pos != event_mcast.end())
                    mcast = true;
            }

//
// Unsubscribe this event from the socket
//

            if (mcast == false)
                event_sub_sock->setsockopt(ZMQ_UNSUBSCRIBE,event_name,::strlen(event_name));
            else
            {
                delete pos->second;
                event_mcast.erase(pos);
            }
        }
        break;

        case ZMQ_CONNECT_MCAST_EVENT:
        {
//
// First extract the endpoint and the event name from received buffer
//

            const char *endpoint = &(tmp_ptr[1]);
            int start = ::strlen(endpoint) + 2;
            const char *event_name = &(tmp_ptr[start]);
            start = start + ::strlen(event_name) + 1;
            Tango::DevLong rate,ivl;
            ::memcpy(&rate,&(tmp_ptr[start]),sizeof(Tango::DevLong));
            start = start + sizeof(Tango::DevLong);
            ::memcpy(&ivl,&(tmp_ptr[start]),sizeof(Tango::DevLong));
cout << "Connect subscriber to endpoint " << endpoint << " for event " << event_name << " with rate = " << rate << " and ivl = " << ivl << endl;

//
// Connect the socket to the publisher
//

            bool created_sub = false;
            string ev_name(event_name);
            map<string,zmq::socket_t *>::iterator pos;

            if (event_mcast.empty() == false)
            {
                pos = event_mcast.find(ev_name);
                if (pos != event_mcast.end())
                    created_sub = true;
            }

            if (created_sub == false)
            {
//
// Create the socket
//

cout << "Create multicast socket" << endl;
                zmq::socket_t *tmp_sock = new zmq::socket_t(zmq_context,ZMQ_SUB);

//
// Set socket rate, ivl and linger
//

                int local_rate = PGM_RATE;

                if (rate != 0)
                    local_rate = rate * 1024;
                tmp_sock->setsockopt(ZMQ_RATE,&local_rate,sizeof(local_rate));

                int local_ivl = PGM_IVL;

                if (ivl != 0)
                    local_ivl = ivl * 1000;
                tmp_sock->setsockopt(ZMQ_RATE,&local_ivl,sizeof(local_ivl));

                int linger = 0;
                tmp_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

//
// Connect the socket
//

cout << "Connect multicast socket to endpoint " << endpoint << endl;
                tmp_sock->connect(endpoint);

//
// Subscribe to the new event
//

cout << "Zmq subscribe with string: " << event_name << endl;
                tmp_sock->setsockopt(ZMQ_SUBSCRIBE,event_name,::strlen(event_name));

//
// Store socket in map
//

                if (event_mcast.insert(make_pair(ev_name,tmp_sock)).second == false)
                {
                    cout << "Error while inserting pair in map !!!!!!!!!!!!!!!" << endl;
                }
            }
        }
        break;

        default:
            cerr << "ZMQ main thread: Received an unknown command code from control socket!" << endl;
        break;
    }

    return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::cleanup_EventChannel_map()
//
// description : 	Method to destroy the DeviceProxy objects
//					stored in the EventChannel map
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::cleanup_EventChannel_map()
{
	EvChanIte evt_it;

    for (evt_it = channel_map.begin(); evt_it != channel_map.end(); ++evt_it)
    {
        EventChannelStruct &evt_ch = evt_it->second;
        if ((evt_ch.channel_type == ZMQ) && (evt_ch.adm_device_proxy != NULL))
        {
            AutoTangoMonitor _mon(evt_ch.channel_monitor);

//
// Release the connection to the device server administration device
//

            delete evt_ch.adm_device_proxy;
            evt_ch.adm_device_proxy = NULL;
        }
    }

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;

    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is only a command code
//

        char buffer[10];
        int length = 0;

        buffer[length] = ZMQ_END;
        length++;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);
        sender.send(send_data);

        sender.recv(&reply);
    }
    catch(zmq::error_t) {}
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::connect_event_channel()
//
// description :    Connect to the event channel
//                  This means connect to the heartbeat event
//
// Args in : - channel name : The event channel name (DS admin name)
//           - db : Database object
//           - reconnect: Flag set to true in case this method is called for
//                        event reconnection purpose
//           - dd : The DS admin device command returned data
//                  (ZmqEventSubscriptionChange command)
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::connect_event_channel(string &channel_name,Database *db,bool reconnect,DeviceData &dd)
{

//
// Extract server command result
//

    const DevVarLongStringArray *ev_svr_data;
    dd >> ev_svr_data;

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;
    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        buffer[length] = ZMQ_CONNECT_HEARTBEAT;
        length++;

        ::strcpy(&(buffer[length]),ev_svr_data->svalue[0].in());
        length = length + ::strlen(ev_svr_data->svalue[0].in()) + 1;

        string sub(channel_name);
        sub = sub + '.' + HEARTBEAT_EVENT_NAME;

        ::strcpy(&(buffer[length]),sub.c_str());
        length = length + sub.size() + 1;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);

        sender.send(send_data);

        sender.recv(&reply);
    }
    catch(zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_channel");
    }

//
// Any error during ZMQ main thread socket operations?
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while trying to connect or subscribe the heartbeat ZMQ socket to the new publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_channel");
    }


//
// Init (or create) EventChannelStruct
//

	EvChanIte evt_it = channel_map.end();
	if (reconnect == true)
	{
		evt_it = channel_map.find(channel_name);
		EventChannelStruct &evt_ch = evt_it->second;
		evt_ch.last_heartbeat = time(NULL);
		evt_ch.heartbeat_skipped = false;
		evt_ch.notifd_failed = false;
	}
	else
	{
		EventChannelStruct new_event_channel_struct;

		new_event_channel_struct.last_heartbeat = time(NULL);
		new_event_channel_struct.heartbeat_skipped = false;
		new_event_channel_struct.adm_device_proxy = NULL;
		// create a channel monitor
		new_event_channel_struct.channel_monitor = new TangoMonitor();
		// set the timeout for the channel monitor to 500ms not to block the event consumer for to long.
		new_event_channel_struct.channel_monitor->timeout(500);

		new_event_channel_struct.notifd_failed = false;
		set_channel_type(new_event_channel_struct);

		channel_map[channel_name] = new_event_channel_struct;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::disconnect_event_channel()
//
// description :    Disconnect to the event channel
//                  This means that the process should not receive
//                  the heartbeat event for this channel
//                  It will be filtered out by ZMQ
//
// Args in : - channel name : The event channel name (DS admin name)
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::disconnect_event_channel(string &channel_name)
{
    string unsub(channel_name);
    unsub = unsub + '.' + HEARTBEAT_EVENT_NAME;

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;

    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        buffer[length] = ZMQ_DISCONNECT_HEARTBEAT;
        length++;

        ::strcpy(&(buffer[length]),unsub.c_str());
        length = length + unsub.size() + 1;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);
        sender.send(send_data);

        sender.recv(&reply);
    }
    catch (zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event_channel");
    }

//
// In case of error returned by the main ZMQ thread
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while trying to unsubscribe the heartbeat ZMQ socket from the channel heartbeat publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event_channel");
    }

}


//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::disconnect_event()
//
// description :    Disconnect to the event
//                  This means that the process should not receive
//                  the event any more
//                  It will be filtered out by ZMQ
//
// Args in : - event_name : The event name
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::disconnect_event(string &event_name)
{

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;

    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        buffer[length] = ZMQ_DISCONNECT_EVENT;
        length++;

        ::strcpy(&(buffer[length]),event_name.c_str());
        length = length + event_name.size() + 1;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);
        sender.send(send_data);

        sender.recv(&reply);
    }
    catch (zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event");
    }

//
// In case of error returned by the main ZMQ thread
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while trying to unsubscribe the heartbeat ZMQ socket from the channel heartbeat publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event");
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::connect_event_system()
//
// description :    Connect to the real event (change, archive,...)
//
// argument : in :	- device_name : The device fqdn (lower case)
//                  - att_name : The attribute name
//                  - event_name : The event name
//                  - filters : The event filters given by the user
//                  - evt_it : Iterator pointing to the event channel entry
//                             in channel_map map
//                  - new_event_callback : Structure used for the event callback
//                                         entry in the event_callback_map
//                  - dd : The data returned by the DS admin device
//                         xxxSubscriptionChange command
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::connect_event_system(string &device_name,string &att_name,string &event_name,const vector<string> &filters,
                                            EvChanIte &eve_it,EventCallBackStruct &new_event_callback,DeviceData &dd)
{
    string full_event_name = device_name + '/' + att_name + '.' + event_name;

//
// Extract server command result
//

    const DevVarLongStringArray *ev_svr_data;
    dd >> ev_svr_data;
cout << "Long data 0 = " << ev_svr_data->lvalue[0] << endl;
cout << "Long data 1 = " << ev_svr_data->lvalue[1] << endl;
cout << "Long data 2 = " << ev_svr_data->lvalue[2] << endl;
cout << "Long data 3 = " << ev_svr_data->lvalue[3] << endl;

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;
    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// If the transport is multicast, add main IP interface address in endpoint
//

        bool mcast_transport = false;

        string endpoint(ev_svr_data->svalue[1].in());
        if (endpoint.find(MCAST_PROT) != string::npos)
        {
            mcast_transport = true;

            ApiUtil *au = ApiUtil::instance();
            vector<string> adrs;

            au->get_ip_from_if(adrs);

            for (unsigned int i = 0;i < adrs.size();++i)
            {
                if (adrs[i].find("127.") == 0)
                    continue;
                adrs[i] = adrs[i] + ';';
                string::size_type pos = endpoint.find('/');
                pos = pos + 2;
                endpoint.insert(pos,adrs[i]);
                break;
            }
        }

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        if (mcast_transport == true)
            buffer[length] = ZMQ_CONNECT_MCAST_EVENT;
        else
            buffer[length] = ZMQ_CONNECT_EVENT;
        length++;

        ::strcpy(&(buffer[length]),endpoint.c_str());
        length = length + endpoint.size() + 1;

        ::strcpy(&(buffer[length]),full_event_name.c_str());
        length = length + full_event_name.size() + 1;

//
// In case of multicasting, add rate and ivl parameters
//

        if (mcast_transport == true)
        {
            ::memcpy(&(buffer[length]),&(ev_svr_data->lvalue[2]),sizeof(Tango::DevLong));
            length = length + sizeof(Tango::DevLong);

           ::memcpy(&(buffer[length]),&(ev_svr_data->lvalue[3]),sizeof(Tango::DevLong));
            length = length + sizeof(Tango::DevLong);
        }

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);

        sender.send(send_data);

        sender.recv(&reply);
    }
    catch(zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to create connection to event!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_system");
    }

//
// Any error during ZMQ main thread socket operations?
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to create connection to event!\n";
        o << "Error while trying to connect or subscribe the event ZMQ socket to the new publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_system");
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::push_heartbeat_event()
//
// description :    Method called when the heartbeat event is received
//                  This method retrieve the channel entry in the channel_map
//                  and update the last heartbeat date.
//
// argument(s) : in : - ev_name : The fully qualifed event name
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::push_heartbeat_event(string &ev_name)
{

//
// Remove ".heartbeat" at the end of event name
//

    string::size_type pos = ev_name.find(".heartbeat");
    if (pos == string::npos)
    {
        return;
    }
    ev_name.erase(pos);

//
// Only reading from the maps
//

    map_modification_lock.readerIn();

    std::map<std::string,EventChannelStruct>::iterator ipos;
    ipos = channel_map.find(ev_name);

    if (ipos != channel_map.end())
    {
        EventChannelStruct &evt_ch = ipos->second;
        try
        {
            AutoTangoMonitor _mon(evt_ch.channel_monitor);
            evt_ch.last_heartbeat = time(NULL);
        }
        catch (...)
        {
            cerr << "Tango::ZmqEventConsumer::push_heartbeat_event() timeout on channel monitor of " << ipos->first << endl;
        }
    }
    else
    {
        cerr << "No entry in channel map for heartbeat " << ev_name << "!" << endl;
    }

    map_modification_lock.readerOut();
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::push_zmq_event()
//
// description :    Method called when the event is received
//                  This method retrieve the channel entry in the channel_map
//                  and update the last heartbeat date.
//
// argument(s) : in : - ev_name : The fully qualifed event name
//                    - endian : The sender host endianess
//                    - event_data : The event data still in a ZMQ message
//                    - error : Flag set to true if the event data is an error
//                              stack
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::push_zmq_event(string &ev_name,unsigned char endian,zmq::message_t &event_data,bool error)
{

    map_modification_lock.readerIn();
    bool map_lock = true;

//
// Search for entry within the event_callback map using the event
// name received in the event
//

    map<std::string,EventCallBackStruct>::iterator ipos;

    ipos = event_callback_map.find(ev_name);
    if (ipos != event_callback_map.end())
    {
        const AttributeValue *attr_value = NULL;
        const AttributeValue_3 *attr_value_3 = NULL;
        const AttributeValue_4 *attr_value_4 = NULL;
        const AttributeConfig_2 *attr_conf_2 = NULL;
        const AttributeConfig_3 *attr_conf_3 = NULL;
        const AttDataReady *att_ready = NULL;
        const DevErrorList *err_ptr;
        DevErrorList errors;
        AttributeInfoEx *attr_info_ex = NULL;

        bool ev_attr_conf = false;
        bool ev_attr_ready = false;

        EventCallBackStruct &evt_cb = ipos->second;

//
// Get which type of event data has been received (from the event type)
//

        string::size_type pos = ev_name.rfind('.');
        string event_name = ev_name.substr(pos + 1);

        UserDataEventType data_type;

        if (event_name == CONF_TYPE_EVENT)
            data_type = ATT_CONF;
        else if (event_name == DATA_READY_TYPE_EVENT)
            data_type = ATT_READY;
        else
            data_type = ATT_VALUE;

//
// Unmarshal the event data
//

        long vers;
        DeviceAttribute *dev_attr = NULL;

//
// Check if the buffer returned by ZMQ is aligned on a 8 bytes boundary.
// This is mandatory for omniORB unmarshalling in case of 64 bits
// data transfer
// If it is not the case, shift the buffer starting point
// from the padding CORBA long sent by the event supplier
//
// When transfering 64 bits data, omniORB marshalling/unmarshalling
// layer requires the 64 bits data to be aligned on a 8 bytes
// boundary. The transferred data starts with the union
// descriminator (4 bytes) followed by the sequence element number
// (4 bytes). ZMQ returns a buffer which could be aligned on a 4 bytes
// boudary. In such a case, the first 64 bits data with not be aligned
// in a 8 bytes boundary (4 + 4 + 4 = 12).
//

        bool aligned = true;
        char *data_ptr = (char *)event_data.data();
        size_t data_size = (size_t)event_data.size();

        if (((unsigned long)data_ptr & 0x7) != 0)
        {
            data_ptr = data_ptr + sizeof(CORBA::Long);
            data_size = data_size - sizeof(CORBA::Long);
            aligned = false;
        }

        cdrMemoryStream event_data_cdr(data_ptr,data_size);
        event_data_cdr.setByteSwapFlag(endian);

//
// Unmarshal the data
//
// In case the buffer starting point has not been changed due to alignemnt,
// don't forget to extract the padding CORBA long
// At the moment, abort the process in case it happenss
// (never detected during all the development and testing phase)
//
// TODO: In case, it happens, and if the attribute data type is
// DevLong64, DevULong64 or DevDouble, allocate buffer aligned
// on a 8 bytes boundary and copy the buffer into this memory
// but not the first 4 bytes
//

        if (aligned == true)
        {
            CORBA::Long dummy;
            (CORBA::Long &)dummy <<= event_data_cdr;

            assert(false);
        }

//
// Unmarshall the data
//

        if (error == true)
        {
            (DevErrorList &)del <<= event_data_cdr;
            err_ptr = &del.in();
            errors = *err_ptr;
        }
        else
        {
            switch (data_type)
            {
                case ATT_CONF:
                if (evt_cb.device_idl == 2)
                {
                    (AttributeConfig_2 &)ac2 <<= event_data_cdr;
                    attr_conf_2 = &ac2.in();
                    vers = 2;
                    attr_info_ex = new AttributeInfoEx();
                    *attr_info_ex = const_cast<AttributeConfig_2 *>(attr_conf_2);
                    ev_attr_conf = true;
                }
                else if (evt_cb.device_idl > 2)
                {
                    (AttributeConfig_3 &)ac3 <<= event_data_cdr;
                    attr_conf_3 = &ac3.in();
                    vers = 3;
                    attr_info_ex = new AttributeInfoEx();
                    *attr_info_ex = const_cast<AttributeConfig_3 *>(attr_conf_3);
                    ev_attr_conf = true;
                }
                break;

                case ATT_READY:
                (AttDataReady &)adr <<= event_data_cdr;
                att_ready = &adr.in();
                ev_attr_ready = true;
                break;

                case ATT_VALUE:
                if (evt_cb.device_idl < 3)
                {
                    (AttributeValue &)av <<= event_data_cdr;
                    attr_value = &av.in();
                    vers = 2;
                    dev_attr = new (DeviceAttribute);
                    attr_to_device(attr_value,attr_value_3,vers,dev_attr);
                }
                else if (evt_cb.device_idl == 3)
                {
                    (AttributeValue_3 &)av3 <<= event_data_cdr;
                    attr_value_3 = &av3.in();
                    vers = 3;
                    dev_attr = new (DeviceAttribute);
                    attr_to_device(attr_value,attr_value_3,vers,dev_attr);
                }
                else if (evt_cb.device_idl > 3)
                {
                    (AttributeValue_4 &)av4 <<= event_data_cdr;
                    attr_value_4 = &av4.in();
                    vers = 4;
                    dev_attr = new (DeviceAttribute);
                    attr_to_device(attr_value_4,dev_attr);
                }
                break;
            }
        }

        AutoTangoMonitor _mon(evt_cb.callback_monitor);
        try
        {

//
// Fire the user callback
//

            vector<EventSubscribeStruct>::iterator esspos;

            unsigned int cb_nb = ipos->second.callback_list.size();
            unsigned int cb_ctr = 0;

            for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
            {
                cb_ctr++;
                if (esspos->id > 0)
                {
                    CallBack *callback;
                    callback = esspos->callback;
                    EventQueue *ev_queue;
                    ev_queue = esspos->ev_queue;

                    if (cb_ctr == cb_nb)
                    {
                        map_lock = false;
                        map_modification_lock.readerOut();
                    }

                    if ((ev_attr_conf == false) && (ev_attr_ready == false))
                    {
                        EventData *event_data;
                        if (cb_ctr != cb_nb)
                        {
                            DeviceAttribute *dev_attr_copy = NULL;
                            if (dev_attr != NULL)
                            {
                                dev_attr_copy = new DeviceAttribute();
                                dev_attr_copy->deep_copy(*dev_attr);
                            }

                            event_data = new EventData(event_callback_map[ev_name].device,
                                                                ev_name,
                                                                event_name,
                                                                dev_attr_copy,
                                                                errors);
                        }
                        else
                        {
                            event_data = new EventData (event_callback_map[ev_name].device,
                                                              ev_name,
                                                              event_name,
                                                              dev_attr,
                                                              errors);
                        }

                        // if a callback method was specified, call it!
                        if (callback != NULL )
                        {
                            try
                            {
                                callback->push_event(event_data);
                            }
                            catch (...)
                            {
                                cerr << "Tango::ZmqEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
                            }

                            delete event_data;
                        }

                        // no calback method, the event has to be instered
                        // into the event queue
                        else
                        {
                            ev_queue->insert_event(event_data);
                        }
                    }
                    else if (ev_attr_ready == false)
                    {
                        AttrConfEventData *event_data;

                        if (cb_ctr != cb_nb)
                        {
                            AttributeInfoEx *attr_info_copy = new AttributeInfoEx();
                            *attr_info_copy = *attr_info_ex;
                            event_data = new AttrConfEventData(event_callback_map[ev_name].device,
                                                              ev_name,
                                                              event_name,
                                                              attr_info_copy,
                                                              errors);
                        }
                        else
                        {
                            event_data = new AttrConfEventData(event_callback_map[ev_name].device,
                                                              ev_name,
                                                              event_name,
                                                              attr_info_ex,
                                                              errors);
                        }


                        // if callback methods were specified, call them!
                        if (callback != NULL )
                        {
                            try
                            {
                                callback->push_event(event_data);
                            }
                            catch (...)
                            {
                                cerr << "Tango::ZmqEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
                            }

                            delete event_data;
                        }

                        // no calback method, the event has to be instered
                        // into the event queue
                        else
                        {
                            ev_queue->insert_event(event_data);
                        }
                    }
                    else
                    {
                        DataReadyEventData *event_data = new DataReadyEventData(event_callback_map[ev_name].device,
                                                                const_cast<AttDataReady *>(att_ready),event_name,errors);
                        // if a callback method was specified, call it!
                        if (callback != NULL )
                        {
                            try
                            {
                                callback->push_event(event_data);
                            }
                            catch (...)
                            {
                                cerr << "Tango::ZmqEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
                            }
                            delete event_data;
                        }

                        // no calback method, the event has to be instered
                        // into the event queue
                        else
                        {
                            ev_queue->insert_event(event_data);
                        }
                    }
                }
                else // id < 0
                {
                    if (cb_ctr == cb_nb)
                    {
                        map_lock = false;
                        map_modification_lock.readerOut();
                    }

                    if ((ev_attr_conf == false) && (ev_attr_ready == false))
                        delete dev_attr;
                    else if (ev_attr_ready == false)
                        delete attr_info_ex;
                }
            } // End of for
        }
        catch (...)
        {
            // free the map lock if not already done
            if ( map_lock == true )
            {
                map_modification_lock.readerOut();
            }

            cerr << "Tango::ZmqEventConsumer::push_structured_event() timeout on callback monitor of " << ipos->first << endl;
        }
    }
    else
    {
        cerr << "Event " << ev_name << " not found in event callback map !!!" << endl;
		// even if nothing was found in the map, free the lock
        map_modification_lock.readerOut();
    }
}

} /* End of Tango namespace */

