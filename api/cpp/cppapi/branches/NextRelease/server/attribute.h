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
//=============================================================================

#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include <tango.h>
#include <attrdesc.h>
#include <encoded_attribute.h>
#include <functional>
#include <time.h>

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

template <class A1, class A2, class R>
struct WantedProp : public  binary_function<A1,A2,R>
{
	R operator() (A1 att,A2 name_str) const
	{
		return att.get_name() == name_str;
	}
};

template <class A1, class A2, class R>
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

class AttributeExt
{
public:
	AttributeExt() {}

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
	time_t				event_attr_conf_subscription;	// Last time() a subsription was made
	time_t				event_data_ready_subscription;	// Last time() a subsription was made
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
};

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
 * Miscellaneous method returning boolean flag accorrding to attribute state
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
 * These methods allows the external world to get/set DeviceImpl instance
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
 * Get attribute properties.
 *
 * This method initialise the fields of a AttributeConfig object with the
 * attribute properties value
 *
 * @param conf A AttributeConfig object.
 */
	void get_properties(Tango::AttributeConfig &conf);
/**
 * Get attribute properties version 2.
 *
 * This method initialise the fields of a AttributeConfig_2 object with the
 * attribute properties value
 *
 * @param conf A AttributeConfig_2 object.
 */
	void get_properties_2(Tango::AttributeConfig_2 &conf);
/**
 * Get attribute properties version 3.
 *
 * This method initialise the fields of a AttributeConfig_3 object with the
 * attribute properties value
 *
 * @param conf A AttributeConfig_3 object.
 */
	void get_properties_3(Tango::AttributeConfig_3 &conf);
/**
 * Set attribute properties.
 *
 * This method set the attribute properties value with the content
 * of the fileds in the AttributeConfig object
 *
 * @param conf A AttributeConfig object.
 * @param dev The device pointer.
 */
	void set_properties(const Tango::AttributeConfig &conf,Tango::DeviceImpl *dev);
/**
 * Set attribute properties version 3.
 *
 * This method set the attribute properties value with the content
 * of the fileds in the AttributeConfig_3 object
 *
 * @param conf A AttributeConfig_3 object.
 * @param dev The device pointer.
 */
	void set_properties(const Tango::AttributeConfig_3 &conf,Tango::DeviceImpl *dev);
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
 * event are verified and the event is only pushed if they are fullfilled.
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
 * event are verified and the event is only pushed if they are fullfilled.
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

//
// methods not usable for the external world
//
	virtual void set_rvalue() {};
	void delete_seq();
	bool check_scalar_wattribute();
	void wanted_date(bool flag) {date = flag;}
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

	bool change_event_subscribed() {if (ext->event_change_subscription != 0)return true;else return false;}
	bool periodic_event_subscribed() {if (ext->event_periodic_subscription != 0)return true;else return false;}
	bool archive_event_subscribed() {if (ext->event_archive_subscription != 0)return true;else return false;}
	bool quality_event_subscribed() {if (ext->event_quality_subscription != 0)return true;else return false;}
	bool user_event_subscribed() {if (ext->event_user_subscription != 0)return true;else return false;}

	void set_change_event_sub() {ext->event_change_subscription=time(NULL);}
	void set_periodic_event_sub() {ext->event_periodic_subscription=time(NULL);}
	void set_archive_event_sub() {ext->event_archive_subscription=time(NULL);}
	void set_quality_event_sub() {ext->event_quality_subscription=time(NULL);}
	void set_user_event_sub() {ext->event_user_subscription=time(NULL);}

	long get_attr_idx() {return ext->idx_in_attr;}
	void set_attr_idx(long new_idx) {ext->idx_in_attr=new_idx;}
	DeviceImpl *get_att_device();

	void Attribute_2_AttributeValue(Tango::AttributeValue_3 *,DeviceImpl *);
	void Attribute_2_AttributeValue(Tango::AttributeValue_4 *,DeviceImpl *);
	void AttributeValue_4_2_AttributeValue_3(const Tango::AttributeValue_4 *,Tango::AttributeValue_3 *);

