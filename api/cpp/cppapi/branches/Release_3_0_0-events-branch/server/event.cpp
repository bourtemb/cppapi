 const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
/// \file 	event.cpp
///
///		C++ class for implementing the event server and client
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
///		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>
#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>
#include <event.h>
#include <stdio.h>
#ifdef WIN32
#include <sys/timeb.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace CORBA;

namespace Tango {

EventSupplier *EventSupplier::_instance = NULL;

EventSupplier::EventSupplier(CORBA::ORB_var _orb,
	CosNotifyChannelAdmin::SupplierAdmin_var _supplierAdmin,
	CosNotifyChannelAdmin::ProxyID _proxId,
	CosNotifyChannelAdmin::ProxyConsumer_var _proxyConsumer,
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var _structuredProxyPushConsumer,
	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory,
	CosNotifyChannelAdmin::EventChannel_var _eventChannel)
{
	orb_ = _orb;
	supplierAdmin = _supplierAdmin;
	proxyId = _proxId;
	proxyConsumer = _proxyConsumer;
	structuredProxyPushConsumer = _structuredProxyPushConsumer;
	eventChannelFactory = _eventChannelFactory;
	eventChannel = _eventChannel;

	_instance = this;
}

EventSupplier *EventSupplier::create(CORBA::ORB_var _orb, string server_name, Database *db) 
{
	cout1 << "calling Tango::EventSupplier::create() \n";

// does the EventSupplier singleton exist already ? if so simply return it

	if (_instance != NULL)
	{
		return _instance;
	}

// EventSupplier singleton does not exist, create it

	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory;
	CosNotifyChannelAdmin::EventChannel_var _eventChannel;

//	Tango::Util *tango_util = Tango::Util::instance();
//
// Get a reference to the Notification Service EventChannelFactory from
// the TANGO database
//
	char buffer[256];
	string host_name, factory_name;
	
	gethostname(buffer,sizeof(buffer));
	host_name = buffer;
	factory_name = "notifd/factory/"+host_name;

        CORBA::Any send;
	CORBA::Any_var received;

        send <<= factory_name.c_str();
	try 
	{
        	received = db->command_inout("DbImportEvent",send);
	}
	catch (...) 
	{
		cerr << factory_name << " is not an EventChannelFactory " << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to import the EventChannelFactory from the Tango database",
			(const char*)"EventSupplier::create()");    
	}
 
        const DevVarLongStringArray *dev_import_list;
        received >>= dev_import_list;
 
	string factory_ior;
	int factory_exported;

        factory_ior = string((dev_import_list->svalue)[1]);
        factory_exported = dev_import_list->lvalue[0];

	try
	{
		CORBA::Object *event_factory_obj;
    		event_factory_obj = _orb -> string_to_object(factory_ior.c_str());
#ifndef WIN32
    		if (event_factory_obj -> _non_existent())
			event_factory_obj = CORBA::Object::_nil();
#endif /* WIN32 */

//
// Narrow the CORBA_Object reference to an EventChannelFactory
// reference so we can invoke its methods
//
		_eventChannelFactory =
	    	CosNotifyChannelAdmin::EventChannelFactory::_narrow(event_factory_obj);

//
// Make sure the CORBA object was really an EventChannelFactory
//
		if(CORBA::is_nil(_eventChannelFactory))
		{
			cerr << factory_name << " is not an EventChannelFactory " << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to import the EventChannelFactory from the Tango database",
				(const char*)"EventSupplier::create()");    
		}
	}
	catch (...)
	{
		cerr << "Failed to narrow the EventChannelFactory - events will not be generated (hint: start the notifd daemon on this host)" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to narrow the EventChannelFactory, make sure the notifd process is running on this host",
			(const char*)"EventSupplier::create()");    
	}


//
// Get a reference to an EventChannel for this device server from the 
// TANGO database
//
	int channel_exported=-1;
	string channel_ior, channel_host;
	server_name = "DServer/"+server_name;

        send <<= server_name.c_str();
	try 
	{
        	received = db->command_inout("DbImportEvent",send);
	}
	catch (...) 
	{
		cerr << server_name << " has no event channel defined in the database - try to create it " << endl;
		channel_exported = 0;
	}
	if (channel_exported != 0)
	{
        	received >>= dev_import_list;
        	channel_ior = string((dev_import_list->svalue)[1]);
        	channel_exported = dev_import_list->lvalue[0];
//
// check if the channel is exported on this host, if not assume it
// is an old channel and we need to recreate it on the local host
//
        	channel_host = string((dev_import_list->svalue)[3]);
		if (channel_host !=  host_name) channel_exported = 0;
	}

	if (channel_exported)
	{
        	CORBA::Object *event_channel_obj;
        	event_channel_obj = _orb -> string_to_object(channel_ior.c_str());

		try
		{
        		if (event_channel_obj -> _non_existent())
                		event_channel_obj = CORBA::Object::_nil();
	
			_eventChannel = CosNotifyChannelAdmin::EventChannel::_nil();
 			_eventChannel = CosNotifyChannelAdmin::EventChannel::_narrow(event_channel_obj);

			if(CORBA::is_nil(_eventChannel))
			{
				channel_exported = 0;
			}
		}
		catch (...)
		{
			cout1 << "caught exception while trying to test event_channel object\n";
			channel_exported = 0;
		}
	}
