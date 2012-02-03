
static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               ClassAttribute.cpp
//
// description :        C++ source code for the 
//				AttrProperty
//				ClassAttribute and 
//				MultiClassAttribute
//			classes. These classes
//			are used to manage attribute properties defined at the
//			class level. A Tango DeviceClass class instance has one
//			MultiClassAttribute object which is an aggregate of
//			ClassAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
// Revision 3.12  2010/09/09 13:44:46  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.11  2010/08/25 11:41:27  taurel
// - Fix some bugs preventing dynamic attributes management (in some cases)
//
// Revision 3.10  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.9  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.8  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.7  2007/10/26 11:26:54  taurel
// - Clarifation some exception messages
//
// Revision 3.6  2007/10/16 08:22:51  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
// Revision 3.5  2007/02/16 10:40:09  taurel
// - Implementing a new types of event on the Attribute configuration
//
// Revision 3.4  2005/06/29 08:30:53  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.3  2005/01/13 08:27:38  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.2.2.3  2004/10/22 11:25:00  taurel
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
// Revision 3.2.2.2  2004/09/15 06:45:44  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.2.2.1  2004/08/19 07:44:03  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.2  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.1.2.1  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.1  2003/05/28 14:55:08  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.0  2003/03/25 16:41:52  taurel
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
// Revision 2.11  2003/03/11 17:55:47  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:39  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:15  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:06:21  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:15:36  taurel
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
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:11  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:09  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:12  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:44  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:14  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <classattribute.h>
#include <algorithm>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		AttrProperty::AttrProperty 
// 
// description : 	Constructors for the AttrProperty class.
//			These constructor change the property name to
//			lowercase letters and also change the property
//			value to lowercase letters for the data_format and
//			data_type property
//
// argument : in : 	- name : The property name
//			- value : The property value
//
//--------------------------------------------------------------------------

AttrProperty::AttrProperty(string &name,string &value):attr_name(name),
						       attr_value(value)
{
	attr_lg = 0;
	
//
// Property name in lower case letters
//

	transform(attr_name.begin(),attr_name.end(),attr_name.begin(),::tolower);
	
//
// For data_type or data_format properties, also change property value to
// lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
	{
		transform(attr_value.begin(),attr_value.end(),
			  attr_value.begin(),::tolower);
	}
}

AttrProperty::AttrProperty(const char *name,const char *value):attr_name(name),
							       attr_value(value)
{
	attr_lg = 0;
	
//
// Property name in lower case letters
//

	transform(attr_name.begin(),attr_name.end(),attr_name.begin(),::tolower);
	
//
// For data_type or data_format properties, also change property value to
// lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
	{
		transform(attr_value.begin(),attr_value.end(),
			  attr_value.begin(),::tolower);
	}
}

AttrProperty::AttrProperty(const char *name,long value):attr_name(name),
							attr_lg(value)
{
}

AttrProperty::AttrProperty(const char *name,string &value):attr_name(name),
							   attr_value(value)
{
}

//+-------------------------------------------------------------------------
//
// method : 		AttrProperty::convert 
// 
// description : 	Convert the property value into a long. The long data
//			is also stored in the AttrProperty class
//
//--------------------------------------------------------------------------

