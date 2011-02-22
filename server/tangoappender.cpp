static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         tangoappender.cpp
//
// description :  
//
// project :      TANGO
//
// author(s) :    N.Leclercq - SOLEIL
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
//
// $Revision$
//
// $Log$
// Revision 3.8  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.7  2008/10/03 06:53:09  taurel
// - Add some licensing info in each files
//
// Revision 3.6  2007/04/20 14:41:34  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.5  2007/04/16 14:57:45  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.4  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.3  2003/08/21 07:24:37  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.2.2.6  2004/03/09 16:36:37  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.2.2.5  2004/03/02 07:41:57  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.2.2.4  2003/11/14 12:28:43  nleclercq
// Added Register/UnRegister to the logconsumer interface
//
// Revision 3.2.2.2  2003/10/10 16:54:07  nleclercq
// (re)Activated asynch logging after omniORB bug fix
//
// Revision 3.2.2.1  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.3  2003/08/21 07:24:37  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.2  2003/05/28 14:55:10  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.1  2003/04/03 10:35:04  nleclercq
// Few changes to test asynch logging
//
// Revision 3.0  2003/03/25 16:47:50  taurel
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
// Revision 2.11  2003/03/20 10:41:14  nleclercq
// Final touch on bug fix (reentry pb)
//
// Revision 2.10  2003/03/19 17:52:34  nleclercq
// Few minor modifications
//
// Revision 2.9  2003/03/19 09:18:09  nleclercq
// *** empty log message ***
//
// Revision 2.8  2003/03/18 18:14:10  nleclercq
// Fixed reentry pb in log4tango::Logger::call_appenders (bug finally found)
//
// Revision 2.7  2003/03/18 14:51:09  nleclercq
// Added async call support
//
// Revision 2.6  2003/03/17 12:46:02  nleclercq
// Fixed #include order
//
// Revision 2.5  2003/03/17 08:51:16  taurel
// Adapt new error management to old Streams still used with gcc 2.95
//
// Revision 2.4  2003/03/14 15:58:52  nleclercq
// Changed error management
//
// Revision 2.3  2003/03/13 15:17:50  nleclercq
// Minor modifications on logging implementation
//
// Revision 2.2  2003/03/11 17:55:57  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:45  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
#include <iomanip>
#else
#include <iomanip.h>
#endif

#ifdef TANGO_HAS_LOG4TANGO

#include <tangoappender.h>

#define USE_ASYNC_CALL 

namespace Tango
{
  TangoAppender::TangoAppender (const std::string& src_name,
                                const std::string& name,
                                const std::string& dev_name,
                                bool open_connection)
    : log4tango::Appender(name), 
      _dev_name(dev_name), 
      _src_name(src_name), 
      _dev_proxy(0)
  {
    if (open_connection == true)
      reopen();
  }

  TangoAppender::~TangoAppender ()
  {
    close();
  }

  bool TangoAppender::requires_layout (void) const
  {
    return false; 
  }
      
  void TangoAppender::set_layout (log4tango::Layout*)
  {
    // no-op
  }

  bool TangoAppender::is_valid (void) const
  {
    if (!_dev_proxy) {
      return false;
    }
    try {
       _dev_proxy->ping();
    } catch (...) {
      return false;
    }
    return true;
  }

  int TangoAppender::_append (const log4tango::LoggingEvent& event)
  {
    //------------------------------------------------------------
    //- DO NOT LOG FROM THIS METHOD !!!
    //------------------------------------------------------------
    if (!_dev_proxy) {
      //--DO NOT RETURN -1 (ERROR ALREADY HANDLED)
      return 0;
    }
    try {
      Tango::DevVarStringArray *dvsa = new Tango::DevVarStringArray(6);
      if (dvsa) {
        dvsa->length(6);
        double ts_ms = 1000. * event.timestamp.get_seconds();
        ts_ms += event.timestamp.get_milliseconds();
        TangoSys_OMemStream ts_ms_str;
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
        ts_ms_str << std::fixed 
                  << std::noshowpoint 
                  << std::setprecision(0)  
                  << ts_ms 
                  << ends;
	string st = ts_ms_str.str();
        (*dvsa)[0] = CORBA::string_dup(st.c_str());          
#else
        ts_ms_str.unsetf(ios::showpoint);
	      ts_ms_str.setf(ios::fixed);
        ts_ms_str << setprecision(0) 
                  << ts_ms 
                  << ends;
        (*dvsa)[0] = CORBA::string_dup(ts_ms_str.str()); 
#endif
        (*dvsa)[1] = CORBA::string_dup(log4tango::Level::get_name(event.level).c_str());
        (*dvsa)[2] = CORBA::string_dup(event.logger_name.c_str());
        (*dvsa)[3] = CORBA::string_dup(event.message.c_str());
        (*dvsa)[4] = CORBA::string_dup("");
        omni_thread* ct = omni_thread::self();
        if (ct) {
          TangoSys_OMemStream ctstr;
          ctstr << "@" << hex << event.thread_id << " [" << ct->id() << "]"<< ends;

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
	   string st = ctstr.str();
          (*dvsa)[5] = CORBA::string_dup(st.c_str());          
#else
          (*dvsa)[5] = CORBA::string_dup(ctstr.str()); 
#endif
        } else {
          (*dvsa)[5] = CORBA::string_dup("unknown");
        }
        DeviceData argin;
        argin << dvsa;
#ifdef USE_ASYNC_CALL 
        _dev_proxy->command_inout_asynch("Log", argin, true);
#else
        _dev_proxy->command_inout("Log", argin); 
#endif
      }
    }
    catch (...) {
      close();
      return -1;
    }
    return 0;
  } 

  bool TangoAppender::reopen (void) 
  { 
    bool result = true;
    try {
      close();
      _dev_proxy = new DeviceProxy(const_cast<std::string&>(_dev_name));
      try {
        DeviceData argin;
        argin << const_cast<std::string&>(_src_name);
#ifdef USE_ASYNC_CALL 
        _dev_proxy->command_inout_asynch("Register", argin, true);
#else
       _dev_proxy->command_inout("Register", argin); 
#endif
      }
      catch (...) {

      }
    }
    catch (...) {
      close();
      result = false;
    }
    return result;
  }

  void TangoAppender::close (void)
  { 
    if (_dev_proxy) {
      try {
        DeviceData argin;
        argin << const_cast<std::string&>(_src_name);
#ifdef USE_ASYNC_CALL 
        _dev_proxy->command_inout_asynch("UnRegister", argin, true);
#else
        _dev_proxy->command_inout("UnRegister", argin); 
#endif
      }
      catch (...) {
        // Ignore error: some old logviewer may not support UnRegister 
      }
      delete _dev_proxy;
      _dev_proxy = 0;
    }
  }

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO
