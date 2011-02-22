static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               DServerSignal.cpp
//
// description :        C++ source for the DServer class and its commands. 
//			The class is derived from Device. It represents the
//			CORBA servant object which will be accessed from the
//			network. All commands which can be executed on a
//			DServer object are implemented in this file.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
// $Log$
// Revision 3.14  2008/10/06 15:01:09  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.13  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.12  2008/04/04 14:25:02  jensmeyer
// Added the necessary signal definitions for FreeBSD __freebsd__
//
// Revision 3.11  2008/03/27 16:25:00  jensmeyer
// Changed errno definition for Windows.
//
// Revision 3.10  2008/02/28 12:29:31  jensmeyer
// Added signal definitions for MacOSX (__darwin__).
//
// Revision 3.9  2007/04/20 14:40:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.8  2007/04/16 14:56:36  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.7  2005/05/09 15:34:11  taurel
// - Fix deadlock when read_attribute_no_cache try to start attribute polling
// - Signal management for Sun
//
// Revision 3.6  2005/04/15 11:34:07  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.5  2005/01/13 08:29:05  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.4.2.1  2004/08/19 07:44:59  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.4  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.3  2003/07/03 07:40:51  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.2.2.4  2004/05/17 15:26:48  taurel
// - Add SA_RESTART flag when signals are installed using sigaction()
//
// Revision 3.2.2.3  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.2.2.2  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.2.2.1  2003/09/30 11:50:43  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.2  2003/05/28 14:55:09  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.1  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.0  2003/03/25 16:43:21  taurel
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
// Revision 1.6  2001/10/08 09:03:13  taurel
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
// Revision 1.2  2001/03/09 08:20:16  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:21  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:15  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <new>
#include <dserversignal.h>

#ifndef _TG_WINDOWS_
extern int errno;
#endif

