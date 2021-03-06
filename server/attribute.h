///=============================================================================
//
// file :		Attribute.h
//
// description :	Include file for the Attribute classes.
//			Two classes are declared in this file :
//				The Attribute class
//				The WAttribute class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
//=============================================================================

#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include <tango.h>
#include <attrdesc.h>
#include <encoded_attribute.h>
#include <functional>
#include <time.h>
#include <iterator>

#ifdef _TG_WINDOWS_
	#include <sys/types.h>
	#include <sys/timeb.h>
#endif

namespace Tango
{

//
// Binary function objects to be used by the find_if algorithm.
// The find_if algo. want to have a predicate, this means that the return value
// must be a boolean (R is its name).
// The test is done between a AttrProperty object (name A1) and a string (name A2)
// The find_if algo. needs a unary predicate. This function object is a binary
// function object. It must be used with the bind2nd function adapter
//

template <typename A1, typename A2, typename R>
struct WantedProp : public  binary_function<A1,A2,R>
{
	R operator() (A1 att,A2 name_str) const
	{
		return att.get_name() == name_str;
	}
};

template <typename A1, typename A2, typename R>
struct WantedAttr : public binary_function<A1,A2,R>
{
	R operator() (A1 attr_ptr, A2 name) const
	{
		string st(name);
		if (st.size() != attr_ptr->get_name_size())
			return false;
		transform(st.begin(),st.end(),st.begin(),::tolower);
		return attr_ptr->get_name_lower() == st;
	}
};


class AttrProperty;
class DeviceClass;

typedef union _Attr_CheckVal
{
	short			sh;
	DevLong			lg;
	double			db;
	float 			fl;
	unsigned short	ush;
	unsigned char	uch;
	DevLong64		lg64;
	DevULong		ulg;
	DevULong64		ulg64;
	DevState		d_sta;
}Attr_CheckVal;

typedef union _Attr_Value
{
	DevVarShortArray  	*sh_seq;
	DevVarLongArray   	*lg_seq;
	DevVarFloatArray  	*fl_seq;
	DevVarDoubleArray 	*db_seq;
	DevVarStringArray 	*str_seq;
	DevVarUShortArray 	*ush_seq;
	DevVarBooleanArray	*boo_seq;
	DevVarCharArray		*cha_seq;
	DevVarLong64Array	*lg64_seq;
	DevVarULongArray	*ulg_seq;
	DevVarULong64Array	*ulg64_seq;
	DevVarStateArray	*state_seq;
	DevVarEncodedArray	*enc_seq;
}Attr_Value;


typedef struct last_attr_value
{
	bool 				inited;
	Tango::AttrQuality 	quality;
	CORBA::Any 			value;
	bool 				err;
	DevFailed 			except;
	AttrValUnion		value_4;
} LastAttrValue;

typedef enum prop_type
{
    MIN_VALUE = 0,
    MAX_VALUE,
    MIN_WARNING,
    MAX_WARNING,
    MIN_ALARM,
    MAX_ALARM
} PropType;

class EventSupplier;

//=============================================================================
//
//			The Attribute class
//
//
// description :	There is one instance of this class for each attribute
//			for each device. This class stores the attribute
//			properties and the attribute value.
//
//=============================================================================

/**
 * This class represents a Tango attribute.
 *
 * $Author$
 * $Revision$
 */

class Attribute
{
public:

