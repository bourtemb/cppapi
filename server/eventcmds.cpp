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
// $Revision$
//
// $Log$
// Revision 1.13  2007/02/16 10:40:57  taurel
// - Implementing a new types of event on the Attribute configuration
//
// Revision 1.12  2006/11/10 08:25:08  taurel
// - Fix bug if an EventSubsriptionChange command arrives in the admin
// device while the DS is in its shutdown sequence
//
// Revision 1.11  2006/06/20 08:00:04  jensmeyer
// After changing the the init value of the attribute polling period to -1 insted of 0 I detected
// a campatibility problem with the old versions. I changed back the init value to 0 and
// solved the problem with new methods on the Attribute object.
//
// Revision 1.10  2006/06/14 11:21:35  jensmeyer
// Modifications to make the set_polling_period() for attributes work
// with a polling period set to 0.
//
// Revision 1.9  2006/05/18 14:29:12  jensmeyer
// Changed the handling of the event period for periodic and archiving events.
// The event period is now stored as the configured value and no longer
// multilplied by the factor DELTA_PERIODIC.
// The calculation of the precision to fire periodic events has moved from
// the attribbute to the push methods in the event supplier.
//
// For event periods < 5000 ms a precision of 2% was kept, but for
// times < 5000 ms a fixed precision of 100 ms was added.
// This avoids periodic events with a long event period to be fired to early.
//
// Revision 1.8  2006/04/28 13:17:20  taurel
// - Attribute class: Add state and status attribute special case to the method Attribute_2_AttributeValue
// - Device_3Impl: The monitor taken by the read_attribute_history_3 call was the Device Monitor. Replaced it by the polling list monitor
//
// Revision 1.7  2006/04/13 06:12:28  jensmeyer
// Added methods to configure and fire manually change and archive events without
// polling.
//
// Revision 1.6  2005/12/16 17:38:57  jensmeyer
// Changed the error checking during an attribute subscription.
// Also added an option to allow manual change events.
//
// Revision 1.5  2005/11/09 09:43:57  jensmeyer
// Stopped automatic polling when subscribing for events!
// Added exceptions when polling is not started for an attribute.
//
// Changed the default of the archiving period from 10 seconds to
// no periodic event!
// Added exception when trying to subscribe a not configured attribute.
//
// Revision 1.4  2005/06/29 08:31:18  taurel
// - Last commit before release 5.2 ?
//
// Revision 1.3  2005/01/13 08:29:06  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 1.2.2.3  2004/12/09 08:33:00  taurel
// - Fix a small memory leak in case of error
//
// Revision 1.2.2.2  2004/11/04 09:46:39  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 1.2.2.1  2004/10/22 11:25:51  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 1.2  2004/07/07 07:59:20  taurel
// Added some files
//
// Revision 1.1.4.4  2004/04/24 12:09:25  taurel
// - Replace RCS Header by RCS Id
//
// Revision 1.1.4.3  2004/02/18 15:06:18  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 1.1.4.2  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 1.1.4.1  2004/01/20 08:33:12  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 1.1.2.5  2003/11/16 22:10:43  andy_gotz
// New version which defines 4 types of events - change, quality, periodic and
// archive. Code has been factorised to reduce redundancy. Minimum and maximum
// changes are supported. Event period is taken into account. Relative and
// absolute changes are detected. Whole sequence is taken into account when
// determining change.
//
// Revision 1.1.2.4  2003/07/17 03:46:46  andy_gotz
// Added new events "on_valid" and "on_state_change"; factorized EventSupplier::detect_and_push_events() method;
// Catching more exceptions when creating EventSupplier, device server will start without Notification service;
// Added AttributeProxy and DbAttribute classes to api; created test programs in test directory
//
// Revision 1.1.2.3  2003/04/15 19:01:55  andy_gotz
// added heartbeat on client and server side; cleaned up cout's
//
// Revision 1.1.2.2  2003/04/13 22:12:17  andy_gotz
// added heartbeat; polling starts automatically on subscription
//
// Revision 1.1.2.1  2003/04/10 11:07:44  andy_gotz
// added event commands to DServer admin object
//
//
// copyleft :   European Synchrotron Radiation Facility
//              BP 220, Grenoble 38043
//              FRANCE
//
//-=============================================================================
//
//  		This file was generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================


#include <tango.h>
#include <eventcmds.h>

namespace Tango
{


//+----------------------------------------------------------------------------
//
// method : 		EventSubscriptionChangeCmd::EventSubscriptionChangeCmd()
// 
// description : 	constructor for the command of the EventTester.
//
// In : - name : The command name
//		- in : The input parameter type
//		- out : The output parameter type
//		- in_desc : The input parameter description
//		- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------
EventSubscriptionChangeCmd::EventSubscriptionChangeCmd(const char *name,
						       Tango::CmdArgType in,
				       		       Tango::CmdArgType out,
						       const char *in_desc,
				       		       const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//
//	Constructor without in/out parameters description
//

EventSubscriptionChangeCmd::EventSubscriptionChangeCmd(const char		*name,
						       Tango::CmdArgType in,
				       		       Tango::CmdArgType out)
:Command(name,in,out)
{
}




//+----------------------------------------------------------------------------
//
// method : 		EventSubscriptionChangeCmd::is_allowed()
// 
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------
bool EventSubscriptionChangeCmd::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return true;
}




//+----------------------------------------------------------------------------
//
// method : 		EventSubscriptionChangeCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *EventSubscriptionChangeCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "EventSubscriptionChangeCmd::execute(): arrived" << endl;

	const Tango::DevVarStringArray	*argin;
	extract(in_any, argin);

