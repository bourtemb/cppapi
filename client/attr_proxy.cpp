static const char *RcsId = "$Id$";
//
// attr_proxy.cpp 	- C++ source code file for TANGO attribute proxy api 
//
// programmer(s)	- Andy Gotz (goetz@esrf.fr)
//
// original 		- July 2003
//
// version 		- $Version$
//

#include <tango.h>

#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */

#include <time.h>
#include <signal.h>

#include <algorithm>
                                                    
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// AttributeProxy::AttributeProxy() - constructor for attribute proxy object 
//
//-----------------------------------------------------------------------------

AttributeProxy::AttributeProxy (string &name)
{
	real_constructor(name);
}

AttributeProxy::AttributeProxy (const char *na)
{
	string name(na);
	real_constructor(name);
}

void AttributeProxy::real_constructor (string &name)
{

//
// parse device name
//

	parse_name(name);
	string corba_name;
	bool exported = true;
		
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
			{
				db_attr = new DbAttribute(device_name,attr_name,Tango::Util::instance()->get_database());
				dev_proxy = new DeviceProxy(device_name);
			}
			else
			{
				db_attr = new DbAttribute(device_name,attr_name);
				dev_proxy = new DeviceProxy(device_name);
			}
			int ind = ui->get_db_ind();
			db_host = (ui->get_db_vect())[ind]->get_db_host();
			db_port = (ui->get_db_vect())[ind]->get_db_port();
			db_port_num = (ui->get_db_vect())[ind]->get_db_port_num();
		}
		else
		{
			db_attr = new DbAttribute(device_name,attr_name,db_host,db_port);
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::AttributeProxy() - copy constructor
//
//-----------------------------------------------------------------------------

AttributeProxy::AttributeProxy(const AttributeProxy &prev)
{

//
// First Connection call members
//
	dbase_used = prev.dbase_used;
	from_env_var = prev.from_env_var;
	host = prev.host;
	port = prev.port;
	port_num = prev.port_num;
	db_host = prev.db_host;
	db_port = prev.db_port;
	db_port_num = prev.db_port_num;
	
//
// Now AttributeProxy members
//
	
	device_name = prev.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
			{
				db_attr = new DbAttribute(attr_name,device_name,Tango::Util::instance()->get_database());
				dev_proxy = new DeviceProxy(device_name);
			}
			else
			{
				db_attr = new DbAttribute(attr_name,device_name);
				dev_proxy = new DeviceProxy(device_name);
			}
		}
		else
		{
			db_attr = new DbAttribute(attr_name,device_name,db_host,db_port);
			dev_proxy = new DeviceProxy(device_name);
		}
	}
		
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::AttributeProxy() - assignment operator 
//
//-----------------------------------------------------------------------------

AttributeProxy &AttributeProxy::operator=(const AttributeProxy &rval)
{

//
// First Connection call members
//
	if (dbase_used == true)
		delete db_attr;
	dbase_used = rval.dbase_used;
	from_env_var = rval.from_env_var;
	host = rval.host;
	port = rval.port;
	port_num = rval.port_num;
	db_host = rval.db_host;
	db_port = rval.db_port;
	db_port_num = rval.db_port_num;
	
//
// Now AttributeProxy members
//
	
	attr_name = rval.attr_name;
	device_name = rval.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
			{
				db_attr = new DbAttribute(attr_name,device_name,Tango::Util::instance()->get_database());
				dev_proxy = new DeviceProxy(device_name);
			}
			else
			{
				db_attr = new DbAttribute(attr_name,device_name);
				dev_proxy = new DeviceProxy(device_name);
			}
		}
		else
		{
			db_attr = new DbAttribute(attr_name,device_name,db_host,db_port);
			dev_proxy = new DeviceProxy(device_name);
		}
	}
	
	return *this;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::parse_name() - Parse attribute name according to Tango device
//			       name syntax
//
// in :	- full_name : The attribute name
//
//-----------------------------------------------------------------------------

void AttributeProxy::parse_name(string &full_name)
{
	string name_wo_prot;
	string name_wo_db_mod;
	string dev_name;
	string att_name;

//
// Attribute name in lower case letters
//
	
	transform(full_name.begin(),full_name.end(),full_name.begin(),::tolower);
		
//
// Try to find protocol specification in attribute name and analyse it
//

	string::size_type pos = full_name.find(PROT_SEP);
	if (pos == string::npos)
	{
		name_wo_prot = full_name;
	}
	else
	{
		string protocol = full_name.substr(0,pos);

		if (protocol == TANGO_PROTOCOL)
		{
			name_wo_prot = full_name.substr(pos + 3);
		}		
		else if (protocol == TACO_PROTOCOL)
		{		
			TangoSys_OMemStream desc;
			desc << "Taco protocol is not supported" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"AttributeProxy::parse_name()");			exit(0);
		}
		else
		{		
			TangoSys_OMemStream desc;
			desc << protocol;
			desc << " protocol is an unsupported protocol" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"AttributeProxy::parse_name()");
		}			
	}

