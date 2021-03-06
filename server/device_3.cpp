static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               Device_3.cpp
//
// description :        C++ source code for the DeviceImpl and DeviceClass
//			classes. These classes
//			are the root class for all derived Device classes.
//			They are abstract classes. The DeviceImpl class is the
//			CORBA servant which is "exported" onto the network and
//			accessed by the client.
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <device_3.h>
#include <new>

#include <eventsupplier.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */


namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::Device_3Impl
//
// description : 	constructors for the device_impl class from the
//			class object pointer, the device name,
//			the description field, the state and the status.
//			Device_3Impl inherits from DeviceImpl. These constructors
//			simply call the correct DeviceImpl class
//			constructor
//
//--------------------------------------------------------------------------

Device_3Impl::Device_3Impl(DeviceClass *device_class,string &dev_name):
Device_2Impl(device_class,dev_name),ext_3(new Device_3ImplExt)
{
    real_ctor();
}

Device_3Impl::Device_3Impl(DeviceClass *device_class,
			   string &dev_name,
			   string &desc):
Device_2Impl(device_class,dev_name,desc),ext_3(new Device_3ImplExt)
{
    real_ctor();
}

Device_3Impl::Device_3Impl(DeviceClass *device_class,
	           	   string &dev_name,string &desc,
	           	   Tango::DevState dev_state,string &dev_status):
Device_2Impl(device_class,dev_name,desc,dev_state,dev_status),ext_3(new Device_3ImplExt)
{
    real_ctor();
}

Device_3Impl::Device_3Impl(DeviceClass *device_class,
	           	   const char *dev_name,
                   const char *desc,
	           	   Tango::DevState dev_state,
	           	   const char *dev_status):
Device_2Impl(device_class,dev_name,desc,dev_state,dev_status),ext_3(new Device_3ImplExt)
{
    real_ctor();
}