#ifndef TANGO_HAS_LOG4TANGO
	friend ostream &operator<<(ostream &,Attribute &);
#endif // TANGO_HAS_LOG4TANGO
	friend class EventSupplier;
	friend class DServer;

private:
	void set_data_size();
	void throw_err_format(const char *,string &);
	void throw_err_data_type(const char *,string &);
	void throw_min_max_value(string &,string &,MinMaxValueCheck);
	void check_str_prop(const AttributeConfig &,DbData &,long &,DbData &,long &);

	unsigned long 		name_size;
	string 				name_lower;
	DevEncoded			enc_help;

	AttributeExt		*ext;

protected:
	virtual void init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name);
	virtual void init_event_prop(vector<AttrProperty> &prop_list);
	string &get_attr_value(vector<AttrProperty> &prop_list,const char *name);
	long get_lg_attr_value(vector<AttrProperty> &prop_list,const char *name);
	virtual bool check_rds_alarm() {return false;}
	bool check_level_alarm();
	bool check_warn_alarm();
	void upd_att_prop_db(Tango::Attr_CheckVal &,const char *);

	template <typename T>
    void check_hard_coded_properties(const T &);

    void throw_hard_coded_prop(const char *);

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

inline void Attribute::throw_hard_coded_prop(const char *prop_name)
{
    TangoSys_OMemStream desc;
    desc << "Attribute property " << prop_name << " is not changeable at run time" << ends;

    Except::throw_exception((const char *)"API_AttrNotAllowed",desc.str(),
				      	  (const char *)"Attribute::check_hard_coded_properties");
}

//
// Macro to help coding
//

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
#define MEM_STREAM_2_CORBA(A,B) NEW_MEM_STREAM_2_CORBA(A,B)
#else
#define MEM_STREAM_2_CORBA(A,B) OLD_MEM_STREAM_2_CORBA(A,B)
#endif

#define NEW_MEM_STREAM_2_CORBA(A,B) \
	if (true) \
	{ \
		string s = B.str(); \
		A = CORBA::string_dup(s.c_str()); \
		B.str(""); \
	} \
	else \
		(void)0

#define OLD_MEM_STREAM_2_CORBA(A,B) \
	if (true) \
	{ \
		char *tmp_str = B.str(); \
		A = CORBA::string_dup(tmp_str); \
		delete[]tmp_str; \
	} \
	else \
		(void)0

//
// Define one macro to make code more readable
// Arg list : 	A : property as a string
//		B : stream
//		C : device name
//		D : DbDatum for db update
//		E : DbDatum for db delete
//		F : Number of prop to update
//		G : Number of prop to delete
//		H : Property name
//
// Too many parameters ?
//

