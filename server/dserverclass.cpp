static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               DServerClass.cpp
//
// description :        C++ source for the DServerClass and for the 
//			command class defined for this class. The singleton
//			class derived from DeviceClass. It implements the
//			command list and all properties and methods required
//			by the DServer once per process.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/03/25 16:43:19  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.11  2003/03/11 17:55:53  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:41  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:19  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:21  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:06  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:19  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:54  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:41  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:12  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:10  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:13  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:45  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:15  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:21  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <algorithm>

#include <tango.h>
#include <dserverclass.h>
#include <pollcmds.h>
#include <eventcmds.h>
#ifdef TANGO_HAS_LOG4TANGO
#include <logcmds.h>
#endif
#include <eventcmds.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		DevRestartCmd::DevRestartCmd 
// 
// description : 	constructors for Command class Restart
//
//--------------------------------------------------------------------------

DevRestartCmd::DevRestartCmd(const char *name,
			     Tango::CmdArgType in,
			     Tango::CmdArgType out,
			     const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		DevRestartCmd::execute 
// 
// description : 	restart a device
//
//--------------------------------------------------------------------------

CORBA::Any *DevRestartCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "DevRestart::execute(): arrived " << endl;

//
// Extract the input string
//

	const char *tmp_name;
	if ((in_any >>= tmp_name) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : string",
				        (const char *)"DevRestartCmd::execute");
	}
	string d_name(tmp_name);
	cout4 << "Received string = " << d_name << endl;
		
//	
// call DServer method which implements this command
//

	((DServer *)device)->restart(d_name);

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevKillCmd");
	return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		DevRestartServerCmd::DevRestartServerCmd()
// 
// description : 	constructor for the DevRestartServerCmd command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevRestartServerCmd::DevRestartServerCmd(const char *name,
	  	       			Tango::CmdArgType in,
		       			Tango::CmdArgType out):Command(name,in,out)
{
}

//+----------------------------------------------------------------------------
//
// method : 		DevRestartServerCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the DevRestartServer 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevRestartServerCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevRestartServerCmd::execute(): arrived" << endl;

//	
// call DServer method which implements this command
//

	((DServer *)device)->restart_server();

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevRestartServerCmd");
	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		DevQueryClassCmd::DevQueryClassCmd()
// 
// description : 	constructor for the DevQueryClass command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevQueryClassCmd::DevQueryClassCmd(const char *name,
			     	   Tango::CmdArgType in,
			     	   Tango::CmdArgType out,
				   const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevQueryClassCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "QueryClass" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevQueryClassCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevQueryClassCmd::execute(): arrived" << endl;

//	
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_class();

//
// return data to the caller
//

	CORBA::Any *out_any;
	try
	{	
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in DevQueryClassCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevQueryClassCmd::execute");
	}
	(*out_any) <<= ret;
	
	cout4 << "Leaving DevQueryClassCmd::execute()" << endl;	
	return(out_any);
}


//+----------------------------------------------------------------------------
//
// method : 		DevQueryDeviceCmd::DevQueryDeviceCmd()
// 
// description : 	constructor for the DevQueryDevice command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevQueryDeviceCmd::DevQueryDeviceCmd(const char *name,
			       	     Tango::CmdArgType in,
			       	     Tango::CmdArgType out,
				     const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevQueryDeviceCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "QueryDevice" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevQueryDeviceCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevQueryDeviceCmd::execute(): arrived" << endl;

//	
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_device();

//
// return data to the caller
//
	CORBA::Any *out_any;
	try
	{	
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in DevQueryDeviceCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevQueryDeviceCmd::execute");
	}
	(*out_any) <<= ret;
	
	cout4 << "Leaving DevQueryDeviceCmd::execute()" << endl;		
	return(out_any);
}


//+----------------------------------------------------------------------------
//
// method : 		DevKillCmd::DevKillCmd()
// 
// description : 	constructor for the DevKill command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevKillCmd::DevKillCmd(const char *name,
	  	       Tango::CmdArgType in,
		       Tango::CmdArgType out):Command(name,in,out)
{
}

//+----------------------------------------------------------------------------
//
// method : 		DevKillCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "Kill" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevKillCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevKillCmd::execute(): arrived" << endl;

//	
// call DServer method which implements this command
//

	((DServer *)device)->kill();

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevKillCmd");
	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceLevelCmd::DevSetTraceLevelCmd()
// 
// description : 	constructor for the DevSetTraceLevel command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevSetTraceLevelCmd::DevSetTraceLevelCmd(const char *name,
			     	   	 Tango::CmdArgType in,
			     	   	 Tango::CmdArgType out,
					 const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceLevelCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "SetTraceLevel" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevSetTraceLevelCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevSetTraceLevelCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO
  	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"SetTraceLevel is no more supported, please use SetLoggingLevel",
      				(const char *)"DevSetTraceLevelCmd::execute");
