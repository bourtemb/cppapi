//=============================================================================
//
// file :		MultiAttribute.h
//
// description :	Include file for the MultiAttribute class.
//			Each device has one object of this class. All device
//			attribute objects are stored in this class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
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
// Revision 3.12  2010/09/09 13:46:00  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.11  2010/08/25 11:41:27  taurel
// - Fix some bugs preventing dynamic attributes management (in some cases)
//
// Revision 3.10  2009/01/21 12:47:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.9  2009/01/19 08:06:27  taurel
// - Fix warnings generated by Doxygen
//
// Revision 3.8  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.7  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.6  2007/04/20 14:41:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.5  2006/06/29 11:48:24  jensmeyer
// Added get_attribute_list(0 method to extract the attribute vector.
//
// Revision 3.4  2005/06/29 08:31:19  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.3  2005/01/13 08:30:00  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.2.2.2  2004/11/26 13:53:21  taurel
// - Fix bug if exception thrown during Util class construction
// - Change attribute label and format default values
//
// Revision 3.2.2.1  2004/09/15 06:47:16  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.2  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.1  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.0.4.2  2004/02/18 15:06:18  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 3.0.4.1  2003/09/30 11:50:43  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.0  2003/03/25 16:44:10  taurel
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
//
//=============================================================================

#ifndef _MULTIATTRIBUTE_H
#define _MULTIATTRIBUTE_H

#include <tango.h>

namespace Tango
{

class AttrProperty;
class DeviceClass;

struct EventPar
{
	long		attr_id;
	bool		change;
	bool		archive;
	bool		quality;
	bool		periodic;
	bool		user;
};

class MultiAttributeExt
{
};

//=============================================================================
//
//			The MultiAttribute class
//
//
// description :	There is one instance of this class for each device.
//			This is mainly a helper class. It maintains a vector
//			of all the attribute for the device
//
//=============================================================================

/**
 * There is one instance of this class for each device. This class is mainly
 * an aggregate of Attribute or WAttribute objects. It eases management of
 * multiple attributes
 *
 * $Author$
 * $Revision$
 */

class MultiAttribute
{
public:
/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Create a new MultiAttribute object.
 *
 * This constructor will in-turn call the constructor of the Attribute or
 * WAttribute class of all the device class attributes.
 *
 * @param dev_name The device name
 * @param dev_class Reference to the device DeviceClass object
 * @exception DevFailed If the command sent to the database failed.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	MultiAttribute(string &dev_name,DeviceClass *dev_class);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The MultiAttribute desctructor.
 */
	~MultiAttribute();
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Get Attribute object from its name.
 *
 * This method returns a reference to the Attribute object with a name passed
 * as parameter. The equality on attribute name is case independant.
 *
 * @param attr_name The attribute name
 * @return A reference to the Attribute object
 * @exception DevFailed If the attribute is not defined.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	Attribute &get_attr_by_name(const char *attr_name);
/**
 * Get Attribute object from its index.
 *
 * This method returns a reference to the Attribute object from the index in the
 * main attribute vector
 *
 * @param ind The attribute index
 * @return A reference to the Attribute object
 */
	Attribute &get_attr_by_ind(const long ind) {return *(attr_list[ind]);}
/**
 * Get Writable Attribute object from its name.
 *
 * This method returns a reference to the WAttribute object with a name passed
 * as parameter. The equality on attribute name is case independant.
 *
 * @param attr_name The attribute name
 * @return A reference to the writable attribute object
 * @exception DevFailed If the attribute is not defined.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	WAttribute &get_w_attr_by_name(const char *attr_name);
/**
 * Get Writable Attribute object from its index.
 *
 * This method returns a reference to the Writable Attribute object from the
 * index in the main attribute vector
 *
 * @param ind The attribute index
 * @return A reference to the WAttribute object
 */
	WAttribute &get_w_attr_by_ind(const long ind) {return static_cast<WAttribute &>(*(attr_list[ind]));}
/**
 * Get Attribute index into the main attribute vector from its name.
 *
 * This method returns the index in the Attribute vector (stored in the
 * MultiAttribute object) of an attribute with a given name. The name equality
 * is case independant
 *
 * @param attr_name The attribute name
 * @return The index in the main attributes vector
 * @exception DevFailed If the attribute is not found in the vector.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	long get_attr_ind_by_name(const char *attr_name);
/**
 * Get list of attribute with an alarm level defined.
 *
 * @return  A vector of long data. Each object is the index in the main
 * attribute vector of attribute with alarm level defined
 */
	vector<long> &get_alarm_list() {return alarm_attr_list;}
/**
 * Get attribute number.
 *
 * @return  The attribute number
 */
	unsigned long get_attr_nb() {return (unsigned long)attr_list.size();}

/**
 * Check alarm for one attribute with a given name.
 *
 * This method returns a boolean set to true if the attribute with the given
 * name is in alarm condition
 *
 * @param  attr_name  The attribute name
 * @return A boolean set to true if the attribute is in alarm
 * @exception DevFailed If the attribute does not have any alarm level defined.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm(const char *attr_name) {return get_attr_by_name(attr_name).check_alarm();}
/**
 * Check alarm for one attribute from its index in the main attributes vector.
 *
 * This method returns a boolean set to true if the attribute with the given
 * index in the attrobite object vector is in alarm condition
 *
 * @param  ind  The attribute index
 * @return A boolean set to true if the attribute is in alarm
 * @exception DevFailed If the attribute does not have any alarm level defined.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm(const long ind) {return get_attr_by_ind(ind).check_alarm();}
/**
 * Check alarm on all attribute(s) with an alarm defined.
 *
 * This method returns a boolean set to true if one of the attribute with an
 * alarm level defined is in alarm condition.
 *
 * @return A boolean set to true if one attribute is in alarm
 * @exception DevFailed If the alarm level are not defined for one of the
 * attribute in the list of alarmable one
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm();
/**
 * Add alarm message to device status.
 *
 * This method add alarm mesage to the string passed as parameter. A message
 * is added for each attribute which is in alarm condition
 *
 * @param  status  The string (should be the device status)
 */
	void read_alarm(string &status);
/**
 * Get the vector of attribute objects.
 *
 * Returns the vector of attribute objects.
 *
 */
	 vector<Attribute *> &get_attribute_list(){return attr_list;}
//@}

	void add_write_value(Attribute &);
	void add_attribute(string &,DeviceClass *,long);
	void remove_attribute(string &,bool);
	vector<long> &get_w_attr_list() {return writable_attr_list;}
	bool is_att_quality_alarmed(bool);
	void get_event_param(vector<EventPar> &);
	void add_alarmed_quality_factor(string &);
	void add_default(vector<AttrProperty> &,string &,string &);
	void add_attr(Attribute *att) {attr_list.push_back(att);}

private:
	void concat(vector<AttrProperty> &,vector<AttrProperty> &,vector<AttrProperty> &);
	void add_user_default(vector<AttrProperty> &,vector<AttrProperty> &);
	void check_associated(long,string &);

	MultiAttributeExt		*ext;

protected:
/**@name Class data members */
//@{
/**
 * The Attribute objects vector.
 *
 * This vector is often referred as the main attributes vector
 */
	vector<Attribute *>	attr_list;
/**
 * The list of writable attribute.
 *
 * It is a vector of index in the main attribute vector
 */
	vector<long>		writable_attr_list;
/**
 * The list of attribute with an alarm level defined.
 *
 * It is a vector of index in the main attribute vector
 */
	vector<long>		alarm_attr_list;
//@}
};


} // End of Tango namespace

#endif // _MULTIATTRIBUTE_H