#define CHECK_PROP(A,B,C,D,E,F,G,H) \
	if ((strcmp(A,AlrmValueNotSpec) != 0) && \
	    (strcmp(A,NotANumber) != 0)) \
	{ \
		if ((data_type != Tango::DEV_STRING) && \
		    (data_type != Tango::DEV_BOOLEAN) && \
		    (data_type != Tango::DEV_STATE)) \
		{ \
			short sh; \
			DevLong lg; \
			double db; \
			float fl; \
			unsigned short ush; \
			unsigned char uch; \
			DevLong64 lg64; \
			DevULong ulg; \
			DevULong64 ulg64; \
\
			B.seekp(0); \
			B.seekg(0); \
			B.clear(); \
			B << A << ends; \
			switch (data_type) \
			{ \
			case Tango::DEV_SHORT: \
				if (!(B >> sh)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_LONG: \
				if (!(B >> lg)) \
					throw_err_format(H,C); \
				break;\
\
			case Tango::DEV_LONG64: \
				if (!(B >> lg64)) \
					throw_err_format(H,C); \
				break;\
\
			case Tango::DEV_DOUBLE: \
				if (!(B >> db)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_FLOAT: \
				if (!(B >> fl)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_USHORT: \
				if (!(B >> ush)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_UCHAR: \
				if (!(B >> uch)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_ULONG: \
				if (!(B >> ulg)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_ULONG64: \
				if (!(B >> ulg64)) \
					throw_err_format(H,C); \
				break; \
			} \
		} \
		else \
		{ \
			throw_err_data_type(H,C); \
		}\
\
		DbDatum max_val(H); \
		const char *tmp = A.in(); \
		max_val << tmp; \
		D.push_back(max_val); \
		F++; \
	} \
\
	if ((strcmp(A,NotANumber) == 0) || (TG_strcasecmp(A,AlrmValueNotSpec) == 0)) \
	{ \
		DbDatum max_val(H); \
		E.push_back(max_val); \
		G++; \
	}


//
// Define another macro to make code more readable !!
// Arg list : 	A : property as a string
//		B : stream
//		C : device name
//		D : DbDatum for db update
//		E : DbDatum for db delete
//		F : Number of prop to update
//		G : Number of prop to delete
//		H : Property name
//
// Too many parameters ?
//

#define CHECK_CH_PROP(A,B,C,D,E,F,G,H) \
	if ((strcmp(A,AlrmValueNotSpec) != 0) && \
	    (strcmp(A,NotANumber) != 0)) \
	{ \
		B.seekp(0); \
		B.seekg(0); \
		B.clear(); \
		string st(A); \
		string::size_type pos = st.find(','); \
		if (pos != string::npos) \
			replace(st.begin(),st.end(),',',' '); \
		B << st << ends; \
		double db1,db2; \
		if (!(B >> db1)) \
			throw_err_format(H,C); \
		if (pos != string::npos) \
		{ \
			if (!(B >> db2)) \
				throw_err_format(H,C); \
		}\
		else \
			db2 = db1; \
		db1 = fabs(db1); \
		db2 = fabs(db2); \
		DbDatum max_val(H); \
		if (db1 == db2) \
			max_val << db1; \
		else \
		{ \
			vector<double> vd(2); \
			vd[0] = db1; \
			vd[1] = db2; \
			max_val << vd; \
		} \
		D.push_back(max_val); \
		F++; \
	} \
\
	if ((strcmp(A,NotANumber) == 0) || (TG_strcasecmp(A,AlrmValueNotSpec) == 0)) \
	{ \
		DbDatum max_val(H); \
		E.push_back(max_val); \
		G++; \
	}


//
// Oh, a new macro !!
// Arg list : 	A : property as a string
//		B : stream
//		C : storage place
//

#define SET_EV_PROP(A,B,C) \
	if ((strcmp(A,NotANumber) == 0) || (TG_strcasecmp(A,AlrmValueNotSpec) == 0))\
	{ \
		ext->C[0] = INT_MAX; \
		ext->C[1] = INT_MAX; \
	} \
	else \
	{ \
		if (strcmp(A,AlrmValueNotSpec) != 0) \
		{ \
			double rel_change_min=INT_MAX, rel_change_max=INT_MAX; \
 			B.seekp(0); \
			B.seekg(0); \
			B.clear(); \
			string st(A); \
			string::size_type pos = st.find(','); \
			if (pos != string::npos) \
				replace(st.begin(),st.end(),',',' '); \
			B << st << ends; \
			B >> rel_change_min; \
			if (pos != string::npos) \
				B >> rel_change_max; \
        		if (fabs(rel_change_min) > 0 && rel_change_min != INT_MAX) \
			{ \
				ext->C[0] = -fabs(rel_change_min); \
				ext->C[1] = fabs(rel_change_min); \
        		} \
        		if (rel_change_max > 0 && rel_change_max != INT_MAX) \
			{ \
				ext->C[1] = fabs(rel_change_max); \
        		} \
		} \
	}

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

} // End of Tango namespace

// Add template methods definitions

#include <attribute.tpp>

#endif // _ATTRIBUTE_H
