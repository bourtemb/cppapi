//
// devapi.h - include file for TANGO device api
//
// 

#ifndef _DEVAPI_H
#define _DEVAPI_H

#include <tango.h>
#include <tango_const.h>
#include <apiexcept.h>
#include <cbthread.h>

#include <bitset>

using namespace std;

namespace Tango {

//
// forward declarations

class DeviceData;
class DeviceAttribute;
class DbDevice;
class DbAttribute;
class DbDatum;
class DbDevImportInfo;
class Database;
class AsynReq;
class EventConsumer;
class CallBack;

typedef vector<DbDatum> DbData;

struct _DevCommandInfo
{
	string cmd_name;
	long cmd_tag;
	long in_type;
	long out_type;
	string in_type_desc;
	string out_type_desc;
};

typedef struct _DevCommandInfo DevCommandInfo;

struct _CommandInfo : public DevCommandInfo
{
	Tango::DispLevel disp_level;
};

typedef _CommandInfo CommandInfo;
typedef vector<CommandInfo> CommandInfoList;

struct _DeviceInfo
{
	string dev_class;
	string server_id;
	string server_host;
	long server_version;
	string doc_url;
	int dev_type;
};
	
typedef _DeviceInfo DeviceInfo;



struct _DeviceAttributeConfig
{
	string name;
	AttrWriteType writable;
	AttrDataFormat data_format;
	int data_type;
	int max_dim_x;
	int max_dim_y;
	string description;
	string label;
	string unit;
	string standard_unit;
	string display_unit;
	string format;
	string min_value;
	string max_value;
	string min_alarm;
	string max_alarm;
	string writable_attr_name;
	vector<string> extensions;
};
	
typedef struct _DeviceAttributeConfig DeviceAttributeConfig;

struct _AttributeInfo : public DeviceAttributeConfig
{
	Tango::DispLevel disp_level;
};

typedef _AttributeInfo AttributeInfo;
typedef vector<AttributeInfo> AttributeInfoList;

//
// Can't use CALLBACK (without _) in the following enum because it's a
// pre-defined type on Windows....
//
	
enum asyn_req_type
{
	POLLING,
	CALL_BACK,
	ALL_ASYNCH
};

enum cb_sub_model
{
	PUSH_CALLBACK,
	PULL_CALLBACK
};

#define 	CONNECTION_OK		1
#define 	CONNECTION_NOTOK	0

#define		PROT_SEP		"://"
#define		TACO_PROTOCOL		"taco"
#define		TANGO_PROTOCOL		"tango"

#define		MODIFIER		'#'
#define		DBASE_YES		"dbase=yes"
#define		DBASE_NO		"dbase=no"

#define		HOST_SEP		':'
#define		PORT_SEP		'/'
#define		DEVICE_SEP		'/'

#define		FROM_IOR		"IOR"
#define		NOT_USED		"Unused"

#define		DEFAULT_API_TIMEOUT	3000

class ApiUtil
{
public:
	inline static ApiUtil *instance()
	{if (_instance == NULL)_instance = new ApiUtil();return _instance;}
	
	CORBA::ORB_ptr	get_orb() {return _orb;}
	void		set_orb(CORBA::ORB_ptr orb_in) {_orb = orb_in;}
	void 		create_orb();
	int		get_db_ind();
	int		get_db_ind(string &host,int port);
	vector<Database *> &get_db_vect() {return db_vect;}
	bool		in_server() {return in_serv;}
	AsynReq		*get_pasyn_table() {return asyn_p_table;}
	void		create_event_consumer();
	EventConsumer	*get_event_consumer() {return event_consumer;}
	
	long 		pending_asynch_call(asyn_req_type ty)
	{if (ty==POLLING)return asyn_p_table->get_request_nb();
	 else if (ty==CALL_BACK)return asyn_p_table->get_cb_request_nb();
	 else return (asyn_p_table->get_request_nb()+asyn_p_table->get_cb_request_nb());}
	 
	inline static void cleanup()
	{if (_instance != NULL){delete _instance;_instance=NULL;}}
	
//
//Asynchronous methods
//
	