void Device_3Impl::real_ctor()
{
    ext->idl_version = 3;
	add_state_status_attrs();

	init_cmd_poll_period();
	init_attr_poll_period();

    Tango::Util *tg = Tango::Util::instance();
	if (tg->_UseDb == false)
	{
	    init_poll_no_db();
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::read_attributes_3
//
// description : 	Method called for each read_attributes operation executed
//			from any client on a Tango device version 3.
//
//--------------------------------------------------------------------------

Tango::AttributeValueList_3* Device_3Impl::read_attributes_3(const Tango::DevVarStringArray& names,
					     Tango::DevSource source)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "Device_3Impl::read_attributes_3 arrived for dev " << get_name() << ", att[0] = " << names[0] << endl;

//
// Record operation request in black box
//

	if (ext->store_in_bb == true)
		blackbox_ptr->insert_attr(names,3,source);
	ext->store_in_bb = true;

//
// Build a sequence with the names of the attribute to be read.
// This is necessary in case of the "AllAttr" shortcut is used
// If all attributes are wanted, build this list
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

	Tango::AttributeValueList_3 *back;
	Tango::AttributeValueList_4 *back4 = NULL;
	try
	{
		back = new Tango::AttributeValueList_3(nb_names);
		back->length(nb_names);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_3Impl::read_attributes_3");
	}

//
// If the source parameter specifies device, call the read_attributes method
// which does not throw exception except for major fault (cannot allocate
// memory,....)
//
	vector <long> idx_in_back;

	if (source == Tango::DEV)
	{
		try
		{
			AutoTangoMonitor sync(this);
			read_attributes_no_except(real_names,back,back4,false,idx_in_back);
		}
		catch (...)
		{
			delete back;
			throw;
		}
	}
	else if (source == Tango::CACHE)
	{
		try
		{
			TangoMonitor &mon = get_poll_monitor();
			AutoTangoMonitor sync(&mon);
			read_attributes_from_cache(real_names,back,back4);
		}
		catch (...)
		{
			delete back;
			throw;
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
			read_attributes_from_cache(real_names,back,back4);
		}
		catch (...)
		{
			delete back;
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
       		long nb_err = (*back)[i].err_list.length();
       		if (nb_err != 0)
       		{
       			nb_err--;
       			if ((strcmp((*back)[i].err_list[nb_err].reason,"API_AttrNotPolled") == 0) ||
       					(strcmp((*back)[i].err_list[nb_err].reason,"API_NoDataYet") == 0) ||
       					(strcmp((*back)[i].err_list[nb_err].reason,"API_NotUpdatedAnyMore") == 0) ||
       					(strcmp((*back)[i].err_list[nb_err].origin,"DServer::add_obj_polling") == 0))
       			{
       				nb_attr++;
       				names_from_device.length(nb_attr);
       				names_from_device[nb_attr - 1] = real_names[i];
       				idx_in_back.push_back(i);

       				(*back)[i].err_list.length(0);
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
				read_attributes_no_except(names_from_device,back,back4,true,idx_in_back);
			}
			catch (...)
			{
				delete back;
				throw;
			}

		}
	}

	return back;

}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::read_attributes_no_except
//
// description : 	Read attributes from device but do not throw exception
//			if it fails. This method is mainly a copy of the original
//			DeviceImpl::read_attributes method.
//
// argument: in :	- names: The names of the attribute to read
//			- back : Pointer to the sequence returned to the client.
//			- back4 : Pointer to the sequence returned to the client if
//					  the request arrives through IDL version 4
//				 Memory has already been allocated for
//				 this pointer
//
//--------------------------------------------------------------------------


void Device_3Impl::read_attributes_no_except(const Tango::DevVarStringArray& names,
					     Tango::AttributeValueList_3 *&back,
					     Tango::AttributeValueList_4 *&back4,
					     bool second_try,
					     vector<long> &idx)
{
//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//

	SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
	string last_associated_device = sub.get_associated_device();
	sub.set_associated_device(get_name());

//
// Catch all exceptions to set back the associated device after
// execution
//

	try
	{

//
// Retrieve index of wanted attributes in the device attribute list and clear
// their value set flag
//

		long nb_names = names.length();
		vector<AttIdx> wanted_attr;
		vector<AttIdx> wanted_w_attr;
		bool state_wanted = false;
		bool status_wanted = false;
		long state_idx,status_idx;
		long i;

		state_idx = status_idx = -1;

		for (i = 0;i < nb_names;i++)
		{
			AttIdx x;
			x.idx_in_names = i;
			string att_name(names[i]);
			transform(att_name.begin(),att_name.end(),att_name.begin(),::tolower);

			if (att_name == "state")
			{
				x.idx_in_multi_attr = -1;
				x.failed = false;
				wanted_attr.push_back(x);
				state_wanted = true;
				state_idx = i;
			}
			else if (att_name == "status")
			{
				x.idx_in_multi_attr = -1;
				x.failed = false;
				wanted_attr.push_back(x);
				status_wanted = true;
				status_idx = i;
			}
			else
			{
				try
				{
				    long j;

					j = dev_attr->get_attr_ind_by_name(names[i]);
					if ((dev_attr->get_attr_by_ind(j).get_writable() == Tango::READ_WRITE) ||
				    	(dev_attr->get_attr_by_ind(j).get_writable() == Tango::READ_WITH_WRITE))
					{
						x.idx_in_multi_attr = j;
						x.failed = false;
						Attribute &att = dev_attr->get_attr_by_ind(x.idx_in_multi_attr);
						if(att.is_startup_exception())
							att.throw_startup_exception("Device_3Impl::read_attributes_no_except()");
						wanted_w_attr.push_back(x);
						wanted_attr.push_back(x);
						att.set_value_flag(false);
						att.get_when().tv_sec = 0;
                        att.save_alarm_quality();
					}
					else
					{
						if (dev_attr->get_attr_by_ind(j).get_writable() == Tango::WRITE)
						{
							x.idx_in_multi_attr = j	;
							x.failed = false;
							Attribute &att = dev_attr->get_attr_by_ind(x.idx_in_multi_attr);
							if(att.is_startup_exception())
								att.throw_startup_exception("Device_3Impl::read_attributes_no_except()");
							wanted_w_attr.push_back(x);
						}
						else
						{
							x.idx_in_multi_attr = j;
							x.failed = false;
							Attribute &att = dev_attr->get_attr_by_ind(x.idx_in_multi_attr);
							if(att.is_startup_exception())
								att.throw_startup_exception("Device_3Impl::read_attributes_no_except()");
							wanted_attr.push_back(x);
							att.set_value_flag(false);
							att.get_when().tv_sec = 0;
                            att.save_alarm_quality();
						}
					}
				}
				catch (Tango::DevFailed &e)
				{
					long index;
					if (second_try == false)
						index = i;
					else
						index = idx[i];

					if (back != NULL)
					{
						(*back)[index].err_list = e.errors;
						(*back)[index].quality = Tango::ATTR_INVALID;
						(*back)[index].name = CORBA::string_dup(names[i]);
						clear_att_dim((*back)[index]);
					}
					else
					{
						(*back4)[index].err_list = e.errors;
						(*back4)[index].quality = Tango::ATTR_INVALID;
						(*back4)[index].name = CORBA::string_dup(names[i]);
						clear_att_dim((*back4)[index]);
					}
				}
			}
		}

		long nb_wanted_attr = wanted_attr.size();
		long nb_wanted_w_attr = wanted_w_attr.size();

//
// Call the always_executed_hook
//

		always_executed_hook();

//
// Read the hardware for readable attribute but not for state/status
// Warning:  If the state is one of the wanted attribute, check and
// eventually add all the alarmed attributes index
//

		if (nb_wanted_attr != 0)
		{
			vector<long> tmp_idx;
			for (i = 0;i < nb_wanted_attr;i++)
			{
				long ii = wanted_attr[i].idx_in_multi_attr;
				if (ii != -1)
					tmp_idx.push_back(ii);
			}
			if (state_wanted == true)
			{
				if ((device_state == Tango::ON) || (device_state == Tango::ALARM))
					add_alarmed(tmp_idx);
			}

			if (tmp_idx.empty() == false)
				read_attr_hardware(tmp_idx);
		}

//
// Set attr value (for readable attribute) but not for state/status
//

		for (i = 0;i < nb_wanted_attr;i++)
		{
			if (wanted_attr[i].idx_in_multi_attr != -1)
			{
				Attribute &att = dev_attr->get_attr_by_ind(wanted_attr[i].idx_in_multi_attr);
				bool is_allowed_failed = false;

				try
				{
					vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();
					if (attr_vect[att.get_attr_idx()]->is_allowed(this,Tango::READ_REQ) == false)
					{
						is_allowed_failed = true;
						TangoSys_OMemStream o;

						o << "It is currently not allowed to read attribute ";
						o << att.get_name() << ends;

						Except::throw_exception((const char *)"API_AttrNotAllowed",
					        			o.str(),
					        			(const char *)"Device_3Impl::read_attributes_no_except");
					}

//
// Take the attribute mutex before calling the user read method
//

					if ((att.get_attr_serial_model() == ATTR_BY_KERNEL) && (back4 != NULL))
					{
						cout4 << "Locking attribute mutex for attribute " << att.get_name() << endl;
						omni_mutex *attr_mut = att.get_attr_mutex();
						if (attr_mut->trylock() == 0)
						{
							cout4 << "Mutex for attribute " << att.get_name() << " is already taken.........." << endl;
							attr_mut->lock();
						}
					}

//
// Call the user read method
//

					attr_vect[att.get_attr_idx()]->read(this,att);

				}
				catch (Tango::DevFailed &e)
				{
					long index;
					if (second_try == false)
						index = wanted_attr[i].idx_in_names;
					else
						index = idx[wanted_attr[i].idx_in_names];

					wanted_attr[i].failed = true;
					if (back != NULL)
					{
						(*back)[index].err_list = e.errors;
						(*back)[index].quality = Tango::ATTR_INVALID;
						(*back)[index].name = CORBA::string_dup(names[wanted_attr[i].idx_in_names]);
						clear_att_dim((*back)[index]);
					}
					else
					{
						if ((att.get_attr_serial_model() == ATTR_BY_KERNEL) && (is_allowed_failed == false))
						{
							cout4 << "Releasing attribute mutex for attribute " << att.get_name() << " due to error" << endl;
							omni_mutex *attr_mut = att.get_attr_mutex();
							attr_mut->unlock();
						}

						(*back4)[index].err_list = e.errors;
						(*back4)[index].quality = Tango::ATTR_INVALID;
						(*back4)[index].name = CORBA::string_dup(names[wanted_attr[i].idx_in_names]);
						clear_att_dim((*back4)[index]);
					}
				}
				catch (...)
				{
					long index;
					if (second_try == false)
						index = wanted_attr[i].idx_in_names;
					else
						index = idx[wanted_attr[i].idx_in_names];

					wanted_attr[i].failed = true;
					Tango::DevErrorList del;
					del.length(1);

					del[0].severity = Tango::ERR;
					del[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_no_except");
					del[0].reason = CORBA::string_dup("API_CorbaSysException ");
					del[0].desc = CORBA::string_dup("Unforseen exception when trying to read attribute. It was even not a Tango DevFailed exception");

					if (back != NULL)
					{
						(*back)[index].err_list = del;
						(*back)[index].quality = Tango::ATTR_INVALID;
						(*back)[index].name = CORBA::string_dup(names[wanted_attr[i].idx_in_names]);
						clear_att_dim((*back)[index]);
					}
					else
					{
						if ((att.get_attr_serial_model() == ATTR_BY_KERNEL) && (is_allowed_failed == false))
						{
							cout4 << "Releasing attribute mutex for attribute " << att.get_name() << " due to a severe error which is not a DevFailed" << endl;
							omni_mutex *attr_mut = att.get_attr_mutex();
							attr_mut->unlock();
						}

						(*back4)[index].err_list = del;
						(*back4)[index].quality = Tango::ATTR_INVALID;
						(*back4)[index].name = CORBA::string_dup(names[wanted_attr[i].idx_in_names]);
						clear_att_dim((*back4)[index]);
					}
				}
			}
		}

//
// Set attr value for writable attribute
//

		for (i = 0;i < nb_wanted_w_attr;i++)
		{
			Attribute &att = dev_attr->get_attr_by_ind(wanted_w_attr[i].idx_in_multi_attr);
			Tango::AttrWriteType w_type = att.get_writable();
			try
			{
				if ((w_type == Tango::READ_WITH_WRITE) || (w_type == Tango::WRITE))
					att.set_rvalue();
			}
			catch (Tango::DevFailed &e)
			{
				long index;
				if (second_try == false)
					index = wanted_w_attr[i].idx_in_names;
				else
					index = idx[wanted_w_attr[i].idx_in_names];

				wanted_w_attr[i].failed = true;
				if (back != NULL)
				{
					(*back)[index].err_list = e.errors;
					(*back)[index].quality = Tango::ATTR_INVALID;
					(*back)[index].name = CORBA::string_dup(names[wanted_w_attr[i].idx_in_names]);
					clear_att_dim((*back)[index]);
				}
				else
				{
					AttrSerialModel atsm = att.get_attr_serial_model();
					if ((atsm != ATTR_NO_SYNC) && (w_type == Tango::READ_WITH_WRITE))
					{
						cout4 << "Releasing attribute mutex for attribute " << att.get_name() << " due to error" << endl;
						omni_mutex *attr_mut = (atsm == ATTR_BY_KERNEL) ? att.get_attr_mutex() : att.get_user_attr_mutex();
						attr_mut->unlock();
					}

					(*back4)[index].err_list = e.errors;
					(*back4)[index].quality = Tango::ATTR_INVALID;
					(*back4)[index].name = CORBA::string_dup(names[wanted_w_attr[i].idx_in_names]);
					clear_att_dim((*back4)[index]);
				}
			}
		}

//
// If necessary, read state and/or status
// If the device has some alarmed attributes and some of them have already been
// read and failed, it is not necessary to read state, simply copy faulty
// alarmed attribute error message to the state attribute error messages
//

		Tango::DevState d_state = Tango::UNKNOWN;
		Tango::ConstDevString d_status = Tango_NullPtr;

		if (state_wanted == true)
		{
			if ((device_state == Tango::ON) || (device_state == Tango::ALARM))
			{
				long id = reading_state_necessary(wanted_attr);
				if (id == -1)
				{
					try
					{
						alarmed_not_read(wanted_attr);
						ext->state_from_read = true;
						d_state = dev_state();
						ext->state_from_read = false;
					}
					catch (Tango::DevFailed &e)
					{
						ext->state_from_read = false;
						if (back != NULL)
						{
							(*back)[state_idx].err_list = e.errors;
							(*back)[state_idx].quality = Tango::ATTR_INVALID;
							(*back)[state_idx].name = CORBA::string_dup(names[state_idx]);
							clear_att_dim((*back)[state_idx]);
						}
						else
						{
							(*back4)[state_idx].err_list = e.errors;
							(*back4)[state_idx].quality = Tango::ATTR_INVALID;
							(*back4)[state_idx].name = CORBA::string_dup(names[state_idx]);
							clear_att_dim((*back4)[state_idx]);
						}
					}
				}
				else
				{
					if (back != NULL)
					{
						(*back)[state_idx].err_list = (*back)[wanted_attr[id].idx_in_names].err_list;
						(*back)[state_idx].quality = Tango::ATTR_INVALID;
						(*back)[state_idx].name = CORBA::string_dup(names[state_idx]);
						clear_att_dim((*back)[state_idx]);
					}
					else
					{
						(*back4)[state_idx].err_list = (*back4)[wanted_attr[id].idx_in_names].err_list;
						(*back4)[state_idx].quality = Tango::ATTR_INVALID;
						(*back4)[state_idx].name = CORBA::string_dup(names[state_idx]);
						clear_att_dim((*back4)[state_idx]);
					}
				}
			}
			else
				d_state = dev_state();
		}

		if (status_wanted == true)
		{
			try
			{
				d_status = dev_status();
			}
			catch (Tango::DevFailed &e)
			{
				if (back != NULL)
				{
					(*back)[status_idx].err_list = e.errors;
					(*back)[status_idx].quality = Tango::ATTR_INVALID;
					(*back)[status_idx].name = CORBA::string_dup(names[status_idx]);
					clear_att_dim((*back)[status_idx]);
				}
				else
				{
					(*back4)[status_idx].err_list = e.errors;
					(*back4)[status_idx].quality = Tango::ATTR_INVALID;
					(*back4)[status_idx].name = CORBA::string_dup(names[status_idx]);
					clear_att_dim((*back4)[status_idx]);
				}
			}
		}

//
// Build the sequence returned to caller for readable attributes and check
// that all the wanted attributes set value have been updated
//

		for (i = 0;i < nb_names;i++)
		{
			long index;
			if (second_try == false)
				index = i;
			else
				index = idx[i];

			unsigned long nb_err;
			if (back != NULL)
				nb_err = (*back)[index].err_list.length();
			else
				nb_err = (*back4)[index].err_list.length();

			if ((state_wanted == true) && (state_idx == i))
			{
				if (back != NULL)
				{
					if (nb_err == 0)
						state2attr(d_state,(*back)[index]);
				}
				else
				{
					if (nb_err == 0)
						state2attr(d_state,(*back4)[index]);
				}
				continue;
			}

			if ((status_wanted == true) && (status_idx == i))
			{
				if (back != NULL)
				{
					if (nb_err == 0)
						status2attr(d_status,(*back)[index]);
				}
				else
				{
					if (nb_err == 0)
						status2attr(d_status,(*back4)[index]);
				}
				continue;
			}

			if (nb_err == 0)
			{
				Attribute &att = dev_attr->get_attr_by_name(names[i]);
				Tango::AttrQuality qual = att.get_quality();
				if (qual != Tango::ATTR_INVALID)
				{
					if (att.get_value_flag() == false)
					{
						TangoSys_OMemStream o;

						try
						{
							string att_name(names[i]);
							transform(att_name.begin(),att_name.end(),att_name.begin(),::tolower);

							vector<PollObj *>::iterator ite = get_polled_obj_by_type_name(Tango::POLL_ATTR,att_name);
							long upd = (*ite)->get_upd();
							if (upd == 0)
							{
								o << "Attribute ";
								o << att.get_name();
								o << " value is available only by CACHE.\n";
								o << "Attribute values are set by external polling buffer filling" << ends;
							}
							else
							{
								o << "Read value for attribute ";
								o << att.get_name();
								o << " has not been updated" << ends;
							}
						}
						catch (Tango::DevFailed &)
						{
							o << "Read value for attribute ";
							o << att.get_name();
							o << " has not been updated" << ends;
						}

						if (back != NULL)
						{
							(*back)[index].err_list.length(1);
							(*back)[index].err_list[0].severity = Tango::ERR;
							(*back)[index].err_list[0].reason = CORBA::string_dup("API_AttrValueNotSet");
							(*back)[index].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_no_except");

							string s = o.str();
							(*back)[index].err_list[0].desc = CORBA::string_dup(s.c_str());
							(*back)[index].quality = Tango::ATTR_INVALID;
							(*back)[index].name = CORBA::string_dup(att.get_name().c_str());
							clear_att_dim((*back)[index]);
						}
						else
						{
							AttrSerialModel atsm = att.get_attr_serial_model();
							if ((i != state_idx) && (i != status_idx) && (atsm != ATTR_NO_SYNC) && (att.get_writable() != Tango::WRITE))
							{
								cout4 << "Releasing attribute mutex for attribute " << att.get_name() << " due to error" << endl;
								omni_mutex *attr_mut = (atsm == ATTR_BY_KERNEL) ? att.get_attr_mutex() : att.get_user_attr_mutex();
								attr_mut->unlock();
							}

							(*back4)[index].err_list.length(1);
							(*back4)[index].err_list[0].severity = Tango::ERR;
							(*back4)[index].err_list[0].reason = CORBA::string_dup("API_AttrValueNotSet");
							(*back4)[index].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_no_except");

							string s = o.str();
							(*back4)[index].err_list[0].desc = CORBA::string_dup(s.c_str());
							(*back4)[index].quality = Tango::ATTR_INVALID;
							(*back4)[index].name = CORBA::string_dup(att.get_name().c_str());
							clear_att_dim((*back4)[index]);
						}
					}
					else
					{
						try
						{
							Tango::AttrWriteType w_type = att.get_writable();
							if ((w_type == Tango::READ) ||
			    		    	(w_type == Tango::READ_WRITE) ||
			    		    	(w_type == Tango::READ_WITH_WRITE))
							{
								if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
									dev_attr->add_write_value(att);

//
// Check attribute alarm
//

								if ((att.is_alarmed().any() == true) && (qual != Tango::ATTR_INVALID))
									att.check_alarm();
							}

//
// Data into the network object
//

							data_into_net_object(att,back,back4,index,w_type,true);

//
// Init remaining elements
//

							if (att.get_when().tv_sec == 0)
								att.set_time();
							if (back != NULL)
							{
								(*back)[index].time = att.get_when();
								(*back)[index].quality = att.get_quality();
								(*back)[index].name = CORBA::string_dup(att.get_name().c_str());
								(*back)[index].r_dim.dim_x = att.get_x();
								(*back)[index].r_dim.dim_y = att.get_y();
								if ((w_type == Tango::READ_WRITE) ||
									(w_type == Tango::READ_WITH_WRITE))
								{
									WAttribute &assoc_att = dev_attr->get_w_attr_by_ind(att.get_assoc_ind());
									(*back)[index].w_dim.dim_x = assoc_att.get_w_dim_x();
									(*back)[index].w_dim.dim_y = assoc_att.get_w_dim_y();
								}
								else
								{
									if ( w_type == Tango::WRITE)
									{
										// for write only attributes read and set value are the same!
										(*back)[index].w_dim.dim_x = att.get_x();
										(*back)[index].w_dim.dim_y = att.get_y();
									}
									else
									{
										// Tango::Read : read only attributes
										(*back)[index].w_dim.dim_x = 0;
										(*back)[index].w_dim.dim_y = 0;
									}
								}
							}
							else
							{
								AttrSerialModel atsm = att.get_attr_serial_model();
								if ((atsm != ATTR_NO_SYNC) && (w_type != Tango::WRITE))
								{
									cout4 << "Giving attribute mutex to CORBA structure for attribute " << att.get_name() << endl;
									if (atsm == ATTR_BY_KERNEL)
										GIVE_ATT_MUTEX(back4,index,att);
									else
										GIVE_USER_ATT_MUTEX(back4,index,att);
								}

								(*back4)[index].time = att.get_when();
								(*back4)[index].quality = att.get_quality();
								(*back4)[index].data_format = att.get_data_format();
								(*back4)[index].name = CORBA::string_dup(att.get_name().c_str());
								(*back4)[index].r_dim.dim_x = att.get_x();
								(*back4)[index].r_dim.dim_y = att.get_y();
								if ((w_type == Tango::READ_WRITE) ||
									(w_type == Tango::READ_WITH_WRITE))
								{
									WAttribute &assoc_att = dev_attr->get_w_attr_by_ind(att.get_assoc_ind());
									(*back4)[index].w_dim.dim_x = assoc_att.get_w_dim_x();
									(*back4)[index].w_dim.dim_y = assoc_att.get_w_dim_y();
								}
								else
								{
									if ( w_type == Tango::WRITE)
									{
										// for write only attributes read and set value are the same!
										(*back4)[index].w_dim.dim_x = att.get_x();
										(*back4)[index].w_dim.dim_y = att.get_y();
									}
									else
									{
										// Tango::Read : read only attributes
										(*back4)[index].w_dim.dim_x = 0;
										(*back4)[index].w_dim.dim_y = 0;
									}
								}
							}
						}
						catch (Tango::DevFailed &e)
						{
							if (back != NULL)
							{
								(*back)[index].err_list = e.errors;
								(*back)[index].quality = Tango::ATTR_INVALID;
								(*back)[index].name = CORBA::string_dup(att.get_name().c_str());
								clear_att_dim((*back)[index]);
							}
							else
							{
								cout4 << "Asking CORBA structure to release attribute mutex for attribute " << att.get_name() << endl;
								if (att.get_writable() != Tango::WRITE)
								{
									REL_ATT_MUTEX(back4,index,att);
								}

								(*back4)[index].err_list = e.errors;
								(*back4)[index].quality = Tango::ATTR_INVALID;
								(*back4)[index].name = CORBA::string_dup(att.get_name().c_str());
								clear_att_dim((*back4)[index]);
							}
						}
					}
				}
				else
				{
					if (qual != Tango::ATTR_INVALID)
						qual = Tango::ATTR_INVALID;
					if (att.get_when().tv_sec == 0)
						att.set_time();
					if (back != NULL)
					{
						(*back)[index].time = att.get_when();
						(*back)[index].quality = qual;
						(*back)[index].name = CORBA::string_dup(att.get_name().c_str());
						(*back)[index].r_dim.dim_x = 0;
						(*back)[index].r_dim.dim_y = 0;
						(*back)[index].w_dim.dim_x = 0;
						(*back)[index].w_dim.dim_y = 0;
					}
					else
					{
						AttrSerialModel atsm = att.get_attr_serial_model();
						if ((atsm != ATTR_NO_SYNC) && (att.get_writable() != Tango::WRITE))
						{
							cout4 << "Releasing attribute mutex for attribute " << att.get_name() << " due to error" << endl;
							omni_mutex *attr_mut = (atsm == ATTR_BY_KERNEL) ? att.get_attr_mutex() : att.get_user_attr_mutex();
							attr_mut->unlock();
						}

						(*back4)[index].time = att.get_when();
						(*back4)[index].quality = qual;
						(*back4)[index].data_format = att.get_data_format();
						(*back4)[index].name = CORBA::string_dup(att.get_name().c_str());
						(*back4)[index].r_dim.dim_x = 0;
						(*back4)[index].r_dim.dim_y = 0;
						(*back4)[index].w_dim.dim_x = 0;
						(*back4)[index].w_dim.dim_y = 0;
					}
				}
			}
		}
	}

	catch (...)
	{
		// set back the device attribution for the thread
		// and rethrow the exception.
		sub.set_associated_device(last_associated_device);
		throw;
	}

	// set back the device attribution for the thread
	sub.set_associated_device(last_associated_device);

	cout4 << "Leaving Device_3Impl::read_attributes_no_except" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::read_attributes_from_cache
//
// description : 	Read attributes from cache but do not throw exception
//			if it fails. This method is mainly a copy of the original
//			DeviceImpl::read_attributes method.
//
// argument: in :	- names: The names of the attribute to read
//			- back : Pointer to the sequence returned to the client.
//				 Memory has already been allocated for
//				 this pointer
//			- back4 : Pointer to the equence returned to the client when called
//					  throught the Device_4 IDL interface
//
//--------------------------------------------------------------------------


void Device_3Impl::read_attributes_from_cache(const Tango::DevVarStringArray& names,
					      Tango::AttributeValueList_3 *&back,Tango::AttributeValueList_4 *&back4)
{
	unsigned long nb_names = names.length();
    cout4 << "Reading " << nb_names << " attr in read_attributes_from_cache()" << endl;

//
// Check that device supports the wanted attribute and that the attribute
// is polled. If some are non polled, store
// their index in the real_names sequence in a vector
//

	unsigned long i;
    vector<PollObj *> &poll_list = get_poll_obj_list();
	vector<long> non_polled;
	unsigned long nb_poll = poll_list.size();
	unsigned long j;

	for (i = 0;i < nb_names;i++)
	{
		try
		{
			dev_attr->get_attr_ind_by_name(names[i]);
			for (j = 0;j < nb_poll;j++)
			{
#ifdef _TG_WINDOWS_
				if (_stricmp(poll_list[j]->get_name().c_str(),names[i]) == 0)
#else
				if (strcasecmp(poll_list[j]->get_name().c_str(),names[i]) == 0)
#endif
					break;
			}
			if (j == nb_poll)
			{
				non_polled.push_back(i);
			}
		}
		catch (Tango::DevFailed &e)
		{
			if (back4 == NULL)
			{
				(*back)[i].err_list = e.errors;
				(*back)[i].quality = Tango::ATTR_INVALID;
				(*back)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back)[i]);
			}
			else
			{
				(*back4)[i].err_list = e.errors;
				(*back4)[i].quality = Tango::ATTR_INVALID;
				(*back4)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back4)[i]);
			}
		}
	}

//
// If some attributes are not polled but their polling update period is defined,
// and the attribute is not in the device list of attr which should not be
// polled, start to poll them
//

	vector<long> poll_period;
	unsigned long not_polled_attr = 0;

	if (non_polled.empty() == false)
	{

//
// Check that it is possible to start polling for the non polled attribute
//

		for (i = 0;i < non_polled.size();i++)
		{
			Attribute &att = dev_attr->get_attr_by_name(names[non_polled[i]]);
			poll_period.push_back(att.get_polling_period());

			if (poll_period.back() == 0)
			{
				TangoSys_OMemStream o;
				o << "Attribute " << att.get_name() << " not polled" << ends;

				if (back != NULL)
				{
					(*back)[non_polled[i]].err_list.length(1);
					(*back)[non_polled[i]].err_list[0].severity = Tango::ERR;
					(*back)[non_polled[i]].err_list[0].reason = CORBA::string_dup("API_AttrNotPolled");
					(*back)[non_polled[i]].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

					string s = o.str();
					(*back)[non_polled[i]].err_list[0].desc = CORBA::string_dup(s.c_str());
					(*back)[non_polled[i]].quality = Tango::ATTR_INVALID;
					(*back)[non_polled[i]].name = CORBA::string_dup(att.get_name().c_str());
					clear_att_dim((*back)[non_polled[i]]);
				}
				else
				{
					(*back4)[non_polled[i]].err_list.length(1);
					(*back4)[non_polled[i]].err_list[0].severity = Tango::ERR;
					(*back4)[non_polled[i]].err_list[0].reason = CORBA::string_dup("API_AttrNotPolled");
					(*back4)[non_polled[i]].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

					string s = o.str();
					(*back4)[non_polled[i]].err_list[0].desc = CORBA::string_dup(s.c_str());
					(*back4)[non_polled[i]].quality = Tango::ATTR_INVALID;
					(*back4)[non_polled[i]].name = CORBA::string_dup(att.get_name().c_str());
					clear_att_dim((*back4)[non_polled[i]]);
				}
				not_polled_attr++;
				continue;
			}
		}

//
// Leave method if number of attributes which should not be polled
// is equal to the requested attribute number
//

		if (not_polled_attr == nb_names)
			return;
	}

//
// For each attribute, check that some data are available in cache and that they
// are not too old
//

	for (i = 0;i < nb_names;i++)
	{

		if (back != NULL)
		{
			if ((*back)[i].err_list.length() != 0)
				continue;
		}
		else
		{
			if ((*back4)[i].err_list.length() != 0)
				continue;
		}

		PollObj *polled_attr = NULL;
		unsigned long j;
		for (j = 0;j < poll_list.size();j++)
		{
#ifdef _TG_WINDOWS_
			if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
	   	    	    (_stricmp(poll_list[j]->get_name().c_str(),names[i]) == 0))
#else
			if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
		  	    (strcasecmp(poll_list[j]->get_name().c_str(),names[i]) == 0))
#endif
			{
				polled_attr = poll_list[j];
				break;
			}
		}

