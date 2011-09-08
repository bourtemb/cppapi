static const char *RcsId = "$Id$";
//+=============================================================================
//
// file :        eventcmds.cpp
//
// description : C++ source for the event commands which are part of
//		 the DServer class.
//
// project :     TANGO events
//
// $Author$
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
// $Revision$
//
//-=============================================================================

#include <tango.h>
#include <eventsupplier.h>

namespace Tango
{


//+----------------------------------------------------------------------------
//
// method : 		DServer::event_subscription_change()
//
// description : 	method to execute the command EvnetSubscriptionChange command.
//
// in : - argin : The command input argument
//
// returns : The command output data (Tango lib release number)
//
//-----------------------------------------------------------------------------
DevLong DServer::event_subscription_change(const Tango::DevVarStringArray *argin)
{
    if (argin->length() < 4)
    {
		TangoSys_OMemStream o;
		o << "Not enough input arguments, needs 4 i.e. device name, attribute name, action, event name" << ends;

		Except::throw_exception((const char *)"DServer_Events",
								o.str(),
								(const char *)"DServer::event_subscription_change");
	}

	string dev_name, attr_name, action, event, attr_name_lower;
	dev_name = (*argin)[0];
	attr_name = (*argin)[1];
	action = (*argin)[2];
	event = (*argin)[3];

	attr_name_lower = attr_name;
	transform(attr_name_lower.begin(),attr_name_lower.end(),attr_name_lower.begin(),::tolower);

	cout4 << "EventSubscriptionChangeCmd: subscription for device " << dev_name << " attribute " << attr_name << " action " << action << " event " << event << endl;
	Tango::Util *tg = Tango::Util::instance();

//
// If we receive this command while the DS is in its
// shuting down sequence, do nothing
//

	if (tg->get_heartbeat_thread_object() == NULL)
	{
     	TangoSys_OMemStream o;
		o << "The device server is shutting down! You can no longer subscribe for events" << ends;

		Except::throw_exception((const char *)"DServer_Events",
									    o.str(),
									   (const char *)"DServer::event_subscription_change");
	}

//
// If the EventSupplier object is not created, create it right now
//

	NotifdEventSupplier *ev;
	if ((ev = tg->get_notifd_event_supplier()) == NULL)
	{
		tg->create_notifd_event_supplier();
		ev = tg->get_notifd_event_supplier();
	}

//
// If we are using a file as database, gives port number to event supplier
//

	if (Util::_FileDb == true && ev != NULL)
	{
		string &p_num = tg->get_svr_port_num();
		ev->set_svr_port_num(p_num);
	}

    event_subscription(dev_name,attr_name,action,event,attr_name_lower);

	Tango::DevLong ret_val = (Tango::DevLong)tg->get_tango_lib_release();
	return ret_val;
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::event_subscription()
//
// description : 	method to do all the necessary checks on attribute config
//                  to generate events
//
// in : - dev_name : The device name
//      - attr_name : The attribute name
//      - action : What the user want to do
//      - event : The event type
//      - attr_name_lower : The attribute name in lower case letters
//
//-----------------------------------------------------------------------------

DeviceImpl *DServer::event_subscription(string &dev_name,string &attr_name,string &action,string &event,string &attr_name_lower)
{
    Tango::Util *tg = Tango::Util::instance();

//
// Get device reference
//

	DeviceImpl *dev_impl;
	try
	{
		dev_impl = tg->get_device_by_name(dev_name);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << dev_name << " not found" << ends;

		Except::re_throw_exception(e,(const char *)"API_DeviceNotFound",o.str(),
                                   (const char *)"DServer::event_subscription");
	}

    MultiAttribute *m_attr = dev_impl->get_device_attr();
	int attr_ind = m_attr->get_attr_ind_by_name(attr_name.c_str());
	Attribute &attribute = m_attr->get_attr_by_ind(attr_ind);

//
// Check if the request comes from a Tango 6 client (without client identification)
// If true, the event has to be sent using AttributeValue_3 data structure
//

	client_addr *cl = get_client_ident();
	int cl_release;

	if (cl->client_ident == true)
		cl_release = 4;
	else
		cl_release = 3;

	if (action == "subscribe")
	{
		if (event == "user_event")
		{
			cout4 << "DServer::event_subscription(): update user_event subscription\n";
			attribute.ext->event_user_subscription = time(NULL);
			if (cl_release == 3)
				attribute.ext->event_user_client_3 = true;
		}
		else if (event == "attr_conf")
		{
			cout4 << "DServer::event_subscription(): update attr_conf subscription\n";
			attribute.ext->event_attr_conf_subscription = time(NULL);
		}
		else if (event == "data_ready")
		{
			if (attribute.is_data_ready_event() == false)
			{
				TangoSys_OMemStream o;
				o << "The attribute ";
				o << attr_name;
				o << " is not data ready event enabled" << ends;

				Except::throw_exception((const char*)"API_AttributeNotDataReadyEnabled",
										o.str(),
										(const char *)"DServer::event_subscription");
			}
			cout4 << "DServer::event_subscription(): update data_ready subscription\n";
			attribute.ext->event_data_ready_subscription = time(NULL);
		}
		else
		{

//
// If the polling is necessary to send events, check whether the polling is
// started for the requested attribute.
//

			if (attribute.is_polled() == false )
			{
				TangoSys_OMemStream o;
				o << "The polling (necessary to send events) for the attribute ";
				o << attr_name;
				o << " is not started" << ends;

				if ( event == "change")
				{
					if (attribute.is_change_event() == false)
					{
						Except::throw_exception((const char *)"API_AttributePollingNotStarted",
									o.str(),
									(const char *)"DServer::event_subscription");
					}
				}
				else
				{
					if ( event == "archive")
					{
						if (attribute.is_archive_event() == false)
						{
							Except::throw_exception((const char *)"API_AttributePollingNotStarted",
										o.str(),
										(const char *)"DServer::event_subscription");
						}
					}
					else
					{
						Except::throw_exception((const char *)"API_AttributePollingNotStarted",
									o.str(),
									(const char *)"DServer::event_subscription");
					}
				}
			}


       		if (event == "change")
       		{
				cout4 << "DServer::event_subscription(): update change subscription\n";

//
// Check if the attribute has some of the change properties defined
//

				if (attr_name_lower != "state")
				{
					if ((attribute.get_data_type() != Tango::DEV_STRING) &&
			            (attribute.get_data_type() != Tango::DEV_BOOLEAN) &&
			            (attribute.get_data_type() != Tango::DEV_ENCODED) &&
						(attribute.get_data_type() != Tango::DEV_STATE))
					{
						if ( attribute.is_check_change_criteria() == true )
						{
							if ((attribute.ext->rel_change[0] == INT_MAX) &&
					    		(attribute.ext->rel_change[1] == INT_MAX) &&
					    		(attribute.ext->abs_change[0] == INT_MAX) &&
					    		(attribute.ext->abs_change[1] == INT_MAX))
							{
								TangoSys_OMemStream o;
								o << "Event properties (abs_change or rel_change) for attribute ";
								o << attr_name;
								o << " are not set" << ends;

								Except::throw_exception((const char *)"API_EventPropertiesNotSet",
																o.str(),
																(const char *)"DServer::event_subscription");
							}
						}
					}
				}
       			attribute.ext->event_change_subscription = time(NULL);
				if (cl_release == 3)
					attribute.ext->event_change_client_3 = true;
			}
      		else if (event == "quality")
      		{
				cout4 << "DServer::event_subscription(): update quality_change subscription\n";
       			attribute.ext->event_quality_subscription = time(NULL);
      		}
      		else if (event == "periodic")
      		{
				cout4 << "DServer::event_subscription(): update periodic subscription\n";
       			attribute.ext->event_periodic_subscription = time(NULL);
				if (cl_release == 3)
					attribute.ext->event_periodic_client_3 = true;
      		}
      		else if (event == "archive")
      		{

//
// Check if the attribute has some of the archive properties defined
//

				if (attr_name_lower != "state")
				{
					if ((attribute.get_data_type() != Tango::DEV_STRING) &&
			            (attribute.get_data_type() != Tango::DEV_BOOLEAN) &&
			            (attribute.get_data_type() != Tango::DEV_ENCODED) &&
						(attribute.get_data_type() != Tango::DEV_STATE))
					{
						if ( attribute.is_check_archive_criteria() == true )
						{
							if ((attribute.ext->archive_abs_change[0] == INT_MAX) &&
					      		(attribute.ext->archive_abs_change[1] == INT_MAX) &&
						   	    (attribute.ext->archive_period        == INT_MAX))
							{
								TangoSys_OMemStream o;
								o << "Archive event properties (archive_abs_change or archive_rel_change or archive_period) for attribute ";
								o << attr_name;
								o << " are not set" << ends;

								Except::throw_exception((const char *)"API_EventPropertiesNotSet",
															 	o.str(),
																(const char *)"DServer::event_subscription");
							}
						}
					}
				}

				cout4 << "DServer::event_subscription(): update archive subscription\n";
       			attribute.ext->event_archive_subscription = time(NULL);
				if (cl_release == 3)
					attribute.ext->event_archive_client_3 = true;
      		}
		}

//
// Start polling for attribute in question. I suppose I should
// check to see if the attribute is polled already. For the
// moment I will simply ignore the exception. Why not rather
// introduce a is_polled() method in each Attribute ?
//
// Use the add_obj_polling() admin device method whith no
// db update. The polling will poll the attribute next time
// the server is started if  there no more event client connected
// to the attribute
//

		try
		{
			DServer *adm_dev = tg->get_dserver_device();

			if (adm_dev->get_heartbeat_started() == false)
			{
				adm_dev->add_event_heartbeat();
				adm_dev->set_heartbeat_started(true);
			}
		}
		catch (...)
		{
		}
	}

	return dev_impl;
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::zmq_event_subscription_change()
//
// description : 	method to execute the command ZmqEventSubscriptionChange command.
//
// in : - argin : The command input argument
//
// returns : The command output data (Tango lib release number)
//
//-----------------------------------------------------------------------------
DevVarLongStringArray *DServer::zmq_event_subscription_change(const Tango::DevVarStringArray *argin)
{
    if (argin->length() < 4)
    {
		TangoSys_OMemStream o;
		o << "Not enough input arguments, needs 4 i.e. device name, attribute name, action, event name" << ends;

		Except::throw_exception((const char *)"DServer_Events",
								o.str(),
								(const char *)"DServer::zmq_event_subscription_change");
	}

	string dev_name, attr_name, action, event, attr_name_lower;
	dev_name = (*argin)[0];
	attr_name = (*argin)[1];
	action = (*argin)[2];
	event = (*argin)[3];

	attr_name_lower = attr_name;
	transform(attr_name_lower.begin(),attr_name_lower.end(),attr_name_lower.begin(),::tolower);

	cout4 << "ZmqEventSubscriptionChangeCmd: subscription for device " << dev_name << " attribute " << attr_name << " action " << action << " event " << event << endl;
	Tango::Util *tg = Tango::Util::instance();

//
// If we receive this command while the DS is in its
// shuting down sequence, do nothing
//

	if (tg->get_heartbeat_thread_object() == NULL)
	{
     	TangoSys_OMemStream o;
		o << "The device server is shutting down! You can no longer subscribe for events" << ends;

		Except::throw_exception((const char *)"DServer_Events",
									    o.str(),
									   (const char *)"DServer::zmq_event_subscription_change");
	}

//
// If the EventSupplier object is not created, create it right now
//

	ZmqEventSupplier *ev;
	if ((ev = tg->get_zmq_event_supplier()) == NULL)
	{
		tg->create_zmq_event_supplier();
		ev = tg->get_zmq_event_supplier();
	}

//
// If we are using a file as database, gives port number to event supplier
//

	if (Util::_FileDb == true && ev != NULL)
	{
		string &p_num = tg->get_svr_port_num();
		ev->set_svr_port_num(p_num);
	}

    DeviceImpl *dev = event_subscription(dev_name,attr_name,action,event,attr_name_lower);

//
// Create the event publisher socket (if not already done)
//

    ev->create_event_socket();

//
// Init data returned by command
//

	Tango::DevVarLongStringArray *ret_data = new Tango::DevVarLongStringArray();
	ret_data->lvalue.length(2);
	ret_data->svalue.length(2);

	ret_data->lvalue[0] = (Tango::DevLong)tg->get_tango_lib_release();
	ret_data->lvalue[1] = dev->get_dev_idl_version();

    string &heartbeat_endpoint = ev->get_heartbeat_endpoint();
	ret_data->svalue[0] = CORBA::string_dup(heartbeat_endpoint.c_str());
	string &event_endpoint = ev->get_event_endpoint();
	ret_data->svalue[1] = CORBA::string_dup(event_endpoint.c_str());

	return ret_data;
}

}	// namespace