	enum alarm_flags
	{
		min_level,
		max_level,
		rds,
		min_warn,
		max_warn,
		numFlags
	};

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Create a new Attribute object.
 *
 * @param prop_list The attribute properties list. Each property is an object
 * of the AttrProperty class
 * @param tmp_attr Temporary attribute object built from user parameters
 * @param dev_name The device name
 * @param idx The index of the related Attr object in the MultiClassAttribute
 *            vector of Attr object
 */
	Attribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class
 */
//@{
/**
 * The attribute desctructor.
 */
	virtual ~Attribute() {try{delete ext;}catch(omni_thread_fatal &){}}
//@}

/**@name Check attribute methods
 * Miscellaneous method returning boolean flag according to attribute state
 */
//@{
/**
 * Check if the attribute has an associated writable attribute.
 *
 * This method returns a boolean set to true if the attribute has a writable
 * attribute associated to it.
 *
 * @return A boolean set to true if there is an associated writable attribute
 */
	bool is_writ_associated();
/**
 * Check if the attribute is in minimum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value below the min. alarm).
 */
	bool is_min_alarm() {return alarm.test(min_level);}
/**
 * Check if the attribute is in maximum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value above the max. alarm).
 */
	bool is_max_alarm() {return alarm.test(max_level);}
/**
 * Check if the attribute is in minimum warning condition .
 *
 * @return A boolean set to true if the attribute is in warning condition (read
 * value below the min. warning).
 */
	bool is_min_warning() {return alarm.test(min_warn);}
/**
 * Check if the attribute is in maximum warning condition .
 *
 * @return A boolean set to true if the attribute is in warning condition (read
 * value above the max. warning).
 */
	bool is_max_warning() {return alarm.test(max_warn);}
/**
 * Check if the attribute is in RDS alarm condition .
 *
 * @return A boolean set to true if the attribute is in RDS condition (Read
 * Different than Set).
 */
	bool is_rds_alarm() {return alarm.test(rds);}
/**
 * Check if the attribute has an alarm defined.
 *
 * This method returns a set of bits. Each alarm type is defined by one
 * bit.
 *
 * @return A bitset. Each bit is set if the coresponding alarm is on
 */
	bitset<numFlags> &is_alarmed() {return alarm_conf;}
/**
 * Check if the attribute is polled .
 *
 * @return A boolean set to true if the attribute is polled.
 */
	bool is_polled();
/**
 * Check if the attribute read value is below/above the alarm level.
 *
 * @return A boolean set to true if the attribute is in alarm condition.
 * @exception DevFailed If no alarm level is defined.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	bool check_alarm();
//@}

/**@name Get/Set object members.
 * These methods allow the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Get the attribute writable type (RO/WO/RW).
 *
 * @return The attribute write type.
 */
	Tango::AttrWriteType get_writable() {return writable;}
/**
 * Get attribute name
 *
 * @return The attribute name
 */
	string &get_name() {return name;}
/**
 * Get attribute data type
 *
 * @return The attribute data type
 */
	long get_data_type() {return data_type;}
/**
 * Get attribute data format
 *
 * @return The attribute data format
 */
	Tango::AttrDataFormat get_data_format() {return data_format;}
/**
 * Get name of the associated writable attribute
 *
 * @return The associated writable attribute name
 */
	string &get_assoc_name() {return writable_attr_name;}
/**
 * Get index of the associated writable attribute
 *
 * @return The index in the main attribute vector of the associated writable
 * attribute
 */
	long get_assoc_ind() {return assoc_ind;}
/**
 * Set index of the associated writable attribute
 *
 * @param val The new index in the main attribute vector of the associated writable
 * attribute
 */
	void set_assoc_ind(long val) {assoc_ind = val;}
/**
 * Get attribute date
 *
 * @return The attribute date
 */
	Tango::TimeVal &get_date() {return when;}
/**
 * Set attribute date
 *
 * @param new_date The attribute date
 */
	void set_date(Tango::TimeVal &new_date) {when = new_date;}
#ifdef _TG_WINDOWS_
/**
 * Set attribute date
 *
 * @param The attribute date
 */
	void set_date(struct _timeb &t) {when.tv_sec=(long)t.time;when.tv_usec=(t.millitm*1000);when.tv_nsec=0;}
#endif
/**
 * Set attribute date
 *
 * @param t The attribute date
 */
	void set_date(struct timeval &t) {when.tv_sec=t.tv_sec;when.tv_usec=t.tv_usec;when.tv_nsec=0;}
/**
 * Set attribute date
 *
 * @param new_date The attribute date
 */
	void set_date(time_t new_date) {when.tv_sec=(long)new_date;when.tv_usec=0;when.tv_nsec=0;}
/**
 * Get attribute label property
 *
 * @return The attribute label
 */
 	string &get_label() {return label;}
/**
 * Get attribute data quality
 *
 * @return The attribute data quality
 */
	Tango::AttrQuality &get_quality() {return quality;}
/**
 * Set attribute data quality
 *
 * @param qua	The new attribute data quality
 * @param send_event Boolean set to true if a change event should be sent
 */
	void set_quality(Tango::AttrQuality qua, bool send_event=false);

/**
 * Get attribute data size
 *
 * @return The attribute data size
 */
	long get_data_size() {return data_size;}
/**
 * Get attribute data size in x dimension
 *
 * @return The attribute data size in x dimension. Set to 1 for scalar attribute
 */
	long get_x() {return dim_x;}
/**
 * Get attribute maximum data size in x dimension
 *
 * @return The attribute maximum data size in x dimension. Set to 1 for scalar attribute
 */
	long get_max_dim_x() {return max_x;}
/**
 * Get attribute data size in y dimension
 *
 * @return The attribute data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_y() {return dim_y;}
/**
 * Get attribute maximum data size in y dimension
 *
 * @return The attribute maximum data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_max_dim_y() {return max_y;}
/**
 * Get attribute polling period
 *
 * @return The attribute polling period in mS. Set to 0 when the attribute is
 * not polled
 */
	long get_polling_period() {return ext->poll_period;}
/**
 * Get all modifiable attribute properties in one call
 *
 * This method initializes the members of a MultiAttrProp object with the modifiable
 * attribute properties values
 *
 * @param props A MultiAttrProp object.
 */
	template <typename T>
	void get_properties(Tango::MultiAttrProp<T> &props);
/**
 * Set all modifiable attribute properties in one call
 *
 * This method sets the modifiable attribute properties with the values
 * provided as members of MultiAttrProps object
 *
 * @param props A MultiAttrProp object.
 */
	template <typename T>
	void set_properties(Tango::MultiAttrProp<T> &props);
/**
 * Set attribute serialization model
 *
 * This method allows the user to choose the attribute serialization
 * model.
 *
 * @param ser_model The new serialisation model. The serialization model must be
 * one of ATTR_BY_KERNEL, ATTR_BY_USER or ATTR_NO_SYNC
 */
	void set_attr_serial_model(AttrSerialModel ser_model);
/**
 * Get attribute serialization model
 *
 * Get the attribute serialization model
 *
 * @return The attribute serialization model
 */
	AttrSerialModel get_attr_serial_model() {return ext->attr_serial_model;}
/**
 * Set attribute user mutex
 *
 * This method allows the user to give to the attribute object the pointer to
 * the omni_mutex used to protect its buffer. The mutex has to be locked when passed
 * to this method. The Tango kernel will unlock it when the data will be transferred
 * to the client.
 *
 * @param mut_ptr The user mutex pointer
 */
	void set_user_attr_mutex(omni_mutex *mut_ptr) {ext->user_attr_mutex = mut_ptr;}
//@}


/**@name Set attribute value methods.
 * These methods allows the external world to set attribute object internal
 * value
 */
//@{
/**
 * Set internal attribute value (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevShort *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevLong *p_data,long x = 1, long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevLong64 attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevLong64 *p_data,long x = 1,long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevFloat *p_data,long x = 1,long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevDouble *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevString *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevBoolean *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevUShort *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevUChar *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevULong attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevULong *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevULong64 attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevULong64 *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevState attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevState *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevEncoded *p_data,long x = 1, long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data_str The attribute string part read value
 * @param p_data The attribute raw data part read value
 * @param size Size of the attribute raw data part
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,bool release = false);
/**
 * Set internal attribute value (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param attr Handle to EncodedAttribute object
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::EncodedAttribute *attr);

//---------------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevShort *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);

#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevShort *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevShort *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevLong *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong64 *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevLong64 *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong64 *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevFloat *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevFloat *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevFloat *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif


//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevDouble *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevDouble *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevDouble *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevString *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevBoolean *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevUShort *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevUShort *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevUShort *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevUChar *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevUChar *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevUChar *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevULong *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong64 *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevULong64 *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong64 *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevState attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevState *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevState *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevState attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevState *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevEncoded *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type
 * when splitted in format and data).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data_str The attribute coded format string
 * @param p_data The attribute raw data
 * @param size Size of the attribute raw data part
 * @param t The date
 * @param qual The attribute quality factor
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data_str,
					Tango::DevUChar *p_data,
					long size,
				    time_t t,
				    Tango::AttrQuality qual,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevEncoded *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);

	void set_value_date_quality(Tango::DevString *p_data_str,
					Tango::DevUChar *p_data,
					long size,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevEncoded *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type
 * when splitted in data format and data themselves).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data_str The attribute format string
 * @param p_data The attribute raw data
 * @param size Size of the attribute raw data part
 * @param t The date
 * @param qual The attribute quality factor
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data_str,
					Tango::DevUChar *p_data,
					long size,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    bool release = false);
#endif


/**
 * Fire a change event for the attribute value. The event is pushed to the notification
 * daemon.
 * The attibute data must be set with one of the Attribute::set_value or
 * Attribute::setvalue_date_quality methods before fireing the event.
 * The event is triggered with or without the change event criteria depending
 * on the configuration choosen with set_change_event().
 * ATTENTION: The couple set_value() and fire_change_event() needs to be protected
 * against concurrent accesses to the same attribute. Such an access might happen during
 * a synchronous read or by a reading from the polling thread.
 * Inside all methods reading or writing commands and attributes this protection is
 * automatically done by the Tango serialisation monitor.
 * When fireing change events in your own code, you should use the push_change_event
 * methods of the DeviceImpl class or protect your code with the
 * Tango::AutoTangoMonitor on your device.
 * Example:
 *
 *	{
 * 		Tango::AutoTangoMonitor synch(this);
 *		att_temp_seq.set_value (temp_seq, 100);
 * 		att_temp_seq.fire_archive_event ();
 *	}
 *
 * @param except A pointer to a DevFailed exception to be thrown as archive event.
 */
	void fire_change_event(DevFailed *except = NULL);

/**
 * Set a flag to indicate that the server fires change events manually, without
 * the polling to be started for the attribute.
 * If the detect parameter is set to true, the criteria specified for the change
 * event are verified and the event is only pushed if they are fulfilled.
 * If detect is set to false the event is fired without any value checking!
 *
 * @param implemented True when the server fires change events manually.
 * @param detect Triggers the verification of the change event properties when set to true.
 */
	void set_change_event(bool implemented, bool detect = true)
			{ ext->change_event_implmented = implemented;
			  ext->check_change_event_criteria = detect;
			  if(detect==false){
			  ext->prev_change_event.err=false;
			  ext->prev_change_event.quality=Tango::ATTR_VALID;}}
/**
 * Check if the change event is fired manually (without polling) for this attribute.
 *
 * @return A boolean set to true if a manual fire change event is implemented.
 */
	bool is_change_event() {return ext->change_event_implmented;}
/**
 * Check if the change event criteria should be checked when firing
 * the event manually.
 *
 * @return A boolean set to true if a change event criteria will be checked.
 */
	bool is_check_change_criteria() {return ext->check_change_event_criteria;}


/**
 * Fire an archive event for the attribute value. The event is pushed to the notification
 * daemon.
 * The attibute data must be set with one of the Attribute::set_value or
 * Attribute::setvalue_date_quality methods before fireing the event.
 * The event is triggered with or without the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 * ATTENTION: The couple set_value() and fire_archive_event() needs to be protected
 * against concurrent accesses to the same attribute. Such an access might happen during
 * a synchronous read or by a reading from the polling thread.
 * Inside all methods reading or writing commands and attributes this protection is
 * automatically done by the Tango serialisation monitor.
 * When fireing archive events in your own code, you should use the push_archive_event
 * methods of the DeviceImpl class or protect your code with the
 * Tango::AutoTangoMonitor on your device.
 * Example:
 *
 *	{
 * 		Tango::AutoTangoMonitor synch(this);
 *		att_temp_seq.set_value (temp_seq, 100);
 * 		att_temp_seq.fire_archive_event ();
 *	}
 *
 * @param except A pointer to a DevFailed exception to be thrown as archive event.
 */
	void fire_archive_event(DevFailed *except = NULL);

/**
 * Set a flag to indicate that the server fires archive events manually, without
 * the polling to be started for the attribute
 * If the detect parameter is set to true, the criteria specified for the archive
 * event are verified and the event is only pushed if they are fulfilled.
 * If detect is set to false the event is fired without any value checking!
 *
 * @param implemented True when the server fires archive events manually.
 * @param detect Triggers the verification of the archive event properties when set to true.
 */
	void set_archive_event(bool implemented, bool detect = true)
			{ext->archive_event_implmented = implemented;
			 ext->check_archive_event_criteria = detect;}

/**
 * Check if the archive event is fired manually for this attribute.
 *
 * @return A boolean set to true if a manual fire archive event is implemented.
 */
	bool is_archive_event() {return ext->archive_event_implmented;}
/**
 * Check if the archive event criteria should be checked when firing
 * the event manually.
 *
 * @return A boolean set to true if a archive event criteria will be checked.
 */
	bool is_check_archive_criteria() {return ext->check_archive_event_criteria;}

/**
 * Set a flag to indicate that the server fires data ready events
 *
 * @param implemented True when the server fires change events manually.
 */
	void set_data_ready_event(bool implemented) {ext->dr_event_implmented = implemented;}
/**
 * Check if the data ready event is fired for this attribute.
 *
 * @return A boolean set to true if a fire data ready event is implemented.
 */
	bool is_data_ready_event() {return ext->dr_event_implmented;}


/**
 * Fire a user event for the attribute value. The event is pushed to the notification
 * daemon.
 * The attibute data must be set with one of the Attribute::set_value or
 * Attribute::setvalue_date_quality methods before fireing the event.
 * ATTENTION: The couple set_value() and fire_event() needs to be protected
 * against concurrent accesses to the same attribute. Such an access might happen during
 * a synchronous read or by a reading from the polling thread.
 * Inside all methods reading or writing commands and attributes this protection is
 * automatically done by the Tango serialisation monitor.
 * When fireing archive events in your own code, you should use the push_event
 * methods of the DeviceImpl class or protect your code with the
 * Tango::AutoTangoMonitor on your device.
 * Example:
 *
 *	{
 * 		Tango::AutoTangoMonitor synch(this);
 *		att_temp_seq.set_value (temp_seq, 100);
 * 		att_temp_seq.fire_event ();
 *	}
 *
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param except A pointer to a DevFailed exception to be thrown as archive event.
 */
	void fire_event(vector<string> &filt_names,vector<double> &filt_vals,DevFailed *except = NULL);

/**
 * Remove the attribute configuration from the database.
 * This method can be used to clean-up all the configuration of an attribute to come back to
 * its default values or the remove all configuration of a dynamic attribute before deleting it.
 *
 * The method removes all configured attribute properties and removes the attribute from the
 * list of polled attributes.
 *
 * @exception DevFailed In case of database access problems.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void remove_configuration();
//@}


/**@name Set/Get attribute ranges (min_alarm, min_warning, max_warning, max_alarm) methods.
 * These methods allow the external world to set attribute object min_alarm, min_warning,
 * max_warning and max_alarm values
 */
//@{

/**
 * Set attribute minimum alarm.
 *
 * This method sets the attribute minimum alarm.
 *
 * @param new_min_alarm The new attribute minimum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_min_alarm(const T &new_min_alarm);

	void set_min_alarm(char *new_min_alarm);
	void set_min_alarm(const char *new_min_alarm);

/**
 * Get attribute minimum alarm or throw an exception if the attribute
 * does not have the minimum alarm
 *
 * @param min_al Reference to a variable which value will be set to the attribute's
 * minimum alarm
 */
	template <typename T>
	void get_min_alarm(T &min_al);

/**
 * Set attribute maximum alarm.
 *
 * This method sets the attribute maximum alarm.
 *
 * @param new_max_alarm The new attribute maximum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_max_alarm(const T &new_max_alarm);

	void set_max_alarm(char *new_max_alarm);
	void set_max_alarm(const char *new_max_alarm);

/**
 * Get attribute maximum alarm or throw an exception if the attribute
 * does not have the maximum alarm set
 *
 * @param max_al Reference to a variable which value will be set to the attribute's
 * maximum alarm
 */
	template <typename T>
	void get_max_alarm(T &max_al);

/**
 * Set attribute minimum warning.
 *
 * This method sets the attribute minimum warning.
 *
 * @param new_min_warning The new attribute minimum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_min_warning(const T &new_min_warning);

	void set_min_warning(char *new_min_warning);
	void set_min_warning(const char *new_min_warning);

/**
 * Get attribute minimum warning or throw an exception if the attribute
 * does not have the minimum warning set
 *
 * @param min_war Reference to a variable which value will be set to the attribute's
 * minimum warning
 */
	template <typename T>
	void get_min_warning(T &min_war);

/**
 * Set attribute maximum warning.
 *
 * This method sets the attribute maximum warning.
 *
 * @param new_max_warning The new attribute maximum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_max_warning(const T &new_max_warning);

	void set_max_warning(char *new_max_warning);
	void set_max_warning(const char *new_max_warning);

/**
 * Get attribute maximum warning or throw an exception if the attribute
 * does not have the maximum warning set
 *
 * @param max_war Reference to a variable which value will be set to the attribute's
 * maximum warning
 */
	template <typename T>
	void get_max_warning(T &max_war);
//@}


protected:

/**@name Class data members */
//@{
/**
 * A flag set to true if the attribute value has been updated
 */
	bool 			value_flag;
/**
 * The date when attribute was read
 */
	Tango::TimeVal		when;
/**
 * Flag set to true if the date must be set
 */
	bool			date;
/**
 * The attribute quality factor
 */
	Tango::AttrQuality	quality;

/**
 * The attribute name
 */
	string 			name;
/**
 * The attribute writable flag
 */
	Tango::AttrWriteType	writable;
/**
 * The attribute data type.
 *
 * Eight types are suported. They are Tango::DevShort, Tango::DevLong,
 * Tango::DevDouble, Tango::DevString, Tango::DevUShort, Tango::DevUChar,
 * Tango::DevFloat and Tango::DevBoolean
 */
	long			data_type;
/**
 * The attribute data format.
 *
 * Three data formats are supported. They are SCALAR, SPECTRUM and IMAGE
 */
	Tango::AttrDataFormat	data_format;
/**
 * The attribute maximum x dimension.
 *
 * It is needed for SPECTRUM or IMAGE data format
 */
	long			max_x;
/**
 * The attribute maximum y dimension.
 *
 * It is necessary only for IMAGE data format
 */
	long			max_y;
/**
 * The attribute label
 */
	string			label;
/**
 * The attribute description
 */
	string			description;
/**
 * The attribute unit
 */
	string			unit;
/**
 * The attribute standard unit
 */
	string			standard_unit;
/**
 * The attribute display unit
 */
	string 			display_unit;
/**
 * The attribute format.
 *
 * This string specifies how an attribute value must be printed
 */
	string			format;
/**
 * The name of the associated writable attribute
 */
	string			writable_attr_name;
/**
 * The attribute minimum alarm level
 */
	string			min_alarm_str;
/**
 * The attribute maximun alarm level
 */
	string			max_alarm_str;
/**
 * The attribute minimum value
 */
	string			min_value_str;
/**
 * The attribute maximum value
 */
	string			max_value_str;
/**
 * The attribute minimun  warning
 */
	string			min_warning_str;
/**
 * The attribute maximum warning
 */
	string			max_warning_str;
/**
 * The attribute delta value RDS alarm
 */
	string			delta_val_str;
/**
 * The attribute delta time RDS alarm
 */
	string			delta_t_str;
/**
 * Index in the main attribute vector of the associated writable attribute (if any)
 */
	long			assoc_ind;
/**
 * The attribute minimum alarm in binary format
 */
	Tango::Attr_CheckVal	min_alarm;
/**
 * The attribute maximum alarm in binary format
 */
	Tango::Attr_CheckVal	max_alarm;
/**
 * The attribute minimum warning in binary format
 */
	Tango::Attr_CheckVal	min_warning;
/**
 * The attribute maximum warning in binary format
 */
	Tango::Attr_CheckVal	max_warning;
/**
 * The attribute minimum value in binary format
 */
	Tango::Attr_CheckVal	min_value;
/**
 * The attribute maximum value in binary format
 */
	Tango::Attr_CheckVal	max_value;
/**
 * The attribute value
 */
	Tango::Attr_Value	value;
/**
 * The attribute data size
 */
	long			data_size;
/**
 * Flag set to true if a minimum value is defined
 */
	bool			check_min_value;
/**
 * Flag set to true if a maximum alarm is defined
 */
	bool			check_max_value;
/**
 * Authorized delta between the last written value and the
 * actual read. Used if the attribute has an alarm on
 * Read Different Than Set (RDS)
 */
	Tango::Attr_CheckVal	delta_val;
/**
 * Delta time after which the read value must be checked again the
 * last written value if the attribute has an alarm on
 * Read Different Than Set (RDS)
 */
 	long 			delta_t;
//@}

public:

//
// methods not usable for the external world (outside the lib)
//

    void get_properties(Tango::AttributeConfig &conf);
	void get_properties_2(Tango::AttributeConfig_2 &conf);
	void get_properties_3(Tango::AttributeConfig_3 &conf);
	void set_properties(const Tango::AttributeConfig_3 &conf);
	void set_properties(const Tango::AttributeConfig &conf,Tango::DeviceImpl *dev);
	void set_properties(const Tango::AttributeConfig_3 &conf,Tango::DeviceImpl *dev);

	virtual void set_rvalue() {};
	void delete_seq();
	bool check_scalar_wattribute();

	void wanted_date(bool flag) {date = flag;}
	bool get_wanted_date() {return date;}

	Tango::TimeVal &get_when() {return when;}
	void set_time();

	Tango::DevVarShortArray *get_short_value() {return value.sh_seq;}
	Tango::DevVarLongArray *get_long_value() {return value.lg_seq;}
	Tango::DevVarDoubleArray *get_double_value() {return value.db_seq;}
	Tango::DevVarStringArray *get_string_value() {return value.str_seq;}
	Tango::DevVarFloatArray *get_float_value() {return value.fl_seq;}
	Tango::DevVarBooleanArray *get_boolean_value() {return value.boo_seq;}
	Tango::DevVarUShortArray *get_ushort_value() {return value.ush_seq;}
	Tango::DevVarCharArray *get_uchar_value() {return value.cha_seq;}
	Tango::DevVarLong64Array *get_long64_value() {return value.lg64_seq;}
	Tango::DevVarULongArray *get_ulong_value() {return value.ulg_seq;}
	Tango::DevVarULong64Array *get_ulong64_value() {return value.ulg64_seq;}
	Tango::DevVarStateArray *get_state_value() {return value.state_seq;}
	Tango::DevVarEncodedArray *get_encoded_value() {return value.enc_seq;}