	void get_asynch_replies();
	void get_asynch_replies(long);
	
	void set_asynch_cb_sub_model(cb_sub_model);
	cb_sub_model get_asynch_cb_sub_model() {return auto_cb;}
		
protected:
	ApiUtil();
	virtual ~ApiUtil();
	
	vector<Database *>	db_vect;
	CORBA::ORB_ptr		_orb;
	bool			in_serv;
	
	cb_sub_model		auto_cb;
	CbThreadCmd		cb_thread_cmd;
	CallBackThread		*cb_thread_ptr;
	
	AsynReq			*asyn_p_table;
		
private:
	static ApiUtil *_instance;
	EventConsumer *event_consumer;
};


class DeviceData
{

public :
//
// constructor methods
//
	enum except_flags
	{
                isempty_flag,
                wrongtype_flag,
		numFlags
	};

	DeviceData();
	DeviceData(const DeviceData &);
	DeviceData & operator=(const DeviceData &);
	~DeviceData();

	bool is_empty() {return any_is_null();}
	void exceptions(bitset<numFlags> fl) {exceptions_flags = fl;}
	
	CORBA::Any_var any;
//
// insert methods for native C++ types
//
	void operator << (bool datum) {any <<= CORBA::Any::from_boolean(datum);}
	void operator << (short datum) {any <<= datum;}
	void operator << (unsigned short datum) {any <<= datum;}
	void operator << (long datum) {any <<= datum;}
	void operator << (unsigned long datum) {any <<= datum;}
	void operator << (float datum) {any <<= datum;}
	void operator << (double datum) {any <<= datum;}
	void operator << (char *&datum) {any <<= datum;}
	void operator << (const char *&datum) {any <<= datum;}
	void operator << (string &datum) {any <<= datum.c_str();}
	void operator << (vector<unsigned char>&);
	void operator << (vector<string>&);
	void operator << (vector<short>&);
	void operator << (vector<unsigned short>&);
	void operator << (vector<long>&);
	void operator << (vector<unsigned long>&);
	void operator << (vector<float>&);
	void operator << (vector<double>&);
	void operator << (DevState datum) {(any.inout()) <<= datum;}
	void insert(vector<long>&, vector<string>&);
	void insert(vector<double>&, vector<string>&);
//
// insert methods for TANGO CORBA sequence types
//
	inline void operator << (DevVarCharArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarShortArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarUShortArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarULongArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarFloatArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarStringArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongStringArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleStringArray* datum) { any.inout() <<= datum;}

	inline void operator << (DevVarCharArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarShortArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarUShortArray datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarULongArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarFloatArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarStringArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongStringArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleStringArray &datum) { any.inout() <<= datum;}
	
//
// extract methods for native C++ types
//
	bool operator >> (bool&);
	bool operator >> (short&);
	bool operator >> (unsigned short&);
	bool operator >> (long&);
	bool operator >> (unsigned long&);
	bool operator >> (float&);
	bool operator >> (double&);
	bool operator >> (const char*&);
	bool operator >> (string&);
	bool operator >> (vector<unsigned char>&);
	bool operator >> (vector<string>&);
	bool operator >> (vector<short>&);
	bool operator >> (vector<unsigned short>&);
	bool operator >> (vector<long>&);
	bool operator >> (vector<unsigned long>&);
	bool operator >> (vector<float>&);
	bool operator >> (vector<double>&);
	bool operator >> (DevState&);
	bool extract(vector<long>&, vector<string>&);
	bool extract(vector<double>&, vector<string>&);
//
// extract methods for TANGO CORBA sequence types
//
	bool operator >> (const DevVarCharArray* &datum);
	bool operator >> (const DevVarShortArray* &datum);
	bool operator >> (const DevVarUShortArray* &datum);
	bool operator >> (const DevVarLongArray* &datum);
	bool operator >> (const DevVarULongArray* &datum);
	bool operator >> (const DevVarFloatArray* &datum);
	bool operator >> (const DevVarDoubleArray* &datum);
	bool operator >> (const DevVarStringArray* &datum);
	bool operator >> (const DevVarLongStringArray* &datum);
	bool operator >> (const DevVarDoubleStringArray* &datum);

protected :
	bool any_is_null();
	