//
// The device server event channel does not exist, let's create a new one
//
	if (!channel_exported)
	{
		CosNotification::QoSProperties initialQoS;
		CosNotification::AdminProperties initialAdmin;
		CosNotifyChannelAdmin::ChannelID channelId;

		try
		{
	    		_eventChannel = _eventChannelFactory -> create_channel(initialQoS,
						      initialAdmin,
						      channelId);
			cout1 << "Tango::EventSupplier::create() channel for server " << server_name << " created\n";
			char *_ior = _orb->object_to_string(_eventChannel);
			string ior_string(_ior);

        		Tango::DevVarStringArray *dev_export_list = new Tango::DevVarStringArray;
        		dev_export_list->length(5);
        		(*dev_export_list)[0] = CORBA::string_dup(server_name.c_str());
        		(*dev_export_list)[1] = CORBA::string_dup(ior_string.c_str());
        		(*dev_export_list)[2] = CORBA::string_dup(host_name.c_str());
        		ostringstream ostream;
        		ostream << getpid() << ends;
        		(*dev_export_list)[3] = CORBA::string_dup(ostream.str().c_str());
        		(*dev_export_list)[4] = CORBA::string_dup("1");
        		send <<= dev_export_list;
                	received = db->command_inout("DbExportEvent",send);
                	cout1 << "successfully  exported event channel to Tango database !\n";
		}
		catch(const CosNotification::UnsupportedQoS&)
		{
			cerr << "Failed to create event channel - events will not be generated (hint: start the notifd daemon on this host)" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to create a new EventChannel, make sure the notifd process is running on this host",
				(const char*)"EventSupplier::create()");    
		}
		catch(const CosNotification::UnsupportedAdmin&)
		{
			cerr << "Failed to create event channel - events will not be generated (hint: start the notifd daemon on this host)" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to create a new EventChannel, make sure the notifd process is running on this host",
				(const char*)"EventSupplier::create()");    
		}
	}
	else
	{
		cout1 << "Tango::EventSupplier::create(): _narrow worked, use this event channel\n";
	}

//
// Obtain a Supplier Admin
//

//
// We'll use the channel's default Supplier admin
//
        CosNotifyChannelAdmin::SupplierAdmin_var 
		_supplierAdmin = _eventChannel -> default_supplier_admin();
	if (CORBA::is_nil(_supplierAdmin))
	{
        	cerr << "Could not get CosNotifyChannelAdmin::SupplierAdmin" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to get the default supplier admin from the notification daemon (hint: make sure the notifd process is running on this host)",
			(const char*)"EventSupplier::create()");    
    	}

//
// Obtain a Proxy Consumer
//

//
// We are using the "Push" model and Structured data
//
	CosNotifyChannelAdmin::ProxyID _proxyId;
	CosNotifyChannelAdmin::ProxyConsumer_var _proxyConsumer;
	try
	{
	        _proxyConsumer =
		_supplierAdmin -> obtain_notification_push_consumer(
		    CosNotifyChannelAdmin::STRUCTURED_EVENT, _proxyId);
		if (CORBA::is_nil(_proxyConsumer))
		{
        		cerr << "Could not get CosNotifyChannelAdmin::ProxyConsumer" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to obtain a Notification push consumer, make sure the notifd process is running on this host",
				(const char*)"EventSupplier::create()");    
    		}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		cerr << "Failed to get push consumer from notification daemon - events will not be generated (hint: start the notifd daemon on this host)" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to get push consumer from notification daemon (hint: make sure the notifd process is running on this host)",
			(const char*)"EventSupplier::create()");    
	}

	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var
            _structuredProxyPushConsumer =
		CosNotifyChannelAdmin::StructuredProxyPushConsumer::_narrow(
		    _proxyConsumer);
	if (CORBA::is_nil(_structuredProxyPushConsumer))
	{
       		cerr << "Tango::EventSupplier::create() could not get CosNotifyChannelAdmin::StructuredProxyPushConsumer" << endl;
    	}

	EventSupplier *_event_supplier =
		new EventSupplier(_orb, _supplierAdmin, _proxyId, _proxyConsumer, _structuredProxyPushConsumer, _eventChannelFactory, _eventChannel);
	
	return _event_supplier;
}

void EventSupplier::connect() {
//
// Connect to the Proxy Consumer
//
	try
	{
	    structuredProxyPushConsumer -> connect_structured_push_supplier(_this());
	}
	catch(const CosEventChannelAdmin::AlreadyConnected&)
	{
       		cerr << "Tango::EventSupplier::connect() caught AlreadyConnected exception" << endl;
	}

}

void EventSupplier::disconnect_structured_push_supplier() {

	cout1 << "calling Tango::EventSupplier::disconnect_structured_push_supplier() \n";
}

void EventSupplier::subscription_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled) {

	cout1 << "calling Tango::EventSupplier::subscription_change() \n";
}

void EventSupplier::detect_and_push_events(DeviceImpl *device_impl, AttributeValue &attr_value)
{
	string event, domain_name, attr_name;
	int now, change_subscription, quality_subscription, periodic_subscription, archive_subscription;

	cout2 << "EventSupplier::detect_and_push_events(): called for attribute " << attr_value.name << endl;

	attr_name = attr_value.name;
	Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

	now = time(NULL);
	change_subscription = now-attr.ext->event_change_subscription;
	quality_subscription = now-attr.ext->event_quality_subscription;
	periodic_subscription = now-attr.ext->event_periodic_subscription;
	archive_subscription = now-attr.ext->event_archive_subscription;

	cout2 << "EventSupplier::detect_and_push_events(): last subscription for change " << change_subscription << " quality " << quality_subscription << " periodic " << periodic_subscription << " archive " << archive_subscription << endl;


	if (change_subscription < 600)
	{
		detect_and_push_change_event(device_impl, attr_value, attr, attr_name);
	}
	if (quality_subscription < 600)
	{
		detect_and_push_quality_change_event(device_impl, attr_value, attr, attr_name);
	}
	if (periodic_subscription < 600)
	{
		detect_and_push_periodic_event(device_impl, attr_value, attr, attr_name);
	}
	if (archive_subscription < 600)
	{
		detect_and_push_archive_event(device_impl, attr_value, attr, attr_name);
	}
}