	Tango::DevLong64	*get_tmp_scalar_long64() {return ext->tmp_lo64;}
	Tango::DevULong		*get_tmp_scalar_ulong() {return ext->tmp_ulo;}
	Tango::DevULong64	*get_tmp_scalar_ulong64() {return ext->tmp_ulo64;}
	Tango::DevState		*get_tmp_scalar_state() {return ext->tmp_state;}

	void add_write_value(Tango::DevVarShortArray *);
	void add_write_value(Tango::DevVarLongArray *);
	void add_write_value(Tango::DevVarDoubleArray *);
	void add_write_value(Tango::DevVarStringArray *);
	void add_write_value(Tango::DevVarFloatArray *);
	void add_write_value(Tango::DevVarBooleanArray *);
	void add_write_value(Tango::DevVarUShortArray *);
	void add_write_value(Tango::DevVarCharArray *);
	void add_write_value(Tango::DevVarLong64Array *);
	void add_write_value(Tango::DevVarULongArray *);
	void add_write_value(Tango::DevVarULong64Array *);
	void add_write_value(Tango::DevVarStateArray *);
	void add_write_value(Tango::DevEncoded &);

	unsigned long get_name_size() {return name_size;}
	string &get_name_lower() {return name_lower;}
	void set_value_flag(bool val) {value_flag = val;}
	bool get_value_flag() {return value_flag;}
	DispLevel get_disp_level() {return ext->disp_level;}

