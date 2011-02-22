//=============================================================================
//
// file :               except.h
//
// description :        Include for exception related utilities   
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.2  2007/03/02 09:49:06  jensmeyer
// Added the method compare_exception() to find out whether to DevFailed
// exceptions are equal or different.
//
// Revision 3.1  2005/06/29 08:31:18  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.0  2003/03/25 16:43:22  taurel
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
// Revision 2.8  2002/12/16 12:07:19  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:22  taurel
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
// Revision 2.3  2002/07/02 15:22:25  taurel
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
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:13  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:11  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:14  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:45  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:16  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:43  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:17  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _EXCEPT_H
#define _EXCEPT_H

#include <string>

#include <idl/tango.h>

using namespace std;

namespace Tango
{

//=============================================================================
//
//			The Except class
//
// description :	This class is a container for all exceptions related
//			methods to be used in aa Tango device server/client.
//			Most of these methods are static.
//
//=============================================================================

#define NOSPACEINDOC_EXCEPT

/**
 * Container class for all exception related methods. Most of these methods are
 * static methods
 *
 * $Author$
 * $Revision$
 */
 
class Except
{
public:

/**@name Exception related method */
//@{	
/**
 * Print a TANGO exception.
 *
 * Print all the details of a TANGO exception.
 *
 * @param ex The exception object reference
 */	
	static void print_exception(const CORBA::Exception &ex);
//@}

/**@name Error stack related method */
//@{	
/**
 * Print a TANGO error stack.
 *
 * Print all the details of a TANGO error stack.
 *
 * @param ex The error stack reference
 */	
	static void print_error_stack(const Tango::DevErrorList &ex);

//@}

/**@name Throw exception inline methods (static) */
//@{
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void throw_exception(const char *reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].desc = CORBA::string_dup(desc);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].desc = CORBA::string_dup(desc);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].reason = CORBA::string_dup(reason);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void throw_exception(const char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
			
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin, reason and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
	
		throw Tango::DevFailed(errors);
	}
			
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const string &reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(const string &reason,
					   const string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const string &reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(desc);
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 		
	static inline void throw_exception(const string &reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(const char *reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(const char *reason,
					   const string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const char *reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	
	static inline void throw_exception(const string &reason,
					   const string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const string &reason,
					   char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(desc);
		delete[] desc;
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 		
	static inline void throw_exception(const string &reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
		delete desc;
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(char *reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(char *reason,
					   const string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(desc);
		delete[] desc;
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}

//@}

/**@name Re-throw exception inline methods (static) */
//@{
/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();	
		ex.errors.length(nb_err + 1);
		
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
	
		throw ex;
	}
	
/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;
			
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
 					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin, reason and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;
	
		throw ex;
	}
			
/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin.c_str());
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 		
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin.c_str());
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		delete[] desc;
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 		
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete desc;
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin.c_str());
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   const string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());
	
		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);
	
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		delete[] desc;
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());
	
		throw ex;
	}			
//@}

/**@name Other throw exception methods */
//@{	
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is 
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	static void throw_exception(const CORBA::SystemException &ex,const char *origin);

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is 
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field. The memory
 * allocated for this parameter will be freed by this method.
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	static void throw_exception(const CORBA::SystemException &ex,char *origin);

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is 
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field. The memory
 * allocated for this parameter will be freed by this method.
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	static void throw_exception(const CORBA::SystemException &ex,const string &origin);
	
	
/**
 * Compare two Tango DevFailed exceptions for equality
 *
 * The two DevFailed exceptions are verified by comparing the
 * reason, origin, description and severity of all exceptions in the error stack.
 * The strings reason, origin and description are compared literally.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex1 The first DevFailed exception
 * @param ex2 The second DevFailed exception
 * @return A boolean set to true if the two exceptions are equal
 */	
	static bool compare_exception(Tango::DevFailed &ex1, Tango::DevFailed &ex2);	
	
		
//@}

	static char *print_CORBA_SystemException(const CORBA::SystemException *);

protected:
	static char mess[256];
};

} // End of Tango namespace
		     
#endif /* EXCEPT */