//
// In some cases where data from polling are required by a DS for devices marked as
// polled but for which the polling is not sarted yet, polled_attr could be NULL at the
// end of this loop. Return "No data yet" in this case
//

        if (polled_attr == NULL)
        {
			TangoSys_OMemStream o;
			o << "No data available in cache for attribute " << names[i] << ends;

			if (back != NULL)
			{
				(*back)[i].err_list.length(1);
				(*back)[i].err_list[0].severity = Tango::ERR;
				(*back)[i].err_list[0].reason = CORBA::string_dup("API_NoDataYet");
				(*back)[i].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

				string s = o.str();
				(*back)[i].err_list[0].desc = CORBA::string_dup(s.c_str());
				(*back)[i].quality = Tango::ATTR_INVALID;
				(*back)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back)[i]);
			}
			else
			{
				(*back4)[i].err_list.length(1);
				(*back4)[i].err_list[0].severity = Tango::ERR;
				(*back4)[i].err_list[0].reason = CORBA::string_dup("API_NoDataYet");
				(*back4)[i].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

				string s = o.str();
				(*back4)[i].err_list[0].desc = CORBA::string_dup(s.c_str());
				(*back4)[i].quality = Tango::ATTR_INVALID;
				(*back4)[i].name = CORBA::string_dup(names[i]);
				(*back4)[i].data_format = Tango::FMT_UNKNOWN;
				clear_att_dim((*back4)[i]);
			}
			continue;
        }