	omni_mutex *get_attr_mutex() {return &(ext->attr_mutex);}
	omni_mutex *get_user_attr_mutex() {return ext->user_attr_mutex;}

	void set_properties(const Tango::AttributeConfig &,string &);
	void set_properties(const Tango::AttributeConfig_3 &,string &);
	void upd_database(const Tango::AttributeConfig &,string &);
	void upd_database(const Tango::AttributeConfig_3 &,string &);
	void set_upd_properties(const Tango::AttributeConfig_3 &);
	void set_upd_properties(const Tango::AttributeConfig_3 &,string &);

	bool change_event_subscribed() {if (ext->event_change_subscription != 0)return true;else return false;}
	bool periodic_event_subscribed() {if (ext->event_periodic_subscription != 0)return true;else return false;}
	bool archive_event_subscribed() {if (ext->event_archive_subscription != 0)return true;else return false;}
	bool quality_event_subscribed() {if (ext->event_quality_subscription != 0)return true;else return false;}
	bool user_event_subscribed() {if (ext->event_user_subscription != 0)return true;else return false;}

	bool use_notifd_event() {return ext->notifd_event;}
	bool use_zmq_event() {return ext->zmq_event;}

	void set_change_event_sub() {ext->event_change_subscription=time(NULL);}
	void set_periodic_event_sub() {ext->event_periodic_subscription=time(NULL);}
	void set_archive_event_sub() {ext->event_archive_subscription=time(NULL);}
	void set_quality_event_sub() {ext->event_quality_subscription=time(NULL);}
	void set_user_event_sub() {ext->event_user_subscription=time(NULL);}
	void set_use_notifd_event() {ext->notifd_event = true;}
	void set_use_zmq_event() {ext->zmq_event = true;}

	long get_attr_idx() {return ext->idx_in_attr;}
	void set_attr_idx(long new_idx) {ext->idx_in_attr=new_idx;}
	DeviceImpl *get_att_device();

	void Attribute_2_AttributeValue(Tango::AttributeValue_3 *,DeviceImpl *);
	void Attribute_2_AttributeValue(Tango::AttributeValue_4 *,DeviceImpl *);
	void AttributeValue_4_2_AttributeValue_3(const Tango::AttributeValue_4 *,Tango::AttributeValue_3 *);

	void set_mcast_event(vector<string> &vs) {ext->mcast_event.clear();copy(vs.begin(),vs.end(),back_inserter(ext->mcast_event));}

	bool is_polled(DeviceImpl *);
	void set_polling_period(long per) {ext->poll_period = per;}

	void save_alarm_quality() {ext->old_quality=quality;ext->old_alarm=alarm;}

	bool is_startup_exception() {return ext->check_startup_exceptions;}
	void throw_startup_exception(const char*);

#ifndef TANGO_HAS_LOG4TANGO
	friend ostream &operator<<(ostream &,Attribute &);
#endif // TANGO_HAS_LOG4TANGO
	friend class EventSupplier;
	friend class ZmqEventSupplier;
	friend class DServer;

private:
	void set_data_size();
	void throw_min_max_value(string &,string &,MinMaxValueCheck);
	void check_str_prop(const AttributeConfig &,DbData &,long &,DbData &,long &,vector<AttrProperty> &,vector<AttrProperty> &);
	void log_quality();
    void event_prop_db(const char *,vector<double> &,vector<double> &,DbData &,long &,DbData &,long &);

	unsigned long 		name_size;
	string 				name_lower;
	DevEncoded			enc_help;

protected:

//
// The extension class
//

    class AttributeExt
    {
    public:
        AttributeExt() : poll_period(0),event_period(0),archive_period(0),
                         last_periodic(0.0),archive_last_periodic(0.0),periodic_counter(0),
                         archive_periodic_counter(0),archive_last_event(0.0),
                         dev(NULL),change_event_implmented(false),archive_event_implmented(false),
                         check_change_event_criteria(true),check_archive_event_criteria(true),
                         event_periodic_client_3(false),event_change_client_3(false),event_archive_client_3(false),
                         event_user_client_3(false),user_attr_mutex(NULL),dr_event_implmented(false),
                         scalar_str_attr_release(false),notifd_event(false),zmq_event(false),
                         check_startup_exceptions(false), startup_exceptions_clear(true) {}