bool EventSupplier::detect_change(Attribute &attr, AttributeValue &curr_attr_value, bool archive, double &delta_change_rel, double &delta_change_abs)
{
	const DevVarLongArray *curr_seq_lo, *prev_seq_lo;
	const DevVarShortArray *curr_seq_sh, *prev_seq_sh;
	const DevVarDoubleArray *curr_seq_db, *prev_seq_db;
	const DevVarStringArray *curr_seq_str, *prev_seq_str;
	CORBA::TypeCode_var ty = curr_attr_value.value.type();
	CORBA::TypeCode_var ty_alias = ty->content_type();
	CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
	bool is_change=false;
	double rel_change[2], abs_change[2];
	int i;

	delta_change_rel = delta_change_abs = 0;

	if (!archive)
	{
		rel_change[0] = attr.ext->rel_change[0];
		rel_change[1] = attr.ext->rel_change[1];
		abs_change[0] = attr.ext->abs_change[0];
		abs_change[1] = attr.ext->abs_change[1];
	}
	else
	{
		rel_change[0] = attr.ext->archive_rel_change[0];
		rel_change[1] = attr.ext->archive_rel_change[1];
		abs_change[0] = attr.ext->archive_abs_change[0];
		abs_change[1] = attr.ext->archive_abs_change[1];
	}
	if (attr.ext->prev_change_event.inited)
	{
		if (attr.ext->prev_change_event.attr.quality != Tango::ATTR_INVALID &&
				curr_attr_value.quality != Tango::ATTR_INVALID)
		{
			switch (ty_seq->kind())
			{
			case CORBA::tk_long:		
				curr_attr_value.value >>= curr_seq_lo;
				attr.ext->prev_change_event.attr.value >>= prev_seq_lo;
				for (i=0; i<curr_seq_lo->length(); i++)
				{
					if (rel_change[0] != INT_MAX)
					{
						if ((*prev_seq_lo)[i] != 0)
						{
							delta_change_rel = ((*curr_seq_lo)[i] - (*prev_seq_lo)[i])*100/(*prev_seq_lo)[i];
						}
						else
						{
							delta_change_rel = 100;
							if ((*curr_seq_lo)[i] == (*prev_seq_lo)[i]) delta_change_rel = 0;
						}
						if (delta_change_rel < rel_change[0] || delta_change_rel > rel_change[1])
						{
							is_change = true;
							return(is_change);
						}
					}
					if (attr.ext->abs_change[0] != INT_MAX)
					{
						delta_change_abs = (*curr_seq_lo)[i] - (*prev_seq_lo)[i];
						if (delta_change_abs < abs_change[0] || delta_change_abs > abs_change[1])
						{
							is_change = true;
							return(is_change);
						}
					}
				}
				break;
		
			case CORBA::tk_short:
				curr_attr_value.value >>= curr_seq_sh;
				attr.ext->prev_change_event.attr.value >>= prev_seq_sh;
				for (i=0; i<curr_seq_sh->length(); i++)
				{
					if (rel_change[0] != INT_MAX)
					{
						if ((*prev_seq_sh)[i] != 0)
						{
							delta_change_rel = ((*curr_seq_sh)[i] - (*prev_seq_sh)[i])*100/(*prev_seq_sh)[i];
						}
						else
						{
							delta_change_rel = 100;
							if ((*curr_seq_sh)[i] == (*prev_seq_sh)[i]) delta_change_rel = 0;
						}
						if (delta_change_rel < rel_change[0] || delta_change_rel > rel_change[1])
						{
							is_change = true;
							return(is_change);
						}
					}
					if (abs_change[0] != INT_MAX)
					{
						delta_change_abs = (*curr_seq_sh)[i] - (*prev_seq_sh)[i];
						if (delta_change_abs < abs_change[0] || delta_change_abs > abs_change[1])
						{
							is_change = true;
							return(is_change);
						}
					}
				}
				break;
		
			case CORBA::tk_double:
				curr_attr_value.value >>= curr_seq_db;
				attr.ext->prev_change_event.attr.value >>= prev_seq_db;
				for (i=0; i<curr_seq_db->length(); i++)
				{
					if (rel_change[0] != INT_MAX)
					{
						if ((*prev_seq_db)[i] != 0)
						{
							delta_change_rel = ((*curr_seq_db)[i] - (*prev_seq_db)[i])*100/(*prev_seq_db)[i];
						}
						else
						{
							delta_change_rel = 100;
							if ((*curr_seq_db)[i] == (*prev_seq_db)[i]) delta_change_rel = 0;
						}
						if (delta_change_rel < rel_change[0] || delta_change_rel > rel_change[1])
						{
							is_change = true;
							return(is_change);
						}
					}
					if (abs_change[0] != INT_MAX)
					{
						delta_change_abs = (*curr_seq_db)[i] - (*prev_seq_db)[i];
						if (delta_change_abs < abs_change[0] || delta_change_abs > abs_change[1])
						{
							is_change = true;
							return(is_change);
						}
					}
				}
				break;
	
			case CORBA::tk_string:
				curr_attr_value.value >>= curr_seq_str;
				attr.ext->prev_change_event.attr.value >>= prev_seq_str;
				for (i=0; i<curr_seq_str->length(); i++)
				{
					if ((*curr_seq_str)[i] != (*prev_seq_str)[i])
					{
						delta_change_rel = delta_change_abs = 100.;
						is_change = true;
						return(is_change);
					}
				}
				break;
			}
		}
	}	

	return(is_change);
}