//
// Check that some data is available in cache
//

		if (polled_attr->is_ring_empty() == true)
		{
			TangoSys_OMemStream o;
			o << "No data available in cache for attribute " << names[i] << ends;

			if (back != NULL)
			{
				(*back)[i].err_list.length(1);
				(*back)[i].err_list[0].severity = Tango::ERR;
				(*back)[i].err_list[0].reason = CORBA::string_dup("API_NoDataYet");
				(*back)[i].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

				string s = o.str();
				(*back)[i].err_list[0].desc = CORBA::string_dup(s.c_str());
				(*back)[i].quality = Tango::ATTR_INVALID;
				(*back)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back)[i]);
			}
			else
			{
				(*back4)[i].err_list.length(1);
				(*back4)[i].err_list[0].severity = Tango::ERR;
				(*back4)[i].err_list[0].reason = CORBA::string_dup("API_NoDataYet");
				(*back4)[i].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

				string s = o.str();
				(*back4)[i].err_list[0].desc = CORBA::string_dup(s.c_str());
				(*back4)[i].quality = Tango::ATTR_INVALID;
				(*back4)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back4)[i]);
			}
			continue;
		}

//
// Check that data are still refreshed by the polling thread
// Skip this test for object with external polling triggering (upd = 0)
//

		long tmp_upd = polled_attr->get_upd();
		if (tmp_upd != 0)
		{
			double last = polled_attr->get_last_insert_date();
			struct timeval now;
#ifdef _TG_WINDOWS_
			struct _timeb now_win;
			_ftime(&now_win);
			now.tv_sec = (unsigned long)now_win.time;
			now.tv_usec = (long)now_win.millitm * 1000;
#else
			gettimeofday(&now,NULL);
#endif
			now.tv_sec = now.tv_sec - DELTA_T;
			double now_d = (double)now.tv_sec + ((double)now.tv_usec / 1000000);
			double diff_d = now_d - last;
			if (diff_d > polled_attr->get_authorized_delta())
			{
				TangoSys_OMemStream o;
				o << "Data in cache for attribute " << names[i];
				o << " not updated any more" << ends;

				if (back != NULL)
				{
					(*back)[i].err_list.length(1);
					(*back)[i].err_list[0].severity = Tango::ERR;
					(*back)[i].err_list[0].reason = CORBA::string_dup("API_NotUpdatedAnyMore");
					(*back)[i].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

					string s = o.str();
					(*back)[i].err_list[0].desc = CORBA::string_dup(s.c_str());
					(*back)[i].quality = Tango::ATTR_INVALID;
					(*back)[i].name = CORBA::string_dup(names[i]);
					clear_att_dim((*back)[i]);
				}
				else
				{
					(*back4)[i].err_list.length(1);
					(*back4)[i].err_list[0].severity = Tango::ERR;
					(*back4)[i].err_list[0].reason = CORBA::string_dup("API_NotUpdatedAnyMore");
					(*back4)[i].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

					string s = o.str();
					(*back4)[i].err_list[0].desc = CORBA::string_dup(s.c_str());
					(*back4)[i].quality = Tango::ATTR_INVALID;
					(*back4)[i].name = CORBA::string_dup(names[i]);
					clear_att_dim((*back4)[i]);
				}
				continue;
			}
		}