	bitset<numFlags> exceptions_flags;
};



class DeviceAttribute
{

public :
//
// constructor methods
//
        enum except_flags
	{
                isempty_flag,
                wrongtype_flag,
		numFlags
	};

	DeviceAttribute();
	DeviceAttribute(const DeviceAttribute&);
	DeviceAttribute & operator=(const DeviceAttribute &);
	
	DeviceAttribute(AttributeValue);
	DeviceAttribute(string&, short);
	DeviceAttribute(string&, long);
	DeviceAttribute(string&, double);
	DeviceAttribute(string&, string&);
	~DeviceAttribute();
	
	AttrQuality quality;
	string name;
	int dim_x;
	int dim_y;
	TimeVal time;
		
	string &get_name() {return name;}
	AttrQuality &get_quality() {return quality;}
	int get_dim_x() {return dim_x;}
	int get_dim_y() {return dim_y;}
	TimeVal &get_date() {return time;}
	
	void set_name(string &na) {name =  na;}
	void set_name(const char *na) {string str(na);name = str;}
	bool is_empty();
	void exceptions(bitset<numFlags> fl) {exceptions_flags = fl;}
	
	DevVarLongArray_var LongSeq;
	DevVarShortArray_var ShortSeq;
	DevVarDoubleArray_var DoubleSeq;
	DevVarStringArray_var StringSeq;

//
// insert methods for native C++ types
//
	void operator << (short);
	void operator << (long);
	void operator << (double);
	void operator << (string &);
//
// extract methods for native C++ types
//
	bool operator >> (short &);
	bool operator >> (long &);
	bool operator >> (double &);
	bool operator >> (string&);
	bool operator >> (vector<string>&);
	bool operator >> (vector<short>&);
	bool operator >> (vector<long>&);
	bool operator >> (vector<double>&);
//
// extract methods for TANGO CORBA sequence types
//
	bool operator >> (DevVarShortArray* &datum);
	bool operator >> (DevVarLongArray* &datum);
	bool operator >> (DevVarDoubleArray* &datum);
	bool operator >> (DevVarStringArray* &datum);

protected :	
	bitset<numFlags> exceptions_flags;
	

};

class DeviceDataHistory: public DeviceData
{

public :
//
// constructor methods
//

	DeviceDataHistory(int, int *,DevCmdHistoryList *);
	DeviceDataHistory(const DeviceDataHistory &);
	DeviceDataHistory & operator=(const DeviceDataHistory &);
	
	~DeviceDataHistory();

	bool failed() {return fail;}
	TimeVal &date() {return time;}
	const DevErrorList &errors() {return err.in();}
	
protected:
	bool fail;
	TimeVal time;
	DevErrorList_var err;
		
	DevCmdHistoryList *seq_ptr;
	int *ref_ctr_ptr;
};

typedef vector<DeviceDataHistory> DeviceDataHistoryList;

class DeviceAttributeHistory: public DeviceAttribute
{

public :
//
// constructor methods
//

	DeviceAttributeHistory(int, DevAttrHistoryList_var &);
	DeviceAttributeHistory(const DeviceAttributeHistory &);
	DeviceAttributeHistory & operator=(const DeviceAttributeHistory &);
	
	~DeviceAttributeHistory();

	bool failed() {return fail;}
	TimeVal &date() {return time;}
	const DevErrorList &errors() {return err;}
	
protected:
	bool fail;
	DevErrorList err;		
};

class Connection 
{
protected :
	bool 	dbase_used;			// Dev. with database
	bool 	from_env_var;			// DB from TANGO_HOST
	
	string 	host;				// DS host (if dbase_used=false)
	string 	port;				// DS port (if dbase_used=false)
	int 	port_num;			// DS port (as number)
	
	string 	db_host;			// DB host
	string 	db_port;			// DB port
	int 	db_port_num;			// DB port (as number)
	