void EventSupplier::detect_and_push_change_event(DeviceImpl *device_impl, AttributeValue &attr_value, Attribute &attr, string &attr_name)
{
	string event, domain_name;
	double delta_change_rel, delta_change_abs;
	bool is_change=false;

	cout2 << "EventSupplier::detect_and_push_change_event(): called for attribute " << attr_value.name << endl;

//
// if no attribute of this name is registered with change then
// insert the current value
//
	if (!attr.ext->prev_change_event.inited)
	{
		attr.ext->prev_change_event.attr = attr_value;
		attr.ext->prev_change_event.inited = true;
	}
	else
	{
//
// determine delta_change in percent compared with previous event sent
// 
//
		is_change = detect_change(attr,attr_value,false,delta_change_rel, delta_change_abs);
		cout2 << "EventSupplier::detect_and_push_change_event(): rel_change " << delta_change_rel << " abs_change " << delta_change_abs << " is change = " << is_change << endl;
	}
	
	if (is_change)
	{
		vector<string> filterable_names;
		vector<double> filterable_data;

		attr.ext->prev_change_event.attr = attr_value;

		domain_name = device_impl->get_name()+"/"+attr_name;
		filterable_names.push_back("delta_change_rel");
		filterable_data.push_back(delta_change_rel);
		filterable_names.push_back("delta_change_abs");
		filterable_data.push_back(delta_change_abs);
		push_event(device_impl,"change",filterable_names, filterable_data,attr_value);
	}
}

void EventSupplier::detect_and_push_archive_event(DeviceImpl *device_impl, AttributeValue &attr_value,  Attribute &attr, string &attr_name)
{
	string event, domain_name;
	double delta_change_rel, delta_change_abs;
	bool is_change=false;

	cout2 << "EventSupplier::detect_and_push_archive_event(): called for attribute " << attr_value.name << endl;

//
// if no attribute of this name is registered with change then
// insert the current value
//
	if (!attr.ext->prev_archive_event.inited)
	{
		attr.ext->prev_archive_event.attr = attr_value;
		attr.ext->prev_archive_event.inited = true;
	}
	else
	{
//
// determine delta_change in percent compared with previous event sent
// 
//
		is_change = detect_change(attr,attr_value,true,delta_change_rel, delta_change_abs);
	}
	
	if (is_change)
	{
		vector<string> filterable_names;
		vector<double> filterable_data;

		attr.ext->prev_archive_event.attr = attr_value;

		domain_name = device_impl->get_name()+"/"+attr_name;
		filterable_names.push_back("delta_change_rel");
		filterable_data.push_back(delta_change_rel);
		filterable_names.push_back("delta_change_abs");
		filterable_data.push_back(delta_change_abs);
		push_event(device_impl,"archive",filterable_names, filterable_data,attr_value);
	}
}

void EventSupplier::detect_and_push_quality_change_event(DeviceImpl *device_impl, AttributeValue &attr_value, Attribute &attr, string &attr_name)
{
	string event, domain_name;
	bool is_alarm=false;
	CORBA::TypeCode_var ty = attr_value.value.type();
	CORBA::TypeCode_var ty_alias = ty->content_type();
	CORBA::TypeCode_var ty_seq = ty_alias->content_type();			

	cout2 << "EventSupplier::detect_and_push_quality_change_event(): called for attribute " << attr_value.name << endl;

	if (attr_value.quality != attr.ext->prev_quality_event.attr.quality)
	{
		vector<string> filterable_names;
		vector<double> filterable_data;

		attr.ext->prev_quality_event.inited = true;
		attr.ext->prev_quality_event.attr = attr_value;

		push_event(device_impl,"quality_change",filterable_names, filterable_data,attr_value);
	}
}

void EventSupplier::detect_and_push_periodic_event(DeviceImpl *device_impl, AttributeValue &attr_value, Attribute &attr, string &attr_name)
{
	CosNotification::StructuredEvent structured_event;
	string event, domain_name;
	double now_ms, ms_since_last_periodic;
#ifdef WIN32
        struct _timeb           now_win;
#endif
        struct timeval          now_timeval;

#ifdef WIN32
	_ftime(&now_win);
	now_timeval.tv_sec = (unsigned long)now_win.time;
	now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
	gettimeofday(&now_timeval,NULL);
#endif
	now_ms = (double)now_timeval.tv_sec*1000.+(double)now_timeval.tv_usec/1000.;
	ms_since_last_periodic = now_ms - attr.ext->last_periodic;
	cout1 << "EventSupplier::detect_and_push_is_periodic_event(): delta since last periodic " << ms_since_last_periodic << " event_period " << attr.ext->event_period << " for " << device_impl->get_name()+"/"+attr_name << endl;
	if (ms_since_last_periodic > attr.ext->event_period)
	{
		vector<string> filterable_names;
		vector<double> filterable_data;

		attr.ext->periodic_counter++;
		attr.ext->last_periodic = now_ms;
		filterable_names.push_back("counter");
		filterable_data.push_back(attr.ext->periodic_counter);

		cout1 << "EventSupplier::detect_and_push_is_periodic_event(): detected periodic event for " << device_impl->get_name()+"/"+attr_name << endl;
		push_event(device_impl,"periodic",filterable_names, filterable_data,attr_value);
	}

}