//
// Get attribute data type
//

		Attribute &att = dev_attr->get_attr_by_name(names[i]);
		long type = att.get_data_type();

//
// Finally, after all these checks, get value and store it in the sequence
// sent back to user
// In order to avoid unnecessary copy, don't use the assignement operator of the
// AttributeValue structure which copy each element and therefore also copy
// the Any object. The Any assignement operator is a deep copy!
// Create a new sequence using the attribute buffer and insert it into the
// Any. The sequence inside the source Any has been created using the attribute
// data buffer.
//

		try
		{
			long vers = get_dev_idl_version();
			{
				omni_mutex_lock sync(*polled_attr);

				Tango::AttrQuality qual;

				if (back != NULL)
				{

//
// Get device IDL release. Since release 4, devices are polled using read_attribute_4
//

					if (vers >= 4)
					{
						AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
						qual = att_val.quality;
					}
					else
					{
						AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);
						qual = att_val.quality;
					}
				}
				else
				{
					AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
					qual = att_val.quality;
				}

//
// Copy the polled data into the Any or the union
//

				if (qual != Tango::ATTR_INVALID)
				{
					polled_data_into_net_object(back,back4,i,type,vers,polled_attr,names);
				}

//
// Init remaining structure members
//

				if (back != NULL)
				{
					long vers = get_dev_idl_version();
					if (vers >= 4)
					{
						AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);

						(*back)[i].quality= att_val.quality;
						(*back)[i].time = att_val.time;
						(*back)[i].r_dim = att_val.r_dim;
						(*back)[i].w_dim = att_val.w_dim;
						(*back)[i].name = CORBA::string_dup(att_val.name);
					}
					else
					{
						AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);

						(*back)[i].quality= att_val.quality;
						(*back)[i].time = att_val.time;
						(*back)[i].r_dim = att_val.r_dim;
						(*back)[i].w_dim = att_val.w_dim;
						(*back)[i].name = CORBA::string_dup(att_val.name);
					}
				}
				else
				{
					AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);

					(*back4)[i].quality= att_val.quality;
					(*back4)[i].data_format = att_val.data_format;
					(*back4)[i].time = att_val.time;
					(*back4)[i].r_dim = att_val.r_dim;
					(*back4)[i].w_dim = att_val.w_dim;
					(*back4)[i].name = CORBA::string_dup(att_val.name);
				}
			}
		}
		catch (Tango::DevFailed &e)
		{
			if (back != NULL)
			{
				(*back)[i].err_list = e.errors;
				(*back)[i].quality = Tango::ATTR_INVALID;
				(*back)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back)[i]);
			}
			else
			{
				(*back4)[i].err_list = e.errors;
				(*back4)[i].quality = Tango::ATTR_INVALID;
				(*back4)[i].name = CORBA::string_dup(names[i]);
				clear_att_dim((*back4)[i]);
			}
		}
	}
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::write_attributes_3
//
// description : 	CORBA operation to write attribute(s) value
//
// argument: in :	- values: The new attribute(s) value to be set.
//
//--------------------------------------------------------------------------

void Device_3Impl::write_attributes_3(const Tango::AttributeValueList& values)
throw (Tango::MultiDevFailed, Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_3Impl::write_attributes_3 arrived" << endl;

//
// Record operation request in black box
// If this method is executed with the request to store info in
// blackbox (store_in_bb == true), this means that the request arrives
// through a Device_2 CORBA interface. Check locking feature in this
// case. Otherwise the request has arrived through Device_4 and the check
// is already done
//

	if (ext->store_in_bb == true)
	{
		blackbox_ptr->insert_attr(values,3);
		check_lock("write_attributes_3");
	}
	ext->store_in_bb = true;

//
// Call the method really doing the job
//

	write_attributes_34(&values,NULL);
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::write_attributes_34
//
// description : 	Method to write the attribute. This method is common to
//					the IDL interface 3 and 4.
//
// argument: in :	- values_3: The new attribute(s) value to be set in IDL V3
//					- values_4: The new attribute(s) value to be set in IDL V4
//
//--------------------------------------------------------------------------

void Device_3Impl::write_attributes_34(const Tango::AttributeValueList *values_3,const Tango::AttributeValueList_4 *values_4)
{

//
// Return exception if the device does not have any attribute
//

	unsigned long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				        (const char *)"The device does not have any attribute",
				        (const char *)"DeviceImpl::write_attributes");
	}

	unsigned long nb_failed = 0;
	Tango::NamedDevErrorList errs;

//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//

	SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
	string last_associated_device = sub.get_associated_device();
	sub.set_associated_device(get_name());

