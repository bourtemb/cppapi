static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
///  file 	event.cpp
///
///		C++ classes for implementing the event server and client
///		singleton classes - EventSupplier and EventConsumer.
///		These classes are used to send events from the server
///		to the notification service and to receive events from
///		the notification service. 
///
///		author(s) : A.Gotz (goetz@esrf.fr)
///
///		original : 7 April 2003
///
///		$Revision$
///
///		$Log$
///		Revision 1.31  2008/03/11 14:38:25  taurel
///		- Apply patches from Frederic Picca about compilation with gcc 4.2
///		
///		Revision 1.30  2008/01/23 08:33:49  jensmeyer
///		Modified unsubscribe_event() not to take the callback monitor
///		before erasing the callback from the map.
///		
///		Revision 1.29  2008/01/18 14:25:30  jensmeyer
///		Added the stateless event subscription in the EventConsumer.
///		The subscribe_event method has an option now to subscribe for
///		events in stateless way.
///		
///		Revision 1.28  2008/01/10 13:40:43  jensmeyer
///		Cleaned-up comments with old thread synchronisation mechanism
///		
///		Revision 1.27  2008/01/09 13:03:47  jensmeyer
///		Added a correct clean-up when exiting a device server which
///		is also used as an event client.
///		Added a EventConsumer::disconnect_from_notifd method as for
///		the EventSupplier.
///		
///		Revision 1.26  2007/12/19 15:42:43  taurel
///		- Add some cleanup when quitting applis or device server (notifd objects)
///		
///		Revision 1.25  2007/12/17 12:54:48  taurel
///		- Remove an unused local variable
///		
///		Revision 1.24  2007/11/22 15:32:03  jensmeyer
///		Modified the mutexes to allow calling subscribe_event() and unsubscribe_event() in callback methods of other events.
///		
///		Revision 1.23  2007/11/13 13:19:27  jensmeyer
///		Corrected leavefunc() which is executed at client exit to shutdown the event client threads.
///		
///		Revision 1.22  2007/10/16 08:23:37  taurel
///		- Add management of the TC connection establishment timeout for DB access
///		- Add DB server cache in DS used during DS startup sequence
///		- Comment out the sleep time during DS startup sequence
///		
///		Revision 1.21  2007/06/21 09:05:54  jensmeyer
///		Corrected the creation of the administration device name during
///		event subscription for devices witout database
///		
///		Revision 1.20  2007/06/06 09:12:33  jensmeyer
///		New synchronisation of the event consumer and the keep alive thread.
///		The old version could block the whole event reception during a reconnection
///		by the keep alive thread.
///		The synchroisation no longer done on the whole channel and callback maps
///		but on the individual channel or callback.
///		To allow a subscribe or unsubscribe during event reception a new ReadersWritersLock class is used. The subscription thread is the writer
///		and the event consumer and the keep alive thread are readers.
///		
///		Revision 1.19  2007/04/20 14:40:34  taurel
///		- Ported to Windows 64 bits x64 architecture
///		
///		Revision 1.18  2007/04/16 14:56:36  taurel
///		- Added 3 new attributes data types (DevULong, DevULong64 and DevState)
///		- Ported to omniORB4.1
///		- Increased the MAX_TRANSFER_SIZE to 256 MBytes
///		- Added a new filterable field in the archive event
///		
///		Revision 1.17  2007/03/29 07:05:21  taurel
///		- Change some data types for 64 bits compatibility
///		
///		Revision 1.16  2007/03/06 08:18:04  taurel
///		- Added 64 bits data types for 64 bits computer...
///		
///		Revision 1.15  2007/02/16 10:40:09  taurel
///		- Implementing a new types of event on the Attribute configuration
///		
///		Revision 1.14  2006/09/20 07:46:57  jensmeyer
///		Corrected reconnection to notify daemon in case of network connection problems
///		
///		Revision 1.13  2006/06/20 15:17:55  jensmeyer
///		Corrected bug in the event reconnection.
///		The bug was introduced when implementing the host name check
///		for the notifd.
///		
///		Revision 1.12  2006/06/14 11:24:44  jensmeyer
///		Modifications to reconnect to the good notifyd when changing a device server from
///		one host to another. For all subscribed events of the server another notifyd needs
///		to be used.
///		
///		Revision 1.11  2005/12/16 17:36:24  jensmeyer
///		Suppressed the subscription for quality change events, throwing an exception now.
///		
///		Revision 1.10  2005/07/20 14:40:46  taurel
///		- Fix bug in polling thread out of sync error. Time threshold to generate
///		event heartbeat error changed from 10 to 11. Fix some comments
///		
///		Revision 1.9  2005/06/29 08:31:17  taurel
///		- Last commit before release 5.2 ?
///		
///		Revision 1.8  2005/05/20 15:17:13  taurel
///		- Some changes to support gcc on Solaris
///		- Fix bug in fire_event_quality()
///		
///		Revision 1.7  2005/05/10 13:49:50  taurel
///		- Minor changes for Windows compiler
///		
///		Revision 1.6  2005/05/10 12:51:59  taurel
///		- Correctly init. the event name and attr name fields of the EventData instance
///		passed to callback in case of no more server heartbeat
///		
///		Revision 1.5  2005/04/15 11:34:07  taurel
///		- Changes to support Tango on 64 bits computer
///		- Support for Linux 2.6 kernel with NPTL (Signal management)
///		
///		Revision 1.4  2005/03/14 09:49:29  taurel
///		- Fix some bugs in filedatabase (Change in the data transferred between client and
///		  server).
///		- Fix bug in event re-connection
///		- Add event support even for device server started with the -file command line option
///		
///		Revision 1.3  2005/01/13 08:29:05  taurel
///		- Merge trunk with Release_5_0 from brach Release_5_branch
///		
///		Revision 1.2.2.8  2004/11/26 13:53:20  taurel
///		- Fix bug if exception thrown during Util class construction
///		- Change attribute label and format default values
///		
///		Revision 1.2.2.7  2004/11/15 12:32:43  taurel
///		- Some changes for omniORB 4.0.5 (Don't use USE_stub_in_nt_dll...)
///		
///		Revision 1.2.2.6  2004/11/04 09:46:39  taurel
///		- Add a tuning method in the polling thread
///		- Some minor fixes to pass test suite
///		
///		Revision 1.2.2.5  2004/10/27 05:59:46  taurel
///		- Some minor changes to compile on all our supported platforms
///		
///		Revision 1.2.2.4  2004/10/22 11:25:50  taurel
///		Added warning alarm
///		Change attribute config. It now includes alarm and event parameters
///		Array attribute property now supported
///		subscribe_event throws exception for change event if they are not correctly configured
///		Change in the polling thread: The event heartbeat has its own work in the work list
///		Also add some event_unregister
///		Fix order in which classes are destructed
///		Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
///		Change in the CORBA info call for the device type
///		
///		Revision 1.2.2.3  2004/09/15 06:46:28  taurel
///		- Added four new types for attributes (boolean, float, unsigned short and unsigned char)
///		- It is also possible to read state and status as attributes
///		- Fix bug in Database::get_class_property() method (missing ends insertion)
///		- Fix bug in admin device DevRestart command (device name case problem)
///		
///		Revision 1.2.2.2  2004/08/26 07:34:45  taurel
///		- Implement a way to directly fills command or attribute polling buffer
///		
///		Revision 1.2.2.1  2004/07/15 15:04:06  taurel
///		- Added the way to externally filled the polling buffer for attribute
///		  (Command will come soon)
///		
///		Revision 1.2  2004/07/07 07:59:20  taurel
///		Added some files
///		
///		Revision 1.1.4.13  2004/05/24 08:44:05  taurel
///		- Fix bug if device name stored in db used upper case. Add a DeviceImpl::get_name_lower() method
///		
///		Revision 1.1.4.12  2004/05/17 15:35:26  taurel
///		- Added full (server and client) re-connection to notifid in case it crashes
///		  or if the computer where it runs is re-boot
///		
///		Revision 1.1.4.11  2004/04/07 11:22:10  taurel
///		- Add some import/export declaration for Windows DLL
///		- Add test on minor code for the CORBA::IMP_LIMIT exception before
///		  printing it
///		
///		Revision 1.1.4.10  2004/04/05 12:43:34  taurel
///		- A last minor change for HP 11 (thank's Claudio)
///		
///		Revision 1.1.4.9  2004/04/02 14:58:16  taurel
///		Changes for release 4.1
///		- Change the event.h inclusion place in tango.h
///		- Fix bugs in event.cpp file and add a clean way to shutdown event system
///		- Now support attribute min,max,min_alarm and max_alarm defined in scientific notation for long attribute
///		- Added debian30 support in Make.rules
///		
///		Revision 1.1.4.8  2004/03/19 15:25:39  taurel
///		- Changes for the Windows DLL which does not execute the functions   registered by atexit at the same time than static libs or Unix libs !!!
///		- Work to be able to destroy the EventCOnsumer class (via ApiUtil dtor)
///		  but I don't think it is ready as it is commited now
///		- FIx bug in subscribe_event() if several filters in its vector<string>
///		  (thanks Majid)
///		
///		Revision 1.1.4.7  2004/03/09 16:36:36  taurel
///		- Added HP aCC port (thanks to Claudio from Elettra)
///		- Some last small bugs fixes
///		
///		Revision 1.1.4.6  2004/03/02 12:43:08  taurel
///		- Add a cleanup_heartbeat_filters() method to destroy heartbeat filters
///		
///		Revision 1.1.4.5  2004/03/02 07:41:56  taurel
///		- Fix compiler warnings (gcc used with -Wall)
///		- Fix bug in DbDatum insertion operator fro vectors
///		- Now support "modulo" as periodic filter
///		
///		Revision 1.1.4.4  2004/02/25 16:27:44  taurel
///		Minor changes to compile library using Solaris CC compiler
///		
///		Revision 1.1.4.3  2004/02/18 15:06:17  taurel
///		Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
///		restart
///		
///		Revision 1.1.4.2  2004/02/06 11:58:51  taurel
///		- Many changes in the event system
///		
///
///		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>