        Tango::DispLevel 	disp_level;						// Display level
        long				poll_period;					// Polling period
        double				rel_change[2];					// Delta for relative change events in %
        double				abs_change[2];					// Delta for absolute change events
        double				archive_rel_change[2];			// Delta for relative archive change events in %
        double				archive_abs_change[2];			// Delta for absolute change events
        int					event_period;					// Delta for periodic events in ms
        int					archive_period;					// Delta for archive periodic events in ms
        double				last_periodic;					// Last time a periodic event was detected
        double				archive_last_periodic;			// Last time an archive periodic event was detected
        long				periodic_counter;				// Number of periodic events sent so far
        long				archive_periodic_counter;		// Number of periodic events sent so far
        LastAttrValue		prev_change_event;				// Last change attribute
        LastAttrValue		prev_quality_event;				// Last quality attribute
        LastAttrValue		prev_archive_event;				// Last archive attribute
        time_t				event_change_subscription;		// Last time() a subscription was made
        time_t				event_quality_subscription;		// Last time() a subscription was made
        time_t				event_periodic_subscription;	// Last time() a subscription was made
        time_t				event_archive_subscription; 	// Last time() a subscription was made
        time_t				event_user_subscription; 		// Last time() a subscription was made
        time_t				event_attr_conf_subscription;	// Last time() a subscription was made
        time_t				event_data_ready_subscription;	// Last time() a subscription was made
        double				archive_last_event;				// Last time an archive event was detected (periodic or not)
        long				idx_in_attr;					// Index in MultiClassAttribute vector
        string				d_name;							// The device name
        DeviceImpl 			*dev;							// The device object
        bool				change_event_implmented;		// Flag true if a manual fire change event is implemented.
        bool				archive_event_implmented;		// Flag true if a manual fire archive event is implemented.
        bool				check_change_event_criteria;	// True if change event criteria should be checked when sending the event
        bool				check_archive_event_criteria;	// True if change event criteria should be checked when sending the event
        bool				event_periodic_client_3;		// True if at least one periodic event client is using IDL 3
        bool				event_change_client_3;			// True if at least one periodic event client is using IDL 3
        bool				event_archive_client_3;			// True if at least one periodic event client is using IDL 3
        bool				event_user_client_3;			// True if at least one periodic event client is using IDL 3
        Tango::DevLong64	tmp_lo64[2];
        Tango::DevULong		tmp_ulo[2];
        Tango::DevULong64	tmp_ulo64[2];
        Tango::DevState		tmp_state[2];
        omni_mutex			attr_mutex;						// Mutex to protect the attributes shared data buffer
        omni_mutex			*user_attr_mutex;				// Ptr for user mutex in case he manages exclusion
        AttrSerialModel		attr_serial_model;				// Flag for attribute serialization model
        bool				dr_event_implmented;			// Flag true if fire data ready event is implemented
        bool				scalar_str_attr_release;		// Need memory freeing (scalar string attr, R/W att)
        bool                notifd_event;                   // Set to true if event required using notifd
        bool                zmq_event;                      // Set to true if event required using ZMQ
        vector<string>      mcast_event;                    // In case of multicasting used for event transport
        AttrQuality         old_quality;                    // Previous attribute quality
        bitset<numFlags>    old_alarm;                      // Previous attribute alarm
        map<string,const DevFailed> startup_exceptions;		// Map containing exceptions related to attribute configuration raised during the server startup sequence
        bool 				check_startup_exceptions;		// Flag set to true if there is at least one exception in startup_exceptions map
        bool 				startup_exceptions_clear;		// Flag set to true when the cause for the device startup exceptions has been fixed
    };

	AttributeExt		*ext;

	virtual void init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name);
	virtual void init_event_prop(vector<AttrProperty> &prop_list,const string &dev_name,Attr &att);
	string &get_attr_value(vector<AttrProperty> &prop_list,const char *name);
	long get_lg_attr_value(vector<AttrProperty> &prop_list,const char *name);
	virtual bool check_rds_alarm() {return false;}
	bool check_level_alarm();
	bool check_warn_alarm();
	void upd_att_prop_db(Tango::Attr_CheckVal &,const char *);
	DeviceClass *get_att_device_class(string &);

	template <typename T>
    void check_hard_coded_properties(const T &);

	void add_startup_exception(string,const DevFailed &);
	void delete_startup_exception(string);

    void throw_hard_coded_prop(const char *);
	void throw_err_format(const char *,const string &,const char *);
	void throw_incoherent_val_err(const char *,const char *,const string &,const char *);
	void throw_err_data_type(const char *,const string &,const char *);
    void validate_change_properties(const string &,const char *,string &,vector<double> &,vector<bool> &,vector<bool> &);
    void validate_change_properties(const string &,const char *,string &,vector<double> &);
    bool prop_in_list(const char *,string &,size_t,vector<AttrProperty> &);

    void avns_in_db(const char *,string &);
    void avns_in_att(prop_type);

	bitset<numFlags>	alarm_conf;
	bitset<numFlags>	alarm;

	long				dim_x;
	long				dim_y;

	Tango::DevShort		tmp_sh[2];
	Tango::DevLong		tmp_lo[2];
	Tango::DevFloat		tmp_fl[2];
	Tango::DevDouble	tmp_db[2];
	Tango::DevString	tmp_str[2];
	Tango::DevUShort	tmp_ush[2];
	Tango::DevBoolean	tmp_boo[2];
	Tango::DevUChar		tmp_cha[2];
	Tango::DevEncoded	tmp_enc[2];

	vector<AttrProperty>::iterator pos_end;
};

//
// Some inline methods
//

//+-------------------------------------------------------------------------
//
// method :		Attribute::throw_hard_coded_prop
//
// description : Throw a "Hard coded properties can't be changed" exception
//
// args: in : - prop_name : The name of the property which should be modified
//
//--------------------------------------------------------------------------

inline void Attribute::throw_hard_coded_prop(const char *prop_name)
{
    TangoSys_OMemStream desc;
    desc << "Attribute property " << prop_name << " is not changeable at run time" << ends;

    Except::throw_exception((const char *)"API_AttrNotAllowed",desc.str(),
				      	  (const char *)"Attribute::check_hard_coded_properties()");
}

//+-------------------------------------------------------------------------
//
// method :		Attribute::throw_startup_exception
//
// description : Throw a startup exception
//
// args: in : - origin : The method name where this method is called from
//
//--------------------------------------------------------------------------