//
// Catch all exceptions to set back the associated device after
// execution
//

	try
	{

//
// Retrieve index of wanted attributes in the device attribute list
//

		vector<AttIdx> updated_attr;
		unsigned long nb_updated_attr;
		if (values_3 != NULL)
			nb_updated_attr = values_3->length();
		else
			nb_updated_attr = values_4->length();

		unsigned long i;
		for (i = 0;i < nb_updated_attr;i++)
		{
			const char *single_att_name;
			long single_att_dimx,single_att_dimy;

			if (values_3 != NULL)
			{
				single_att_name = (*values_3)[i].name;
				single_att_dimx = (*values_3)[i].dim_x;
				single_att_dimy = (*values_3)[i].dim_y;
			}
			else
			{
				single_att_name = (*values_4)[i].name;
				single_att_dimx = (*values_4)[i].w_dim.dim_x;
				single_att_dimy = (*values_4)[i].w_dim.dim_y;
			}

			try
			{
				AttIdx idxs;
				idxs.idx_in_names = i;
				idxs.idx_in_multi_attr = dev_attr->get_attr_ind_by_name(single_att_name);
				updated_attr.push_back(idxs);
//
// Check that these attributes are writable.
// For attributes which are not scalar, also check that
// their dimensions are correct
//

				Attribute &att = dev_attr->get_attr_by_ind(updated_attr.back().idx_in_multi_attr);
				if ((att.get_writable() == Tango::READ) ||
					(att.get_writable() == Tango::READ_WITH_WRITE))
				{
					TangoSys_OMemStream o;

					o << "Attribute ";
					o << att.get_name();
					o << " is not writable" << ends;

					updated_attr.pop_back();
					Except::throw_exception((const char *)"API_AttrNotWritable",
					        	o.str(),
					        	(const char *)"DeviceImpl::write_attributes");
				}

				if (att.get_data_format() != Tango::SCALAR)
				{
					TangoSys_OMemStream o;
					bool err = false;

					if (att.get_max_dim_x() < single_att_dimx)
					{
						err = true;
						o << "X ";
					}

					if (err == false)
					{
						if (att.get_max_dim_y() < single_att_dimy)
						{
							err = true;
							o << "Y ";
						}
					}

					if (err == true)
					{
						o << "dimesion is greater than the max defined for attribute ";
						o << att.get_name();
						o << ends;

						updated_attr.pop_back();
						Except::throw_exception((const char *)"API_WAttrOutsideLimit",
					        	o.str(),
					        	(const char *)"DeviceImpl::write_attributes");
					}
				}

//
// Check if there are some startup exceptions for the attribute (due to invalid
// attribute properties configuration). If so, do not allow to write the attribute.
//

				if(att.is_startup_exception() == true)
				{
					updated_attr.pop_back();
					att.throw_startup_exception("DeviceImpl::write_attributes()");
				}
			}
			catch (Tango::DevFailed &e)
			{
				nb_failed++;
				errs.length(nb_failed);
				errs[nb_failed - 1].name = CORBA::string_dup(single_att_name);
				errs[nb_failed - 1].index_in_call = i;
				errs[nb_failed - 1].err_list = e.errors;
			}
		}

//
// Call the always_executed_hook
//

		if (nb_failed != nb_updated_attr)
			always_executed_hook();

//
// Set attribute internal value
//

		vector<AttIdx>::iterator ctr;
		for (ctr = updated_attr.begin();ctr < updated_attr.end();++ctr)
		{

			const char *single_att_name;
			long single_att_dimx,single_att_dimy;

			if (values_3 != NULL)
			{
				single_att_name = (*values_3)[ctr->idx_in_names].name;
				single_att_dimx = (*values_3)[ctr->idx_in_names].dim_x;
				single_att_dimy = (*values_3)[ctr->idx_in_names].dim_y;
			}
			else
			{
				single_att_name = (*values_4)[ctr->idx_in_names].name;
				single_att_dimx = (*values_4)[ctr->idx_in_names].w_dim.dim_x;
				single_att_dimy = (*values_4)[ctr->idx_in_names].w_dim.dim_y;
			}

			try
			{
				if (values_3 == NULL)
					dev_attr->get_w_attr_by_ind(ctr->idx_in_multi_attr).check_written_value((*values_4)[ctr->idx_in_names].value,
												(unsigned long)single_att_dimx,
												(unsigned long)single_att_dimy);
				else
					dev_attr->get_w_attr_by_ind(ctr->idx_in_multi_attr).check_written_value((*values_3)[ctr->idx_in_names].value,
								 	  			(unsigned long)single_att_dimx,
								 	  			(unsigned long)single_att_dimy);
			}
			catch (Tango::DevFailed &e)
			{
				nb_failed++;
				errs.length(nb_failed);
				errs[nb_failed - 1].name = CORBA::string_dup(single_att_name);
				errs[nb_failed - 1].index_in_call = ctr->idx_in_names;
				errs[nb_failed - 1].err_list = e.errors;
				ctr = updated_attr.erase(ctr);
				if (ctr >= updated_attr.end())
					break;
				else
				{
					if (ctr == updated_attr.begin())
						break;
					else
						--ctr;
				}
			}
		}

//
// Write the hardware. Call this method one attribute at a time in order to
// correctly initialized the MultiDevFailed exception in case one of the
// attribute failed.
//

		if (nb_failed != nb_updated_attr)
		{
			vector<AttIdx>::iterator ite;
			for(ite = updated_attr.begin();ite != updated_attr.end();++ite)
			{
                WAttribute &att = dev_attr->get_w_attr_by_ind((*ite).idx_in_multi_attr);
				try
				{
					att.set_value_flag(false);
					att.set_user_set_write_value(false);
					vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();
					if (attr_vect[att.get_attr_idx()]->is_allowed(this,Tango::WRITE_REQ) == false)
					{
						TangoSys_OMemStream o;

						o << "It is currently not allowed to write attribute ";
						o << att.get_name();
						o << ". The device state is " << Tango::DevStateName[get_state()] << ends;


						Except::throw_exception((const char *)"API_AttrNotAllowed",
					        			o.str(),
					        			(const char *)"Device_3Impl::write_attributes");
					}
					attr_vect[att.get_attr_idx()]->write(this,att);
				}
				catch (Tango::DevFailed &e)
				{
					nb_failed++;
					if (att.get_data_format() == SCALAR)
                        att.rollback();
					errs.length(nb_failed);
					if (values_3 != NULL)
						errs[nb_failed - 1].name = CORBA::string_dup((*values_3)[(*ite).idx_in_names].name);
					else
						errs[nb_failed - 1].name = CORBA::string_dup((*values_4)[(*ite).idx_in_names].name);
					errs[nb_failed - 1].index_in_call = (*ite).idx_in_names;
					errs[nb_failed - 1].err_list = e.errors;
					ite = updated_attr.erase(ite);
					if (ite >= updated_attr.end())
						break;
					else
					{
						if (ite == updated_attr.begin())
							break;
						else
							--ite;
					}
				}
			}
		}

//
// Copy data into Attribute object, store the memorized one in db
// and if the attribute has a RDS alarm, set the write date
//
// Warning: Do not copy caller value if the user has manually set the
// attribute written value in its write method
//
// WARNING: --> The DevEncoded data type is supported only as SCALAR and is not
// memorizable. Therefore, no need to call copy_data
//

		vector<long> att_in_db;

		for (i = 0;i < updated_attr.size();i++)
		{
			WAttribute &att = dev_attr->get_w_attr_by_ind(updated_attr[i].idx_in_multi_attr);

			if (values_3 != NULL)
			{
				if (att.get_user_set_write_value() == false)
					att.copy_data((*values_3)[updated_attr[i].idx_in_names].value);
			}
			else
			{
				if (att.get_user_set_write_value() == false)
					att.copy_data((*values_4)[updated_attr[i].idx_in_names].value);
			}

			if (att.is_memorized() == true)
			{
                att_in_db.push_back(i);
                if (att.get_mem_value() == MemNotUsed)
                    att.set_mem_value("Set");
			}
			if (att.is_alarmed().test(Attribute::rds) == true)
				att.set_written_date();
		}

		if ((Tango::Util::_UseDb == true) && (att_in_db.empty() == false))
		{
			try
			{
				write_attributes_in_db(att_in_db,updated_attr);
			}
			catch (Tango::DevFailed &e)
			{
				errs.length(nb_failed + att_in_db.size());
				for (i = 0;i < att_in_db.size();i++)
				{
					const char *single_att_name;

					if (values_3 != NULL)
						single_att_name = (*values_3)[updated_attr[att_in_db[i]].idx_in_names].name;
					else
						single_att_name = (*values_4)[updated_attr[att_in_db[i]].idx_in_names].name;

					errs[nb_failed + i].name = CORBA::string_dup(single_att_name);
					errs[nb_failed + i].index_in_call = updated_attr[att_in_db[i]].idx_in_names;
					errs[nb_failed + i].err_list = e.errors;
				}
				nb_failed = nb_failed + att_in_db.size();
			}
		}
	}
	catch (...)
	{
		// set back the device attribution for the thread
		// and rethrow the exception.
		sub.set_associated_device(last_associated_device);
		throw;
	}

	// set back the device attribution for the thread
	sub.set_associated_device(last_associated_device);

//
// Return to caller.
//

	cout4 << "Leaving Device_3Impl::write_attributes_34" << endl;

	if (nb_failed != 0)
	{
		throw Tango::MultiDevFailed(errs);
	}

}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::read_attribute_history_3
//
// description : 	CORBA operation to read attribute value history from
//			the polling buffer.
//
// argument: in :	- name : attribute name
//			- n : history depth (in record number)
//
// This method returns a pointer to a DevAttrHistoryList with one
// DevAttrHistory structure for each attribute record
//
//--------------------------------------------------------------------------

Tango::DevAttrHistoryList_3 *Device_3Impl::read_attribute_history_3(const char* name,
								    CORBA::Long n)
throw(Tango::DevFailed, CORBA::SystemException)
{
	TangoMonitor &mon = get_poll_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_3Impl::read_attribute_history_3 arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Read_Attr_history_3);

	Tango::DevAttrHistoryList_3 *back = NULL;
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
// Check that the wanted attribute is polled.
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
	if (polled_attr == NULL)
	{
		TangoSys_OMemStream o;
		o << "Attribute " << attr_str << " not polled" << ends;
		Except::throw_exception((const char *)"API_AttrNotPolled",
					o.str(),
					(const char *)"Device_3Impl::read_attribute_history_3");
	}

//
// Check that some data is available in cache
//

	if (polled_attr->is_ring_empty() == true)
	{
		TangoSys_OMemStream o;
		o << "No data available in cache for attribute " << attr_str << ends;
		Except::throw_exception((const char *)"API_NoDataYet",
					o.str(),
					(const char *)"Device_3Impl::read_attribute_history_3");
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
		back = new Tango::DevAttrHistoryList_3(n);
                back->length(n);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_3Impl::read_attribute_history_3");
	}

//
// Get attribute value history
//

	long vers = get_dev_idl_version();

	if (vers < 4)
		polled_attr->get_attr_history(n,back,att.get_data_type());
	else
		polled_attr->get_attr_history_43(n,back,att.get_data_type());

	cout4 << "Leaving Device_3Impl::command_inout_history_3 method" << endl;
	return back;
}


//+-------------------------------------------------------------------------
//
// method :		Device_3Impl::info_3
//
// description :	CORBA operation to get device info
//
//--------------------------------------------------------------------------


Tango::DevInfo_3 *Device_3Impl::info_3()
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "Device_3Impl::info_3 arrived" << endl;

	Tango::DevInfo_3 *back = NULL;

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

	try
	{
		back = new Tango::DevInfo_3();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"Device_3Impl::info_3");
	}

