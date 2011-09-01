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

ZmqEventConsumer::ZmqEventConsumer(ApiUtil *ptr) : EventConsumer(ptr),omni_thread((void *)ptr)
{
cout << "Entering ZmqEventConsumer ctor" << endl;

	cout3 << "calling Tango::ZmqEventConsumer::ZmqEventConsumer() \n";

	_instance = this;

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
// description :
//
//-----------------------------------------------------------------------------

void *ZmqEventConsumer::run_undetached(void *arg)
{

//
// activate POA and go into endless loop waiting for events to be pushed
// this method should run as a separate thread so as not to block the client
//

	ApiUtil *api_util_ptr = (ApiUtil *)arg;

cout << "Entering sleep call in Zmq main thread" << endl;
sleep(10000);

	return (void *)NULL;
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
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::connect_event_channel()
//
// description :
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::connect_event_channel(string &channel_name,Database *db,bool reconnect)
{
	EvChanIte evt_it = channel_map.end();
	if (reconnect == true)
	{
		evt_it = channel_map.find(channel_name);
		EventChannelStruct &evt_ch = evt_it->second;
		evt_ch.last_heartbeat = time(NULL);
		evt_ch.heartbeat_skipped = false;
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
		set_channel_type(new_event_channel_struct);

		channel_map[channel_name] = new_event_channel_struct;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::connect_event_system()
//
// description :
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::connect_event_system(string &device_name,string &att_name,string &event_name,const vector<string> &filters,
                                            EvChanIte &eve_it,EventCallBackStruct &new_event_callback)
{

}


} /* End of Tango namespace */