inline void Attribute::throw_startup_exception(const char* origin)
{
	if(ext->check_startup_exceptions)
	{
		string err_msg;
		vector<string> event_exceptions;
		vector<string> opt_exceptions;
		for(map<string,const DevFailed>::iterator it = ext->startup_exceptions.begin(); it != ext->startup_exceptions.end(); ++it)
		{
			if(it->first == "event_period" || it->first == "archive_period" || it->first == "rel_change" || it->first == "abs_change" || it->first == "archive_rel_change" || it->first == "archive_abs_change")
				event_exceptions.push_back(it->first);
			else
				opt_exceptions.push_back(it->first);
			for(size_t i = 0 ; i < it->second.errors.length(); i++)
			{
				string tmp_msg = string(it->second.errors[i].desc);
				size_t pos = tmp_msg.rfind('\n');
				if(pos != string::npos)
					tmp_msg.erase(0,pos+1);
				err_msg += "\n" + tmp_msg;
			}
		}
		err_msg = "\nDevice " + ext->d_name + "-> Attribute : " + name + err_msg;

		if(event_exceptions.size() == ext->startup_exceptions.size())
		{
			if(event_exceptions.size() == 1)
				err_msg += "\nSetting a valid value (also 'NaN', 'Not specified' and '' - empty string) for any property for this attribute will automatically bring the above-mentioned property to its library defaults";
			else
				err_msg += "\nSetting a valid value (also 'NaN', 'Not specified' and '' - empty string) for any property for this attribute will automatically bring the above-listed properties to their library defaults";
		}
		else if(event_exceptions.empty() == false)
		{
			if(opt_exceptions.size() == 1)
				err_msg += "\nSetting valid value (also 'NaN', 'Not specified' and '' - empty string) for " + opt_exceptions[0] + " ";
			else
			{
				err_msg += "\nSetting valid values (also 'NaN', 'Not specified' and '' - empty string) for ";
				for(size_t i = 0; i < opt_exceptions.size(); i++)
					err_msg += ((i == (opt_exceptions.size() - 1) && i != 0) ? "and " : "") + opt_exceptions[i] + ((i != (opt_exceptions.size() - 1) && i != (opt_exceptions.size() - 2)) ? "," : "") + " ";
			}
			err_msg += "will automatically bring ";
			for(size_t i = 0; i < event_exceptions.size(); i++)
				err_msg += ((i == (event_exceptions.size() - 1) && i != 0) ? "and " : "") + event_exceptions[i] + ((i != (event_exceptions.size() - 1) && i != (event_exceptions.size() - 2)) ? "," : "") + " ";
			if(event_exceptions.size() == 1)
				err_msg += "to its library defaults";
			else
				err_msg += "to their library defaults";
		}

		err_msg += "\nHint : Check also class level attribute properties";

		Except::throw_exception("API_AttrConfig",err_msg,origin);
	}
}

//+-------------------------------------------------------------------------
//
// method :		Attribute::prop_in_list
//
// description : Search for a property in a list
//
// args: in : - prop_name : The property name
//            - list_size : The size list
//            - list : The list
//       out : - prop_str : String initialized with prop. value (if found)
//
//--------------------------------------------------------------------------


inline bool Attribute::prop_in_list(const char *prop_name,string &prop_str,size_t list_size,vector<AttrProperty> &list)
{
    bool ret = false;

    if (list_size != 0)
    {
        size_t i;
        for (i = 0;i < list_size;i++)
        {
            if (list[i].get_name() == prop_name)
                break;
        }
        if (i != list_size)
        {
            prop_str = list[i].get_value();
            ret = true;
        }
    }

    return ret;
}

//
// Macro to help coding
//

#define MEM_STREAM_2_CORBA(A,B) \
	if (true) \
	{ \
		string s = B.str(); \
		A = CORBA::string_dup(s.c_str()); \
		B.str(""); \
		B.clear(); \
	} \
	else \
		(void)0

//
// Define one macro to make code more readable
// but this macro is nearly unreadable !!!
//
// Arg list :
//		A : property as a string
//		B : stream
//		C : device name
//		D : DbData for db update
//		E : DbData for db delete
//		F : Number of prop to update
//		G : Number of prop to delete
//		H : Property name
//		I : Default user properties vector ref
//      J : Default class properties vector ref
//
// Too many parameters ?
//
// Comment 1: If the user input value is equal to user default value do not store it in
// the database.
//
// Comment 2: User default value (if defined) has to be converted to double before it is
// compared with the user input to determine if to store the new value in the database.
// String comparison is not appropriate in case of floating point numbers,
// e.g. "5.0" is numerically equal to "5.00" but the strings differ.
//
// Comment 3: If user defaults are defined - at first place the input string is converted
// to double to determine if it is a number. If so, the double value is cast to the type
// corresponding with the type of the attribute and further compared with the double
// representation of the user default value.
// The purpose of casting is as follows.
// Lets take an example of an attribute of DevShort data type with user default value for
// min_alarm set to 5. Now, if the user inputs "5.678" as a new value for min_alarm
// it would normally be cast to DevShort and stored in the database as "5" (this is the
// standard behaviour if the user inputs a floating point value for a property of
// non-floating point type). But now the outcome "5" is equal to the user default value 5
// and should not be stored in the database. This is why there is the cast of the user
// input value to the attribute data type before comparison with the user default value.
//