void EventSupplier::push_event(DeviceImpl *device_impl, string event_name, vector<string> &filterable_names, vector<double> &filterable_data, AttributeValue &attr_value)
{
	CosNotification::StructuredEvent structured_event;
	string event, domain_name, attr_name;

	cout2 << "EventSupplier::push_event(): called for attribute " << attr_value.name << endl;
	

	attr_name = attr_value.name;
	domain_name = device_impl->get_name()+"/"+attr_name;
	structured_event.header.fixed_header.event_type.domain_name = 
		CORBA::string_dup(domain_name.c_str());
  	structured_event.header.fixed_header.event_type.type_name   = 
    		CORBA::string_dup("Tango::EventValue");
  	structured_event.header.variable_header.length( 0 );
	if (filterable_names.size() == filterable_data.size())
	{
  		structured_event.filterable_data.length(filterable_data.size());
		for (int i; i < filterable_data.size(); i++)
		{
  			structured_event.filterable_data[i].name = filterable_names[i].c_str();
		  	structured_event.filterable_data[i].value <<= (CORBA::Double) filterable_data[i];
		}
	}
	structured_event.remainder_of_body <<= attr_value;
  	structured_event.header.fixed_header.event_name   = 
    		CORBA::string_dup(event_name.c_str());

	cout1 << "EventSupplier::push_event(): push event " << event_name << " for " << device_impl->get_name()+"/"+attr_name << endl;
//
// Push the event
//
	try
	{
		structuredProxyPushConsumer -> push_structured_event(structured_event);
	}
	catch(const CosEventComm::Disconnected&)
	{
		cout1 << "EventSupplier::push_event() event channel disconnected !\n";
	}
       	catch(const CORBA::TRANSIENT&)
       	{
       		cout1 << "EventSupplier::push_event() caught a CORBA::TRANSIENT ! " << endl;
       	}
       	catch(const CORBA::COMM_FAILURE&)
       	{
       		cout1 << "EventSupplier::push_event() caught a CORBA::COMM_FAILURE ! " << endl;
	}
    	catch(const CORBA::SystemException& ex)
    	{
       		cout1 << "EventSupplier::push_event() caught a CORBA::SystemException ! " << endl;
    	}
}

void EventSupplier::push_heartbeat_event()
{
	CosNotification::StructuredEvent structured_event;
	string event, domain_name;
	unsigned int delta_time;
	static int heartbeat_counter=0;
//
// Heartbeat - check wether a heartbeat event has been sent recently
// if not then send it. A heartbeat contains no data, it is used by the
// consumer to know that the supplier is still alive. 
//
	Tango::Util *tg = Tango::Util::instance();
	DServer *adm_dev = tg->get_dserver_device();
	delta_time = time(NULL);
	delta_time = delta_time-adm_dev->last_heartbeat;
	cout1 << "EventSupplier::push_heartbeat_event(): delta time since last heartbeat " << delta_time << endl;
	if (delta_time >= 10)
	{
		domain_name = "DServer/"+adm_dev->get_full_name();
		structured_event.header.fixed_header.event_type.domain_name = 
			CORBA::string_dup(domain_name.c_str());
  		structured_event.header.fixed_header.event_type.type_name   = 
    			CORBA::string_dup("Tango::Heartbeat");
  		structured_event.header.variable_header.length( 0 );
		cout1 << "EventSupplier::push_heartbeat_event(): detected heartbeat event for " << domain_name << endl;
		cout1 << "EventSupplier::push_heartbeat_event(): delta _time " << delta_time << endl;
  		structured_event.header.fixed_header.event_name   = 
    			CORBA::string_dup("heartbeat");
  		structured_event.filterable_data.length(1);
  		structured_event.filterable_data[0].name = (const char*) "heartbeat_counter";
  		structured_event.filterable_data[0].value <<= (CORBA::Long) heartbeat_counter++;
		adm_dev->last_heartbeat = time(NULL);
		structured_event.remainder_of_body <<= (CORBA::Long)adm_dev->last_heartbeat;

//
// Push the event
//
		try
		{
			structuredProxyPushConsumer -> push_structured_event(structured_event);
		}
		catch(const CosEventComm::Disconnected&)
		{
			cout1 << "EventSupplier::push_heartbeat_event() event channel disconnected !\n";
		}
       		catch(const CORBA::TRANSIENT&)
       		{
       			cout1 << "EventSupplier::push_heartbeat_event() caught a CORBA::TRANSIENT ! " << endl;
       		}
       		catch(const CORBA::COMM_FAILURE&)
       		{
       			cout1 << "EventSupplier::push_heartbeat_event() caught a CORBA::COMM_FAILURE ! " << endl;
		}
    		catch(const CORBA::SystemException& ex)
    		{
       			cout1 << "EventSupplier::push_heartbeat_event() caught a CORBA::SystemException ! " << endl;
    		}
	}
}

/***********************/
/* EventConsumer class */
/***********************/

EventConsumer *EventConsumer::_instance = NULL;

EventConsumer::EventConsumer() : omni_thread()
{


	cout1 << "calling Tango::EventConsumer::EventConsumer() \n";

	tango_db = new Tango::Database();

	orb_ = ApiUtil::instance()->get_orb();

	_instance = this;

//	Logging::init("EventConsumer", orb_, (int)4, true);


//	cout << "Tango::EventConsumer::EventConsumer(): start POA and wait for events in separate thread\n";
	start_undetached();
}

EventConsumer *EventConsumer::create()
{

// check if the EventConsumer singleton exists, if so return it

	if (_instance != NULL)
	{
		return _instance;
	}

// EventConsumer singelton does not exist, create it

	return new EventConsumer();
}

//
// activate POA and go into endless loop waiting for events to be pushed
// this method should run as a separate thread so as not to block the client
//

void *EventConsumer::run_undetached(void *arg) {

	EventConsumerKeepAliveThread *keep_alive_thread = new EventConsumerKeepAliveThread();

	CORBA::Object_var obj = orb_->resolve_initial_references("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
        PortableServer::POAManager_var pman = poa->the_POAManager();
        pman->activate();

	orb_->run();

	return NULL;

	}

void EventConsumer::disconnect_structured_push_consumer() {

	cout1 << "calling Tango::EventConsumer::disconnect_structured_push_consumer() \n";
}

void EventConsumer::offer_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled) {

	cout1 << "calling Tango::EventConsumer::subscription_change() \n";
}

