//+============================================================================
//
// file :               Attribute.tpp
//
// description :        C++ source code for the Attribute class template
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

#ifndef _ATTRIBUTE_TPP
#define _ATTRIBUTE_TPP


//+-------------------------------------------------------------------------
//
// method : 		Attribute::check_hard_coded_properties()
//
// description : 	Check if the user tries to change attribute
//                  properties considered as hard coded
//                  Throw exception in case of
//
// in :	user_conf : The attribute configuration sent by the user
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::check_hard_coded_properties(const T &user_conf)
{
//
// Check attribute name
//

    string user_att_name(user_conf.name.in());
    transform(user_att_name.begin(),user_att_name.end(),user_att_name.begin(),::tolower);
    if (user_att_name != get_name_lower())
    {
        throw_hard_coded_prop("name");
    }

//
// Check data type
//

    if (user_conf.data_type != data_type)
    {
        throw_hard_coded_prop("data_type");
    }

//
// Check data format
//

    if (user_conf.data_format != data_format)
    {
        throw_hard_coded_prop("data_format");
    }

//
// Check writable
//

    if (user_conf.writable != writable)
    {
        throw_hard_coded_prop("writable");
    }

//
// Check max_dim_x
//

    if (user_conf.max_dim_x != max_x)
    {
        throw_hard_coded_prop("max_dim_x");
    }

//
// Check max_dim_y
//

    if (user_conf.max_dim_y != max_y)
    {
        throw_hard_coded_prop("max_dim_y");
    }

//
// Check writable_attr_name
//

    string local_w_name(writable_attr_name);
    transform(local_w_name.begin(),local_w_name.end(),local_w_name.begin(),::tolower);
    string user_w_name(user_conf.writable_attr_name.in());
    transform(user_w_name.begin(),user_w_name.end(),user_w_name.begin(),::tolower);

    if (user_w_name != local_w_name)
    {
        throw_hard_coded_prop("writable_attr_name");
    }
}

template <typename T>
void Attribute::set_min_alarm(const T &new_min_alarm)
{

//
// Store new min alarm as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_min_alarm; // to represent the numeric value
	else
		str << new_min_alarm;
	string min_alarm_str;
	min_alarm_str = str.str();

//
// Make a copy of min alarm value in binary format
//
	Attr_CheckVal min_alarm_bin;
	memcpy((void *)&min_alarm_bin, (void *)&new_min_alarm, sizeof(T));

//
// Extract min alarm type information and execute the code of the setter implementation
//

	attr_range min_alarm_range(min_alarm_bin, min_alarm_str, ranges_type2const<T>::enu, ranges_type2const<T>::str, sizeof(T));
	set_min_alarm_impl(min_alarm_range);
}

template <typename T>
void Attribute::get_min_alarm(T &min_al)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Incompatible attribute type, expected type is : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_min_alarm()");
	}

	if (!alarm_conf[min_level])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Minimum alarm not defined for this attribute",
					(const char *)"Attribute::get_min_alarm()");
	}

	memcpy((void *)&min_al,(void *)&min_alarm,sizeof(T));
}

template <typename T>
void Attribute::set_min_warning(const T &new_min_warning)
{

//
// Store new min warning as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_min_warning; // to represent the numeric value
	else
		str << new_min_warning;
	string min_warning_str;
	min_warning_str = str.str();

//
// Make a copy of min warning value in binary format
//
	Attr_CheckVal min_warning_bin;
	memcpy((void *)&min_warning_bin, (void *)&new_min_warning, sizeof(T));

//
// Extract min warning type information and execute the code of the setter implementation
//

	attr_range min_warninig_range(min_warning_bin, min_warning_str, ranges_type2const<T>::enu, ranges_type2const<T>::str, sizeof(T));
	set_min_warning_impl(min_warninig_range);
}

template <typename T>
void Attribute::get_min_warning(T &min_war)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Incompatible attribute type, expected type is : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_min_warning()");
	}

	if (!alarm_conf[min_warn])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Minimum warning not defined for this attribute",
					(const char *)"Attribute::get_min_warning()");
	}

	memcpy((void *)&min_war,(void *)&min_warning,sizeof(T));
}

template <typename T>
void Attribute::set_max_warning(const T &new_max_warning)
{

//
// Store new max warning as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_max_warning; // to represent the numeric value
	else
		str << new_max_warning;
	string max_warning_str;
	max_warning_str = str.str();

//
// Make a copy of max warning value in binary format
//
	Attr_CheckVal max_warning_bin;
	memcpy((void *)&max_warning_bin, (void *)&new_max_warning, sizeof(T));

//
// Extract max warning type information and execute the code of the setter implementation
//

	attr_range max_warninig_range(max_warning_bin, max_warning_str, ranges_type2const<T>::enu, ranges_type2const<T>::str, sizeof(T));
	set_max_warning_impl(max_warninig_range);
}

template <typename T>
void Attribute::get_max_warning(T &max_war)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Incompatible attribute type, expected type is : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_max_warning()");
	}

	if (!alarm_conf[max_warn])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Maximum warning not defined for this attribute",
					(const char *)"Attribute::get_max_warning()");
	}

	memcpy((void *)&max_war,(void *)&max_warning,sizeof(T));
}

template <typename T>
void Attribute::set_max_alarm(const T &new_max_alarm)
{

//
// Store new max alarm as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_max_alarm; // to represent the numeric value
	else
		str << new_max_alarm;
	string max_alarm_str;
	max_alarm_str = str.str();

//
// Make a copy of max alarm value in binary format
//
	Attr_CheckVal max_alarm_bin;
	memcpy((void *)&max_alarm_bin, (void *)&new_max_alarm, sizeof(T));

//
// Extract max alarm type information and execute the code of the setter implementation
//

	attr_range max_alarm_range(max_alarm_bin, max_alarm_str, ranges_type2const<T>::enu, ranges_type2const<T>::str, sizeof(T));
	set_max_alarm_impl(max_alarm_range);
}

template <typename T>
void Attribute::get_max_alarm(T &max_al)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Incompatible attribute type, expected type is : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_max_alarm()");
	}

	if (!alarm_conf[max_level])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Maximum alarm not defined for this attribute",
					(const char *)"Attribute::get_max_alarm()");
	}

	memcpy((void *)&max_al,(void *)&max_alarm,sizeof(T));
}

#endif // _ATTRIBUTE_TPP