namespace Tango
{

DServerSignal *DServerSignal::_instance = NULL;
DevSigAction DServerSignal::reg_sig[_NSIG];
string DServerSignal::sig_name[_NSIG];
#ifdef _TG_WINDOWS_
HANDLE DServerSignal::win_ev = NULL;
int DServerSignal::win_signo = 0;
#endif


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::Instance()
// 
// description : 	Instance method for DServerSignal object. This class is
//			a singleton and this method creates the object the
//			first time it is called or simply returns a pointer
//			to the already created object for all the other calls.
//
//-----------------------------------------------------------------------------

DServerSignal *DServerSignal::instance()
{
	if (_instance == NULL)
	{
		try
		{
			_instance = new DServerSignal();
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
// method : 		DServerSignal::DServerSignal()
// 
// description : 	constructor for DServerSignal object. As this class is
//			a singleton, this method is protected
//
//-----------------------------------------------------------------------------

DServerSignal::DServerSignal():TangoMonitor("signal")
{
	cout4 << "Entering DServerSignal constructor" << endl;
	
//
// Set array of signal name
//

#ifdef _TG_WINDOWS_
	sig_name[SIGINT] = "SIGINT";
	sig_name[SIGILL] = "SIGILL";
	sig_name[SIGFPE] = "SIGFPE";
	sig_name[SIGSEGV] = "SIGSEGV";
	sig_name[SIGTERM] = "SIGTERM";
	sig_name[SIGBREAK] = "SIGBREAK";
	sig_name[SIGABRT] = "SIGABRT";
#else
	sig_name[SIGHUP] = "SIGHUP";
	sig_name[SIGINT] = "SIGINT";
	sig_name[SIGQUIT] = "SIGQUIT";
	sig_name[SIGILL] = "SIGILL";
	sig_name[SIGTRAP] = "SIGTRAP";
	sig_name[SIGABRT] = "SIGABRT";
	sig_name[SIGFPE] = "SIGFPE";
	sig_name[SIGKILL] = "SIGKILL";
	sig_name[SIGBUS] = "SIGBUS";
	sig_name[SIGSEGV] = "SIGSEGV";
	sig_name[SIGPIPE] = "SIGPIPE";
	sig_name[SIGALRM] = "SIGALRM";
	sig_name[SIGTERM] = "SIGTERM";
	sig_name[SIGUSR1] = "SIGUSR1";
	sig_name[SIGUSR2] = "SIGUSR2";
	sig_name[SIGCHLD] = "SIGCHLD";
	sig_name[SIGVTALRM] = "SIGVTALRM";
	sig_name[SIGPROF] = "SIGPROF";
	sig_name[SIGIO] = "SIGIO";
	sig_name[SIGWINCH] = "SIGWINCH";
	sig_name[SIGSTOP] = "SIGSTOP";
	sig_name[SIGTSTP] = "SIGTSTP";
	sig_name[SIGCONT] = "SIGCONT";
	sig_name[SIGTTIN] = "SIGTTIN";
	sig_name[SIGTTOU] = "SIGTTOU";
	sig_name[SIGURG]  = "SIGURG";
	sig_name[SIGSYS]  = "SIGSYS";
#ifdef linux
	sig_name[SIGXCPU] = "SIGXCPU";
	sig_name[SIGXFSZ] = "SIGXFSZ";
	sig_name[SIGCLD]  = "SIGCLD";
	sig_name[SIGPWR]  = "SIGPWR";
#else
#ifdef sun
	sig_name[SIGCLD]  = "SIGCLD";
	sig_name[SIGPWR]  = "SIGPWR";
	sig_name[SIGLOST] = "SIGLOST";
	sig_name[SIGEMT]  = "SIGEMT";
#else
#ifdef __darwin__
	sig_name[SIGEMT]  = "SIGEMT";
	sig_name[SIGINFO] = "SIGINFO";
#else
#ifdef __freebsd__
	sig_name[SIGXCPU] = "SIGXCPU";
	sig_name[SIGXFSZ] = "SIGXFSZ";
#endif /* __freebsd__ */
#endif /* __darwin__ */
#endif /* sun */
#endif /* linux */
#endif /* _TG_WINDOWS_ */

	TangoSys_OMemStream o;	
	for (long i = 0;i < _NSIG;i++)
	{
		if (sig_name[i].size() == 0)
		{
			o << i << ends;
			sig_name[i] = o.str();
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			o.seekp(0);
			o.clear();
#else
			o.rdbuf()->freeze(false);
#endif
		}
	}

	sig_to_install = false;
	sig_to_remove = false;

#ifdef sun
//
// With Solaris, the SIGINT and SIGQUIT defalut actions are set to SIG_IGN for
// all processes started in the background. Signal SIGINT is used by
// Tango in its signal management (due to pre 2.6 kernel thread management
// on Linux), reset the default action to default
//

	sigset(SIGINT, SIG_DFL);
	sigset(SIGQUIT, SIG_DFL);
#endif

#ifndef _TG_WINDOWS_
	
//
// Block signals in thread other than the thread dedicated to signal
//

	sigset_t sigs_to_block;
	sigemptyset(&sigs_to_block);
	
	sigfillset(&sigs_to_block);

	sigdelset(&sigs_to_block,SIGABRT);
	sigdelset(&sigs_to_block,SIGKILL);
	sigdelset(&sigs_to_block,SIGILL);
	sigdelset(&sigs_to_block,SIGTRAP);
	sigdelset(&sigs_to_block,SIGIOT);
	sigdelset(&sigs_to_block,SIGFPE);
	sigdelset(&sigs_to_block,SIGBUS);
	sigdelset(&sigs_to_block,SIGSEGV);
	sigdelset(&sigs_to_block,SIGSYS);
	sigdelset(&sigs_to_block,SIGPIPE);
	sigdelset(&sigs_to_block,SIGSTOP);
	
	sigdelset(&sigs_to_block,SIGTSTP);
#if (defined __linux)
	sigdelset(&sigs_to_block,SIGUSR1);
	sigdelset(&sigs_to_block,SIGUSR2);
#endif
#ifdef sun
	sigdelset(&sigs_to_block,SIGEMT);
	sigprocmask(SIG_BLOCK,&sigs_to_block,NULL);
#endif
	sigprocmask(SIG_BLOCK,&sigs_to_block,NULL);
#else /* _TG_WINDOWS_ */
	win_ev = CreateEvent(NULL,FALSE,FALSE,NULL);

	register_handler(SIGINT);
	register_handler(SIGTERM);
	register_handler(SIGABRT);
	if (Util::_service == false)
		register_handler(SIGBREAK);
#endif /* _TG_WINDOWS_ */

//
// Start the thread dedicated to signal
//
	
	sig_th = new ThSig(this);
	sig_th->start();			
	
	cout4 << "leaving DServerSignal constructor" << endl;
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::register_class_signal()
// 
// description : 	method to register a signal handler at the class
//			level
//
// in :			long signo - Signal number
//			DeviceClass *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

#if !(defined __linux)
void DServerSignal::register_class_signal(long signo,DeviceClass *cl_ptr)
#else
void DServerSignal::register_class_signal(long signo,bool handler,DeviceClass *cl_ptr)
#endif
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}
	
	if (auth_signal(signo) == false)
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized with your OS" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}
	
#if (defined __linux)
	if ((auto_signal(signo) == true) && (handler == true))
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized using your own handler" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}
#endif	
		
//
// If nothing is registered for this signal, install the OS signal handler
//

	bool installed = false;
	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_devices.empty() == true) &&
		    (reg_sig[signo].registered_classes.empty() == true))
		{
#if !(defined __linux)
	    		register_handler(signo);
#else
			register_handler(signo,handler);
#endif
			installed = true;
		}
	}
			
//
// Check if class is already registered for this signal. If it is already done,
// leave method. Otherwise, record class pointer
//