//
// Try to find database database modifier and analyse it
//

	pos = name_wo_prot.find(MODIFIER);
	if (pos != string::npos)
	{
		string mod = name_wo_prot.substr(pos + 1);
		
		if (mod == DBASE_YES)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos)); 
			dbase_used = true;
		}
		else if (mod == DBASE_NO)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos));
			dbase_used = false;
		}
		else
		{
			cerr << mod << " is a non supported database modifier!" << endl;

			TangoSys_OMemStream desc;
			desc << mod;
			desc << " modifier is an unsupported db momdifier" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedDBaseModifier",
						desc.str(),
						(const char*)"AttributeProxy::parse_name()");
		}
	}
	else
	{
		name_wo_db_mod = name_wo_prot;
		dbase_used = true;
	}

	if (dbase_used == false)
	{
	
//
// Extract host name and port number
//

		pos = name_wo_db_mod.find(HOST_SEP);
		if (pos == string::npos)
		{
			cerr << "Host and port not correctly defined in device name " << full_name << endl;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongDeviceNameSyntax",
						(const char*)"Host and port not correctly defined in device name",
						(const char*)"AttributeProxy::parse_name()");
		}
		
		host = name_wo_db_mod.substr(0,pos);
		string::size_type tmp = name_wo_db_mod.find(PORT_SEP);
		if (tmp == string::npos)
		{
			cerr << "Host and port not correctly defined in device name " << full_name << endl;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongDeviceNameSyntax",
						(const char*)"Host and port not correctly defined in device name",
						(const char*)"AttributeProxy::parse_name()");
		}
		port = name_wo_db_mod.substr(pos + 1,tmp - pos - 1);
		TangoSys_MemStream s;
		s << port << ends;
		s >> port_num;
		device_name = name_wo_db_mod.substr(tmp + 1);
		
		db_host = db_port = NOT_USED;
		db_port_num = 0;
				 
	}
	else
	{
	
//
// Search if host and port are specified
//

		pos = name_wo_db_mod.find(PORT_SEP);
		if (pos == string::npos)
		{
			device_name = name_wo_db_mod;
			from_env_var = true;
			port_num = 0;
			host = FROM_IOR;
			port = FROM_IOR;
		}
		else
		{
			string bef_sep = name_wo_db_mod.substr(0,pos);
			string::size_type tmp = bef_sep.find(HOST_SEP);
			if (tmp == string::npos)
			{
				device_name = name_wo_db_mod;
				from_env_var = true;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
			else
			{
				db_host = bef_sep.substr(0,tmp);
				db_port = bef_sep.substr(tmp + 1);
				TangoSys_MemStream s;
				s << db_port << ends;
				s >> db_port_num;
				device_name = name_wo_db_mod.substr(pos + 1);
				from_env_var = false;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
		}

	}

// decompose device_name into device and attribute

	int n_sep = 0;
	string device_name_tmp(device_name);
	string::size_type device_name_end_pos=0;
	do 
	{
		pos = device_name_tmp.find(DEVICE_SEP);
		if (pos != string::npos) 
		{
			n_sep++;
			device_name_tmp = device_name_tmp.substr(pos+1);
			device_name_end_pos += pos+1;
		}
	} 
	while (pos != string::npos);

	if (n_sep != 0 && n_sep != 3)
	{

		ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
			(const char*)"Attribute name must have four fields separated by /'s or no /'s at all if it is an alias (e.g. my/device/name/an_attr or myalias)",
			(const char*)"AttributeProxy::parse_name()");
	}

// if this is an alias (no slashes in name) then get the device and attribute name from the database
	
	if (n_sep == 0)
	{
		Tango::Util::instance()->get_database()->get_attribute_alias(device_name,device_name_tmp);
		do 
		{
			pos = device_name_tmp.find(DEVICE_SEP);
			if (pos != string::npos) 
			{
				n_sep++;
				device_name_tmp = device_name_tmp.substr(pos+1);
				device_name_end_pos += pos+1;
			}
		} 
		while (pos != string::npos);

		if (n_sep != 3)
		{

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongAttributeNameSyntax",
				(const char*)"Attribute name must have four fields separated by /'s (check the alais entry in the database) ",
				(const char*)"AttributeProxy::parse_name()");
		}
		attr_name = device_name_tmp.substr(device_name_end_pos);
		device_name = device_name_tmp.substr(0,device_name_end_pos-1);
	}

// attribute name has four fields, separate them into device and attribute names
	else
	{
		attr_name = device_name.substr(device_name_end_pos);
		device_name = device_name.substr(0,device_name_end_pos-1);
	}
		
		
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::~AttributeProxy() - destructor to destroy proxy to TANGO device
//
//-----------------------------------------------------------------------------

