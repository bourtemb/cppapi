//+======================================================================
// $Source$
//
// Project:   Tango
//
// Description:  Cpp source code for the AccessProxy class definition .
//
// $Author$
//
// $Revision$
//
// $Log$
//
// Copyleft 2008 by European Synchrotron Radiation Facility, Grenoble, France
//               All Rights Reversed
//-======================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <accessproxy.h>

#include <sys/types.h>
#include <pwd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Tango
{


/** 
 *	This class is extends TangoApi.DeviceProxy 
 *	to manage Tango access device.
 *	 - Check if control access is requested.
 *	 - Check who is the user and the host.
 *	 - Check access for this user, this host and the specified device.
 *
 * @author  verdier
 * @Revision 
 */


//===============================================================
/**
*	Constructor for Access device proxy
*
*	@param devname	access device name
*/
//===============================================================

AccessProxy::AccessProxy(string &devname) : DeviceProxy(devname,false),
forced(false)
{

//
//	Build device proxy and check if present.
//

	ping();

//
//	Check if forced mode
//

	char *forced_str = getenv("SUPER_TANGO");
	if (forced_str != NULL)
	{
		string tmp_forced(forced_str);
		transform(tmp_forced.begin(),tmp_forced.end(),tmp_forced.begin(),::tolower);
		if (tmp_forced == "true")
			forced = true;
	}
	
	set_access_control(ACCESS_WRITE);
}


//===============================================================
/**
 *	Check access for specified device
 *
 *	@param devname	device name to check access
 */
//===============================================================

AccessControlType AccessProxy::check_access_control(string &devname)
{
	if (forced)
		return ACCESS_WRITE;

	try
	{

//
//	If not already done, get user name.
// I am using the effective UID in order to allow applications using the seteuid(0) system call
// to change the effective user id and therefore to take someone else rights
//

		if (user.empty() == true)
		{
			uid_t user_id = geteuid();

			struct passwd pw;
			struct passwd *pw_ptr;
			char buffer[__AC_BUFFER_SIZE];

			if (getpwuid_r(user_id,&pw,buffer,sizeof(buffer),&pw_ptr) != 0)
			{
				cerr << "AccessProxy::check_access_control: Can't get the user UID !" << endl;
				cerr << "Access right set to ACCESS_READ" << endl;

				return ACCESS_READ;
			}

			if (pw_ptr == NULL)
			{
				cerr << "AccessProxy::check_access_control: Can't get the user UID !" << endl;
				cerr << "Access right set to ACCESS_READ" << endl;

				return ACCESS_READ;
			}

			user = pw.pw_name;
			transform(user.begin(),user.end(),user.begin(),::tolower);
		}

//
//	If not already done, get host address
//

		if (host.empty() == true)
		{
			char h_name[80];
			int res = gethostname(h_name,80);
			if (res == 0)
			{
				struct hostent my_addr;
				struct hostent *my_addr_ptr;
				char buffer[__AC_BUFFER_SIZE];
				int err;

				if (gethostbyname_r(h_name,&my_addr,buffer,sizeof(buffer),&my_addr_ptr,&err) != 0)
				{
					cerr << "AccessProxy::check_access_control: Can't get my IP address !" << endl;
					cerr << "Access right set to ACCESS_READ" << endl;

					return ACCESS_READ;
				}

				struct in_addr **addr_list;

//
// TODO : Manage the case of host with several network address
//
					
				addr_list = (struct in_addr **)my_addr.h_addr_list;
				host = inet_ntoa(*addr_list[0]);
			}
			else
			{
				cerr << "AccessProxy::check_access_control: Can't get my host name !" << endl;
				cerr << "Access right set to ACCESS_READ" << endl;

				return ACCESS_READ;
			}
		}

//
//	Check for user and host rights on specified device
//

		DeviceData din,dout;
		DevVarStringArray dvsa;
		dvsa.length(3);
		dvsa[0] = CORBA::string_dup(user.c_str());
		dvsa[1] = CORBA::string_dup(host.c_str());
		dvsa[2] = CORBA::string_dup(devname.c_str());
		din << dvsa;

		dout = command_inout("GetAccess",din);

		string right;
		dout >> right;
		if (right == "write")
			return ACCESS_WRITE;
		else
			return ACCESS_READ;
	}
	catch (Tango::DevFailed &e)
	{
		if (::strcmp(e.errors[0].reason.in(),"API_DeviceNotExported") == 0)
		{
			Except::re_throw_exception(e,(const char *)"API_CannotCheckAccessControl",
										(const char *)"Cannot import Access Control device !",
										(const char *)"AccessProxy::check_access_control()");
		}
		else
			throw;
	}

	return ACCESS_READ;
}



//===============================================================
/**
 *	Check for specified device, the specified command is allowed.
 *
 *	@param	classname Specified device class name.
 *	@param	cmd Specified command name.
 */
//===============================================================

bool AccessProxy::is_command_allowed(string &classname,string &cmd)
{
	bool ret = false;

//
// Try to find allowed commands for device class in map
//

	map<string,vector<string> >::iterator pos;
	pos = allowed_cmd_table.find(classname);

//
//	If allowed commands for this class not already in map, get them
//

	vector<string> allowed;
	if (pos == allowed_cmd_table.end())
		get_allowed_commands(classname,allowed);
	else
		allowed = pos->second;

//
//	If no cmd allowed returns false
//

	if (allowed.empty() == true)
		ret = false;

//
//	Else, check is the one passed as argument is allowed
//

	else
	{
		for (int i = 0;i < allowed.size();++i)
		{
			if (TG_strcasecmp(allowed[i].c_str(),cmd.c_str()) == 0)
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}


//===============================================================
/**
 *	query access device to know allowed commands for the class
 */
//===============================================================

void AccessProxy::get_allowed_commands(string &classname,vector<string> &allowed)
{
	try
	{
		DeviceData	din,dout;
		din << classname;

		dout = command_inout("GetAllowedCommands", din);

		dout >> allowed;
	}
	catch (Tango::DevFailed &e)
	{
		allowed.clear();
	}
	allowed_cmd_table.insert(map<string,vector<string> >::value_type(classname, allowed));
}

} // End of Tango namespace