	vector<DeviceClass *>::iterator f = find_class(signo,cl_ptr);
	
	if (f == reg_sig[signo].registered_classes.end())
	{
		reg_sig[signo].registered_classes.push_back(cl_ptr);
#if (defined __linux)
		reg_sig[signo].own_handler = handler;
#endif
	}			
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::find_class
// 
// description : 	method to check if a class is already registered for a 
//			signal. If it is true, this method returns in which
//			element of the vector the class is registered
//
// in :			long signo - Signal number
//			DeviceClass *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

vector<DeviceClass *>::iterator DServerSignal::find_class(long signo,DeviceClass *cl_ptr)
{
	vector<DeviceClass *>::iterator p;		
	for (p = reg_sig[signo].registered_classes.begin();p < reg_sig[signo].registered_classes.end();p++)
	{
		if ((*p) == cl_ptr)
			break;
	}
	return p;
}


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::register_dev_signal()
// 
// description : 	method to register a signal handler at the device
//			level
//
// in :			long signo - Signal number
//			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

#if !(defined __linux)
void DServerSignal::register_dev_signal(long signo,DeviceImpl *dev_ptr)
#else
void DServerSignal::register_dev_signal(long signo,bool handler,DeviceImpl *dev_ptr)
#endif
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_dev_signal");
	}

	if (auth_signal(signo) == false)
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized with your OS" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_dev_signal");
	}
	
#if (defined __linux)
	if ((auto_signal(signo) == true) && (handler == true))
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized using your own handler" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}
#endif
			
//
// If nothing is registered for this signal, install the OS signal handler
//

	bool installed = false;
	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_devices.empty() == true) &&
		    (reg_sig[signo].registered_classes.empty() == true))
		{
#if !(defined __linux)
	    		register_handler(signo);
#else
			register_handler(signo,handler);
#endif
			installed = true;
		}
	}
			
//
// Check if devices is already registered for this signal. If it is already done,
// leave method. Otherwise, record class pointer
//


	vector<DeviceImpl *>::iterator f = find_device(signo,dev_ptr);
	
	if (f == reg_sig[signo].registered_devices.end())
	{
		reg_sig[signo].registered_devices.push_back(dev_ptr);
#if (defined __linux)
		reg_sig[signo].own_handler = handler;
#endif
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::find_device
// 
// description : 	method to check if a device is already registered for a 
//			signal. If it is true, this method returns in which
//			element of the vector the device is registered
//
// in :			long signo - Signal number
//			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

vector<DeviceImpl *>::iterator DServerSignal::find_device(long signo,DeviceImpl *dev_ptr)
{	
	vector<DeviceImpl *>::iterator p;		
	for (p = reg_sig[signo].registered_devices.begin();p < reg_sig[signo].registered_devices.end();p++)
	{
		if ((*p) == dev_ptr)
			break;
	}
	return p;
}


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_class_signal()
// 
// description : 	method to unregister a signal handler at the class
//			level
//
// in :			long signo - Signal number
//			DeviceClass *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_class_signal(long signo,DeviceClass *cl_ptr)
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_proc_signal");
	}
				
//
// Check if class is already registered for this signal. If it is already done,
// leave method. Otherwise, record class pointer
//

	vector<DeviceClass *>::iterator f = find_class(signo,cl_ptr);
	
	if (f == reg_sig[signo].registered_classes.end())
			return;
	else
		reg_sig[signo].registered_classes.erase(f);
				
//
// If nothing is registered for this signal, unregister the OS signal handler
// and (eventually) the event handler
//

	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_classes.empty() == true) &&
	    	    (reg_sig[signo].registered_devices.empty() == true))
	    		unregister_handler(signo);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_dev_signal()