void AttrProperty::convert()
{
	TangoSys_MemStream o;
	o << attr_value;
	if (!(o >> attr_lg && o.eof()))
	{
		Except::throw_exception((const char *)"API_AttrOptProp",
				      (const char *)"Can't convert property value",
				      (const char *)"AttrProperty::convert");
	}
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			AttrProperty class
//
//--------------------------------------------------------------------------
#ifndef TANGO_HAS_LOG4TANGO

ostream &operator<<(ostream &o_str,const AttrProperty &p)
{
	o_str << "Property name = " << p.attr_name << ", Property value = " << p.attr_value;
	return o_str;
}

#endif // TANGO_HAS_LOG4TANGO

//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::MultiClassAttribute
// 
// description : 	constructor for the MultiClassAttribute class from the 
//			device class name
//
//--------------------------------------------------------------------------

MultiClassAttribute::~MultiClassAttribute()
{
	long nb_attr = attr_list.size();
	for (int i = 0;i < nb_attr;i++)
		delete attr_list[i];
}


//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::MultiClassAttribute
// 
// description : 	constructor for the MultiClassAttribute class from the 
//			device class name
//
//--------------------------------------------------------------------------

MultiClassAttribute::MultiClassAttribute()
{
	cout4 << "Entering MultiClassAttribute constructor" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::init_class_attribute
// 
// description : 	Ask the database for prperties defined at class
//			level and build the ClassAttribute object for
//			each attribute with defined properties
//
// argument : in : 	- class_name : The device class name
//
//--------------------------------------------------------------------------

void MultiClassAttribute::init_class_attribute(string &class_name,long base)
{
	cout4 << "Entering MultiClassAttribute::init_class_attribute" << endl;
	long i;

	Tango::Util *tg = Tango::Util::instance();
	CORBA::Any send;

	long nb_attr;
	if (base == 0)	
		nb_attr = attr_list.size();
	else
		nb_attr = 1;
	
//
// Get class attribute(s) properties stored in DB
// No need to implement
// a retry here (in case of db server restart) because the db reconnection
// is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

	if ((nb_attr != 0) && (Tango::Util::_UseDb == true))
	{
		Tango::DbData db_list;
		
		for(i = 0;i < nb_attr;i++)
			db_list.push_back(DbDatum(attr_list[i + base]->get_name()));
	
		try
		{
			tg->get_database()->get_class_attribute_property(class_name,db_list,tg->get_db_cache());
		}
		catch (Tango::DevFailed &e)
		{	
			TangoSys_OMemStream o;
			o << "Can't get class attribute properties for class " << class_name << ends;

			Except::re_throw_exception(e,(const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"MultiClassAttribute::init_class_attribute");
		}
	
//
// Sort property for each attribute and create a ClassAttribute object for each
// of them
//
	
		long ind = 0;	
		for (i = 0;i < nb_attr;i++)
		{
			vector<AttrProperty> prop_list;

			string attr_name = db_list[ind].name;				
			long nb_prop;
			db_list[ind] >> nb_prop;
		
			ind++;
			for (long j = 0;j < nb_prop;j++)
			{
				if (db_list[ind].size() > 1)
				{
					string tmp(db_list[ind].value_string[0]);
					long nb = db_list[ind].size();
					for (int k = 1;k < nb;k++)
					{
						tmp = tmp + " ";
						tmp = tmp + db_list[ind].value_string[k];
					}
					prop_list.push_back(AttrProperty(db_list[ind].name,tmp));					
				}
				else
					prop_list.push_back(AttrProperty(db_list[ind].name,db_list[ind].value_string[0]));
				ind++;
			}

			if (nb_prop != 0)
			{
//
// Find this attribute in the attribute list
//

				unsigned int k;
				for (k = 0;k < attr_list.size();k++)
				{
					if (attr_name == attr_list[k]->get_name())
						break;
				}
				if (k == attr_list.size())
				{
					TangoSys_OMemStream o;
					o << "Attribute " << attr_name << " not found in class attribute(s)" << ends;
		
					Except::throw_exception((const char *)"API_AttrNotFound",
							        o.str(),
							        (const char *)"MultiClassAttribute::init_class_attribute");
				}
				
//
// Add its class property list
//

				attr_list[k]->set_class_properties(prop_list);
			}		
		}
	}

  	for (i = 0;i < nb_attr;i++)
	{
		cout4 << *(attr_list[i + base]) << endl;
  	}

	cout4 << "Leaving MultiClassAttribute::init_class_attribute" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::get_attr
// 
// description : 	Get the Attr object for the attribute with
//			name passed as parameter
//
// in :			attr_name : The attribute name
//
// This method returns a reference to the ClassAttribute object or throw
// an exceptionif the attribute is not found
//
//--------------------------------------------------------------------------


Attr &MultiClassAttribute::get_attr(string &attr_name)
{

//
// Search for the wanted attribute in the attr_list vector from its name
//

	vector<Attr *>::iterator pos;
	
	pos = find_if(attr_list.begin(),attr_list.end(),
		      bind2nd(WantedClassAttr<Attr *,string,bool>(),attr_name));
	
	if (pos == attr_list.end())
	{
		TangoSys_OMemStream o;
		o << "Attribute " << attr_name << " not found in class attribute(s)" << ends;
		
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"MultiClassAttribute::get_attr");
	}
	
	return *(*pos);
}

//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::remove_attr
// 
// description : 	Remove the Attr object for the attribute with
//			name passed as parameter
//
// in :			attr_name : The attribute name
//				cl_name : The attribute class name
//
//--------------------------------------------------------------------------


void MultiClassAttribute::remove_attr(string &attr_name,const string &cl_name)
{
	vector<Tango::Attr *>::iterator ite;
	for (ite = attr_list.begin();ite != attr_list.end();++ite)
	{
		if (((*ite)->get_name() == attr_name) && ((*ite)->get_cl_name() == cl_name))
		{
			attr_list.erase(ite);
			break;
		}
	}
}


//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			Attr class. It prints all the attribute
//			property(ies) name and value defined in DB
//
//--------------------------------------------------------------------------
#ifndef TANGO_HAS_LOG4TANGO

ostream &operator<<(ostream &o_str,const Attr &c)
{
	long nb_prop = c.class_properties.size();
	for (long i = 0;i < nb_prop;i++)
	{
		o_str << c.class_properties[i];
		if (i <= (nb_prop - 2))
			o_str << endl;
	}
	return o_str;
}

#endif // TANGO_HAS_LOG4TANGO


} // End of Tango namespace