#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>

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


/************************************************************************/
/*		       															*/		
/* 		(client_)leavfunc function 										*/
/*			-----------------											*/
/*		       															*/
/* This function will be executed at process exit or when the main      */
/* returned.  It has to be executed to properly shutdown and destroy    */
/* the ORB used by as a server by the event system. The ORB loop is in  */
/* EventConsumer thread. Therefore, get a reference to it, shutdown the */
/* ORB and wait until the thread exit.				        			*/
/* It also destroys the heartbeat filters								*/
/*																		*/
/************************************************************************/
void client_leavefunc()
{
	leavefunc();
}

void leavefunc()
{
	// flag to avoid calling client_leavefunc twice
	static bool already_executed = false;

	EventConsumer *ev = ApiUtil::instance()->get_event_consumer();
	
	if (ev != NULL && already_executed == false)
	{
		ev->disconnect_from_notifd();			
		
		//
		// Shut-down the ORB and wait for the thread to exit
		//
		
		int *rv;
		ev->orb_->shutdown(true);
		ev->join((void **)&rv);
	
		already_executed = true;
	}
		
}



/************************************************************************/
/*		       															*/		
/* 			EventConsumer class 										*/
/*			-------------------											*/
/*		       															*/
/************************************************************************/



EventConsumer *EventConsumer::_instance = NULL;

EventConsumer::EventConsumer(ApiUtil *ptr) : omni_thread((void *)ptr)
{
	cout3 << "calling Tango::EventConsumer::EventConsumer() \n";
	orb_ = ptr->get_orb();

	_instance = this;

//
// Install a function to be executed at exit
// This is the only way I found to properly
// shutdown and destroy the ORB.
// Don't do this for windows DLL.
//
// Is this necessary when events are used within a server ?
//

#ifndef _USRDLL
	if (ptr->in_server() == false)
		{
		atexit(leavefunc);
		}
#endif

	start_undetached();
}

EventConsumer *EventConsumer::create()
{
//
// check if the EventConsumer singleton exists, if so return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// EventConsumer singleton does not exist, create it
//

	ApiUtil *ptr =  ApiUtil::instance();
	return new EventConsumer(ptr);
}

//
// activate POA and go into endless loop waiting for events to be pushed
// this method should run as a separate thread so as not to block the client
//

void *EventConsumer::run_undetached(void *arg)
{
	// initialise the unique event id for the client;
	subscribe_event_id = 0;
	
	cmd.cmd_pending = false;
	keep_alive_thread = new EventConsumerKeepAliveThread(cmd);
	keep_alive_thread->start();

	ApiUtil *api_util_ptr = (ApiUtil *)arg;
	if (api_util_ptr->in_server() == false)
	{
		CORBA::Object_var obj = orb_->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
       	PortableServer::POAManager_var pman = poa->the_POAManager();
       	pman->activate();

		orb_->run();

		orb_->destroy();
		
		CORBA::release(orb_);
	}
	
	return (void *)NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::disconnect_from_notifd()
// 
// description : 	Method to stop the keep alive thread and to
//                disconnect from all used notifd event channels
//
//-----------------------------------------------------------------------------
void EventConsumer::disconnect_from_notifd()
{

	cout3 << "calling Tango::EventConsumer::disconnect_from_notifd() \n";
	
	//
	// Shut-down the KeepAliveThread and wait for it to exit
	//
	
	{
		omni_mutex_lock sync(cmd);
			
		cmd.cmd_pending = true;
		cmd.cmd_code = EXIT_TH;
			
		cmd.cond.signal();
	}
				
		int *rv;
		keep_alive_thread->join((void **)&rv);
			
	//
	// Destroy DeviceProxy object stored in map container
	//

	cleanup_EventChannel_map();			
}


void EventConsumer::disconnect_structured_push_consumer()
{
	cout3 << "calling Tango::EventConsumer::disconnect_structured_push_consumer() \n";
}

void EventConsumer::offer_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled)
{
	cout3 << "calling Tango::EventConsumer::subscription_change() \n";
}






void EventConsumer::connect(DeviceProxy *device_proxy)
{

	string device_name = device_proxy->dev_name();

	string adm_name;
	try
	{
		adm_name = device_proxy->adm_name();
	}
	catch(...)
	{
		TangoSys_OMemStream o;
		o << "Can't subscribe to event for device " << device_name << "\n";
		o << "Check that device server is running..." << ends;
		Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        o.str(),
				        (const char *)"EventConsumer::connect()");
	}
	
	string channel_name = adm_name;

//
// If no connection exists to this channel then connect to it.
// Sometimes, this method is called in order to reconnect
// to the notifd. In such a case, the lock is already
// locked before the method is called
//

	std::map<std::string,EventChannelStruct>::iterator ipos;

	{
		ipos = channel_map.find(channel_name);

		if (ipos == channel_map.end())
		{
			connect_event_channel(channel_name,device_proxy->get_device_db(),false);
		}
		if (channel_map[channel_name].adm_device_proxy != NULL)
			delete channel_map[channel_name].adm_device_proxy;
		try
		{
			if (device_proxy->get_from_env_var() == false &&
			   device_proxy->get_db_host() != NOT_USED )
			{
				string added_str = device_proxy->get_db_host();
				added_str = added_str + ':' + device_proxy->get_db_port() + '/';
				adm_name.insert(0,added_str);
			}
			channel_map[channel_name].adm_device_proxy = new DeviceProxy(adm_name);
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Cannot contact the DS admin dev for your device" << ends;
			Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        o.str(),
				        (const char *)"EventConsumer::connect()");
		}
		channel_map[channel_name].full_adm_name = adm_name;
	}
	
	device_channel_map[device_name] = channel_name;
}






void EventConsumer::connect_event_channel(string &channel_name,Database *db,bool reconnect)
{
	CORBA::Any_var received;
	const DevVarLongStringArray *dev_import_list;
	
//
// Get a reference to an EventChannel for this device server from the 
// TANGO database or from the DS admin device (for device in a DS
// started with the -file option)
//

	int channel_exported;
	string channel_ior;
	string hostname;

	if (db != NULL)
	{
		try 
		{
			received = db->import_event(channel_name);
		}
		catch (...) 
		{
			TangoSys_OMemStream o;

			o << channel_name;
			o << " has no event channel defined in the database\n";
			o << "Maybe the server is not running or is not linked with Tango release 4.x (or above)... " << ends;
			Except::throw_exception((const char *)"API_NotificationServiceFailed",
			      			o.str(),
			      			(const char *)"EventConsumer::connect_event_channel");
		}
	
     	received.inout() >>= dev_import_list;
     	channel_ior = string((dev_import_list->svalue)[1]);
     	channel_exported = dev_import_list->lvalue[0];
			
	  	// get the hostname where the notifyd should be running
		hostname = string (dev_import_list->svalue[3]);
	}
	else
	{
		DeviceProxy adm(channel_name);
		
		try
		{
			DeviceData dd;
			dd = adm.command_inout("QueryEventChannelIOR");
			dd >> channel_ior;
			channel_exported = true;
			
			// get the hostname where the notifyd should be running
			DeviceInfo info = adm.info();
			hostname = info.server_host;		
		}
		catch (...)
		{
			TangoSys_OMemStream o;

			o << channel_name;
			o << " has no event channel\n";
			o << "Maybe the server is not running or is not linked with Tango release 4.x (or above)... " << ends;
			Except::throw_exception((const char *)"API_NotificationServiceFailed",
			      			o.str(),
			      			(const char *)"EventConsumer::connect_event_channel");
		}
		
//
// Remove extra info from channel name (protocol,  dbase=xxx)
//

		string::size_type pos;
		if ((pos = channel_name.find('#')) != string::npos)
			channel_name.erase(pos);
		if ((pos = channel_name.find("://")) != string::npos)
		{
			pos = pos + 3;
			if ((pos = channel_name.find('/',pos)) != string::npos)
				channel_name.erase(0,pos + 1);
		}
	}

	if (channel_exported)
	{
		try
		{
        	CORBA::Object *event_channel_obj;
        	event_channel_obj = orb_ -> string_to_object(channel_ior.c_str());

        	if (event_channel_obj -> _non_existent())
                event_channel_obj = CORBA::Object::_nil();
		
			eventChannel = CosNotifyChannelAdmin::EventChannel::_nil();
 			eventChannel = CosNotifyChannelAdmin::EventChannel::_narrow(event_channel_obj);

			if(CORBA::is_nil(eventChannel))
			{
				channel_exported = 0;
			}
		}
		catch (...)
		{
                        //cerr << "Failed to narrow EventChannel from notification daemon (hint: make sure the notifd process is running on this host)" << endl;
         	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                        	(const char*)"Failed to narrow EventChannel from notification daemon (hint: make sure the notifd process is running on this host)",
                        	(const char*)"EventConsumer::connect_event_channel()");        
		}
	}
	else
	{
    	EventSystemExcept::throw_exception((const char*)"API_EventChannelNotExported",
                        	(const char*)"Failed to narrow EventChannel (hint: make sure a notifd process is running on the server host)",
                        	(const char*)"EventConsumer::connect_event_channel()");  
	}
			
