static const char *RcsId = "$Id: device_4.cpp 20742 2012-06-21 14:20:20Z taurel $";

//+===================================================================================================================
//
// file :               Device_5.cpp
//
// description :        C++ source code for the Device_5Impl class. This class is the root class for all derived Device
//						classes starting with Tango 9
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision: 20742 $
//
//-===================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <device_5.h>
#include <eventsupplier.h>
#include <device_3.tpp>


namespace Tango
{

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::Device_5Impl
//
// description :
//		Constructors for the Device_5impl class from the class object pointer, the device name, the description field,
// 		the state and the status. Device_5Impl inherits from DeviceImpl. These constructors simply call the correct
//		DeviceImpl class constructor
//
//--------------------------------------------------------------------------------------------------------------------

Device_5Impl::Device_5Impl(DeviceClass *device_class,string &dev_name):
Device_4Impl(device_class,dev_name),ext_5(Tango_nullptr)
{
	idl_version = 5;
}

Device_5Impl::Device_5Impl(DeviceClass *device_class,
				string &dev_name,
				string &desc):
Device_4Impl(device_class,dev_name,desc),ext_5(Tango_nullptr)
{
	idl_version = 5;
}

Device_5Impl::Device_5Impl(DeviceClass *device_class,
				string &dev_name,string &desc,
				Tango::DevState dev_state,string &dev_status):
Device_4Impl(device_class,dev_name,desc,dev_state,dev_status),ext_5(Tango_nullptr)
{
	idl_version = 5;
}

Device_5Impl::Device_5Impl(DeviceClass *device_class,
				const char *dev_name,
				const char *desc,
				Tango::DevState dev_state,
				const char *dev_status):
Device_4Impl(device_class,dev_name,desc,dev_state,dev_status),ext_5(Tango_nullptr)
{
	idl_version = 5;
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::read_attributes_5
//
// description :
//		Method called for each read_attributes operation executed from any client on a Tango device version 5.
//
//------------------------------------------------------------------------------------------------------------------

Tango::AttributeValueList_5* Device_5Impl::read_attributes_5(const Tango::DevVarStringArray& names,
					     Tango::DevSource source,const Tango::ClntIdent &cl_id)
{
	cout4 << "Device_5Impl::read_attributes_5 arrived for dev " << get_name() << ", att[0] = " << names[0] << endl;

//
// Record operation request in black box
//

	if (store_in_bb == true)
		blackbox_ptr->insert_attr(names,cl_id,5,source);
	store_in_bb = true;

//
// Build a sequence with the names of the attribute to be read. This is necessary in case of the "AllAttr" shortcut is
// used. If all attributes are wanted, build this list
//

	unsigned long nb_names = names.length();
	unsigned long nb_dev_attr = dev_attr->get_attr_nb();
	Tango::DevVarStringArray real_names(nb_names);
	unsigned long i;

	if (nb_names == 1)
	{
		string att_name(names[0]);
		if (att_name == AllAttr)
		{
			real_names.length(nb_dev_attr);
			for (i = 0;i < nb_dev_attr;i++)
			{
				real_names[i] = dev_attr->get_attr_by_ind(i).get_name().c_str();
			}
		}
		else
		{
			real_names = names;
		}
	}
	else
	{
		real_names = names;
	}
	nb_names = real_names.length();

//
// Allocate memory for the AttributeValue structures
//

	AttributeIdlData aid;

	try
	{
		Tango::AttributeValue_5 *l_back = new Tango::AttributeValue_5 [nb_names];
		aid.data_5 = new Tango::AttributeValueList_5(nb_names,nb_names,l_back,true);

		for (unsigned long loop = 0;loop < nb_names;loop++)
			(*aid.data_5)[loop].value.union_no_data(true);
	}
	catch (bad_alloc)
	{
		Except::throw_exception(API_MemoryAllocation,"Can't allocate memory in server",
				        "Device_5Impl::read_attributes_5");
	}

//
// Store source parameter (used in case of forwarded attribute(s))
//

	set_call_source(source);

//
// If the source parameter specifies device, call the read_attributes method which does not throw exception except for
// major fault (cant allocate memory,....)
//

	vector <long> idx_in_back;

	if (source == Tango::DEV)
	{
		try
		{
			AutoTangoMonitor sync(this);
			read_attributes_no_except(real_names,aid,false,idx_in_back);
		}
		catch (...)
		{
			delete aid.data_5;
			throw;
		}
	}
	else if (source == Tango::CACHE)
	{

//
// If the device has some forwarded attribute, do not try to get data from the local cache but get it
// from the root device
// Create two arrays with local attribute names and fwd attribute names
//

		bool fwd_att_in_call = false;
		Tango::DevVarStringArray local_names(nb_names);
		Tango::DevVarStringArray fwd_names(nb_names);

		if (with_fwd_att == true)
		{
			for (size_t loop = 0;loop < nb_names;loop++)
			{
				Attribute &att = dev_attr->get_attr_by_name(real_names[loop]);
				size_t nb_fwd = 0;
				size_t nb_local = 0;

				if (att.is_fwd_att() == true)
				{
					fwd_att_in_call = true;
					nb_fwd++;
					fwd_names.length(nb_fwd);
					fwd_names[nb_fwd - 1] = real_names[loop];
					idx_in_back.push_back(loop);
				}
				else
				{
					nb_local++;
					local_names.length(nb_local);
					local_names[nb_local - 1] = real_names[loop];
				}
			}
		}

		if (fwd_att_in_call == false)
		{

//
// No fwd attributes in call, get values from local cache
//

			try
			{
				TangoMonitor &mon = get_poll_monitor();
				AutoTangoMonitor sync(&mon);
				read_attributes_from_cache(real_names,aid);
			}
			catch (...)
			{
				delete aid.data_5;
				throw;
			}
		}
		else
		{

//
// Some fwd attributes in call: First get data for local attributes then get data for forwarded attributes from
// root devices
//

			try
			{
				TangoMonitor &mon = get_poll_monitor();
				{
					AutoTangoMonitor sync(&mon);
					read_attributes_from_cache(local_names,aid);
				}

				{
					AutoTangoMonitor sync(this);
					read_attributes_no_except(fwd_names,aid,true,idx_in_back);
					idx_in_back.clear();
				}
			}
			catch (...)
			{
				delete aid.data_5;
				throw;
			}
		}
	}
	else
	{

//
// It must be now CACHE_DEVICE (no other choice), first try to get
// values from cache
//

		try
		{
			TangoMonitor &mon = get_poll_monitor();
			AutoTangoMonitor sync(&mon);
			read_attributes_from_cache(real_names,aid);
		}
		catch (...)
		{
			delete aid.data_5;
			throw;
		}

//
// Now, build the list of attributes which it was not possible
// to get their value from cache
//

		Tango::DevVarStringArray names_from_device(nb_names);
		long nb_attr = 0;

       	for (i = 0;i < nb_names;i++)
       	{
       		long nb_err = (*aid.data_5)[i].err_list.length();
       		if (nb_err != 0)
       		{
       			nb_err--;
       			if ((strcmp((*aid.data_5)[i].err_list[nb_err].reason,API_AttrNotPolled) == 0) ||
       					(strcmp((*aid.data_5)[i].err_list[nb_err].reason,API_NoDataYet) == 0) ||
       					(strcmp((*aid.data_5)[i].err_list[nb_err].reason,API_NotUpdatedAnyMore) == 0) ||
       					(strcmp((*aid.data_5)[i].err_list[nb_err].origin,"DServer::add_obj_polling") == 0))
       			{
       				nb_attr++;
       				names_from_device.length(nb_attr);
       				names_from_device[nb_attr - 1] = real_names[i];
       				idx_in_back.push_back(i);

       				(*aid.data_5)[i].err_list.length(0);
       			}
       		}
       	}

		if (nb_attr != 0)
		{

//
// Try to get their values from device
//

			try
			{
				AutoTangoMonitor sync(this);
				read_attributes_no_except(names_from_device,aid,true,idx_in_back);
			}
			catch (...)
			{
				delete aid.data_5;
				throw;
			}
		}
	}

	return aid.data_5;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::write_read_attributes_5
//
// description :
//		CORBA operation to write then read attribute(s) value. Nowadays, it is possible to write then read attribute
//		for only 1 attribute at a time even if the IDL is written for several attribute
//
//------------------------------------------------------------------------------------------------------------------

Tango::AttributeValueList_5* Device_5Impl::write_read_attributes_5(const Tango::AttributeValueList_4 &values,
									  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_5Impl::write_read_attributes_5 arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_wr_attr(values,cl_id,5);

//
// Check if the device is locked and by who
//

	check_lock("write_read_attributes_5");

//
// Check the attribute write type (only READ_WRITE or READ_WITH_WRITE allowed)
//

	Tango::Attribute &att = dev_attr->get_attr_by_name(values[0].name);
	Tango::AttrWriteType awt = att.get_writable();
	if ((awt == Tango::READ) || (awt == Tango::WRITE))
	{
		TangoSys_OMemStream o;
		o << "Attribute " << values[0].name << " is not a READ_WRITE or READ_WITH_WRITE attribute" << ends;

		Except::throw_exception((const char *)API_AttrNotWritable,o.str(),
								(const char *)"Device_5Impl::write_read_attribute_5");
	}

	Tango::AttributeValueList_5 *read_val_ptr;

//
// Check if the attribute is a forwarded one. If yes, do special stuff
//

	if (att.is_fwd_att() == true)
	{
		try
		{
			FwdAttribute &fwd_att = static_cast<FwdAttribute &>(att);
			read_val_ptr = fwd_att.write_read_root_att(const_cast<AttributeValueList_4 &>(values));
		}
		catch (Tango::DevFailed &e)
		{
			stringstream ss;
			ss << "Write_read_attribute on attribute " << att.get_name() << " on device " << get_name() << " failed!";
			Tango::Except::re_throw_exception(e,API_AttributeFailed,ss.str(),"Device_5Impl::write_read_attribute_5");
		}
	}
	else
	{
//
// First, write the attribute
//

		store_in_bb = false;
		write_attributes_4(values,cl_id);

//
// Now, read the attribute
//

		Tango::DevVarStringArray att_name(1);
		att_name.length(1);
		att_name[0] = CORBA::string_dup(values[0].name);
		Tango::ClntIdent dummy_cl_id;
		Tango::CppClntIdent cci = 0;
		dummy_cl_id.cpp_clnt(cci);

		store_in_bb = false;
		read_val_ptr = read_attributes_5(att_name,Tango::DEV,dummy_cl_id);
	}

	return read_val_ptr;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::get_attribute_config_5
//
// description :
//		CORBA operation to get attribute configuration.
//
// 		WARNING !!!!!!!!!!!!!!!!!!
//
// 		This is the release 3 of this CORBA operation which returns much more parameter than in release 2
// 		The code has been duplicated in order to keep it clean (avoid many "if" on version number in a common method)
//
// argument:
//		in :
//			- names: name of attribute(s)
//
// return :
//		Pointer to a AttributeConfigList_5 with one AttributeConfig_5 structure for each attribute
//
//-------------------------------------------------------------------------------------------------------------------

Tango::AttributeConfigList_5 *Device_5Impl::get_attribute_config_5(const Tango::DevVarStringArray& names)
{
	TangoMonitor &mon = get_att_conf_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_5Impl::get_attribute_config_5 arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList_5 *back = NULL;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config_5);

//
// Get attribute number and device version
//

	long nb_dev_attr = dev_attr->get_attr_nb();

//
// Check if the caller want to get config for all attribute.
// If the device implements IDL 3 or above  (State and status as attributes) and the client is an old one (not able to
// read state/status as attribute), decrement attribute number
//

	string in_name(names[0]);
	if (nb_attr == 1)
	{
		if (in_name == AllAttr_3)
		{
			all_attr = true;
			nb_attr = nb_dev_attr;
		}
	}

//
// Allocate memory for the AttributeConfig structures
//

	try
	{
		back = new Tango::AttributeConfigList_5(nb_attr);
		back->length(nb_attr);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_5Impl::get_attribute_config_5");
	}

//
// Fill in these structures
//

	for (long i = 0;i < nb_attr;i++)
	{
		try
		{
			if (all_attr == true)
			{
				Attribute &attr = dev_attr->get_attr_by_ind(i);
				attr.get_properties((*back)[i]);
			}
			else
			{
				Attribute &attr = dev_attr->get_attr_by_name(names[i]);
				attr.get_properties((*back)[i]);
			}
		}
		catch (Tango::DevFailed &)
		{
			delete back;
			throw;
		}
	}

//
// Return to caller
//

	cout4 << "Leaving Device_3Impl::get_attribute_config_5" << endl;

	return back;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::set_attribute_config_5
//
// description :
//		CORBA operation to set attribute configuration locally and in the Tango database
//
// argument:
//		in :
//			- new_conf: The new attribute(s) configuration. One AttributeConfig structure is needed for each
//				    	attribute to update
//
//--------------------------------------------------------------------------------------------------------------------

void Device_5Impl::set_attribute_config_5(const Tango::AttributeConfigList_5& new_conf,
										  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_5Impl::set_attribute_config_5 arrived" << endl;

//
// The attribute conf. is protected by two monitors. One protects access between get and set attribute conf.
// The second one protects access between set and usage. This is the classical device monitor
//

	TangoMonitor &mon1 = get_att_conf_monitor();
	AutoTangoMonitor sync1(&mon1);

//
// Is it from the fwd attribute callback ?
//

	bool from_fwd_cb = false;
	Tango::LockerLanguage cl_lang = cl_id._d();
	if (cl_lang == Tango::CPP && cl_id.cpp_clnt() == 0)
	{
		from_fwd_cb = true;
	}

//
// Record operation request in black box
//

	if (from_fwd_cb == false)
		blackbox_ptr->insert_op(Op_Set_Attr_Config_5,cl_id);

//
// Check if the device is locked and by who
//

	check_lock("set_attribute_config_5");

//
// Call the Device_3Impl set_attribute_config
//

	store_in_bb = false;
	return set_attribute_config_3_local(new_conf,new_conf[0],from_fwd_cb,5);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::read_attribute_history_5
//
// description :
//		CORBA operation to read attribute value history from the polling buffer.
//
// argument:
//		in :
//			- name : attribute name
//			- n : history depth (in record number)
//
// return :
//		A pointer to a DevAttrHistory_5 structure
//
//-------------------------------------------------------------------------------------------------------------------

Tango::DevAttrHistory_5 *Device_5Impl::read_attribute_history_5(const char* name,CORBA::Long n)
{
	TangoMonitor &mon = get_poll_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_5Impl::read_attribute_history_5 arrived, requested history depth = " << n << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Read_Attr_history_5);

	Tango::DevAttrHistory_5 *back = Tango_nullptr;
	vector<PollObj *> &poll_list = get_poll_obj_list();
	long nb_poll = poll_list.size();

//
// Check that the device supports this attribute. This method returns an
// exception in case of unsupported attribute
//

	Attribute &att = dev_attr->get_attr_by_name(name);

	string attr_str(name);
	transform(attr_str.begin(),attr_str.end(),attr_str.begin(),::tolower);

//
// Check that the wanted attribute is polled (Except in case of forwarded attribute)
//

	long j;
	PollObj *polled_attr = NULL;
	for (j = 0;j < nb_poll;j++)
	{
		if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
			(poll_list[j]->get_name() == attr_str))
		{
			polled_attr = poll_list[j];
			break;
		}
	}
	if ((polled_attr == NULL) && (att.is_fwd_att() == false))
	{
		TangoSys_OMemStream o;
		o << "Attribute " << attr_str << " not polled" << ends;
		Except::throw_exception(API_AttrNotPolled,o.str(),"Device_5Impl::read_attribute_history_5");
	}

//
// If it is a forwarded attribute, get data from the root attribute
//

	if (att.is_fwd_att() == true)
	{
		try
		{
			FwdAttribute &fwd_att = static_cast<FwdAttribute &>(att);
			back = fwd_att.read_root_att_history(n);
		}
		catch (Tango::DevFailed &e)
		{
			stringstream ss;
			ss << "Reading history for attribute " << attr_str << " on device " << get_name() << " failed!";
			Tango::Except::re_throw_exception(e,API_AttributeFailed,ss.str(),"Device_5Impl::read_attribute_history_5");
		}
	}
	else
	{

//
// Check that some data is available in cache
//

		if (polled_attr->is_ring_empty() == true)
		{
			TangoSys_OMemStream o;
			o << "No data available in cache for attribute " << attr_str << ends;
			Except::throw_exception(API_NoDataYet,o.str(),"Device_5Impl::read_attribute_history_5");
		}

//
// Set the number of returned records
//

		long in_buf = polled_attr->get_elt_nb_in_buffer();
		if (n > in_buf)
			n = in_buf;

//
// Allocate memory for the returned value
//

		try
		{
			back = new Tango::DevAttrHistory_5;
			back->dates.length(n);
		}
		catch (bad_alloc)
		{
			Except::throw_exception(API_MemoryAllocation,
								"Can't allocate memory in server",
								"Device_5Impl::read_attribute_history_5");
		}

//
// Init attribute name in the returned structure
//

		back->name = CORBA::string_dup(name);

//
// Get attribute value history
// Trick: To identify the state used as an attribute from a classical attribute with type
// DEV_STATE, use DEV_VOID for state as data type.
//

		if (att.get_name_lower() == "state")
			polled_attr->get_attr_history(n,back,Tango::DEV_VOID);
		else
			polled_attr->get_attr_history(n,back,att.get_data_type());
	}

	cout4 << "Leaving Device_5Impl::read_attribute_history_5 method" << endl;
	return back;
}

} // End of Tango namespace
