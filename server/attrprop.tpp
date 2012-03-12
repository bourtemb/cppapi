//+============================================================================
//
// file :               AttrProp.tpp
//
// description :        C++ source code for the MultiAttrProp class template
//                      methods
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012
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
// $Revision: 17240 $
//
//-============================================================================

#ifndef _ATTRPROP_TPP
#define _ATTRPROP_TPP

namespace Tango
{

//template <>
//class AttrProp<DevEncoded>
//{
//public:
///**@name Constructors
// * Miscellaneous constructors */
////@{
///**
// * Default constructor.
// */
//	AttrProp() {}
///**
// * Create a new AttrProp object.
// *
// * @param value The attribute property value.
// */
//	AttrProp(const DevUChar &value) : val(value), is_value(true)
//	{
//		TangoSys_MemStream st;
//		st.precision(TANGO_FLOAT_PRECISION);
//		st << (short)value; // to represent the numeric value
//		str = st.str();
//	}
///**
// * Create a new AttrProp object.
// *
// * @param value_str The 'C string' representation of attribute property.
// */
//	AttrProp(const char *value_str) : str(string(value_str)), is_value(false) {}
///**
// * Create a new AttrProp object.
// *
// * @param value_str The string representation of attribute property value.
// */
//	AttrProp(const string &value_str) : str(value_str), is_value(false) {}
////@}
//
//	operator string()
//	{
//		return str;
//	}
//	operator const char *()
//	{
//		return str.c_str();
//	}
//
///**@name Assignment operators
// * These operators allow to assign the value of the property by providing
// * the value or its string representation.
// */
////@{
///**
// * Assign the value of the attribute property.
// *
// * @param value A value of the attribute property.
// *
// * @return AttrProp object with both value and its string representation set.
// */
//	AttrProp &operator=(const DevUChar &value)
//	{
//		TangoSys_MemStream st;
//		st.precision(TANGO_FLOAT_PRECISION);
//		st << (short)value; // to represent the numeric value
//		str = st.str();
//		val = value;
//		is_value = true;
//		return *this;
//	}
///**
// * Assign the string representation of the attribute property value.
// *
// * @param value_str A 'C string' representation of the attribute property value.
// *
// * @return AttrProp object with the string representation of its value set.
// */
//	AttrProp &operator=(const char *value_str)
//	{
//		str = value_str;
//		is_value = false;
//		return *this;
//	}
///**
// * Assign the string representation of the attribute property value.
// *
// * @param value_str A string representation of the attribute property value.
// *
// * @return AttrProp object with the string representation of its value set.
// */
//	AttrProp &operator=(const string &value_str)
//	{
//		str = value_str;
//		is_value = false;
//		return *this;
//	}
////@}
//
///**@name Get/Set object members.
// * These methods allow the external world to get/set AttrProp instance
// * data members
// */
////@{
///**
// * Get the attribute property value.
// *
// * @return The attribute property value.
// */
//	DevUChar get_val()
//	{
//		if(is_value == false)
//		{
//			string err_msg = "Numeric representation of the property's value (" + str + ") has not been set";
//			Tango::Except::throw_exception("API_AttrPropValueNotSet",err_msg,"AttrProp::get_val",Tango::ERR);
//		}
//		return val;
//	}
///**
// * Get string representation of the attribute property value.
// *
// * @return The string representation of the attribute property value.
// */
//	string &get_str() {return str;}
///**
// * Set the attribute property value.
// *
// * The value is automatically converted to its string representation.
// *
// * @param value The value of the attribute property.
// */
//	void set_val(const DevUChar &value)
//	{
//		TangoSys_MemStream st;
//		st.precision(TANGO_FLOAT_PRECISION);
//		st << (short)value; // to represent the numeric value
//		str = st.str();
//		val = value;
//		is_value = true;
//	}
///**
// * Set string representation of the attribute property value.
// *
// * @param value_str The the 'C string' representation of the attribute property value.
// */
//	void set_str(const char *value_str) {str = string(value_str); is_value = false;}
///**
// * Set string representation of the attribute property value.
// *
// * @param value_str The the string representation of the attribute property value.
// */
//	void set_str(const string &value_str) {str = value_str; is_value = false;}
////@}
//
///**@name Check method
// * A method returning a boolean flag set to true if
// * the attribute property value has been assigned.
// */
////@{
///**
// * Check if the attribute property value has been assigned.
// *
// * This method returns a boolean set to true if the attribute property value has been assigned.
// *
// * @return A boolean set to true if the attribute property value has been assigned
// */
//	bool is_val() {return is_value;}
////@}
//
//private:
//	DevUChar val;
//	string str;
//	bool is_value;
//
////
//// The extension class
////
//
//	class AttrPropExt {};
//
//#ifdef HAS_UNIQUE_PTR
//    unique_ptr<AttrPropExt>		ext;           // Class extension
//#else
//    AttrPropExt					*ext;
//#endif
//};



template <>
class MultiAttrProp<DevEncoded>
{
public:
/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Default constructor.
 */
	MultiAttrProp() {}
//@}
/**@name Class data members */
//@{
/**
 * Attribute label
 */
		string 					label;
/**
 * Attribute description
 */
		string 					description;
/**
 * Attribute unit
 */
		string 					unit;
/**
 * Attribute standard_unit
 */
		string 					standard_unit;
/**
 * Attribute display_unit
 */
		string 					display_unit;
/**
 * Attribute format
 */
		string 					format;
/**
 * Attribute min_value
 */
        AttrProp<DevUChar>                 	min_value;
/**
 * Attribute max_value
 */
        AttrProp<DevUChar>                 	max_value;
/**
 * Attribute min_alarm
 */
        AttrProp<DevUChar>                 	min_alarm;
/**
 * Attribute max_alarm
 */
        AttrProp<DevUChar>                 	max_alarm;
/**
 * Attribute min_warning
 */
        AttrProp<DevUChar>                 	min_warning;
/**
 * Attribute max_warning
 */
        AttrProp<DevUChar>                 	max_warning;
/**
 * Attribute delta_t
 */
        AttrProp<DevLong>					delta_t;
/**
 * Attribute delta_val
 */
        AttrProp<DevUChar>                 	delta_val;
/**
 * Attribute event_period
 */
        AttrProp<DevLong>                 	event_period;
/**
 * Attribute archive_period
 */
        AttrProp<DevLong>                 	archive_period;
/**
 * Attribute rel_change
 */
        DoubleAttrProp<DevDouble>	        rel_change;
/**
 * Attribute abs_change
 */
        DoubleAttrProp<DevDouble>	        abs_change;
/**
 * Attribute archive_rel_change
 */
        DoubleAttrProp<DevDouble>	        archive_rel_change;
/**
 * Attribute archive_abs_change
 */
        DoubleAttrProp<DevDouble>	        archive_abs_change;
//@}
private:

//
// The extension class
//

	class MultiAttrPropExt {};

#ifdef HAS_UNIQUE_PTR
	unique_ptr<MultiAttrPropExt>	ext;           // Class extension
#else
	MultiAttrPropExt				*ext;
#endif
};

} // End of Tango namespace
#endif // _ATTRPROP_TPP