//
// Obtain a Consumer Admin
//

//
// We'll use the channel's default Consumer admin
//

	try
	{
       	consumerAdmin = eventChannel->default_consumer_admin();

		if (CORBA::is_nil(consumerAdmin))
		{
        		//cerr << "Could not get CosNotifyChannelAdmin::ConsumerAdmin" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::connect_event_channel()");        
        	exit((void*)1);
    	}
	}
	catch (...)
	{
     	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::connect_event_channel()");        
	}

//
// Obtain a Proxy Supplier
//

//
// We are using the "Push" model and Structured data
//

	try
	{
	    proxySupplier =
		consumerAdmin -> obtain_notification_push_supplier(CosNotifyChannelAdmin::STRUCTURED_EVENT, proxyId);
		if (CORBA::is_nil(proxySupplier))
		{
        		//cerr << "Could not get CosNotifyChannelAdmin::ProxySupplier" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to obtain a push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::connect_event_channel()");        
    	}

        structuredProxyPushSupplier =
		CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow(
		    		proxySupplier);
		if (CORBA::is_nil(structuredProxyPushSupplier))
		{
       			//cerr << "Tango::EventConsumer::EventConsumer() could not get CosNotifyChannelAdmin::StructuredProxyPushSupplier" << endl;
       		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to narrow the push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::connect_event_channel()");        
    	}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get PushSupplier from notification daemon due to AdminLimitExceeded (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::connect_event_channel()");        
	}

//
// Connect to the Proxy Consumer
//

	try
	{
	    structuredProxyPushSupplier -> connect_structured_push_consumer(_this());
	}
	catch(const CosEventChannelAdmin::AlreadyConnected&)
	{
       	cerr << "Tango::EventConsumer::EventConsumer() caught AlreadyConnected exception" << endl;
	}

	EventChannelStruct new_event_channel_struct;

	if (reconnect == true)
	{
		channel_map[channel_name].eventChannel = eventChannel;
		channel_map[channel_name].structuredProxyPushSupplier = structuredProxyPushSupplier;
		channel_map[channel_name].last_heartbeat = time(NULL);
		channel_map[channel_name].heartbeat_skipped = false;
		channel_map[channel_name].notifyd_host = hostname;
		channel_map[channel_name].notifd_failed = false;
		channel_map[channel_name].has_notifd_closed_the_connection = 0;
	}
	else
	{
		new_event_channel_struct.eventChannel = eventChannel;
		new_event_channel_struct.structuredProxyPushSupplier = structuredProxyPushSupplier;
		new_event_channel_struct.last_heartbeat = time(NULL);
		new_event_channel_struct.heartbeat_skipped = false;
		new_event_channel_struct.adm_device_proxy = NULL;
		// create a channel monitor
		new_event_channel_struct.channel_monitor = new TangoMonitor();
		// set the timeout for the channel monitor to 500ms not to block the event consumer for to long.
		new_event_channel_struct.channel_monitor->timeout(500);
		
		channel_map[channel_name] = new_event_channel_struct;
		channel_map[channel_name].notifyd_host = hostname;
		channel_map[channel_name].notifd_failed = false;
		channel_map[channel_name].has_notifd_closed_the_connection = 0;
	}

//
// add a filter for heartbeat events
//

    char constraint_expr[256];
    ::sprintf(constraint_expr,"$event_name == \'heartbeat\'");
    CosNotifyFilter::FilterFactory_var ffp;
    CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
    try
	{
		ffp    = channel_map[channel_name].eventChannel->default_filter_factory();
		filter = ffp->create_filter("EXTENDED_TCL");
	}
	catch (...)
	{
                //cerr << "Caught exception obtaining filter object" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating heartbeat filter (check filter)",
                       	(const char*)"EventConsumer::connect_event_channel()");        
	}

//	
// Construct a simple constraint expression; add it to fadmin
//

    CosNotification::EventTypeSeq evs;
    CosNotifyFilter::ConstraintExpSeq exp;
    exp.length(1);
    exp[0].event_types = evs;
    exp[0].constraint_expr = CORBA::string_dup(constraint_expr);
    CORBA::Boolean res = 0; // OK
    try
	{
   		CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);
		channel_map[channel_name].heartbeat_filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
	}
	catch (...)
	{
		res = 1; // error     
	}
	
//
// If error, destroy filter
//
	
	if (res == 1)
	{ 
		try
		{
			filter->destroy();
		}
		catch (...) { }
		
		filter = CosNotifyFilter::Filter::_nil();
		
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while adding constraint for heartbeat (check filter)",
                       	(const char*)"EventConsumer::connect_event_channel()");
	}
}

void EventConsumer::attr_to_device(const AttributeValue *attr_value,
				   const AttributeValue_3 *attr_value_3,
				   long vers,DeviceAttribute *dev_attr)
{
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarLong64Array *tmp_seq_64;
	CORBA::LongLong *tmp_64;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	const DevVarFloatArray *tmp_seq_fl;
	CORBA::Float *tmp_fl;
	const DevVarBooleanArray *tmp_seq_boo;
	CORBA::Boolean *tmp_boo;
	const DevVarUShortArray *tmp_seq_ush;
	CORBA::UShort *tmp_ush;
	const DevVarCharArray *tmp_seq_uch;
	CORBA::Octet *tmp_uch;
	const DevVarULongArray *tmp_seq_ulo;
	CORBA::ULong *tmp_ulo;
	const DevVarULong64Array *tmp_seq_u64;
	CORBA::ULongLong *tmp_ulolo;
	const DevVarStateArray *tmp_seq_state;
	Tango::DevState *tmp_state;
		
	CORBA::ULong max,len;

	if (vers >= 3)
	{	
		dev_attr->name = attr_value_3->name;
		dev_attr->quality = attr_value_3->quality;
		dev_attr->time = attr_value_3->time;
		dev_attr->dim_x = attr_value_3->r_dim.dim_x;
		dev_attr->dim_y = attr_value_3->r_dim.dim_y;
		dev_attr->ext->w_dim_x = attr_value_3->w_dim.dim_x;
		dev_attr->ext->w_dim_y = attr_value_3->w_dim.dim_y;
		dev_attr->ext->err_list = new DevErrorList(attr_value_3->err_list);
	}
	else
	{
		dev_attr->name = attr_value->name;
		dev_attr->quality = attr_value->quality;
		dev_attr->time = attr_value->time;
		dev_attr->dim_x = attr_value->dim_x;
		dev_attr->dim_y = attr_value->dim_y;
	}
	
	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		CORBA::TypeCode_var ty;
		if (vers >= 3)
			ty = attr_value_3->value.type();
		else
			ty = attr_value->value.type();
			
		if (ty->kind() == tk_enum)
		{
			attr_value_3->value >>= dev_attr->d_state;
			dev_attr->d_state_filled = true;
		}
		else
		{		
			CORBA::TypeCode_var ty_alias = ty->content_type();
			CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
			switch (ty_seq->kind())
			{
				case tk_long:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_lo;
					else		
						attr_value->value >>= tmp_seq_lo;
					max = tmp_seq_lo->maximum();
					len = tmp_seq_lo->length();
					if (tmp_seq_lo->release() == true)
					{
						tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
						dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
					}
					else
					{
						tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
						dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
					}
					break;
	
				case tk_longlong:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_64;
					else		
						attr_value->value >>= tmp_seq_64;
					max = tmp_seq_64->maximum();
					len = tmp_seq_64->length();
					if (tmp_seq_64->release() == true)
					{
						tmp_64 = (const_cast<DevVarLong64Array *>(tmp_seq_64))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_64,true);
					}
					else
					{
						tmp_64 = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
						dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_64,false);
					}
					break;
					
				case tk_short:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_sh;
					else
						attr_value->value >>= tmp_seq_sh;
					max = tmp_seq_sh->maximum();
					len = tmp_seq_sh->length();
					if (tmp_seq_sh->release() == true)
					{
						tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
						dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
					}
					else
					{
						tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
						dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
					}
					break;
	
				case tk_double:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_db;
					else
						attr_value->value >>= tmp_seq_db;
					max = tmp_seq_db->maximum();
					len = tmp_seq_db->length();
					if (tmp_seq_db->release() == true)
					{
						tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
						dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
					}
					else
					{
						tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
						dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
					}
					break;
	
				case tk_string:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_str;
					else
						attr_value->value >>= tmp_seq_str;
					max = tmp_seq_str->maximum();
					len = tmp_seq_str->length();
					if (tmp_seq_str->release() == true)
					{
						tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
						dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,true);
					}
					else
					{
						tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
						dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,false);
					}
					break;
	
				case tk_float:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_fl;
					else		
						attr_value->value >>= tmp_seq_fl;
					max = tmp_seq_fl->maximum();
					len = tmp_seq_fl->length();
					if (tmp_seq_fl->release() == true)
					{
						tmp_fl = (const_cast<DevVarFloatArray *>(tmp_seq_fl))->get_buffer((CORBA::Boolean)true);
						dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
					}
					else
					{
						tmp_fl = const_cast<CORBA::Float *>(tmp_seq_fl->get_buffer());
						dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,false);
					}
					break;
	
				case tk_boolean:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_boo;
					else		
						attr_value->value >>= tmp_seq_boo;
					max = tmp_seq_boo->maximum();
					len = tmp_seq_boo->length();
					if (tmp_seq_boo->release() == true)
					{
						tmp_boo = (const_cast<DevVarBooleanArray *>(tmp_seq_boo))->get_buffer((CORBA::Boolean)true);
						dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
					}
					else
					{
						tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq_boo->get_buffer());
						dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,false);
					}
					break;
	
				case tk_ushort:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_ush;
					else		
						attr_value->value >>= tmp_seq_ush;
					max = tmp_seq_ush->maximum();
					len = tmp_seq_ush->length();
					if (tmp_seq_ush->release() == true)
					{
						tmp_ush = (const_cast<DevVarUShortArray *>(tmp_seq_ush))->get_buffer((CORBA::Boolean)true);
						dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
					}
					else
					{
						tmp_ush = const_cast<CORBA::UShort *>(tmp_seq_ush->get_buffer());
						dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,false);
					}
					break;
	
				case tk_octet:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_uch;
					else		
						attr_value->value >>= tmp_seq_uch;
					max = tmp_seq_uch->maximum();
					len = tmp_seq_uch->length();
					if (tmp_seq_uch->release() == true)
					{
						tmp_uch = (const_cast<DevVarCharArray *>(tmp_seq_uch))->get_buffer((CORBA::Boolean)true);
						dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
					}
					else
					{
						tmp_uch = const_cast<CORBA::Octet *>(tmp_seq_uch->get_buffer());
						dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,false);
					}
					break;
					
				case tk_ulong:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_ulo;
					else		
						attr_value->value >>= tmp_seq_ulo;
					max = tmp_seq_ulo->maximum();
					len = tmp_seq_ulo->length();
					if (tmp_seq_ulo->release() == true)
					{
						tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
					}
					else
					{
						tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq_ulo->get_buffer());
						dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
					}
					break;
					
				case tk_ulonglong:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_u64;
					else		
						attr_value->value >>= tmp_seq_u64;
					max = tmp_seq_u64->maximum();
					len = tmp_seq_u64->length();
					if (tmp_seq_u64->release() == true)
					{
						tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_u64))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
					}
					else
					{
						tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq_u64->get_buffer());
						dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
					}
					break;
					
				case tk_enum:
					if (vers >= 3)
						attr_value_3->value >>= tmp_seq_state;
					else		
						attr_value->value >>= tmp_seq_state;
					max = tmp_seq_state->maximum();
					len = tmp_seq_state->length();
					if (tmp_seq_state->release() == true)
					{
						tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
					}
					else
					{
						tmp_state = const_cast<Tango::DevState *>(tmp_seq_state->get_buffer());
						dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
					}
					break;
					
				default:
					break;
				}
		}
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::push_structured_event()
// 
// description : 	Method run when an event is received
//
// argument : in :	event : The event itself...
//
//-----------------------------------------------------------------------------