#define CHECK_PROP(A,B,C,D,E,F,G,H,I,J) \
{ \
	size_t nb_user = I.size(); \
	size_t nb_class = J.size(); \
	string usr_def_val; \
	string class_def_val; \
	bool user_defaults = false; \
	bool class_defaults = false; \
	bool store_in_db = true; \
	bool avns = false; \
	bool user_val = false; \
    double user_def_val_db; \
    double class_def_val_db; \
    bool equal_user_def = false; \
    bool equal_class_def = false; \
\
    user_defaults = prop_in_list(H,usr_def_val,nb_user,I); \
    if (user_defaults) \
    { \
        B.str(""); \
        B.clear(); \
        B << usr_def_val; \
        B >> user_def_val_db; \
    } \
    class_defaults = prop_in_list(H,class_def_val,nb_class,J); \
    if (class_defaults) \
    { \
        B.str(""); \
        B.clear(); \
        B << class_def_val; \
        B >> class_def_val_db;  \
    } \
\
    if(user_defaults) \
    { \
        double db; \
        B.str(""); \
        B.clear(); \
        B << A; \
        if (B >> db && B.eof()) \
        { \
            switch (data_type) \
            { \
            case Tango::DEV_SHORT: \
                if((DevShort)db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_LONG: \
                if((DevLong)db == user_def_val_db) \
                    equal_user_def = true; \
                break;\
\
            case Tango::DEV_LONG64: \
                if((DevLong64)db == user_def_val_db) \
                    equal_user_def = true; \
                break;\
\
            case Tango::DEV_DOUBLE: \
                if(db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_FLOAT: \
                if(db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_USHORT: \
                if((DevUShort)db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_UCHAR: \
                if((DevUChar)db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_ULONG: \
                if((DevULong)db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_ULONG64: \
                if((DevULong64)db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
\
            case Tango::DEV_ENCODED: \
                if((DevUChar)db == user_def_val_db) \
                    equal_user_def = true; \
                break; \
            } \
        }\
    }\
\
    if(class_defaults) \
    { \
        double db; \
        B.str(""); \
        B.clear(); \
        B << A; \
        if (B >> db && B.eof()) \
        { \
            switch (data_type) \
            { \
            case Tango::DEV_SHORT: \
                if((DevShort)db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_LONG: \
                if((DevLong)db == class_def_val_db) \
                    equal_class_def = true; \
                break;\
\
            case Tango::DEV_LONG64: \
                if((DevLong64)db == class_def_val_db) \
                    equal_class_def = true; \
                break;\
\
            case Tango::DEV_DOUBLE: \
                if(db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_FLOAT: \
                if(db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_USHORT: \
                if((DevUShort)db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_UCHAR: \
                if((DevUChar)db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_ULONG: \
                if((DevULong)db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_ULONG64: \
                if((DevULong64)db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
\
            case Tango::DEV_ENCODED: \
                if((DevUChar)db == class_def_val_db) \
                    equal_class_def = true; \
                break; \
            } \
        }\
    }\
\
    if (TG_strcasecmp(A,AlrmValueNotSpec) == 0) \
    { \
        if (user_defaults == true || class_defaults == true) \
        { \
            store_in_db = true; \
            avns = true; \
        } \
        else \
        { \
			store_in_db = false; \
        } \
    } \
    else if (strlen(A) == 0) \
    { \
        if (class_defaults == true) \
        { \
			store_in_db = true; \
			if (user_defaults) \
                user_val = true; \
            else \
                avns = true; \
        } \
        else \
        { \
			store_in_db = false; \
        } \
    } \
    else if (TG_strcasecmp(A,NotANumber) == 0) \
    { \
        store_in_db = false; \
    } \
    else if (user_defaults == true && equal_user_def == true) \
    { \
        if (class_defaults == true) \
        { \
			store_in_db = true; \
			user_val = true; \
        } \
        else \
        { \
            store_in_db = false; \
        } \
    } \
    else if (class_defaults == true && equal_class_def == true) \
    { \
        store_in_db = false; \
    } \
    else \
    { \
        store_in_db = true; \
    } \
\
	if(store_in_db) \
	{ \
		string tmp = A.in(); \
		if(user_val == true) \
            tmp = usr_def_val.c_str(); \
		else if(avns == true) \
			tmp = AlrmValueNotSpec; \
		else \
		{ \
			if ((data_type != Tango::DEV_STRING) && \
				(data_type != Tango::DEV_BOOLEAN) && \
				(data_type != Tango::DEV_STATE)) \
			{ \
				double db; \
\
				B.str(""); \
				B.clear(); \
				B << A; \
                if (!(B >> db && B.eof())) \
                { \
                    throw_err_format(H,C,"Attribute::upd_database"); \
                }\
				switch (data_type) \
				{ \
				case Tango::DEV_SHORT: \
					B.str(""); \
					B.clear(); \
					B << (DevShort)db; \
					break; \
\
				case Tango::DEV_LONG: \
					B.str(""); \
					B.clear(); \
					B << (DevLong)db; \
					break;\
\
				case Tango::DEV_LONG64: \
					B.str(""); \
					B.clear(); \
					B << (DevLong64)db; \
					break;\
\
				case Tango::DEV_DOUBLE: \
					break; \
\
				case Tango::DEV_FLOAT: \
					break; \
\
				case Tango::DEV_USHORT: \
					B.str(""); \
					B.clear(); \
					(db < 0.0) ? B << (DevUShort)(-db) : B << (DevUShort)db; \
					break; \
\
				case Tango::DEV_UCHAR: \
					B.str(""); \
					B.clear(); \
					(db < 0.0) ? B << (short)((DevUChar)(-db)) : B << (short)((DevUChar)db); \
					break; \
\
				case Tango::DEV_ULONG: \
					B.str(""); \
					B.clear(); \
					(db < 0.0) ? B << (DevULong)(-db) : B << (DevULong)db; \
					break; \
\
				case Tango::DEV_ULONG64: \
					B.str(""); \
					B.clear(); \
					(db < 0.0) ? B << (DevULong64)(-db) : B << (DevULong64)db; \
					break; \
				} \
                if (data_type != Tango::DEV_FLOAT && data_type != Tango::DEV_DOUBLE) \
                    tmp = B.str(); \
			} \
			else \
			{ \
				throw_err_data_type(H,C,"Attribute::upd_database"); \
			} \
		} \
\
		DbDatum dd(H); \
		dd << tmp.c_str(); \
		D.push_back(dd); \
		F++; \
	} \
	else \
	{ \
		DbDatum del_dd(H); \
		E.push_back(del_dd); \
		G++; \
	} \
} \


//
// Yet another macros !!
// Arg list : 	A : The sequence pointer
//		B : Index in sequence
//		C : Attribute reference
//

#define GIVE_ATT_MUTEX(A,B,C) \
	if (true) \
	{\
		Tango::AttributeValue_4 *tmp_ptr = &((*A)[B]); \
		(tmp_ptr)->set_attr_mutex(C.get_attr_mutex()); \
	} \
	else \
		(void)0


#define GIVE_USER_ATT_MUTEX(A,B,C) \
	if (true) \
	{ \
		Tango::AttributeValue_4 *tmp_ptr = &((*A)[B]); \
		(tmp_ptr)->set_attr_mutex(C.get_user_attr_mutex()); \
	} \
	else \
		(void)0

//
// Yet another macro !!
// Arg list : 	A : The sequence pointer
//		B : Index in sequence
//		C : Attribute reference
//

#define REL_ATT_MUTEX(A,B,C) \
	if (C.get_attr_serial_model() != ATTR_NO_SYNC) \
	{ \
		Tango::AttributeValue_4 *tmp_ptr = &((*A)[B]); \
		(tmp_ptr)->rel_attr_mutex(); \
	} \
	else \
		(void)0

// Add template methods definitions

} // End of Tango namespace

#endif // _ATTRIBUTE_H