    if (argin->length() < 4)
    {
		TangoSys_OMemStream o;
		o << "Not enough input arguments, needs 4 i.e. device name, attribute name, action, event name" << ends;

		Except::throw_exception((const char *)"DServer_Events",
								o.str(),
								(const char *)"EventSubscriptionChangeCmd::execute");
	}
 
	string dev_name, attr_name, action, event, attr_name_lower;
	dev_name = (*argin)[0];
	attr_name = (*argin)[1];
	action = (*argin)[2];
	event = (*argin)[3];

	attr_name_lower = attr_name;
	transform(attr_name_lower.begin(),attr_name_lower.end(),attr_name_lower.begin(),::tolower);
	
	cout4 << "EventSubscriptionChangeCmd: execute(): subscribtion for device " << dev_name << " attribute " << attr_name << " action " << action << " event " << event << endl;
	Tango::Util *tg = Tango::Util::instance();

//
// If we receive this command while the DS is in its
// shuting down sequence, do nothing
//

	if (tg->get_polling_thread_object() == NULL)
	{
        CORBA::Any *ret = return_empty_any("EventSubscriptionChange");
		return ret;
	}
		
//
// If the EventSupplier object is not created, create it right now
//

	if (tg->get_event_supplier() == NULL)
	{
		tg->create_event_supplier();
	}
	
//
// Get device reference
//
	
	DeviceImpl *dev_impl;
	try
	{
		dev_impl = tg->get_device_by_name((*argin)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (*argin)[0] << " not found" << ends;
 
		Except::re_throw_exception(e,(const char *)"API_DeviceNotFound",o.str(),
                                   (const char *)"DServer::EventSubscriptionChangeCmd:");
	}                                                                                 

	int attr_ind = dev_impl->dev_attr->get_attr_ind_by_name(attr_name.c_str());
	Attribute &attribute = dev_impl->dev_attr->get_attr_by_ind(attr_ind);
	
	if (action == "subscribe")
	{
		if (event == "user_event")
		{
			cout4 << "EventSubscriptionChangeCmd::execute(): update user_event subscription\n";
			attribute.ext->event_user_subscription = time(NULL);
		}
		else if (event == "attr_conf")
		{
			cout4 << "EventSubscriptionChangeCmd::execute(): update attr_conf subsription\n";
			attribute.ext->event_attr_conf_subscription = time(NULL);
		}
		else
		{		
			// If the polling is necessary to send events, check whether the polling is
			// started for the requested attribute.
			
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
									(const char *)"EventSubscriptionChangeCmd::execute");
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
										(const char *)"EventSubscriptionChangeCmd::execute");
						}
					}
					else
					{					
						Except::throw_exception((const char *)"API_AttributePollingNotStarted",
									o.str(),
									(const char *)"EventSubscriptionChangeCmd::execute");					
					}
				}
			}
			
				
       		if (event == "change")
       		{
				cout4 << "EventSubscriptionChangeCmd::execute(): update change subscription\n";

				//
				// Check if the attribute has some of the change properties defined
				//

				if (attr_name_lower != "state")
				{
					if ((attribute.get_data_type() != Tango::DEV_STRING) &&
			            (attribute.get_data_type() != Tango::DEV_BOOLEAN))
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
																(const char *)"EventSubscriptionChangeCmd::execute");
							}
						}
					}
				}
       			attribute.ext->event_change_subscription = time(NULL);
			}
      		else if (event == "quality")
      		{
				cout4 << "EventSubscriptionChangeCmd::execute(): update quality_change subscription\n";
       			attribute.ext->event_quality_subscription = time(NULL);
      		}
      		else if (event == "periodic")
      		{
				cout4 << "EventSubscriptionChangeCmd::execute(): update periodic subscription\n";
       			attribute.ext->event_periodic_subscription = time(NULL);
      		}                                                  
      		else if (event == "archive")
      		{
				//
				// Check if the attribute has some of the archive properties defined
				//

				if (attr_name_lower != "state")
				{
					if ((attribute.get_data_type() != Tango::DEV_STRING) &&
			                    (attribute.get_data_type() != Tango::DEV_BOOLEAN))
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
																(const char *)"EventSubscriptionChangeCmd::execute");
							}
						}
					}
				}		
		
				cout4 << "EventSubscriptionChangeCmd::execute(): update archive subscription\n";
       			attribute.ext->event_archive_subscription = time(NULL);
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
			
		//DevVarLongStringArray *send = new DevVarLongStringArray();
		try
		{
			Tango::Util *tg = Tango::Util::instance();
			DServer *adm_dev = tg->get_dserver_device();

			if (adm_dev->get_heartbeat_started() == false)
			{				
				adm_dev->add_event_heartbeat();
				adm_dev->set_heartbeat_started(true);
			}

				//send->lvalue.length(1);
				//send->svalue.length(3);
				//send->svalue[0] = CORBA::string_dup(dev_name.c_str());
				//send->svalue[1] = CORBA::string_dup("attribute");

				//send->lvalue[0] = attribute.ext->poll_period;
				//send->svalue[2] = CORBA::string_dup(attr_name.c_str());

				//cout4 << "EventSubscriptionChangeCmd::execute(): start attribute " << attr_name << " polling with period " << attribute.ext->poll_period << endl;
				//adm_dev->add_obj_polling(send,false);
		}
		catch (...)
		{
			//cout << "EventSubscriptionChangeCmd::execute(): failed to start polling !\n";
			// nada
		}
		//delete send;			
		//}
	}

	CORBA::Any *ret = return_empty_any("EventSubscriptionChange");
	return ret;
}


}	// namespace