//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevSetTraceLevelCmd");
	return ret;

#else // TANGO_HAS_LOG4TANGO
	
//
// Get new level
//

	long new_level;
	if ((in_any >>= new_level) == false)
	{
		cout3 << "DevSetTraceLevelCmd::execute() --> Wrong argument type" << endl;
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				      (const char *)"Imcompatible command argument type, expected type is : long",
				      (const char *)"DevSetTraceLevelCmd::execute");
	}

//
// Set new level
//

	Tango::Util::instance()->set_trace_level(new_level);
	
//
// Return to the caller
//

	CORBA::Any *ret = return_empty_any("DevSetTraceLevelCmd");
	return ret;
#endif
}


//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceLevelCmd::DevGetTraceLevelCmd()
// 
// description : 	constructor for the DevGetTraceLevel command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevGetTraceLevelCmd::DevGetTraceLevelCmd(const char *name,
			     	   	 Tango::CmdArgType in,
			     	   	 Tango::CmdArgType out,
					 const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceLevelCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "DevGetTraceLevel" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevGetTraceLevelCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevGetTraceLevelCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO

  	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"GetTraceLevel is no more supported, please use GetLoggingLevel",
      				(const char *)"DevGetTraceLevelCmd::execute");

//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevGetTraceLevelCmd");
	return ret;

#else // TANGO_HAS_LOG4TANGO 
	
//
// Get level
//

	long level = Tango::Util::instance()->get_trace_level();
	
//
// return data to the caller
//
	
	CORBA::Any *out_any = new CORBA::Any();
	(*out_any) <<= level;
	
	cout4 << "Leaving DevGetTraceLevelCmd::execute()" << endl;	
	return(out_any);
#endif
}


//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceOutputCmd::DevGetTraceOutputCmd()
// 
// description : 	constructor for the DevGetTraceoutput command of the 
//			DServer.
//
//-----------------------------------------------------------------------------

DevGetTraceOutputCmd::DevGetTraceOutputCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceOutputCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "DevGetTraceOutput" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevGetTraceOutputCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevGetTraceOutputCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO
	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"GetTraceOutput is no more supported, please use GetLoggingTarget",
      				(const char *)"DevGetTraceOutputCmd::execute");
//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevGetTraceOutputCmd");
	return ret;

#else
	
//
// Get Trace output
//

	string st = Tango::Util::instance()->get_trace_output();
	
//
// return data to the caller
//
	
	CORBA::Any *out_any = new CORBA::Any();
	(*out_any) <<= st.c_str();
	
	cout4 << "Leaving DevGetTraceOutputCmd::execute()" << endl;	
	return(out_any);
#endif
}

//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceOutputCmd::DevSetTraceOutputCmd()
// 
// description : 	constructor for the DevSetTraceoutput command of the 
//			DServer.
//
//-----------------------------------------------------------------------------


DevSetTraceOutputCmd::DevSetTraceOutputCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceOutputCmd::execute(string &s)
// 
// description : 	method to trigger the execution of the "Kill" 
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevSetTraceOutputCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevSetTraceOutputCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO

	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"SetTraceOutput is no more supported, please use AddLoggingTarget",
      				(const char *)"DevSetTraceOutputCmd::execute");
//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevSetTraceOutputCmd");
	return ret;

#else

//
// Extract the input string and try to create a output file stream from it
//

	const char *in_file_ptr;
	if ((in_any >>= in_file_ptr) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				      (const char *)"Imcompatible command argument type, expected type is : string",
				      (const char *)"DevSetTraceOutputCmd::execute");
	}
	string in_file(in_file_ptr);
	cout4 << "Received string = " << in_file << endl;

	if (in_file == InitialOutput)
	{
		delete(Tango::Util::instance()->get_trace_output_stream());
		Tango::Util::instance()->set_trace_output_stream((ofstream *)NULL);
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
		ostream &tmp_stream = Tango::Util::instance()->get_out();

//
// For windows, the stdc++ library also implements the new IOstreams where the
// xx_with_assign classes do not exist. To copy stream, I have used the advice 
// from the C++ report of June 1997
//
		
		cout.copyfmt(tmp_stream);
		cout.clear(tmp_stream.rdstate());
		cout.rdbuf(tmp_stream.rdbuf());
#else
		cout = Tango::Util::instance()->get_out();
#endif
		Tango::Util::instance()->set_trace_output(in_file);
	}
	else
	{	
		ofstream *ofp = new ofstream(in_file_ptr);
		if (ofp->good())
		{
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
			cout.copyfmt(*ofp);
			cout.clear(ofp->rdstate());
			cout.rdbuf(ofp->rdbuf());
#else
			cout = *ofp;
#endif
			delete(Tango::Util::instance()->get_trace_output_stream());
			Tango::Util::instance()->set_trace_output(in_file);
			Tango::Util::instance()->set_trace_output_stream(ofp);
		}
		else
		{
			cout3 << "Cannot open ofstream" << endl;
			TangoSys_OMemStream o;
		
			o << "Impossible to open file " << in_file << ends;
			Except::throw_exception((const char *)"API_CannotOpenFile",
					      o.str(),
					      (const char *)"DevSetTraceoutput::execute");
		}
	}
			