void EventConsumer::push_structured_event(const CosNotification::StructuredEvent& event)
{

	string domain_name(event.header.fixed_header.event_type.domain_name);
	string event_type(event.header.fixed_header.event_type.type_name);
	string event_name(event.header.fixed_header.event_name);	
	
	if (event_name == "heartbeat")
	{
		// only reading from the maps
      map_modification_lock.readerIn();
		
		std::map<std::string,EventChannelStruct>::iterator ipos;
		ipos = channel_map.find(domain_name);
 
      if (ipos != channel_map.end())
      {
			try 
			{
				ipos->second.channel_monitor->get_monitor();
				ipos->second.last_heartbeat = time(NULL);
				ipos->second.channel_monitor->rel_monitor();	
			}
			catch (...)
			{
				cerr << "Tango::EventConsumer::push_structured_event() timeout on channel monitor of " << ipos->first << endl;
			}
		}
		
		map_modification_lock.readerOut();
	}
	
	else
	{
		map_modification_lock.readerIn();
		bool map_lock = true;
		
		string attr_event_name = domain_name + "." + event_name;		
		map<std::string,EventCallBackStruct>::iterator ipos;

		ipos = event_callback_map.find(attr_event_name);
	
		if (ipos != event_callback_map.end())
		{
			try 
			{
				ipos->second.callback_monitor->get_monitor();
				
				CallBack *callback;
				callback = ipos->second.callback;	
				map_modification_lock.readerOut();
				map_lock = false;	
				
				AttributeValue *attr_value = NULL;
				AttributeValue_3 *attr_value_3 = NULL;
				AttributeConfig_2 *attr_conf_2 = NULL;
				AttributeConfig_3 *attr_conf_3 = NULL;
				AttributeInfoEx *attr_info_ex = NULL;
				DevErrorList errors;
				bool ev_attr_conf = false;
				const DevErrorList *err_ptr;
				
				
				if (callback != NULL)
				{

//
// Check if the event transmit error
//

					DeviceAttribute *dev_attr = NULL;
					CORBA::TypeCode_var ty = event.remainder_of_body.type();
					if (ty->kind() == tk_struct)
					{
						CORBA::String_var st_name;
						st_name = ty->name();
						const char *tmp_ptr = st_name.in();
						long vers;
						if (::strcmp(tmp_ptr,"AttributeValue_3") == 0)
						{
							dev_attr = new (DeviceAttribute);
							event.remainder_of_body >>= attr_value_3;
							vers = 3;
							attr_to_device(attr_value,attr_value_3,vers,dev_attr);
						}
						else if (::strcmp(tmp_ptr,"AttributeValue") == 0)
						{
							dev_attr = new (DeviceAttribute);
							event.remainder_of_body >>= attr_value;
							vers = 2;
							attr_to_device(attr_value,attr_value_3,vers,dev_attr);
						}
						else if (::strcmp(tmp_ptr,"AttributeConfig_2") == 0)
						{
							event.remainder_of_body >>= attr_conf_2;
							vers = 2;
							attr_info_ex = new AttributeInfoEx();
							*attr_info_ex = attr_conf_2;
							ev_attr_conf = true;
						}
						else if (::strcmp(tmp_ptr,"AttributeConfig_3") == 0)
						{
							event.remainder_of_body >>= attr_conf_3;
							vers = 3;
							attr_info_ex = new AttributeInfoEx();
							*attr_info_ex = attr_conf_3;
							ev_attr_conf = true;
						}
						else
						{
							errors.length(1);

							errors[0].severity = Tango::ERR;
							errors[0].origin = CORBA::string_dup("EventConsumer::push_structured_event()");
							errors[0].reason = CORBA::string_dup("API_IncompatibleAttrDataType");
							errors[0].desc = CORBA::string_dup("Unknown structure used to pass attribute value (Need compilation ?)");
							dev_attr = NULL;						
						}

					}
					else
					{
						event.remainder_of_body >>= err_ptr;
						errors = *err_ptr;

//
// We need to find which type of event we have received
//

						string::size_type pos = attr_event_name.find('.');
						string att_type = attr_event_name.substr(pos + 1);
						if (att_type == CONF_TYPE_EVENT)
							ev_attr_conf = true;
					}			

//
// Fire the user callback
//

					if (ev_attr_conf == false)
					{
						EventData *event_data = new EventData(event_callback_map[attr_event_name].device,
															  domain_name,
															  event_name,
															  dev_attr,
															  errors);
						try
						{
							callback->push_event(event_data);
						}
						catch (...)
						{
							cerr << "Tango::EventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
						}						
						
						delete dev_attr;
						event_data->attr_value = NULL;
						delete event_data;
					}
					else
					{
						AttrConfEventData *event_data = new AttrConfEventData(event_callback_map[attr_event_name].device,
															  domain_name,
															  event_name,
															  attr_info_ex,
															  errors);
						try
						{
							callback->push_event(event_data);
						}
						catch (...)
						{
							cerr << "Tango::EventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
						}
						
						delete attr_info_ex;
						event_data->attr_conf = NULL;
						delete event_data;
					}
				}

				ipos->second.callback_monitor->rel_monitor();
				
			}
			catch (...)
			{
				// free the map lock if not already done
				if ( map_lock == true )
					{
					map_modification_lock.readerOut();
					}
				
				cerr << "Tango::EventConsumer::push_structured_event() timeout on callback monitor of " << ipos->first << endl;
			}				
		}
	else
		{
		// even if nothing was found in the map, free the lock
		map_modification_lock.readerOut();
		}	
	}

}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
// 
// description : 	Method to subscribe to an event. 
//                Can be called in a stateless way, that it even works 
//                when the attribute is not available.
//
// argument : in :	device : The device handle
//			         attribute : The name of the attribute
//			             event : The type of event to subscribe for
//			          callback : A pointer to the callback object
//			           filters : Eventual event filter strings
//                stateless : Flag to enable the stateless connection when set to true
//
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   const vector<string> &filters,
					bool stateless)
{
	if ((device == NULL) || (callback == NULL))
	{		
		EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"Device or callback pointer NULL",
                       	(const char*)"EventConsumer::subscribe_event()");
	}

	string device_name = device->dev_name();
	cout3 << "Tango::EventConsumer::subscribe_event(" << device_name << "," << attribute <<"," << event << ")\n";

	string event_name;	
	switch (event) 
	{
		case CHANGE_EVENT : event_name = "change";
				    break;

		case QUALITY_EVENT : event_name = "quality";
					  EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"The quality change event does`nt exist any more. A change event is fired on a qaulity change!",
                       	(const char*)"EventConsumer::subscribe_event()");
				    
					  break;

		case PERIODIC_EVENT : event_name = "periodic";
				      break;

		case ARCHIVE_EVENT : event_name = "archive";
				     break;
				     
		case USER_EVENT : event_name = "user_event";
				  break;
				  
		case ATTR_CONF_EVENT : event_name = "attr_conf";
					      break;
	}

	//
	// Build callback map key
	//
	
	string att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	string callback_key = device_name + "/" + att_name_lower + "." + event_name;

	// only reading from the maps
	{
		ReaderLock r (map_modification_lock);
		
		if (event_callback_map.find(callback_key) != event_callback_map.end())
		{
			map_modification_lock.readerOut();
		
			TangoSys_OMemStream o;

			o << "Already connected to event " << callback_key << ends;
			EventSystemExcept::throw_exception((const char*)"API_MethodArgument",
                       	o.str(),
                       	(const char*)"EventConsumer::subscribe_event()");
		}
	}
				
	try
	{
		int event_id = connect_event (device, attribute, event, callback, 
		                             filters, event_name);
		return event_id;		
	}
		
	catch (Tango::DevFailed &e)
	{
		// if the stateless flag is not true, rethrow the exception
		if ( stateless == false )
		{
			throw;
		}
		
		// when the subscribe event has not worked, store the connection data in a vector of not
		// yet connected events.
		// Retry to connect in the next heartbeat period.
		
		EventNotConnected conn_params;
		conn_params.device     = device;
		conn_params.attribute  = attribute;
		conn_params.event_type = event;
		conn_params.event_name = event_name;
		conn_params.callback   = callback;
		conn_params.filters    = filters;
		conn_params.last_heartbeat = time(NULL);
		
		
		// protect the vector as the other maps!
		WriterLock w(map_modification_lock);
		
		// create and save the unique event ID
		subscribe_event_id++;	
		conn_params.event_id = subscribe_event_id;
	
		event_not_connected.push_back (conn_params);	
		return subscribe_event_id;
	}
}





