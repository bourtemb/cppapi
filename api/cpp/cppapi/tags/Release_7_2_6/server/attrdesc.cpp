static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               attrdesc.cpp
//
// description :        C++ source code for the BlackBoxElt and BlackBox
//			classes. These classes are used to implement the 
//			tango device server black box. There is one
//			black box for each Tango device. This black box
//			keeps info. on all the activities on a device.
//			A client is able to retrieve these data via a Device
//			attribute
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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
// $Log$
// Revision 3.17  2010/09/09 13:44:46  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.16  2009/12/09 15:48:56  taurel
// - Add Attr and Attribute methods set_data_ready_event() and is_data_ready_event().
// - Admin device command EventSubcriptionChange fails is one of these methods has not been called
//
// Revision 3.15  2009/09/16 12:16:19  taurel
// - Better management of the RDS alarm properties. They were not taken into
// account by the set_default_properties() method
//
// Revision 3.14  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.13  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.12  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.11  2008/06/14 11:29:15  taurel
// - DevEncoded attribute data type implementation work going on
//
// Revision 3.10  2008/05/20 12:44:09  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.9.2.1  2008/05/20 06:17:44  taurel
// - Last commit before merge with trunk
// (start the implementation of the new DevEncoded data type)
//
// Revision 3.9  2007/04/16 14:56:36  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.8  2007/03/06 08:18:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.7  2006/04/27 13:44:51  taurel
// - Fix compatibility bug introduced in previous release by moving new
// class data members in their corresponding extension classes
//
// Revision 3.6  2006/04/13 14:49:37  jensmeyer
// Added methods to the Attr class to initialise the fire event configuration
// with pogo.
//
// Revision 3.5  2005/11/07 12:15:00  jensmeyer
// Corrected initialisation of memorised attributes. No longer exits when receiving an
// exception during initialisation. Also added an option to only initialise the attribute setpoint and not writing to the attribute.
// set_memorized_init(false) -> only applies setpoint.
// set_memorized_init(true) -> also writes setpoint value to the attribute.
//
// Revision 3.4  2005/01/13 08:27:32  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.3.2.1  2004/09/15 06:45:44  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.3  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.2  2003/08/21 07:23:45  taurel
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
// Revision 3.1  2003/05/28 14:55:06  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.0  2003/03/25 16:41:46  taurel
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
// Revision 2.9  2003/01/09 12:03:15  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:06:20  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:14:41  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:05  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:18  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:53  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:23  taurel
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
// Revision 2.2  2002/04/30 10:50:40  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:02  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:08  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:10  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:09  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:12  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:43  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:13  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attrdesc.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Attr::Attr 
// 
// description : 	Constructor for the Attr class.
//			This constructor simply set the internal values
//
//--------------------------------------------------------------------------

Attr::Attr(const char *att_name,long att_type,AttrWriteType att_writable,
           const char *assoc)