//
// return to the caller
//
	
	CORBA::Any *ret = return_empty_any("DevSetTraceOutputCmd");
	return ret;
#endif 	
}


DServerClass *DServerClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		DServerClass::DServerClass()
// 
// description : 	constructor for the DServerClass class
//			The constructor add specific class commands to the
//			command list, create a device of the DServer class
//			retrieve all classes which must be created within the
//			server and finally, creates these classes
//
// argument : in : 	- s : The class name
//
//-----------------------------------------------------------------------------

bool less_than_dserver (Command *a,Command *b)
{
	if (a->get_name() < b->get_name())
		return true;
	else
		return false;
}

DServerClass::DServerClass(string &s):DeviceClass(s)
{
	try
	{
	
//
// Add class command(s) to the command_list
//

		command_factory();

//
// Sort commands
//

		sort(get_command_list().begin(),get_command_list().end(),less_than_dserver);
		
//
// Create device name from device server name
//

		string dev_name(DSDeviceDomain);
		dev_name.append(1,'/');
		dev_name.append(Tango::Util::instance()->get_ds_exec_name());
		dev_name.append(1,'/');
		dev_name.append(Tango::Util::instance()->get_ds_inst_name());
		
		Tango::DevVarStringArray dev_list(1);
                dev_list.length(1);
		dev_list[0] = dev_name.c_str();		
				
//
// Create the device server device
//

		device_factory(&dev_list);
				
	}
	catch (bad_alloc)
	{
		for (unsigned long i = 0;i < command_list.size();i++)
			delete command_list[i];
		command_list.clear();
		
		if (device_list.empty() == false)
		{
			for (unsigned long i = 0;i < device_list.size();i++)
				delete device_list[i];
			device_list.clear();
		}
		cerr << "Can't allocate memory while building the DServerClass object" << endl;
		throw;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServerClass::Instance
// 
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
//-----------------------------------------------------------------------------

DServerClass *DServerClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class is not initialised!" << endl;
		exit(-1);	
	}		
	return _instance;
}

DServerClass *DServerClass::init()
{
	if (_instance == NULL)
	{
		try
		{
			string s("DServer");
			_instance = new DServerClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}		
	}		
	return _instance;
}

//+----------------------------------------------------------------------------
//
// method : 		DServerClass::command_factory
// 
// description : 	Create the command object(s) and store them in the 
//			command list
//
//-----------------------------------------------------------------------------