	string 	ior;
	long 	pasyn_ctr;
	long	pasyn_cb_ctr;
		
	Tango::Device_var device;
	Tango::Device_2_var device_2;
	
	int 	timeout;
	int 	timeout_changed;
	
	enum timeout_flags
	{
		all_devices_flag,
		device_flag,
		device_2_flag,
		numFlags
	};

	int connection_state;
	int version;
	Tango::DevSource source;

	virtual string get_corba_name()=0;
	virtual string build_corba_name()=0;


	
public :
	virtual string dev_name()=0;
	
	Connection(CORBA::ORB *orb = NULL);
	virtual ~Connection();

	string &get_db_host() {return db_host;}
	string &get_db_port() {return db_port;}
	int get_db_port_num() {return db_port_num;}
	bool get_from_env_var() {return from_env_var;}
	
	void connect(string &name);
	void reconnect(bool);
	void set_timeout_millis(int timeout,bitset<numFlags> fl=bitset<numFlags>(1));
	int get_timeout_millis();
	int get_idl_version() {return version;}
	Tango::DevSource get_source() {return source;}
	void set_source(Tango::DevSource sou) {source = sou;}
	
	Tango::Device_var &get_device() {return device;} 	// For CORBA expert !!

	DeviceData command_inout(string &);
	DeviceData command_inout(const char *co) {string str(co);return command_inout(str);}
	DeviceData command_inout(string &, DeviceData &);
	DeviceData command_inout(const char *co,DeviceData &d) {string str(co);return command_inout(str,d);}
	CORBA::Any_var command_inout(string &, CORBA::Any&);
	CORBA::Any_var command_inout(const char *co, CORBA::Any &d) {string str(co);return command_inout(str,d);}

//
// Asynchronous methods
//

	void Cb_Cmd_Request(CORBA::Request_ptr,Tango::CallBack *);
	void Cb_ReadAttr_Request(CORBA::Request_ptr,Tango::CallBack *);
	void Cb_WriteAttr_Request(CORBA::Request_ptr req,Tango::CallBack *cb_ptr);
	void dec_asynch_counter(asyn_req_type ty)
	{if (ty==POLLING)pasyn_ctr--;else if (ty==CALL_BACK)pasyn_cb_ctr--;}
		
	long command_inout_asynch(const char *,DeviceData &argin,bool forget=false);	
	long command_inout_asynch(string &,DeviceData &argin,bool forget=false);	
	long command_inout_asynch(const char *,bool forget=false);	
	long command_inout_asynch(string &,bool forget=false);

	DeviceData command_inout_reply(long);
	DeviceData command_inout_reply(long,long);
	
	void command_inout_asynch(const char *,DeviceData &argin,CallBack &cb);    
	void command_inout_asynch(string &,DeviceData &argin,CallBack &cb);	   
	void command_inout_asynch(const char *,CallBack &cb);	   
	void command_inout_asynch(string &,CallBack &cb);

	void get_asynch_replies();
	void get_asynch_replies(long);
};

//
// DeviceProxy - device proxy object for implementing generic high-level
//               interface for TANGO device objects
//

class DeviceProxy: public Tango::Connection
{
private :
	void real_constructor(string &);
	
	Tango::DbDevice *db_dev;
	string device_name;
	DeviceProxy *adm_device;

	void connect_to_adm_device();
	void polling_cmd_except(Tango::DevFailed *,string &,const char *);

	enum read_attr_type
	{
		SIMPLE,
		MULTIPLE
	};

	
	void read_attr_except(CORBA::Request_ptr,long,read_attr_type);
	void write_attr_except(CORBA::Request_ptr,long);
			
protected :
	virtual string get_corba_name();
	virtual string build_corba_name();

	enum polled_object
	{
		Cmd,
		Attr
	};
		
	bool is_polled(polled_object,string &, string &);

public :
	DeviceProxy(string &name, CORBA::ORB *orb=NULL);
	DeviceProxy(const char *, CORBA::ORB *orb=NULL);
	DeviceProxy(const DeviceProxy &);
	DeviceProxy & operator=(const DeviceProxy &);
	~DeviceProxy();

//
// general methods
//
	DeviceInfo info();
	inline string dev_name() { return device_name; }
	void parse_name(string &);
	