:name(att_name),writable(att_writable),type(att_type),assoc_name(assoc),
      mem(false),mem_init(true)
{
	format = Tango::SCALAR;
	ext = new AttrExt();
	
	ext->fire_change_event = false;
	ext->check_change_event = true;
	ext->fire_archive_event = false;
	ext->check_archive_event = true;
	ext->fire_dr_event = false;
	
	if (name != "State")
		check_type();
	
	if ((writable == Tango::WRITE) && (assoc_name != AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Associated attribute is not supported" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}
	
	if ((writable == Tango::READ_WITH_WRITE) && (assoc_name == AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Associated attribute not defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}
	
	if (writable == READ_WRITE)
		assoc_name = name;
		
}

Attr::Attr(const char *att_name,long att_type,DispLevel level,
	   AttrWriteType att_writable, const char *assoc)
:name(att_name),writable(att_writable),type(att_type),assoc_name(assoc),mem(false)
{
	format = Tango::SCALAR;
	ext = new AttrExt(level);
	if (name != "State")
		check_type();
	
	if ((writable == Tango::WRITE) && (assoc_name != AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Associated attribute is not supported" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}
	
	if ((writable == Tango::READ_WITH_WRITE) && (assoc_name == AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Associated attribute not defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}
	
	if (writable == READ_WRITE)
		assoc_name = name;
		
}

Attr::~Attr()
{
	delete ext;
}

//+-------------------------------------------------------------------------
//
// method : 		Attr::check_type 
// 
// description : 	This method checks data type and throws an exception
//			in case of unsupported data type
//
//--------------------------------------------------------------------------

void Attr::check_type()
{
	bool unsuported = true;
	
	if (type == Tango::DEV_SHORT)
		unsuported = false;
	else if (type == Tango::DEV_LONG)
		unsuported = false;
	else if (type == Tango::DEV_LONG64)
		unsuported = false;
	else if (type == Tango::DEV_DOUBLE)
		unsuported = false;
	else if (type == Tango::DEV_STRING)
		unsuported = false;
	else if (type == Tango::DEV_FLOAT)
		unsuported = false;
	else if (type == Tango::DEV_BOOLEAN)
		unsuported = false;
	else if (type == Tango::DEV_USHORT)
		unsuported = false;
	else if (type == Tango::DEV_UCHAR)
		unsuported = false;
	else if (type == Tango::DEV_ULONG)
		unsuported = false;
	else if (type == Tango::DEV_ULONG64)
		unsuported = false;
	else if (type == Tango::DEV_STATE)
		unsuported = false;
	else if (type == Tango::DEV_ENCODED)
		unsuported = false;

	if (unsuported == true)
	{
		cout3 << "Attr::check_type throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Data type is not supported" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::check_type");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attr::set_default_properties
// 
// description : 	This method set the default user properties in the
//			Attr object. At this level, each attribute property
//			is represented by one instance of the Attrproperty
//			class.
//
//--------------------------------------------------------------------------

void Attr::set_default_properties(UserDefaultAttrProp &prop_list)
{
	if (prop_list.label.empty() == false)
		user_default_properties.push_back(AttrProperty("label",prop_list.label));

	if (prop_list.description.empty() == false)
		user_default_properties.push_back(AttrProperty("description",prop_list.description));

	if (prop_list.unit.empty() == false)
		user_default_properties.push_back(AttrProperty("unit",prop_list.unit));

	if (prop_list.standard_unit.empty() == false)
		user_default_properties.push_back(AttrProperty("standard_unit",prop_list.standard_unit));

	if (prop_list.display_unit.empty() == false)
		user_default_properties.push_back(AttrProperty("display_unit",prop_list.display_unit));
		
	if (prop_list.format.empty() == false)
		user_default_properties.push_back(AttrProperty("format",prop_list.format));

	if (prop_list.min_value.empty() == false)
		user_default_properties.push_back(AttrProperty("min_value",prop_list.min_value));

	if (prop_list.max_value.empty() == false)
		user_default_properties.push_back(AttrProperty("max_value",prop_list.max_value));

	if (prop_list.min_alarm.empty() == false)
		user_default_properties.push_back(AttrProperty("min_alarm",prop_list.min_alarm));

	if (prop_list.max_alarm.empty() == false)
		user_default_properties.push_back(AttrProperty("max_alarm",prop_list.max_alarm));
		
	if (prop_list.delta_val.empty() == false)
		user_default_properties.push_back(AttrProperty("delta_val",prop_list.delta_val));
		
	if (prop_list.delta_t.empty() == false)
		user_default_properties.push_back(AttrProperty("delta_t",prop_list.delta_t));

}

//+-------------------------------------------------------------------------
//
// method : 		Attr::set_memorized
// 
// description : 	This method set the attribute as memorized in database
//			This is allowed only for scalar attribute and for
//			writable one
//
//--------------------------------------------------------------------------

void Attr::set_memorized()
{
	if (format != Tango::SCALAR)
	{
		cout3 << "Attr::set_memorized() throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name; 
		o << " is not scalar and can not be memorized" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::set_memorized");
	}

	if ((type == DEV_STATE) || (type == DEV_ENCODED))
	{
		cout3 << "Attr::set_memorized() throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name; 
		o << " can not be memorized" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::set_memorized");
	}
		
	if ((writable == READ) || (writable == READ_WITH_WRITE))
	{
		cout3 << "Attr::set_memorized() throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name; 
		o << " is not writable and therefore can not be memorized" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::set_memorized");
	}
	
	mem = true;
}



//+-------------------------------------------------------------------------
//
// method : 		SpectrumAttr::SpectrumAttr 
// 
// description : 	Constructor for the SpectrumAttr class.
//			This constructor simply set the internal values
//
//--------------------------------------------------------------------------

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,long x)
:Attr(att_name,att_type),ext(NULL)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}
	
	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;	
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,long x)
:Attr(att_name,att_type,w_type),ext(NULL)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}
	
	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;	
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,long x,DispLevel level)
:Attr(att_name,att_type,level),ext(NULL)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}
	
	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;	
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,long x,DispLevel level)
:Attr(att_name,att_type,level,w_type),ext(NULL)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;	
}

//+-------------------------------------------------------------------------
//
// method : 		ImageAttr::ImageAttr 
// 
// description : 	Constructor for the ImageAttr class.
//			This constructor simply set the internal values
//
//--------------------------------------------------------------------------

ImageAttr::ImageAttr(const char *att_name,long att_type,long x,long y)
:SpectrumAttr(att_name,att_type,x),ext(NULL)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;	
}

ImageAttr::ImageAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,
		     long x,long y)
:SpectrumAttr(att_name,att_type,w_type,x),ext(NULL)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;	
}

ImageAttr::ImageAttr(const char *att_name,long att_type,long x,
		     long y,DispLevel level)
:SpectrumAttr(att_name,att_type,x,level),ext(NULL)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;	
}

ImageAttr::ImageAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,
		     long x, long y,DispLevel level)
:SpectrumAttr(att_name,att_type,w_type,x,level),ext(NULL)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << "Attribute : " << name << ": "; 
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;	
}
} // End of Tango namespace