void DServerClass::command_factory()
{
	command_list.push_back(new DevRestartCmd("DevRestart",
						 Tango::DEV_STRING,
						 Tango::DEV_VOID,
						 "Device name"));
	command_list.push_back(new DevRestartServerCmd("RestartServer",
						       Tango::DEV_VOID,
						       Tango::DEV_VOID));
	command_list.push_back(new DevQueryClassCmd("QueryClass",
						    Tango::DEV_VOID,
						    Tango::DEVVAR_STRINGARRAY,
						    "Device server class(es) list"));
	command_list.push_back(new DevQueryDeviceCmd("QueryDevice",
						     Tango::DEV_VOID,
						     Tango::DEVVAR_STRINGARRAY,
						     "Device server device(s) list"));
	command_list.push_back(new DevKillCmd("Kill",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID));
							
//
// Now, commands related to polling
//

	command_list.push_back(new PolledDeviceCmd("PolledDevice",
						   Tango::DEV_VOID,
						   Tango::DEVVAR_STRINGARRAY,
						   "Polled device name list"));
	command_list.push_back(new DevPollStatusCmd("DevPollStatus",
						   Tango::DEV_STRING,
						   Tango::DEVVAR_STRINGARRAY,
						   "Device name",
						   "Device polling status"));
	string msg("Lg[0]=Upd period.");
	msg = msg + (" Str[0]=Device name");
	msg = msg + (". Str[1]=Object type");
	msg = msg + (". Str[2]=Object name");
	
	command_list.push_back(new AddObjPollingCmd("AddObjPolling",
						    Tango::DEVVAR_LONGSTRINGARRAY,
						    Tango::DEV_VOID,
						    msg));
						    
	command_list.push_back(new UpdObjPollingPeriodCmd("UpdObjPollingPeriod",
							  Tango::DEVVAR_LONGSTRINGARRAY,
							  Tango::DEV_VOID,
							  msg));

	msg = "Str[0]=Device name. Str[1]=Object type. Str[2]=Object name";
								  
	command_list.push_back(new RemObjPollingCmd("RemObjPolling",
						    Tango::DEVVAR_STRINGARRAY,
						    Tango::DEV_VOID,
						    msg));
						    
	command_list.push_back(new StopPollingCmd("StopPolling",
						  Tango::DEV_VOID,
						  Tango::DEV_VOID));
						  
	command_list.push_back(new StartPollingCmd("StartPolling",
						   Tango::DEV_VOID,
						   Tango::DEV_VOID));

#ifdef TANGO_HAS_LOG4TANGO
  	msg = "Str[i]=Device-name. Str[i+1]=Target-type::Target-name";

  	command_list.push_back(new AddLoggingTarget("AddLoggingTarget",
			  			    Tango::DEVVAR_STRINGARRAY,
			  			    Tango::DEV_VOID,
			  			    msg));

  	command_list.push_back(new RemoveLoggingTarget("RemoveLoggingTarget",
			  			       Tango::DEVVAR_STRINGARRAY,
			  			       Tango::DEV_VOID,
			  			       msg));

  	command_list.push_back(new GetLoggingTarget("GetLoggingTarget",
			       			    Tango::DEV_STRING,
			  			    Tango::DEVVAR_STRINGARRAY,
			  			    std::string("Device name"),
			  			    std::string("Logging target list")));

	command_list.push_back(new SetLoggingLevel("SetLoggingLevel",
			  			   Tango::DEVVAR_LONGSTRINGARRAY,
			  			   Tango::DEV_VOID,
			  			   std::string("Lg[i]=Logging Level. Str[i]=Device name.")));

	command_list.push_back(new GetLoggingLevel("GetLoggingLevel",
			  			   Tango::DEVVAR_STRINGARRAY,
			  			   Tango::DEVVAR_LONGSTRINGARRAY,
			  			   std::string("Device list"),
			  			   std::string("Lg[i]=Logging Level. Str[i]=Device name.")));

	command_list.push_back(new StopLogging("StopLogging",
			  		       Tango::DEV_VOID,
			  		       Tango::DEV_VOID));

	command_list.push_back(new StartLogging("StartLogging",
			  			Tango::DEV_VOID,
			  			Tango::DEV_VOID));
#else
	command_list.push_back(new DevSetTraceLevelCmd("SetTraceLevel",
						       Tango::DEV_LONG,
						       Tango::DEV_VOID,
						       "New trace level"));
	command_list.push_back(new DevGetTraceLevelCmd("GetTraceLevel",
						       Tango::DEV_VOID,
						       Tango::DEV_LONG,
						       "Device server trace level"));
	command_list.push_back(new DevSetTraceOutputCmd("SetTraceOutput",
						        Tango::DEV_STRING,
						        Tango::DEV_VOID,
							"New device server output file"));
	command_list.push_back(new DevGetTraceOutputCmd("GetTraceOutput",
						        Tango::DEV_VOID,
						        Tango::DEV_STRING,
							"Device server output file"));
#endif // TANGO_HAS_LOG4TANGO
	command_list.push_back(new EventSubscriptionChangeCmd("EventSubscriptionChange",
							Tango::DEVVAR_STRINGARRAY, Tango::DEV_VOID,
							"list of events consumer wants to subscribe to",
							"none"));
	command_list.push_back(new EventObtainOfferedTypesCmd("EventObtainOfferedTypes",
							Tango::DEV_VOID, Tango::DEVVAR_STRINGARRAY,
							"none",
							"list of subscribed events"));

}


//+----------------------------------------------------------------------------
//
// method : 		DServerClass::device_factory
// 
// description : 	Create the device object(s) and store them in the 
//			device list
//
// in :			Tango::DevVarStringArray *devlist_ptr :
//			The device name list
//
//-----------------------------------------------------------------------------

void DServerClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{
	
	for (unsigned long i = 0;i < devlist_ptr->length();i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i] << endl;
						
//
// Create device and add it into the device list
//

		device_list.push_back(new DServer(this,
						  (*devlist_ptr)[i],
						  "A device server device !!",
						  Tango::ON,
						  "The device is ON"));
							 

//
// Export device to the outside world
//

		if (Tango::Util::_UseDb == true)
			export_device(device_list.back());
		else
			export_device(device_list.back(),(*devlist_ptr)[i]);
	}
}

} // End of Tango namespace