// 
// description : 	method to unregister a signal handler at the class
//			level
//
// in :			long signo - Signal number
//			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_dev_signal(long signo,DeviceImpl *dev_ptr)
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_proc_signal");
	}
				
//
// Check if device is already registered for this signal. If yes, remove it.
// Otherwise, leave method
//


	vector<DeviceImpl *>::iterator f = find_device(signo,dev_ptr);
	
	if (f == reg_sig[signo].registered_devices.end())
		return;
	else
		reg_sig[signo].registered_devices.erase(f);
			
//
// If nothing is registered for this signal, unregister the OS signal handler
// and eventually the event handler
//

	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_classes.empty() == true) &&
	    	    (reg_sig[signo].registered_devices.empty() == true))
		{
	    		unregister_handler(signo);
		}
	}

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_dev_signal()
// 
// description : 	method to unregister a signal handler at the device
//			level for all signals
//
// in :			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_dev_signal(DeviceImpl *dev_ptr)
{
	long i;
	
	for (i = 0;i < _NSIG;i++)
	{
				
//
// Check if device is registered for this signal. If yes, remove it.
// Otherwise, go to next signal
//

		vector<DeviceImpl *>::iterator f = find_device(i,dev_ptr);
	
		if (f == reg_sig[i].registered_devices.end())
			continue;
		else
			reg_sig[i].registered_devices.erase(f);
				
//
// If nothing is registered for this signal, unregister the OS signal handler
//

		if (auto_signal(i) == false)
		{
			if ((reg_sig[i].registered_classes.empty() == true) &&
	    	    	    (reg_sig[i].registered_devices.empty() == true))
	    			unregister_handler(i);
		}
	}

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_class_signal()
// 
// description : 	method to unregister a signal handler at the class
//			level for all signals
//
// in :			DeviceImpl *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_class_signal(DeviceClass *cl_ptr)
{
	long i;
	
	for (i = 0;i < _NSIG;i++)
	{
				
//
// Check if classes is registered for this signal. If yes, remove it.
// Otherwise, go to next signal
//

		vector<DeviceClass *>::iterator f = find_class(i,cl_ptr);
	
		if (f == reg_sig[i].registered_classes.end())
			continue;
		else
			reg_sig[i].registered_classes.erase(f);
				
//
// If nothing is registered for this signal, unregister the OS signal handler
//

		if (auto_signal(i) == false)
		{
			if ((reg_sig[i].registered_classes.empty() == true) &&
	    	    	    (reg_sig[i].registered_devices.empty() == true))
	    			unregister_handler(i);
		}
	}

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::register_handler()
// 
// description : 	method to register in the OS the main signal handler 
//			for a given signal
//
// in :			long signo - Signal number
//
//-----------------------------------------------------------------------------

#if !(defined __linux)
void DServerSignal::register_handler(long signo)
#else
void DServerSignal::register_handler(long signo,bool handler)
#endif
{
	cout4 << "Installing OS signal handler for signal " << sig_name[signo] << endl;

#ifdef _TG_WINDOWS_
	if (::signal(signo, DServerSignal::main_sig_handler) == SIG_ERR)
	{
		TangoSys_OMemStream o;
		o << "Can't install signal " << signo << ". OS error = " << errno << ends;
		Except::throw_exception((const char *)"API_CantInstallSignal",
				      o.str(),
				      (const char *)"DServerSignal::register_handler");
	}
#else
	#ifdef sun
	
//
// For Solaris, a signal is correctly managed by thread if its not ignored.
// Install a dummy signal handler if the requested signal is one signal with
// default action is to ignore signal. These signals are SIGCHLD, SIGPWR,
// SIGWINCH, SIGURG, SIGCONT and SIGFREEZE
//

		if (ign_signal(signo) == true)
		{
			struct sigaction sa;
	
			sa.sa_flags = SA_RESTART;
			sa.sa_handler = DServerSignal::main_sig_handler;
			sigemptyset(&sa.sa_mask);
	
			if (sigaction((int)signo,&sa,0) == -1)
			{
				TangoSys_OMemStream o;
				o << "Can't install signal " << signo << ". OS error = " << errno << ends;
				Except::throw_exception((const char *)"API_CantInstallSignal",
				     			o.str(),
				      			(const char *)"DServerSignal::register_handler");
			}
		}
	#endif

	#if (defined __linux)
	if (handler == true)
	{
		sigset_t sigs_to_unblock;
		sigemptyset(&sigs_to_unblock);
		sigaddset(&sigs_to_unblock,signo);

		if (pthread_sigmask(SIG_UNBLOCK,&sigs_to_unblock,NULL) != 0)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				     		o.str(),
				      		(const char *)"DServerSignal::register_handler");
		}
		
		struct sigaction sa;
	
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = DServerSignal::main_sig_handler;
		sigemptyset(&sa.sa_mask);

		if (sigaction((int)signo,&sa,0) == -1)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				     		o.str(),
				      		(const char *)"DServerSignal::register_handler");
		}
	}
	else
	{
	#endif
	{
		omni_mutex_lock sy(*this);
		
		while(sig_to_install == true)
		{
			wait();
		}	
		sig_to_install = true;
		inst_sig = signo;
	}

	pthread_kill(sig_th->my_thread,SIGINT);
	#if (defined __linux)
	}
	#endif