	string status();
	DevState state();
	string adm_name();
	string description();
	string name();
	
	int ping();
	vector<string> *black_box(int);	
//
// device methods
//
	CommandInfo command_query(string);
	CommandInfoList *command_list_query();
	
	DbDevImportInfo import_info();
//
// property methods
//
	void get_property(string&, DbData&); 
	void get_property(vector<string>&, DbData&);	
	void get_property(DbData&);	
	void put_property(DbData&);
	void delete_property(string&);
	void delete_property(vector<string>&);
	void delete_property(DbData&);	
//
// attribute methods
//
	vector<string> *get_attribute_list();
	
	AttributeInfoList *get_attribute_config(vector<string>&);
	AttributeInfo get_attribute_config(string &);
	
	AttributeInfo attribute_query(string name) {return get_attribute_config(name);}
	AttributeInfoList *attribute_list_query();
	
	void set_attribute_config(AttributeInfoList &);
	
	DeviceAttribute read_attribute(string&);
	DeviceAttribute read_attribute(const char *at) {string str(at);return read_attribute(str);}
	vector<DeviceAttribute> *read_attributes(vector<string>&);
	
	void write_attribute(DeviceAttribute&);
	void write_attributes(vector<DeviceAttribute>&);
//
// history methods
//
	vector<DeviceDataHistory> *command_history(string &,int);
	vector<DeviceDataHistory> *command_history(const char *na,int n)
	{string str(na);return command_history(str,n);}
	
	vector<DeviceAttributeHistory> *attribute_history(string &,int);
	vector<DeviceAttributeHistory> *attribute_history(const char *na,int n)
	{string str(na);return attribute_history(str,n);}
//
// Polling administration methods
//
	vector<string> *polling_status();
	
	void poll_command(string &, int);
	void poll_command(const char *na, int per) {string tmp(na);poll_command(na,per);}
	void poll_attribute(string &, int);
	void poll_attribute(const char *na, int per) {string tmp(na);poll_attribute(na,per);}
		
	int get_command_poll_period(string &);
	int get_command_poll_period(const char *na)
	{string tmp(na);return get_command_poll_period(tmp);}	
	int get_attribute_poll_period(string &);
	int get_attribute_poll_period(const char *na)
	{string tmp(na);return get_attribute_poll_period(tmp);}
	
	bool is_command_polled(string &);
	bool is_command_polled(const char *na) {string tmp(na);return is_command_polled(tmp);}
	bool is_attribute_polled(string &);
	bool is_attribute_polled(const char *na) {string tmp(na);return is_attribute_polled(tmp);}
	
	void stop_poll_command(string &);
	void stop_poll_command(const char *na) {string tmp(na);stop_poll_command(tmp);}
	void stop_poll_attribute(string &);
	void stop_poll_attribute(const char *na) {string tmp(na);stop_poll_attribute(tmp);}	
//
// Asynchronous methods
//
	long read_attribute_asynch(const char *na) {string tmp(na);return read_attribute_asynch(tmp);}	
	long read_attribute_asynch(string &att_name);	
	long read_attributes_asynch(vector <string> &);	

	vector<DeviceAttribute> *read_attributes_reply(long);
	vector<DeviceAttribute> *read_attributes_reply(long,long);
	DeviceAttribute *read_attribute_reply(long);
	DeviceAttribute *read_attribute_reply(long,long);
	
	long write_attribute_asynch(DeviceAttribute &);
	long write_attributes_asynch(vector<DeviceAttribute> &);
	
	void write_attributes_reply(long);
	void write_attributes_reply(long,long);
	void write_attribute_reply(long id) {write_attributes_reply(id);}
	void write_attribute_reply(long to,long id) {write_attributes_reply(to,id);}