AttributeProxy::~AttributeProxy()
{
	if (dbase_used == true)
		delete db_attr;
		
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::ping() - ping TANGO device and return time elapsed in microseconds
//
//-----------------------------------------------------------------------------

int AttributeProxy::ping()
{

	return(dev_proxy->ping());
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::state() - return TANGO state of device
//
//-----------------------------------------------------------------------------

DevState AttributeProxy::state()
{

	return(dev_proxy->state());
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::status() - return TANGO status of device
//
//-----------------------------------------------------------------------------

string AttributeProxy::status()
{
	return(dev_proxy->status());
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::get_property(string &property_name, DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::get_property");
	}
	else
	{
		db_data.resize(1);
		db_data[0] = DbDatum(property_name);

		db_attr->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::get_property(vector<string> &property_names, DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::get_property");
	}
	else
	{
		db_data.resize(property_names.size());
		for (unsigned int i=0; i<property_names.size(); i++)
		{
			db_data[i] = DbDatum(property_names[i]);
		}

		db_attr->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::get_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::get_property");
	}
	else
	{
		db_attr->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::put_property() - put a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::put_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::put_property");
	}
	else
	{
		db_attr->put_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::delete_property() - deleteget a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::delete_property(string &property_name) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::delete_property");
	}
	else
	{
		DbData db_data;

		db_data.push_back(DbDatum(property_name));

		db_attr->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::delete_property(vector<string> &property_names) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::delete_property");
	}
	else
	{
		DbData db_data;

		for (unsigned int i=0; i<property_names.size(); i++)
		{
			db_data.push_back(DbDatum(property_names[i]));
		}

		db_attr->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::delete_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"AttributeProxy::delete_property");
	}
	else
	{
		db_attr->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_config() - return attribute config
//
//-----------------------------------------------------------------------------
 
AttributeInfo AttributeProxy::get_config() 
{

	return (dev_proxy->get_attribute_config(attr_name));

}

//-----------------------------------------------------------------------------
//
// AttributeProxy::set_attribute_config() - set config for attribute
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::set_config(AttributeInfo &dev_attr_info) 
{
	AttributeInfoList attr_info_list;

	attr_info_list.push_back(dev_attr_info);
	try 
	{
		dev_proxy->set_attribute_config(attr_info_list);

	}
        catch (CORBA::SystemException &ce)
        {
		TangoSys_OMemStream desc;
		desc << "Failed to execute set_attribute_config on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"AttributeProxy::set_attribute_config()");
	}


	return;
}


//-----------------------------------------------------------------------------
//
// AttributeProxy::read() - read attribute 
//
//-----------------------------------------------------------------------------
 

DeviceAttribute AttributeProxy::read() 
{
	return(dev_proxy->read_attribute(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::write() - write attribute
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::write(DeviceAttribute& attr_value) 
{
	attr_value.set_name(attr_name);
	dev_proxy->write_attribute(attr_value);

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::history() - get attribute history
//				      (only for polled attribute)
//
//-----------------------------------------------------------------------------
 
vector<DeviceAttributeHistory> *AttributeProxy::history(int depth) 
{
	string cmd_name;

	return(dev_proxy->attribute_history(cmd_name, depth));

}

//-----------------------------------------------------------------------------
//
// AttributeProxy::get_poll_period() - Return attribute polling period
//					    (in mS)
//
//-----------------------------------------------------------------------------
 
int AttributeProxy::get_poll_period() 
{
	return(dev_proxy->get_attribute_poll_period(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::poll() - If object is already polled, just update its
//			    polling period. If object is not polled, add
//			    it to the list of polled objects
//
//-----------------------------------------------------------------------------
 
void AttributeProxy::poll(int period) 
{
	dev_proxy->poll_attribute(attr_name, period);

	return;
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::ispolled() - return true if the attribute is polled
//
//-----------------------------------------------------------------------------

bool AttributeProxy::is_polled()
{
	return(dev_proxy->is_attribute_polled(attr_name));
}

//-----------------------------------------------------------------------------
//
// AttributeProxy::stop_poll() - Stop polling attribute
//
//-----------------------------------------------------------------------------

void AttributeProxy::stop_poll()
{
	dev_proxy->stop_poll_attribute(attr_name);

}

//-----------------------------------------------------------------------------
//
// AttributeProxy::subscribe_event - Subscribe to an event
//
//-----------------------------------------------------------------------------

int AttributeProxy::subscribe_event (EventType event, CallBack *callback, const vector<string> &filters)
{
  	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		ApiUtil::instance()->create_event_consumer();
	}

	ApiUtil::instance()->get_event_consumer()->connect(dev_proxy);

	
	return ApiUtil::instance()->get_event_consumer()->subscribe_event(dev_proxy, attr_name, event, callback, filters);
}

} // End of Tango namespace