//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::connect_event()
// 
// description : 	Method to connect to an event and to
//                return an approriate exception if the 
//                connection fails.
//
// argument : in :	device : The device handle
//			         attribute : The name of the attribute
//			             event : The type of event to subscribe for
//			          callback : A pointer to the callback object
//			           filters : Eventual event filter strings
//                event_id  : the unique event ID
//
//-----------------------------------------------------------------------------

int EventConsumer::connect_event(DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   const vector<string> &filters,
					string &event_name,
					int event_id)
{
	int ret_event_id = event_id;
	string device_name = device->dev_name();
	cout3 << "Tango::EventConsumer::connect_event(" << device_name << "," << attribute <<"," << event << ")\n";

//
// Build callback map key
//
	
	string att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	string callback_key = device_name + "/" + att_name_lower + "." + event_name;

//
// Inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is
// down !)
// To do this, we need to build DS adm device proxy. If it is not the first call for this
// DS, we should find it in map. Otherwise, get it.
//

	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(device_name);
	subscriber_info.push_back(att_name_lower);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(event_name);
	subscriber_in << subscriber_info;


	DeviceProxy *adm_dev;	
	map<std::string,std::string>::iterator ipos;
	bool allocated = false;
	string channel_name;
	
	map_modification_lock.readerIn();
	
	ipos = device_channel_map.find(device_name);
	if (ipos == device_channel_map.end())
	{
		string adm_name;
		try
		{
			adm_name = device->adm_name();
			
			if (device->get_from_env_var() == false &&
			   device->get_db_host() != NOT_USED)
			{
				string added_str = device->get_db_host();
				added_str = added_str + ':' + device->get_db_port() + '/';
				adm_name.insert(0,added_str);
			}
			adm_dev = new DeviceProxy(adm_name);
			allocated = true;
		}
		catch(...)
		{
			map_modification_lock.readerOut();
			
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Check that device server is running..." << ends;
			Except::throw_exception((const char *)"API_BadConfigurationProperty",
				       	 	o.str(),
				         	(const char *)"EventConsumer::subscribe_event()");
		}
	}
	else
	{
		channel_name = device_channel_map[device_name];
		
		channel_map[channel_name].channel_monitor->get_monitor();
		adm_dev = channel_map[channel_name].adm_device_proxy;
		channel_map[channel_name].channel_monitor->rel_monitor();
	}

	try
	{
    	adm_dev->command_inout("EventSubscriptionChange",subscriber_in);
	}
	catch (Tango::DevFailed &e)
	{
		map_modification_lock.readerOut();
		
		if (allocated == true)
			delete adm_dev;
		EventSystemExcept::re_throw_exception(e,(const char*)"API_DSFailedRegisteringEvent",
                       				(const char*)"Device server send exception while trying to register event",
                      				(const char*)"EventConsumer::subscribe_event()");
	}
	
	if (allocated == true)
		delete adm_dev;
	
//
// Search (or create) entry for channel map
//
	
	ipos = device_channel_map.find(device_name);
	if (ipos == device_channel_map.end())
	{
		// delete the readers lock and take the writer lock now.
		// The channel map will be modified.
		map_modification_lock.readerOut();
		WriterLock w(map_modification_lock);

		cout3 << "device " << device_name << " is not connected, going to connect to the event channel !\n";		
		connect(device);
		
		ipos = device_channel_map.find(device_name);
		if (ipos == device_channel_map.end())
		{
			TangoSys_OMemStream o;
			
			o << "Failed to connect to event channel for device " << device_name << ends;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       				o.str(),
                       				(const char*)"EventConsumer::subscribe_event()");
		}

		channel_name = device_channel_map[device_name];		
		{
			channel_map[channel_name].last_subscribed = time(NULL);
		}
	}
	else
	{
		map_modification_lock.readerOut();
	}

//
// Build a filter using the CORBA Notify constraint Language
// (use attribute name in lowercase letters)
//

	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;

	// take the map reader lock again
	map_modification_lock.readerIn();
	
	try
	{
		channel_map[channel_name].channel_monitor->get_monitor();
    	ffp    = channel_map[channel_name].eventChannel->default_filter_factory();  
    	filter = ffp->create_filter("EXTENDED_TCL");
		channel_map[channel_name].channel_monitor->rel_monitor();
  	}
	catch (CORBA::COMM_FAILURE &)
	{
		map_modification_lock.readerOut();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	catch (...)
	{
		map_modification_lock.readerOut();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	
//  	
// Construct a simple constraint expression; add it to fadmin
//
	
	char constraint_expr[512];
	::sprintf(constraint_expr,"$domain_name == \'%s/%s\' and $event_name == \'%s\'",
		device_name.c_str(),att_name_lower.c_str(),event_name.c_str());
		
	if (filters.size() != 0)
	{
		::strcat(&(constraint_expr[strlen(constraint_expr)])," and ((");
		for (unsigned int i = 0;i < filters.size();i++)
		{
			::strcat(&(constraint_expr[strlen(constraint_expr)]),filters[i].c_str());

			if (i != filters.size() - 1)			
				::strcat(&(constraint_expr[strlen(constraint_expr)])," and ");
		}
		::strcat(&(constraint_expr[strlen(constraint_expr)]),") or $forced_event > 0.5)");
	}

	CosNotification::EventTypeSeq evs;
  	CosNotifyFilter::ConstraintExpSeq exp;
  	exp.length(1);
  	exp[0].event_types = evs;
  	exp[0].constraint_expr = CORBA::string_dup(constraint_expr);
  	CORBA::Boolean res = 0; // OK
	
  	try
	{
		// delete the readers lock and take the writer lock now.
		// The callback map will be modified.
		map_modification_lock.readerOut();
		WriterLock w(map_modification_lock);
		
		
    	CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);
		{		
    		filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
		}

		EventCallBackStruct new_event_callback;
		new_event_callback.device = device;
		new_event_callback.attr_name = attribute;
		new_event_callback.event_name = event_name;
		new_event_callback.channel_name = channel_name;
		new_event_callback.callback = callback;
		new_event_callback.filter_id = filter_id;
		new_event_callback.filter_constraint = constraint_expr;
		
		// if an event ID was passwd to the method, reuse it!
		
		if ( ret_event_id <= 0 )
		{
			subscribe_event_id++;
			ret_event_id = subscribe_event_id;
		}
		new_event_callback.id = ret_event_id;
				
		// create a callback monitor
		new_event_callback.callback_monitor = new TangoMonitor();
		// set the timeout for the callback monitor to 500ms not to block the event consumer for to long.
		new_event_callback.callback_monitor->timeout(500);
		
		event_callback_map[callback_key] = new_event_callback;
  	}
  	catch(CosNotifyFilter::InvalidConstraint &)
	{
    	//cerr << "Exception thrown : Invalid constraint given "
	  	//     << (const char *)constraint_expr << endl;
		res = 1;
  	}
  	catch (...)
	{
    	//cerr << "Exception thrown while adding constraint " 
	 	//     << (const char *)constraint_expr << endl; 
		res = 1;
  	}
	
//
// If error, destroy filter. Else, set the filter_ok flag to true
//
	
  	if (res == 1)
	{ 
    	try
		{
      		filter->destroy();
    	}
		catch (...) { }
		
    	filter = CosNotifyFilter::Filter::_nil();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	else
	{
		ReaderLock r(map_modification_lock);
		event_callback_map[callback_key].callback_monitor->get_monitor();
		event_callback_map[callback_key].filter_ok = true;
		event_callback_map[callback_key].callback_monitor->rel_monitor();
	}
	
//
// Read the attribute by a simple synchronous call
// This is necessary for the first point in "change" mode
// Force callback execution when it is done
//

	if ((event == CHANGE_EVENT) ||
	    (event == QUALITY_EVENT) || 
	    (event == ARCHIVE_EVENT) ||
	    (event == USER_EVENT))
	{
		ReaderLock r(map_modification_lock);
		
		DeviceAttribute da;
		DevErrorList err;
		err.length(0);
		string domain_name = device_name + "/" + att_name_lower;

		try
		{
			da = device->read_attribute(attribute.c_str());
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}
				
		EventData *event_data = new EventData(device,
						      domain_name,
						      event_name,
						      &da,
						      err);
		{
			event_callback_map[callback_key].callback_monitor->get_monitor();
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
			}
			event_callback_map[callback_key].callback_monitor->rel_monitor();			
		}
		event_data->attr_value = NULL;
		delete event_data;				
	}
	else if (event == ATTR_CONF_EVENT)
	{
		ReaderLock r(map_modification_lock);
		
		DevErrorList err;
		err.length(0);
		string domain_name = device_name + "/" + att_name_lower;
		AttributeInfoEx aie;

		try
		{
			aie = device->get_attribute_config(const_cast<string &>(attribute));
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}
		
		AttrConfEventData *event_data = new AttrConfEventData(device,
						      domain_name,
						      event_name,
						      &aie,
						      err);
		{
			event_callback_map[callback_key].callback_monitor->get_monitor();
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
			}
			event_callback_map[callback_key].callback_monitor->rel_monitor();			
		}
		event_data->attr_conf = NULL;
		delete event_data;
	}

	return ret_event_id;
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::unsubscribe_event()
// 
// description : 	Method to unsubscribe from an event
//
// argument : in :	event_id : The event identifier
//
//-----------------------------------------------------------------------------