#endif

}	

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_handler()
// 
// description : 	method to unregister from the OS the main signal handler 
//			for a given signal
//
// in :			long signo - Signal number
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_handler(long signo)
{
	cout4 << "Removing OS signal handler for signal " << sig_name[signo] << endl;


#ifdef _TG_WINDOWS_
	if (::signal(signo, SIG_DFL) == SIG_ERR)
	{
		TangoSys_OMemStream o;
		o << "Can't install signal " << signo << ". OS error = " << errno << ends;
		Except::throw_exception((const char *)"API_CantInstallSignal",
				      o.str(),
				      (const char *)"DServerSignal::register_handler");
	}
#else

	#ifdef sun
	if (ign_signal(signo) == true)
	{
		struct sigaction sa;
	
		sa.sa_flags = 0;
		sa.sa_handler = SIG_DFL;
		sigemptyset(&sa.sa_mask);
	
		if (sigaction((int)signo,&sa,0) == -1)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				      		o.str(),
				     		 (const char *)"DServerSignal::register_handler");
		}
	}
	#endif

	#if (defined __linux)
	if (reg_sig[signo].own_handler == true)
	{
		struct sigaction sa;
	
		sa.sa_flags = 0;
		sa.sa_handler = SIG_DFL;
		sigemptyset(&sa.sa_mask);
	
		if (sigaction((int)signo,&sa,0) == -1)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				      		o.str(),
				     		 (const char *)"DServerSignal::register_handler");
		}
	}
	else
	{
	#endif
	{
		omni_mutex_lock sy(*this);
		
		while(sig_to_remove == true)
		{
			wait();
		}	
		sig_to_remove = true;
		rem_sig = signo;
	}	
	pthread_kill(sig_th->my_thread,SIGINT);
	
	#if (defined __linux)
	}
	#endif
#endif
}

#if (defined __linux)
pid_t DServerSignal::get_sig_thread_pid()
{
	omni_mutex_lock syn(*this);

	if (sig_th->my_pid == 0)
	{	
		wait(1000);
	}
	return sig_th->my_pid;
}
#endif
//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::main_sig_handler()
// 
// description : 	This is a dummy signal handler used only with solaris
//			which needs one for signal with a default ignore
//			action to work correctly with the sigwait()
//			call.
//
// in :			int signo - Signal number
//
//-----------------------------------------------------------------------------

#ifndef _TG_WINDOWS_
	#if (defined __linux)
void DServerSignal::main_sig_handler(int signo)
{
	cout4 << "In main sig_handler !!!!" << endl;

	DevSigAction *act_ptr = &(DServerSignal::reg_sig[signo]);
		
//
// First, execute all the handlers installed at the class level
//

	if (act_ptr->registered_classes.empty() == false)
	{
		long nb_class = act_ptr->registered_classes.size();
		for (long j = 0;j < nb_class;j++)
		{
			act_ptr->registered_classes[j]->signal_handler((long)signo);
		}
	}
		
//
// Then, execute all the handlers installed at the device level
//

	if (act_ptr->registered_devices.empty() == false)
	{
		long nb_dev = act_ptr->registered_devices.size();
		for (long j = 0;j < nb_dev;j++)
		{
			act_ptr->registered_devices[j]->signal_handler((long)signo);
		}
	}
}
	#else
void DServerSignal::main_sig_handler(int signo)
{
	cout4 << "In main sig_handler !!!!" << endl;
}
	#endif
#else
void DServerSignal::main_sig_handler(int signo)
{
	cout4 << "In main sig_handler !!!!" << endl;

	win_signo = signo;	
	SetEvent(win_ev);
}
#endif

} // End of Tango namespace	