//
// Retrieve server host
//

	Tango::Util *tango_ptr = Tango::Util::instance();
	back->server_host = CORBA::string_dup(tango_ptr->get_host_name().c_str());

//
// Fill-in remaining structure fields
//

	back->dev_class = CORBA::string_dup(device_class->get_name().c_str());
	back->server_id = CORBA::string_dup(tango_ptr->get_ds_name().c_str());
	back->server_version = DevVersion;

//
// Build the complete info sent in the doc_url string
//

	string doc_url("Doc URL = ");
	doc_url = doc_url + device_class->get_doc_url();
	string &cvs_tag = device_class->get_cvs_tag();
	if (cvs_tag.size() != 0)
	{
		doc_url = doc_url + "\nCVS Tag = ";
		doc_url = doc_url + cvs_tag;
	}
	string &cvs_location = device_class->get_cvs_location();
	if (cvs_location.size() != 0)
	{
		doc_url = doc_url + "\nCVS Location = ";
		doc_url = doc_url + cvs_location;
	}
	back->doc_url = CORBA::string_dup(doc_url.c_str());

//
// Set the device type
//

	back->dev_type = CORBA::string_dup(device_class->get_type().c_str());

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Info_3);

//
// Return to caller
//

	cout4 << "Leaving Device_3Impl::info_3" << endl;
	return back;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::get_attribute_config_3
//
// description : 	CORBA operation to get attribute configuration.
//
// argument: in :	- names: name of attribute(s)
//
// This method returns a pointer to a AttributeConfigList_3 with one
// AttributeConfig_3 structure for each atribute
//
//
// WARNING !!!!!!!!!!!!!!!!!!
//
// This is the release 3 of this CORBA operation which
// returns much more parameter than in release 2
// The code has been duplicated in order to keep it clean
// (avoid many "if" on version number in a common method)
//
//--------------------------------------------------------------------------

Tango::AttributeConfigList_3 *Device_3Impl::get_attribute_config_3(const Tango::DevVarStringArray& names)
throw(Tango::DevFailed, CORBA::SystemException)
{
	TangoMonitor &mon = get_att_conf_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_3Impl::get_attribute_config_3 arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList_3 *back = NULL;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config_3);

//
// Get attribute number and device version
//

	long nb_dev_attr = dev_attr->get_attr_nb();

//
// Check if the caller want to get config for all attribute
// If the device implements IDL 3 (State and status as attributes)
// and the client is an old one (not able to read state/status as
// attribute), decrement attribute number
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
		back = new Tango::AttributeConfigList_3(nb_attr);
		back->length(nb_attr);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_3Impl::get_attribute_config_3");
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
				attr.get_properties_3((*back)[i]);
			}
			else
			{
				Attribute &attr = dev_attr->get_attr_by_name(names[i]);
				attr.get_properties_3((*back)[i]);
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

	cout4 << "Leaving Device_3Impl::get_attribute_config_3" << endl;

	return back;
}

//+-------------------------------------------------------------------------
//
// method :		Device_3Impl::set_attribute_config_3
//
// description :	CORBA operation to set attribute configuration locally
//			and in the Tango database
//
// argument: in :	- new_conf: The new attribute(s) configuration. One
//				    AttributeConfig structure is needed for each
//				    attribute to update
//
//--------------------------------------------------------------------------

void Device_3Impl::set_attribute_config_3(const Tango::AttributeConfigList_3& new_conf)
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "DeviceImpl::set_attribute_config_3 arrived" << endl;

//
// The attribute conf. is protected by two monitors. One protects access between
// get and set attribute conf. The second one protects access between set and
// usage. This is the classical device monitor
//

	TangoMonitor &mon1 = get_att_conf_monitor();
	AutoTangoMonitor sync1(&mon1);

//
// Record operation request in black box
// If this method is executed with the request to store info in
// blackbox (store_in_bb == true), this means that the request arrives
// through a Device_2 CORBA interface. Check locking feature in this
// case. Otherwise the request has arrived through Device_4 and the check
// is already done
//
//

	if (ext->store_in_bb == true)
	{
		blackbox_ptr->insert_op(Op_Set_Attr_Config_3);
		check_lock("set_attribute_config_3");
	}
	ext->store_in_bb = true;

//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				        (const char *)"The device does not have any attribute",
				        (const char *)"Device_3Impl::set_attribute_config_3");
	}

//
// Get some event related data
//

	EventSupplier *event_supplier_nd = NULL;
	EventSupplier *event_supplier_zmq = NULL;

	Tango::Util *tg = Tango::Util::instance();

//
// Update attribute config first locally then in database
//

	long nb_attr = new_conf.length();
	long i;

    EventSupplier::AttributeData ad;
    ::memset(&ad,0,sizeof(ad));

	try
	{
		for (i = 0;i < nb_attr;i++)
		{
			Attribute &attr = dev_attr->get_attr_by_name(new_conf[i].name);
			bool old_alarm = attr.is_alarmed().any();

			attr.set_upd_properties(new_conf[i],device_name);

//
// In case the attribute quality factor was set to ALARM, reset it to VALID
//

			if ((attr.get_quality() == Tango::ATTR_ALARM) &&
			    (old_alarm == true) &&
			    (attr.is_alarmed().any() == false))
				attr.set_quality(Tango::ATTR_VALID);

//
// Send the event
//

            if (attr.use_notifd_event() == true)
                event_supplier_nd = tg->get_notifd_event_supplier();
            else
                event_supplier_nd = NULL;

            if (attr.use_zmq_event() == true)
                event_supplier_zmq = tg->get_zmq_event_supplier();
            else
                event_supplier_zmq = NULL;

			if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL))
			{
				string tmp_name(new_conf[i].name);
				ad.attr_conf_3 = &(new_conf[i]);
				if (event_supplier_nd != NULL)
                    event_supplier_nd->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);
                if (event_supplier_zmq != NULL)
                    event_supplier_zmq->push_att_conf_events(this,ad,(Tango::DevFailed *)NULL,tmp_name);
			}
		}

	}
	catch (Tango::DevFailed &e)
	{

//
// Re build the list of "alarmable" attribute
//

		dev_attr->get_alarm_list().clear();
		for (long j = 0;j < nb_dev_attr;j++)
		{
			Attribute &att = dev_attr->get_attr_by_ind(j);
			if (att.is_alarmed().any() == true)
			{
				if (att.get_writable() != Tango::WRITE)
					dev_attr->get_alarm_list().push_back(j);
			}
		}

//
// Change the exception reason flag
//

		TangoSys_OMemStream o;

		o << e.errors[0].reason;
		if (i != 0)
			o << "\nAll previous attribute(s) have been successfully updated";
		if (i != (nb_attr - 1))
			o << "\nAll remaining attribute(s) have not been updated";
		o << ends;

		string s = o.str();
		e.errors[0].reason = CORBA::string_dup(s.c_str());
		throw;
	}

//
// Re build the list of "alarmable" attribute
//

	dev_attr->get_alarm_list().clear();
	for (i = 0;i < nb_dev_attr;i++)
	{
		Tango::Attribute &attr = dev_attr->get_attr_by_ind(i);
		Tango::AttrWriteType w_type = attr.get_writable();
		if (attr.is_alarmed().any() == true)
		{
			if (w_type != Tango::WRITE)
				dev_attr->get_alarm_list().push_back(i);
		}
	}

//
// Return to caller
//

	cout4 << "Leaving Device_3Impl::set_attribute_config_3" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::write_attributes_in_db
//
// description : 	Method to write memorized attributes in database
//
// argument: in :	- name : attribute name
//			- n : history depth (in record number)
//
// This method returns a pointer to a DevAttrHistoryList with one
// DevAttrHistory structure for each attribute record
//
//--------------------------------------------------------------------------

void Device_3Impl::write_attributes_in_db(vector<long> &att_in_db,vector<AttIdx> &updated_attr)
{
//
// Store memorized attribute in db
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::Database *db = tg->get_database();

	Tango::DbData db_data;

	for (unsigned long i = 0;i < att_in_db.size();i++)
	{
		Tango::DbDatum tmp_db;

//
// Update one property
//

		long idx = att_in_db[i];
		WAttribute &att = dev_attr->get_w_attr_by_ind(updated_attr[idx].idx_in_multi_attr);
		tmp_db.name = att.get_name();
		tmp_db << (short)1;
		db_data.push_back(tmp_db);

//
// Init property value
//

		tmp_db.name = MemAttrPropName;
		const char *ptr;
		switch (att.get_data_type())
		{
		case Tango::DEV_SHORT :
			tmp_db << (*att.get_last_written_sh())[0];
			break;

		case Tango::DEV_LONG :
			tmp_db << (*att.get_last_written_lg())[0];
			break;

		case Tango::DEV_LONG64 :
			tmp_db << (*att.get_last_written_lg64())[0];
			break;

		case Tango::DEV_DOUBLE :
			tmp_db << (*att.get_last_written_db())[0];
			break;

		case Tango::DEV_STRING :
			ptr = (*att.get_last_written_str())[0].in();
			tmp_db << ptr;
			break;

		case Tango::DEV_FLOAT :
			tmp_db << (*att.get_last_written_fl())[0];
			break;

		case Tango::DEV_BOOLEAN :
			tmp_db << (*att.get_last_written_boo())[0];
			break;

		case Tango::DEV_USHORT :
			tmp_db << (*att.get_last_written_ush())[0];
			break;

		case Tango::DEV_UCHAR :
			tmp_db << (*att.get_last_written_uch())[0];
			break;

		case Tango::DEV_ULONG :
			tmp_db << (*att.get_last_written_ulg())[0];
			break;

		case Tango::DEV_ULONG64 :
			tmp_db << (*att.get_last_written_ulg64())[0];
			break;

		case Tango::DEV_STATE :
			{
			Tango::DevState tmp_state = (*att.get_last_written_state())[0];
			tmp_db << (short)tmp_state;
			}
			break;
		}
		db_data.push_back(tmp_db);
	}

	db->put_device_attribute_property(device_name,db_data);

}