void EventConsumer::connect(DeviceProxy *device_proxy)
{
//
// Get a reference to the Notification Service EventChannelFactory
//
        CORBA::Any send;
	CORBA::Any_var received;

	string device_name = device_proxy->dev_name();

        send <<= device_name.c_str();
	try 
	{
        	received = tango_db->command_inout("DbImportDevice",send);
	}
	catch (...) 
	{
		cerr << device_name << " is not a valid device name " << endl;
                EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                        (const char*)"Failed to import device from database (hint: check device name)",
                        (const char*)"EventConsumer::connect()");        
		exit((void*)1);
	}
 
        const DevVarLongStringArray *dev_import_list;
        received >>= dev_import_list;
 
	string server_name;
	string server_host_name;

        server_name = string((dev_import_list->svalue)[3]);
        server_host_name = string((dev_import_list->svalue)[4]);

	string channel_name;

	channel_name = "DServer/"+server_name;

// if no connection exists to this channel then connect to it

	std::map<std::string,EventChannelStruct>::iterator ipos;

	ipos = channel_map.find(channel_name);

	if (ipos == channel_map.end())
	{
		connect_event_channel(channel_name);

	}

	string adm_name = device_proxy->adm_name();
	channel_map[channel_name].adm_device_proxy = new DeviceProxy(adm_name);
	device_channel_map[device_name] = channel_name;
}

void EventConsumer::connect_event_channel(string channel_name)
{
        CORBA::Any send;
	CORBA::Any_var received;
        const DevVarLongStringArray *dev_import_list;
//
// Get a reference to an EventChannel for this device server from the 
// TANGO database
//
	int channel_exported=-1;
	string channel_ior;


        send <<= channel_name.c_str();
	try 
	{
        	received = tango_db->command_inout("DbImportEvent",send);
	}
	catch (...) 
	{
		cerr << channel_name << " has no event channel defined in the database - maybe the server is not running ... " << endl;
		channel_exported = 0;
	}
	if (channel_exported != 0)
	{
        	received >>= dev_import_list;
        	channel_ior = string((dev_import_list->svalue)[1]);
        	channel_exported = dev_import_list->lvalue[0];
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
                        	(const char*)"EventConsumer::create()");        
		}
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
                       		(const char*)"EventConsumer::create()");        
        		exit((void*)1);
    		}
	}
	catch (...)
	{
                EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::create()");        
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
		consumerAdmin -> obtain_notification_push_supplier(
		    CosNotifyChannelAdmin::STRUCTURED_EVENT, proxyId);
		if (CORBA::is_nil(proxySupplier))
		{
        		//cerr << "Could not get CosNotifyChannelAdmin::ProxySupplier" << endl;
                	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to obtain a push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::create()");        
    		}

        	structuredProxyPushSupplier =
			CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow(
		    		proxySupplier);
		if (CORBA::is_nil(structuredProxyPushSupplier))
		{
       			//cerr << "Tango::EventConsumer::EventConsumer() could not get CosNotifyChannelAdmin::StructuredProxyPushSupplier" << endl;
                	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to narrow the push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::create()");        
    		}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get PushSupplier from notification daemon due to AdminLimitExceeded (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::create()");        
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

	new_event_channel_struct.eventChannel = eventChannel;
	new_event_channel_struct.structuredProxyPushSupplier = structuredProxyPushSupplier;
	new_event_channel_struct.last_heartbeat = time(NULL);
	channel_map[channel_name] = new_event_channel_struct;

//
// add a filter for heartbeat events
//
        char constraint_expr[256];
        ::sprintf(constraint_expr,"$event_name == \'heartbeat\'");
        CosNotifyFilter::FilterFactory_var ffp;
        CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
        try {
                ffp    = channel_map[channel_name].eventChannel->default_filter_factory();
                filter = ffp->create_filter("EXTENDED_TCL");
        } catch (...) {
                //cerr << "Caught exception obtaining filter object" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating heartbeat filter (check filter)",
                       	(const char*)"EventConsumer::create()");        
        }
        // Construct a simple constraint expression; add it to fadmin
        CosNotification::EventTypeSeq evs;
        CosNotifyFilter::ConstraintExpSeq exp;
        exp.length(1);
        exp[0].event_types = evs;
        exp[0].constraint_expr = CORBA::string_dup(constraint_expr);
        CORBA::Boolean res = 0; // OK
        try {
                filter->add_constraints(exp);
                channel_map[channel_name].heartbeat_filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
        }
        catch (...) {
                //cerr << "Exception thrown while adding constraint "
                //     << (const char *)constraint_expr << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while adding constraint for heartbeat (check filter)",
                       	(const char*)"EventConsumer::create()");        
                res = 1; // error
        }
        if (res == 1) { // error so destroy filter
                try {
                        filter->destroy();
                } catch (...) { }
                filter = CosNotifyFilter::Filter::_nil();
        }
}

void EventConsumer::attr_to_device(const AttributeValue *attr_value, DeviceAttribute *dev_attr)
{

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	CORBA::ULong max,len;
	
	dev_attr->name = attr_value->name;
	dev_attr->quality = attr_value->quality;
	dev_attr->time = attr_value->time;
	dev_attr->dim_x = attr_value->dim_x;
	dev_attr->dim_y = attr_value->dim_y;

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		CORBA::TypeCode_var ty = attr_value->value.type();
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
		switch (ty_seq->kind())
		{
			case tk_long:		
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
		
			case tk_short:
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
		}
	}	
}