void EventConsumer::unsubscribe_event(int event_id)
{
	
	WriterLock w(map_modification_lock);

//
// First remove the callback entry from the callback map
//
	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			//cout << "Tango::EventConsumer::unsubscribe_event() - found event id " << event_id << " going to remove_filter()\n";
			try
			{
				CosNotifyFilter::Filter_var f = channel_map[epos->second.channel_name].structuredProxyPushSupplier->get_filter(epos->second.filter_id);
				channel_map[epos->second.channel_name].structuredProxyPushSupplier->remove_filter(epos->second.filter_id);
				f->destroy();
			}
			catch (...)
			{
				EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
					(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() (hint: check the Notification daemon is running ",
					(const char*)"EventConsumer::unsubscribe_event()");    
			}

			string deleted_channel_name = epos->second.channel_name;			
			event_callback_map.erase(epos);

//
// Check if there is another callback using the same channel
//
			
			std::map<std::string,EventCallBackStruct>::iterator cb_pos;
			bool channel_used_elsewhere = false;
			for (cb_pos = event_callback_map.begin(); cb_pos != event_callback_map.end(); ++cb_pos)
			{
				if (cb_pos->second.channel_name == deleted_channel_name)
				{
					channel_used_elsewhere = true;
					break;
				}
			}

//
// This channel is not used anymore in the app, remove its entry in the channel maps
//
			
			if (channel_used_elsewhere == false)
			{
				std::map<std::string,EventChannelStruct>::iterator chan_pos;
				for (chan_pos = channel_map.begin(); chan_pos != channel_map.end(); ++chan_pos)
				{
					if (chan_pos->first == deleted_channel_name)
					{
						if (chan_pos->second.adm_device_proxy != NULL)
						{
							try
							{
								CosNotifyFilter::Filter_var f = chan_pos->second.structuredProxyPushSupplier->get_filter(chan_pos->second.heartbeat_filter_id);
								chan_pos->second.structuredProxyPushSupplier->remove_filter(chan_pos->second.heartbeat_filter_id);
								f->destroy();

								chan_pos->second.structuredProxyPushSupplier->disconnect_structured_push_supplier();
							}
							catch (...)
							{
								EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
									(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() on the heartbeat filter (hint: check the Notification daemon is running ",
									(const char*)"EventConsumer::unsubscribe_event()");    
							}
							
							delete chan_pos->second.adm_device_proxy;
							if (chan_pos->second.channel_monitor != NULL)
								delete chan_pos->second.channel_monitor;
						}
			
						channel_map.erase(chan_pos);
						break;
					}
				}
				
				std::map<std::string,std::string>::iterator dev_pos;
				for (dev_pos = device_channel_map.begin(); dev_pos != device_channel_map.end(); ++dev_pos)
				{
					if (dev_pos->second == deleted_channel_name)
					{
						device_channel_map.erase(dev_pos);
						break;
					}
				}
			}
			
			return;
		}
	}
	
	// check also the vector of not yet connected events
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
			   // delete element from vector when subscribe worked
				
			   event_not_connected.erase(vpos);
			   return;
			}
		}	
	}
		
	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");    
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::cleanup_EventChannel_map()
// 
// description : 	Method to destroy the DeviceProxy objects
//			stored in the EventChannel map
//
//-----------------------------------------------------------------------------

void EventConsumer::cleanup_EventChannel_map()
{
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); epos++)
	{
		CosNotifyFilter::Filter_var f = channel_map[epos->second.channel_name].structuredProxyPushSupplier->get_filter(epos->second.filter_id);
		channel_map[epos->second.channel_name].structuredProxyPushSupplier->remove_filter(epos->second.filter_id);
		f->destroy();
	}
	
	std::map<std::string,EventChannelStruct>::iterator ipos;
	{
		for (ipos = channel_map.begin(); ipos != channel_map.end(); ipos++)
		{
			if (ipos->second.adm_device_proxy != NULL)
			{
				ipos->second.channel_monitor->get_monitor();
				
				// Destroy the filter created in the
				// notification service for the heartbeat event
				
			   	CosNotifyFilter::Filter_var f = ipos->second.structuredProxyPushSupplier->get_filter(ipos->second.heartbeat_filter_id);
				ipos->second.structuredProxyPushSupplier->remove_filter(ipos->second.heartbeat_filter_id);
				f->destroy();
				
				// disconnect the pushsupplier to stop receiving events
				//cout << "EventConsumer::cleanup_EventChannel_map(): Disconnect push supplier!" << endl;
				ipos->second.structuredProxyPushSupplier->disconnect_structured_push_supplier();
							
				// Release the connection to the device server administration device
				
				delete ipos->second.adm_device_proxy;
				ipos->second.adm_device_proxy = NULL;
				
				ipos->second.channel_monitor->rel_monitor();
			}
		}
	}
}


/************************************************************************/
/*		       															*/		
/* 			EventData class 											*/
/*			---------------												*/
/*		       															*/
/************************************************************************/


//+----------------------------------------------------------------------
//
// 	EventData copy constructor
//
//-----------------------------------------------------------------------

