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
// Copyright (C) :      2011
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


namespace Tango
{

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

} // End of Tango namespace

#endif // _ATTRIBUTE_TPP