void Device_3Impl::write_attributes_in_db(vector<long> &att_in_db,vector<long> &updated_attr)
{
	vector<AttIdx> v;
	for (unsigned int i = 0;i < updated_attr.size();i++)
	{
		AttIdx ai;
		ai.idx_in_multi_attr = updated_attr[i];
		v.push_back(ai);
	}

	write_attributes_in_db(att_in_db,v);
}

//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::add_state_status_attrs
//
// description : 	Add state and status in the device attribute list
//
//--------------------------------------------------------------------------

void Device_3Impl::add_state_status_attrs()
{

//
// First, create the State attribute with default properties
//

	Tango::Attr att_state("State",Tango::DEV_STATE);
	vector<AttrProperty> prop_list_state;
	string att_name("State");
	dev_attr->add_default(prop_list_state,device_name,att_name);

	dev_attr->add_attr(new Attribute(prop_list_state,att_state,device_name,-1));

//
// Now, create the status attribute also with default properties
//

	Tango::Attr att_status("Status",Tango::DEV_STRING);
	vector<AttrProperty> prop_list_status;
	att_name = "Status";
	dev_attr->add_default(prop_list_status,device_name,att_name);

	dev_attr->add_attr(new Attribute(prop_list_status,att_status,device_name,-1));
}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::add_alarmed
//
// description : 	Method to add alarmed attributes (if not already
//			there) in the attribute list passed as argument
//
// argument: in :	- att_list : The attribute index in the
//				     multi attribute instance
//
//--------------------------------------------------------------------------

void Device_3Impl::add_alarmed(vector<long> &att_list)
{
	vector<long> &alarmed_list = dev_attr->get_alarm_list();
	long nb_wanted_attr = alarmed_list.size();

	if (nb_wanted_attr != 0)
	{
		for (int i = 0;i < nb_wanted_attr;i++)
		{
			long nb_attr = att_list.size();
			bool found = false;

			for (int j = 0;j < nb_attr;j++)
			{
				if (att_list[j] == alarmed_list[i])
				{
					found = true;
					break;
				}
			}

//
// If not found, add it
//

			if (found == false)
			{
				att_list.push_back(alarmed_list[i]);
			}
		}
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::reading_state_necessary
//
// description : 	Method to check ifi t is necessary to read
//			state. If the device has some alarmed
//			attribute and one of these attributes
//			has already been read and failed, it is
//			not necessary to read state. It will also
//			failed.
//
// argument: in :	- wanted_attr : The list of attribute to
//					be read by this call
//
// This  method returns -1 if reading state is possible.
// Otherwise, it returns the index in the wanted_attr list
// of the alarmed attribute which failed
//
//--------------------------------------------------------------------------

long Device_3Impl::reading_state_necessary(vector<AttIdx> &wanted_attr)
{
	vector<long> &alarmed_list = dev_attr->get_alarm_list();
	long nb_alarmed_attr = alarmed_list.size();
	long ret = -1;

	if (nb_alarmed_attr == 0)
		ret = -1;

	else
	{
		long nb_attr = wanted_attr.size();
		for (int j = 0;j < nb_alarmed_attr;j++)
		{
			for (int i = 0;i < nb_attr;i++)
			{
				if (alarmed_list[j] == wanted_attr[i].idx_in_multi_attr)
				{
					if (wanted_attr[i].failed == true)
						return i;
				}
			}
		}
	}

	return ret;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::alarmed_not_read
//
// description : 	This method find all the attributes which
//			will be read by the state (because alarmed)
//			and which have been already read.
//			It builds a vector with the list of attribute
//			not read
//
// argument: in :	- wanted_attr : The list of attribute to
//					be read by this call
//
//--------------------------------------------------------------------------

void Device_3Impl::alarmed_not_read(vector<AttIdx> &wanted_attr)
{
	vector<long> &alarmed_list = dev_attr->get_alarm_list();
	long nb_alarmed_attr = alarmed_list.size();
	long nb_attr = wanted_attr.size();

	ext->alarmed_not_read.clear();

	for (int i = 0;i < nb_alarmed_attr;i++)
	{
		bool found = false;
		for (int j = 0;j < nb_attr;j++)
		{
			if (alarmed_list[i] == wanted_attr[j].idx_in_multi_attr)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			ext->alarmed_not_read.push_back(alarmed_list[i]);
		}
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::state2attr
//
// description : 	Method to send a device state as an attribute object
//
// argument: in :	- state : The device state
//			- back : The AttributeValue structure
//
//--------------------------------------------------------------------------

void Device_3Impl::state2attr(Tango::DevState state,Tango::AttributeValue_3 &back)
{
	back.value <<= state;
#ifdef _TG_WINDOWS_
	struct _timeb after_win;

	_ftime(&after_win);
	back.time.tv_sec = (long)after_win.time;
	back.time.tv_usec = (long)after_win.millitm * 1000;
	back.time.tv_nsec = 0;
#else
	struct timeval after;

	gettimeofday(&after,NULL);
	back.time.tv_sec = after.tv_sec;
	back.time.tv_usec = after.tv_usec;
	back.time.tv_nsec = 0;
#endif
	back.quality = Tango::ATTR_VALID;
	back.name = CORBA::string_dup("State");
	back.r_dim.dim_x = 1;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}

void Device_3Impl::state2attr(Tango::DevState state,Tango::AttributeValue_4 &back)
{
	back.value.dev_state_att(state);
#ifdef _TG_WINDOWS_
	struct _timeb after_win;

	_ftime(&after_win);
	back.time.tv_sec = (long)after_win.time;
	back.time.tv_usec = (long)after_win.millitm * 1000;
	back.time.tv_nsec = 0;
#else
	struct timeval after;

	gettimeofday(&after,NULL);
	back.time.tv_sec = after.tv_sec;
	back.time.tv_usec = after.tv_usec;
	back.time.tv_nsec = 0;
#endif
	back.quality = Tango::ATTR_VALID;
	back.data_format = Tango::SCALAR;
	back.name = CORBA::string_dup("State");
	back.r_dim.dim_x = 1;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_3Impl::status2attr
//
// description : 	Method to send a device status string as an
//			attribute object
//
// argument: in :	- status : The device status
//			- back : The AttributeValue structure
//
//--------------------------------------------------------------------------

void Device_3Impl::status2attr(Tango::ConstDevString status,Tango::AttributeValue_3 &back)
{
	Tango::DevVarStringArray str_seq(1);
	str_seq.length(1);
	str_seq[0] = CORBA::string_dup(status);

	back.value <<= str_seq;
#ifdef _TG_WINDOWS_
	struct _timeb after_win;

	_ftime(&after_win);
	back.time.tv_sec = (long)after_win.time;
	back.time.tv_usec = (long)after_win.millitm * 1000;
	back.time.tv_nsec = 0;
#else
	struct timeval after;

	gettimeofday(&after,NULL);
	back.time.tv_sec = after.tv_sec;
	back.time.tv_usec = after.tv_usec;
	back.time.tv_nsec = 0;
#endif
	back.quality = Tango::ATTR_VALID;
	back.name = CORBA::string_dup("Status");
	back.r_dim.dim_x = 1;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}

void Device_3Impl::status2attr(Tango::ConstDevString status,Tango::AttributeValue_4 &back)
{
	Tango::DevVarStringArray str_seq(1);
	str_seq.length(1);
	str_seq[0] = CORBA::string_dup(status);

	back.value.string_att_value(str_seq);
#ifdef _TG_WINDOWS_
	struct _timeb after_win;

	_ftime(&after_win);
	back.time.tv_sec = (long)after_win.time;
	back.time.tv_usec = (long)after_win.millitm * 1000;
	back.time.tv_nsec = 0;
#else
	struct timeval after;

	gettimeofday(&after,NULL);
	back.time.tv_sec = after.tv_sec;
	back.time.tv_usec = after.tv_usec;
	back.time.tv_nsec = 0;
#endif
	back.quality = Tango::ATTR_VALID;
	back.data_format = Tango::SCALAR;
	back.name = CORBA::string_dup("Status");
	back.r_dim.dim_x = 1;
	back.r_dim.dim_y = 0;
	back.w_dim.dim_x = 0;
	back.w_dim.dim_y = 0;
}


} // End of Tango namespace