void EventConsumer::push_structured_event(const CosNotification::StructuredEvent& event)
{
	string domain_name(event.header.fixed_header.event_type.domain_name);
	string event_type(event.header.fixed_header.event_type.type_name);
	string event_name(event.header.fixed_header.event_name);

	if (event_name == "heartbeat")
	{
        	std::map<std::string,EventChannelStruct>::iterator ipos;
 
        	ipos = channel_map.find(domain_name);
 
        	if (ipos != channel_map.end())
        	{
                	channel_map[domain_name].last_heartbeat = time(NULL);
 
		}
        }
	else
	{
		/*
		 * new interface to Callback does not pass the filterable data
		 * therefore do not extract it
		 *
		vector<double> filterable_data;
		for (int i=0; i < event.filterable_data.length(); i++)
		{
			double filterable_value;
			event.filterable_data[i].value >>= filterable_value;
			filterable_data.push_back(filterable_value);
		}
		*/
		string attr_event_name = domain_name+"."+event_name;
	
		map<std::string,EventCallBackStruct>::iterator ipos;
	
		ipos = event_callback_map.find(attr_event_name);
	
		if (ipos != event_callback_map.end())
		{
			AttributeValue *attr_value;
			CallBack *callback;
			DevErrorList errors;
			callback = event_callback_map[attr_event_name].callback;
			if (callback != NULL)
			{
				event.remainder_of_body >>= attr_value;
				DeviceAttribute *dev_attr = new(DeviceAttribute);
				attr_to_device(attr_value, dev_attr);
				EventData *event_data = new EventData(event_callback_map[attr_event_name].device,
							domain_name,
							event_name,
							dev_attr,
							errors);
				callback->push_event(event_data);
				delete event_data;
				delete dev_attr;
			}
		}
	}

}

