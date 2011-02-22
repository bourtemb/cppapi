//
// AccessProxy.h -	include file for TANGO AccessProxy class
//
// 
#ifndef _ACCESSPROXY_H
#define _ACCESSPROXY_H

#include <tango.h>

using namespace std;

namespace Tango {

#define		__AC_BUFFER_SIZE		1024

class AccessProxy: public Tango::DeviceProxy
{
public:
	AccessProxy(string &);
	~AccessProxy() {}

	AccessControlType check_access_control(string &);
	bool is_command_allowed(string &,string &);

protected:
	string							user;
	string							host;
	bool							forced;
	map<string,vector<string> > 	allowed_cmd_table;

	void get_allowed_commands(string &,vector<string> &);
};

} // End of Tango namespace

#endif /* _ACCESSPROXY_H */