EventData::EventData(const EventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	attr_value = new (DeviceAttribute);
	*attr_value = *(sou.attr_value);
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	EventData assignement operator
//
//-----------------------------------------------------------------------

EventData & EventData::operator=(const EventData &ri)
{
	if (&ri == this)
		return *this;
		
	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	attr_value = new (DeviceAttribute);
	*attr_value = *(ri.attr_value);
	err = ri.err;
	errors = ri.errors;
	
	return *this;
}
	
//+----------------------------------------------------------------------
//
// 	EventData destructor
//
//-----------------------------------------------------------------------

EventData::~EventData()
{
	if (attr_value != NULL)
		delete attr_value;
}


/************************************************************************/
/*		       															*/		
/* 			AttrConfEventData class 									*/
/*			-----------------											*/
/*		       															*/
/************************************************************************/


//+----------------------------------------------------------------------
//
// 	AttrConfEventData copy constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(const AttrConfEventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	attr_conf = new (AttributeInfoEx);
	*attr_conf = *(sou.attr_conf);
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	AttrConfEventData assignement operator
//
//-----------------------------------------------------------------------

AttrConfEventData & AttrConfEventData::operator=(const AttrConfEventData &ri)
{
	if (&ri == this)
		return *this;
		
	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	attr_conf = new (AttributeInfoEx);
	*attr_conf = *(ri.attr_conf);
	err = ri.err;
	errors = ri.errors;
	
	return *this;
}
	
//+----------------------------------------------------------------------
//
// 	AttrConfEventData destructor
//
//-----------------------------------------------------------------------

AttrConfEventData::~AttrConfEventData()
{
	if (attr_conf != NULL)
		delete attr_conf;
}

	
/************************************************************************/
/*		       															*/		
/* 			EventConsumerKeepAlive class 								*/
/*			----------------------------								*/
/*		       															*/
/************************************************************************/




//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::reconnect_to_channel()
// 
// description : 	Method to reconnect the process to an event channel
//			in case of reconnection to a notifd
//
// argument : in :	ipos : An iterator to the EventChannel structure to
//			       reconnect to in the Event Channel map
//			event_consumer : Pointer to the EventConsumer
//					 singleton
//
// This method returns true if the reconnection succeeds. Otherwise, returns
// false
//
//-----------------------------------------------------------------------------

bool EventConsumerKeepAliveThread::reconnect_to_channel(EvChanIte &ipos,EventConsumer *event_consumer)
{
	bool ret = true;
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect()" << endl;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
	{
		if ((epos->second.channel_name == ipos->first) && (epos->second.callback != NULL))
		{
			try
			{
				string adm_name = ipos->second.full_adm_name;
				event_consumer->connect_event_channel(adm_name,
								      epos->second.device->get_device_db(),
								      true);
		
				if (ipos->second.adm_device_proxy != NULL)
					delete ipos->second.adm_device_proxy;
				ipos->second.adm_device_proxy = new DeviceProxy(ipos->second.full_adm_name);
				cout3 << "Reconnected to event channel" << endl;
			}
			catch(...)
			{
				ret = false;
			}
			
			break;
		}
	}

	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::reconnect_to_event()
// 
// description : 	Method to reconnect each event associated to a specific
//			event channel to the just reconnected event channel
//
// argument : in :	ipos : An iterator to the EventChannel structure in the 
//			       Event Channel map
//			event_consumer : Pointer to the EventConsumer
//					 singleton
//
//-----------------------------------------------------------------------------

void EventConsumerKeepAliveThread::reconnect_to_event(EvChanIte &ipos,EventConsumer *event_consumer)
{
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect_to_event()" << endl;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
	{
		if ((epos->second.channel_name == ipos->first) && (epos->second.callback != NULL))
		{
			try
			{
				epos->second.callback_monitor->get_monitor();
			
				try
				{
					re_subscribe_event(epos,ipos);
					epos->second.filter_ok = true;
					cout3 << "Reconnected to event" << endl;
				}
				catch(...)
				{
					epos->second.filter_ok = false;
				}
			
				epos->second.callback_monitor->rel_monitor();
			}
			catch (...)
			{
				cerr << "EventConsumerKeepAliveThread::reconnect_to_event() cannot get callback monitor for " << epos->first << endl;
			}
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::re_subscribe_event()
// 
// description : 	Method to reconnect a specific event to an
//			event channel just reconnected
//
// argument : in :	epos : An iterator to the EventCallback structure in the 
//			       Event Callback map
//			ipos : Pointer to the EventChannel structure in the
//			       Event Channel map
//
//-----------------------------------------------------------------------------

void EventConsumerKeepAliveThread::re_subscribe_event(EvCbIte &epos,EvChanIte &ipos)
{
//
// Build a filter using the CORBA Notify constraint Language
// (use attribute name in lowercase letters)
//

	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;

	string channel_name = epos->second.channel_name;	
	try
	{
   		ffp    = ipos->second.eventChannel->default_filter_factory();  
   		filter = ffp->create_filter("EXTENDED_TCL");
  	}
	catch (CORBA::COMM_FAILURE &)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");        
  	}
	catch (...)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");        
  	}
	
//  	
// Construct a simple constraint expression; add it to fadmin
//

	string constraint_expr = epos->second.filter_constraint;

	CosNotification::EventTypeSeq evs;
  	CosNotifyFilter::ConstraintExpSeq exp;
  	exp.length(1);
  	exp[0].event_types = evs;
  	exp[0].constraint_expr = CORBA::string_dup(constraint_expr.c_str());
  	CORBA::Boolean res = 0; // OK
  	try
	{
    	CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);

    	filter_id = ipos->second.structuredProxyPushSupplier->add_filter(filter);

		epos->second.filter_id = filter_id;
  	}
  	catch(CosNotifyFilter::InvalidConstraint &)
	{
    	//cerr << "Exception thrown : Invalid constraint given "
	  	//     << (const char *)constraint_expr << endl;
		res = 1;
  	}
  	catch (...)
	{
    	//cerr << "Exception thrown while adding constraint " 
	 	//     << (const char *)constraint_expr << endl; 
		res = 1;
  	}
	
//
// If error, destroy filter
//
	
  	if (res == 1)
	{ 
    	try
		{
      		filter->destroy();
    	}
		catch (...) { }
		
    	filter = CosNotifyFilter::Filter::_nil();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");        
  	}
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::run_undetached
// 
// description : 	The main code of the KeepAliveThread
//
//-----------------------------------------------------------------------------

void *EventConsumerKeepAliveThread::run_undetached(void *arg) 
{	
	int time_to_sleep;
	time_t now;
	EventConsumer *event_consumer;

//
// first sleep 2 seconds to give the event system time to startup
//

#ifndef _TG_WINDOWS_
	unsigned int time_left;
	time_left = ::sleep(2);
	if (time_left != 0)
		::sleep(time_left);
#else
	Sleep(2000L);
#endif /* _TG_WINDOWS_ */

	bool exit_th = false;
	event_consumer = ApiUtil::instance()->get_event_consumer();
	while (exit_th == false)
	{
		time_to_sleep = EVENT_HEARTBEAT_PERIOD;
		
//
// go to sleep until next heartbeat
// Wait on a monitor. This allows another thread to wake-up this thread
// before the end of the EVENT_HEARTBEAT_PERIOD time which is 10 seconds
// Only one command can now be send to the thread. It is a stop command
//

		{
			omni_mutex_lock sync(shared_cmd);
			if (shared_cmd.cmd_pending == false)
			{
				unsigned long s,n;

				unsigned long nb_sec,nb_nanos;
				nb_sec = time_to_sleep ;
				nb_nanos = 0;

				omni_thread::get_time(&s,&n,nb_sec,nb_nanos);
				shared_cmd.cond.timedwait(s,n);
			}
			if (shared_cmd.cmd_pending == true)
				exit_th = true;
		}
		
//
// Re-subscribe
//
 
		// lock the maps only for reading
		event_consumer->map_modification_lock.readerIn();
			
		now = time(NULL);
			
		//
		// check the list of not yet connected events and try to subscribe
		//
			
		if ( event_consumer->event_not_connected.size() > 0 )
			{
			std::vector<EventNotConnected>::iterator vpos;
			for (vpos = event_consumer->event_not_connected.begin(); 
				 vpos != event_consumer->event_not_connected.end();
				 vpos++ )
				{
				
				// check wether it is necessary to try to subscribe again!
				if ( (now - vpos->last_heartbeat) >= (EVENT_HEARTBEAT_PERIOD) )
					{
					try
						{
						// try to subscribe
						
						event_consumer->map_modification_lock.readerOut();
						event_consumer->connect_event (vpos->device,vpos->attribute,vpos->event_type, 
																					vpos->callback, 
																					vpos->filters,
																					vpos->event_name,
																					vpos->event_id);	
						event_consumer->map_modification_lock.readerIn();
							
						// delete element from vector when subscribe worked
						vpos = event_consumer->event_not_connected.erase(vpos);
						if ( vpos == event_consumer->event_not_connected.end() )
							{
							break;
							}
						}
		
					catch (Tango::DevFailed &e) 
						{
						event_consumer->map_modification_lock.readerIn();
						
						// subscribe has not worked, try again in the next hearbeat period
						vpos->last_heartbeat = now;
						
						// The event can still not be connected.
						// Send the return error message as event to the client application.
						//
						// push an event with the error message!
						
						DevErrorList err;
						err.length(0);
						string domain_name = vpos->device->dev_name() + "/" + vpos->attribute;
						err = e.errors;
											
						//
						// For attribute data event
						//
						
						if ((vpos->event_name == "change") ||
	    					(vpos->event_name == "quality") || 
	    					(vpos->event_name == "archive") ||
	    					(vpos->event_name == "user_event"))
							{						
							DeviceAttribute da;
							EventData *event_data = new EventData(vpos->device,
													      			domain_name,
													      			vpos->event_name,
													      			&da,
													      			err);
							try
								{
								vpos->callback->push_event(event_data);
								}
							catch (...)
								{
								cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << domain_name << endl;
								}
						
							event_data->attr_value = NULL;							
							delete event_data;						
							}
						
						//
						// For attribute configuration event
						//
							
						else if (vpos->event_name == CONF_TYPE_EVENT)
							{
							AttributeInfoEx aie;
							AttrConfEventData *event_data = new AttrConfEventData(vpos->device,
													      			domain_name,
													      			vpos->event_name,
													      			&aie,
													      			err);
							try
								{
								vpos->callback->push_event(event_data);
								}
							catch (...)
								{
								cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << domain_name << endl;
								}
							
							event_data->attr_conf = NULL;
							delete event_data;				
							}
						}
					
					catch (...)
						{
						event_consumer->map_modification_lock.readerIn();
						
						// subscribe has not worked, try again in the next hearbeat period
						vpos->last_heartbeat = now;
						
						cout << "During the event subscription an exception was send which is not a Tango::DevFailed exception!" << endl;	
						}						
					}
				}
			}
		event_consumer->map_modification_lock.readerOut();
		
		
		// check for all other event reconnections
			
		{
			// lock the maps only for reading
			ReaderLock r (event_consumer->map_modification_lock);	
			
			std::map<std::string,EventChannelStruct>::iterator ipos;
			std::map<std::string,EventCallBackStruct>::iterator epos;

			for (ipos = event_consumer->channel_map.begin(); ipos != event_consumer->channel_map.end(); ipos++)
			{
				try
				{
					// lock the event cahnnel
					ipos->second.channel_monitor->get_monitor();

					if ((now - ipos->second.last_subscribed) > EVENT_RESUBSCRIBE_PERIOD/3)
					{
						for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
						{
							if (epos->second.channel_name == ipos->first )
							{
								try
								{
									// lock the callback 
									epos->second.callback_monitor->get_monitor();

									DeviceData subscriber_in;
									vector<string> subscriber_info;
									subscriber_info.push_back(epos->second.device->dev_name());
									subscriber_info.push_back(epos->second.attr_name);
									subscriber_info.push_back("subscribe");
									subscriber_info.push_back(epos->second.event_name);
									subscriber_in << subscriber_info;
									ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);

									ipos->second.last_subscribed = time(NULL);
        							epos->second.last_subscribed = time(NULL);

									epos->second.callback_monitor->rel_monitor();
								}
								catch (...) 
								{
									epos->second.callback_monitor->rel_monitor();
								};	
							}
						}
					}

		//
		// Check if a heartbeat have been skipped
		// If a heartbeat is missing, there are four possibilities :
		// 1 - The notifd is dead (or the crate is rebooting or has already reboot)
		// 2 - The server is dead
		// 3 - The network was down;
		// 4 - The server has been restarted on another host.
		//

 					bool heartbeat_skipped;
					heartbeat_skipped = ((now - ipos->second.last_heartbeat) > (EVENT_HEARTBEAT_PERIOD + 1));
										
					if (heartbeat_skipped || ipos->second.heartbeat_skipped ||
			   		ipos->second.notifd_failed == true )
					{
						ipos->second.heartbeat_skipped = true;

		//
		// Check notifd by trying to read an attribute of the event channel
		//

						try
						{
		// 
		//  Check if the device server is now running on a different host.
		//  In this case we have to reconnect to another notification daemon.
		//						 
							DeviceInfo info;
							try
							{
								info = ipos->second.adm_device_proxy->info();
							}
							catch (Tango::DevFailed &e)
							{
								// in case of failure, just stay connected to the actual notifd
								info.server_host = ipos->second.notifyd_host;	
							}

							if ( ipos->second.notifyd_host != info.server_host )
							{
								ipos->second.notifd_failed = true;
							}
							else
								CosNotifyChannelAdmin::EventChannelFactory_var ecf = ipos->second.eventChannel->MyFactory();
						}
						catch (...)
						{
							ipos->second.notifd_failed = true;
							cout3 << "Notifd is dead !!!" << endl;
						}

		
						// if the connection to the notify daemon is marked as ok, the device server is working fine but
						// the heartbeat is still not coming back since three periods:
						// The notify deamon might have closed the connection, try to reconnect!
						 
						if ( ipos->second.notifd_failed == false &&
						     ipos->second.has_notifd_closed_the_connection >= 3 )
						{
							ipos->second.notifd_failed = true;
						}
		
		//
		// Re-build connection to the event channel
		// This is a two steps process. First, reconnect
		// to the new event channel, then reconnect
		// callbacks to this new event channel
		//

						if ( ipos->second.notifd_failed == true )
						{
							bool notifd_reco = reconnect_to_channel(ipos,event_consumer);
							if ( notifd_reco )
								ipos->second.notifd_failed = false;
							else
								ipos->second.notifd_failed = true;

							if ( ipos->second.notifd_failed == false ) 
							{
								reconnect_to_event(ipos,event_consumer);
							}
						}				

						Tango::DevErrorList errors(1);

						errors.length(1);
						errors[0].severity = Tango::ERR;
						errors[0].origin = CORBA::string_dup("EventConsumer::KeepAliveThread()");
						errors[0].reason = CORBA::string_dup("API_EventTimeout");
						errors[0].desc = CORBA::string_dup("Event channel is not responding anymore, maybe the server or event system is down");
						DeviceAttribute *dev_attr = NULL;
						AttributeInfoEx *dev_attr_conf = NULL;

						for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
						{
							if ((epos->second.channel_name == ipos->first) && (epos->second.callback != NULL))
							{
								// lock the callback
								try
								{
									epos->second.callback_monitor->get_monitor();

									if (epos->second.filter_ok == false)
									{
										try
										{
											re_subscribe_event(epos,ipos);
											epos->second.filter_ok = true;
										}
										catch(...) {}
									}

									string domain_name;
									string event_name;

									string::size_type pos = epos->first.find('.');
									if (pos == string::npos)
									{
										domain_name = "domain_name";
										event_name = "event_name";
									}
									else
									{
										domain_name = epos->first.substr(0,pos);
										event_name = epos->first.substr(pos + 1);
									}						

									CallBack *callback;
									callback = epos->second.callback;

			//
			// Push an event with error set
			//

									if (event_name == CONF_TYPE_EVENT)
									{
										AttrConfEventData *event_data = new AttrConfEventData(epos->second.device,
										      												domain_name,
										      												event_name,
										      												dev_attr_conf,
																								errors);
										try
										{
											callback->push_event(event_data);
										}
										catch (...)
										{
											cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
										}
										delete event_data;
									}
									else
									{
										EventData *event_data = new EventData(epos->second.device,
										      			domain_name,
										      			event_name,
										      			dev_attr,
										      			errors);
										try
										{
											callback->push_event(event_data);
										}
										catch (...)
										{
											cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
										}
										delete event_data;
									}

									if ( ipos->second.notifd_failed == false )
									{
										DeviceData subscriber_in;
       									vector<string> subscriber_info;
       									subscriber_info.push_back(epos->second.device->dev_name());
       									subscriber_info.push_back(epos->second.attr_name);
       									subscriber_info.push_back("subscribe");
       									subscriber_info.push_back(epos->second.event_name);
       									subscriber_in << subscriber_info;

										bool ds_failed = false;

										try 
										{
											ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
											ipos->second.heartbeat_skipped = false;
        									ipos->second.last_subscribed = time(NULL);
										}
										catch (...) {ds_failed = true;}

										if (ds_failed == false)
										{

			//
			// Push an event with the value just read from the
			// re-connected server
			//

											if ((epos->second.event_name == "change") ||
	    							   			 (epos->second.event_name == "quality") || 
	    							   			 (epos->second.event_name == "archive") ||
	    							   			 (epos->second.event_name == "user_event"))
											{

			//
			// For attribute data event
			//

												DeviceAttribute da;
												DevErrorList err;
												err.length(0);
												string domain_name = epos->second.device->dev_name() + "/" + epos->second.attr_name;

												bool old_transp = epos->second.device->get_transparency_reconnection();
												epos->second.device->set_transparency_reconnection(true);

												try
												{
													da = epos->second.device->read_attribute(epos->second.attr_name.c_str());
													
													// The reconnection worked fine. The heartbeat should come back now,
													// when the notifd has not closed the connection.
													// Increase the counter to detect when the heartbeat is not coming back.
													ipos->second.has_notifd_closed_the_connection++;
												}
												catch (DevFailed &e)
												{
													err = e.errors;
												}
												epos->second.device->set_transparency_reconnection(old_transp);


												EventData *event_data = new EventData(epos->second.device,
													      			domain_name,
													      			epos->second.event_name,
													      			&da,
													      			err);
												try
												{
													callback->push_event(event_data);
												}
												catch (...)
												{
													cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
												}
												event_data->attr_value = NULL;
												delete event_data;				
											}
											else if (epos->second.event_name == CONF_TYPE_EVENT)
											{

			//
			// For attribute configuration event
			//

												AttributeInfoEx aie;
												DevErrorList err;
												err.length(0);
												string domain_name = epos->second.device->dev_name() + "/" + epos->second.attr_name;

												bool old_transp = epos->second.device->get_transparency_reconnection();
												epos->second.device->set_transparency_reconnection(true);

												try
												{
													aie = epos->second.device->get_attribute_config(epos->second.attr_name);
													
													// The reconnection worked fine. The heartbeat should come back now,
													// when the notifd has not closed the connection.
													// Increase the counter to detect when the heartbeat is not coming back.
													ipos->second.has_notifd_closed_the_connection++;		
												}
												catch (DevFailed &e)
												{
													err = e.errors;
												}
												epos->second.device->set_transparency_reconnection(old_transp);


												AttrConfEventData *event_data = new AttrConfEventData(epos->second.device,
													      			domain_name,
													      			epos->second.event_name,
													      			&aie,
													      			err);

												try
												{
													callback->push_event(event_data);
												}
												catch (...)
												{
													cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
												}
												event_data->attr_conf = NULL;
												delete event_data;				
											}
										}
									}
									// release callback monitor
									epos->second.callback_monitor->rel_monitor();
								}
								catch (...)
								{
									cerr << "EventConsumerKeepAliveThread::run_undetached() timeout on callback monitor of " << epos->first << endl;
								}	
							}
						}
					}
					else
					{
						// When the heartbeat has worked, mark the connection to the notifd a OK
						ipos->second.has_notifd_closed_the_connection = 0;
					}

					// release channel monitor
					ipos->second.channel_monitor->rel_monitor();
				}
				catch (...)
				{
					cerr << "EventConsumerKeepAliveThread::run_undetached() timeout on callback monitor of " << epos->first << endl;
				}			
			}
		}
	}
	
//
// If we arrive here, this means that we have received the exit thread
// command.
//

	return (void *)NULL;
		
}
	
} /* End of Tango namespace */