int EventConsumer::subscribe_event(DeviceProxy *device, const string &attribute, EventType event, CallBack *callback, const vector<string> &filters)
{
	static int subscribe_event_id=0; // unique event id

	string device_name = device->dev_name();
	cout1 << "Tango::EventConsumer::subscribe_event(" << device_name << "," << attribute <<"," << event << ")\n";

	map<std::string,std::string>::iterator ipos;

	ipos = device_channel_map.find(device_name);
	if (ipos == device_channel_map.end())
	{
		cout1 << "device " << device_name << " is not connected, going to connect to the event channel !\n";
		connect(device);
		ipos = device_channel_map.find(device_name);
		if (ipos == device_channel_map.end())
		{
			cout1 << "failed to connect to event channel for device " << device_name << endl;
			return -1;
		}
	}

	string channel_name = device_channel_map[device_name];

	string event_name;

	switch (event) 
	{
		case CHANGE_EVENT : event_name = "change";
				    break;

		case QUALITY_EVENT : event_name = "quality";
				     break;

		case PERIODIC_EVENT : event_name = "periodic";
				      break;

		case ARCHIVE_EVENT : event_name = "archive";
				     break;
	}
//
// inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is
// down !)
//
	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(device_name);
	subscriber_info.push_back(attribute);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(event_name);
	subscriber_in << subscriber_info;
    	channel_map[channel_name].adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
    	//channel_map[channel_name].adm_device_proxy->command_inout_asynch("EventSubscriptionChange",subscriber_in,false);
	channel_map[channel_name].last_subscribed = time(NULL);

	char constraint_expr[256];
	::sprintf(constraint_expr,"$domain_name == \'%s/%s\' and $event_name == \'%s\'",
		device_name.c_str(),attribute.c_str(),event_name.c_str());
	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;
	try {
    		ffp    = channel_map[channel_name].eventChannel->default_filter_factory();  
    		filter = ffp->create_filter("EXTENDED_TCL");
  	} catch (CORBA::COMM_FAILURE& ex) {
    		//cerr << "Caught COMM_FAILURE obtaining filter object" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	} catch (...) {
    		//cerr << "Caught exception obtaining filter object" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
  	// Construct a simple constraint expression; add it to fadmin
	CosNotification::EventTypeSeq evs;
  	CosNotifyFilter::ConstraintExpSeq exp;
  	exp.length(1);
  	exp[0].event_types = evs;
  	exp[0].constraint_expr = CORBA::string_dup(constraint_expr);
  	CORBA::Boolean res = 0; // OK
  	try {
    		filter->add_constraints(exp);
    		filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
		EventCallBackStruct new_event_callback;
		new_event_callback.device = device;
		new_event_callback.attr_name = attribute;
		new_event_callback.event_name = event_name;
		new_event_callback.channel_name = channel_name;
		new_event_callback.callback = callback;
		new_event_callback.filter_id = filter_id;
		subscribe_event_id++; new_event_callback.id = subscribe_event_id;
		string callback_key = device_name+"/"+attribute+"."+event_name;
		event_callback_map[callback_key] = new_event_callback;
		//cout1 << "EventConsumer::subscribe_event(): callback key " << callback_key << endl;
  	}
  	catch(CosNotifyFilter::InvalidConstraint& _exobj1) {
    		//cerr << "Exception thrown : Invalid constraint given "
	  	//     << (const char *)constraint_expr << endl;
		res ==  1;
  	}
  	catch (...) {
    		//cerr << "Exception thrown while adding constraint " 
	 	//     << (const char *)constraint_expr << endl; 
		res = 1;
  	}
  	if (res == 1) { // error so destroy filter
    		try {
      			filter->destroy();
    		} catch (...) { }
    		filter = CosNotifyFilter::Filter::_nil();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	return subscribe_event_id;
}

void EventConsumer::unsubscribe_event(int event_id)
{
	std::map<std::string,EventConsumer::EventCallBackStruct>::iterator epos;
	bool found=false;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); epos++)
	{
		if(epos->second.id == event_id)
		{
			//cout << "Tango::EventConsumer::unsubscribe_event() - found event id " << event_id << " going to remove_filter()\n";
			try
			{
				channel_map[epos->second.channel_name].structuredProxyPushSupplier->remove_filter(epos->second.filter_id);
			}
			catch (...)
			{
				EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
					(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() (hint: check the Notification daemon is running ",
					(const char*)"EventConsumer::unsubscribe_event()");    
			}
			event_callback_map.erase(epos);
			found=true;
			break;
		}
	}
	if (!found)
	{
		EventIdExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");    
	}
	return;
}


EventConsumerKeepAliveThread::EventConsumerKeepAliveThread() : omni_thread()
{

//	cout << "Tango::EventConsumer::EventConsumer(): start thread which will keep event subscriptions alive and check for server crashes\n";
	mutex = new omni_mutex();
	condition = new omni_condition(mutex);
	start_undetached();
}
void *EventConsumerKeepAliveThread::run_undetached(void *arg) 
{
	long unsigned int time_to_sleep_sec, time_to_sleep_nsec;
	int time_to_sleep, now;
	EventConsumer *event_consumer;

//
// first sleep 2 seconds to give the event system time to startup
//
	//cout << "EventConsumerKeepAliveThread::run_detached(): go to sleep for 2 seconds\n";
#ifndef WIN32
	omni_thread::get_time(&time_to_sleep_sec, &time_to_sleep_nsec, (unsigned long)2, (unsigned long)0);
	//cout << "EventConsumerKeepAliveThread::run_detached(): time_to_sleep " << time_to_sleep_sec << " sec " << time_to_sleep_nsec << " nsec\n";
	condition->timedwait(time_to_sleep_sec, time_to_sleep_nsec);
#else
	Sleep(2000L);
#endif /* WIN32 */
	//cout << "EventConsumerKeepAliveThread::run_detached(): slept 2 seconds\n";
	event_consumer = ApiUtil::instance()->get_event_consumer();
	for (;;)
	{
		time_to_sleep = EVENT_HEARTBEAT_PERIOD;
//
// goto sleep until next heartbeat 
//
		//cout << "EventConsumerKeepAliveThread::run_detached(): go to sleep for " << time_to_sleep << endl;
#ifndef WIN32
		omni_thread::get_time(&time_to_sleep_sec, &time_to_sleep_nsec, (unsigned long)time_to_sleep, (unsigned long)0);
		//cout << "EventConsumerKeepAliveThread::run_detached(): time_to_sleep " << time_to_sleep_sec << " sec " << time_to_sleep_nsec << " nsec\n";
		condition->timedwait(time_to_sleep_sec, time_to_sleep_nsec);
#else
		Sleep(time_to_sleep*1000);
#endif /* WIN32 */
		//cout << "EventConsumerKeepAliveThread::run_detached(): sleep over\n";

		std::map<std::string,EventConsumer::EventChannelStruct>::iterator ipos;
		std::map<std::string,EventConsumer::EventCallBackStruct>::iterator epos;
		now = time(NULL);
		for (ipos = event_consumer->channel_map.begin(); ipos != event_consumer->channel_map.end(); ipos++)
		{
			if ((now - ipos->second.last_subscribed) > EVENT_RESUBSCRIBE_PERIOD/3)
			{
				for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
				{
					if (epos->second.channel_name == ipos->first )
					{
//						//cout << "resubscribe device " << epos->second.device->dev_name() << " attribute " << epos->second.attr_name << " event " << epos->second.event_name << endl;
       						DeviceData subscriber_in;
       						vector<string> subscriber_info;
       						subscriber_info.push_back(epos->second.device->dev_name());
       						subscriber_info.push_back(epos->second.attr_name);
       						subscriber_info.push_back("subscribe");
       						subscriber_info.push_back(epos->second.event_name);
       						subscriber_in << subscriber_info;
						try 
						{
       							ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
       							//ipos->second.adm_device_proxy->command_inout_asynch("EventSubscriptionChange",subscriber_in,false);
						}
						catch (...) {};
        					ipos->second.last_subscribed = time(NULL);
        					epos->second.last_subscribed = time(NULL);
					}
				}
			}
/*
 *  check if a heartbeat have been skipped
 */
 			bool heartbeat_skipped;

			heartbeat_skipped = (now - ipos->second.last_heartbeat) > EVENT_HEARTBEAT_PERIOD;
//			cout << "last heartbeat was " << now-ipos->second.last_heartbeat << " seconds , skipped is " << ipos->second.heartbeat_skipped << endl;
			if (heartbeat_skipped || ipos->second.heartbeat_skipped)
			{
				ipos->second.heartbeat_skipped = true;
				string domain_name("domain_name"), event_name("event_name");
				Tango::DevErrorList errors(1);

				errors.length(1);
				errors[0].severity = Tango::ERR;
				errors[0].origin = CORBA::string_dup("EventConsumer::KeepAliveThread()");
				errors[0].reason = CORBA::string_dup("API_EventTimeout");
				errors[0].desc = CORBA::string_dup("Event channel is not responding anymore, maybe the server or event system is down");
				DeviceAttribute *dev_attr;
				EventData *event_data = new EventData(epos->second.device,
							domain_name,event_name,
							dev_attr,errors);
				for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
				{
					if (epos->second.channel_name == ipos->first && epos->second.callback != NULL)
					{
						CallBack *callback;
						callback = epos->second.callback;
						callback->push_event(event_data);
						if (ipos->second.heartbeat_skipped)
						{
//							cout << "resubscribe device " << epos->second.device->dev_name() << " attribute " << epos->second.attr_name << " event " << epos->second.event_name << endl;
       							DeviceData subscriber_in;
       							vector<string> subscriber_info;
       							subscriber_info.push_back(epos->second.device->dev_name());
       							subscriber_info.push_back(epos->second.attr_name);
       							subscriber_info.push_back("subscribe");
       							subscriber_info.push_back(epos->second.event_name);
       							subscriber_in << subscriber_info;
							try 
							{
       								ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
							}
							catch (...) {};
        						ipos->second.last_subscribed = time(NULL);
						}
					}
				}
				delete event_data;
			}
/*
 * only reset the heartbeat_skipped flag if at least one good heartbeat received
 */
			if (!heartbeat_skipped && ipos->second.heartbeat_skipped)
			{
				ipos->second.heartbeat_skipped = false;
			}
		}
	}
	return NULL;
}
}