	long pending_asynch_call(asyn_req_type req)
	{if (req == POLLING)return pasyn_ctr;
	 else if (req==CALL_BACK) return pasyn_cb_ctr;
	 else return (pasyn_ctr + pasyn_cb_ctr);}
	
	void read_attributes_asynch(vector<string> &,CallBack &);
	void read_attribute_asynch(const char *na,CallBack &cb) {string tmp(na);read_attribute_asynch(tmp,cb);}
	void read_attribute_asynch(string &,CallBack &);
	
	void write_attribute_asynch(DeviceAttribute &,CallBack &);
	void write_attributes_asynch(vector<DeviceAttribute> &,CallBack &);
//
// Logging administration methods
//
#ifdef TANGO_HAS_LOG4TANGO
  	void add_logging_target(const string &target_type, const string &target_name = "");
  	void add_logging_target(const char *target_type, const char *target_name = "")
	{add_logging_target(string(target_type), string(target_name));}
	
  	void remove_logging_target(const string &target_type, const string &target_name = "");
  	void remove_logging_target(const char *target_type, const char *target_name = "")
	{remove_logging_target(string(target_type), string(target_name));}
	
  	vector<string> get_logging_target (void);
  	int get_logging_level (void);
  	void set_logging_level (int level);
#endif // TANGO_HAS_LOG4TANGO
//
// Event methods
//
	int subscribe_event(const string &attr_name, EventType event, CallBack *, const vector<string> &filters);
	void unsubscribe_event(int event_id);
};

//
// AttributeProxy - attribute proxy object for implementing high-level
//                  interface for TANGO attributes
//

class AttributeProxy
{
private :
	void real_constructor(string &);
	string attr_name;
	string device_name;
	Tango::DeviceProxy *dev_proxy;
	Tango::DbAttribute *db_attr;
        bool    dbase_used;                     // Dev. with database
        bool    from_env_var;                   // DB from TANGO_HOST

        string  host;                           // DS host (if dbase_used=false)
        string  port;                           // DS port (if dbase_used=false)
        int     port_num;                       // DS port (as number)

        string  db_host;                        // DB host
        string  db_port;                        // DB port
        int     db_port_num;                    // DB port (as number)



	
public :
	AttributeProxy(string &name);
	AttributeProxy(const char *);
	AttributeProxy(const AttributeProxy &);
	AttributeProxy & operator=(const AttributeProxy &);
	~AttributeProxy();

//
// general methods
//
	inline string name() { return attr_name; }
	inline DeviceProxy* get_device_proxy() { return dev_proxy; }
	void parse_name(string &);
	
	string status();
	DevState state();
	string description();
	
	int ping();
//
// property methods
//
	void get_property(string&, DbData&); 
	void get_property(vector<string>&, DbData&);	
	void get_property(DbData&);	
	void put_property(DbData&);
	void delete_property(string&);
	void delete_property(vector<string>&);
	void delete_property(DbData&);	
//
// attribute methods
//
	AttributeInfo get_config();
	
	void set_config(AttributeInfo &);
	
	DeviceAttribute read();
	
	void write(DeviceAttribute&);
//
// history methods
//
	vector<DeviceAttributeHistory> *history(int);
//
// Polling administration methods
//
	void poll(int);
	int get_poll_period();
	bool is_polled();
	void stop_poll();
//
// Asynchronous methods
//
	long read_asynch();
	DeviceAttribute *read_reply(long);
	long write_asynch(DeviceAttribute &);
	void write_reply(long id);
	void write_reply(long to,long id);

	void read_asynch(CallBack &);
	void write_asynch(DeviceAttribute &,CallBack &);
//
// Event methods
//
	int subscribe_event(EventType event, CallBack *, const vector<string> &filters);
	void unsubscribe_event(int event_id);
};

//
// minor is also defined (Linux) in sysmacros.h. We want the compiler to
// use the SystemException::minor() method !!
//

#ifdef minor
#undef minor
#endif
	
/*
#ifdef WIN32 \
		if (timeout_changed == 1) \
			connection_state = CONNECTION_NOTOK; \
#endif \
*/
	
} // End of Tango namespace

#endif /* _DEVAPI_H */
