//=============================================================================
//
// file :               utils.h
//
// description :        Include for utility functions or classes  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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

#ifndef _UTILS_H
#define _UTILS_H

#include <tango.h>
#include <pollthread.h>
#include <pollext.h>
#include <subdev_diag.h>
#include <new>

#ifndef _TG_WINDOWS_
	#include <unistd.h>
#else
	#include <coutbuf.h>
	#include <w32win.h>
#endif /* _TG_WINDOWS_ */


//
// For debug purpose
//

#ifndef TANGO_HAS_LOG4TANGO

#define	cout1	if ((Tango::Util::_tracelevel >= 1) && \
		    (Tango::Util::_tracelevel < 5)) cout 
#define cout2  	if ((Tango::Util::_tracelevel >= 2) && \
		    (Tango::Util::_tracelevel < 5)) cout
#define cout3	if ((Tango::Util::_tracelevel >= 3) && \
		    (Tango::Util::_tracelevel < 5)) cout
#define cout4	if ((Tango::Util::_tracelevel >= 4) && \
		    (Tango::Util::_tracelevel < 5)) cout

#define cout5   if (Tango::Util::_tracelevel >= 5) cout

#endif //TANGO_HAS_LOG4TANGO


namespace Tango
{

class DeviceImpl;
class DeviceClass;
class DServer;
class AutoTangoMonitor;
class Util;
class EventSupplier;
class PyLock;
class CreatePyLock;
class DbServerCache;
class SubDevDiag;

struct PollingThreadInfo;
struct DevDbUpd;

#ifdef _TG_WINDOWS_
class CoutBuf;
class W32Win;
#endif


class PyLock
{
public:
	PyLock() {}
	virtual ~PyLock() {}
	
	virtual void Get() {}
	virtual void Release() {}
};

class CreatePyLock
{
public:
	CreatePyLock() {}
	virtual ~CreatePyLock() {}
	
	virtual PyLock *create() {return new PyLock();}
};

class Interceptors
{
public:
	Interceptors() {}
	virtual ~Interceptors() {}
	
	virtual void	create_thread() {}
	virtual	void	delete_thread() {}
};

//=============================================================================
//
//			The Util class
//
// description :	This class contains all properties and methods
//			which a device server process requires only once e.g. 
//			the orb and boa pointers....
//			This class is a singleton ( The constructor is 
//			protected and the _instance data member is static)
//
//			This class must be created at the beginning of each
//			device server process
//
//=============================================================================


class UtilExt
{
public:
	UtilExt():poll_mon("utils_poll"),ser_model(BY_DEVICE),only_one("process"),
	 	  py_interp(NULL),py_ds(false),py_dbg(false),db_cache(NULL),inter(NULL),
		  svr_starting(true),svr_stopping(false),db_svr_version(0),poll_pool_size(ULONG_MAX),
		  conf_needs_db_upd(false),ev_loop_func(NULL),shutdown_server(false),_dummy_thread(false)
	{shared_data.cmd_pending=false;shared_data.trigger=false;
	cr_py_lock = new CreatePyLock();}
	
	vector<string>				cmd_line_name_list;
	
	PollThread					*heartbeat_th;			// The heartbeat thread object
	int							heartbeat_th_id;		// The heartbeat thread identifier
	PollThCmd					shared_data;			// The shared buffer
	TangoMonitor				poll_mon;				// The monitor
	bool						poll_on;				// Polling on flag
	SerialModel					ser_model;				// The serialization model
	TangoMonitor				only_one;				// Serialization monitor
	EventSupplier				*event_supplier;		// The event supplier object

	void						*py_interp;				// The Python interpreter
	bool						py_ds;					// The Python DS flag
	CreatePyLock				*cr_py_lock;			// The python lock creator pointer
	bool						py_dbg;					// Badly written Python dbg flag
	
	DbServerCache				*db_cache;				// The db cache
	Interceptors				*inter;					// The user interceptors
	
	bool						svr_starting;			// Server is starting flag
	bool						svr_stopping;			// Server is shutting down flag
	
	vector<string>				polled_dyn_attr_names;	// Dynamic att. names (used for polling clean-up)
	vector<string>				polled_att_list;		// Full polled att list
	vector<string>				all_dyn_attr;			// All dynamic attr name list
	string						dyn_att_dev_name;		// Device name (use for dyn att clean-up)
	int							db_svr_version;			// Db server version;
	
	unsigned long				poll_pool_size;			// Polling threads pool size
	vector<string>  			poll_pool_conf;			// Polling threads pool conf.
	map<string,int>				dev_poll_th_map;		// Link between device name and polling thread id
	vector<PollingThreadInfo *>	poll_ths;				// Polling threads
	bool						conf_needs_db_upd;		// Polling conf needs to be udated in db
	
	bool 						(*ev_loop_func)(void);	// Ptr to user event loop
	bool						shutdown_server;		// Flag to exit the manual event loop
	
	SubDevDiag					sub_dev_diag;			// Object to handle sub device diagnostics
	bool						_dummy_thread;			// The main DS thread is not the process main thread

	string						svr_port_num;			// Server port when using file as database
};

/**
 * This class is a used to store TANGO device server process data and to provide
 * the user with a set of utilities method. This class is implemented using
 * the singleton design pattern. Therefore a device server process can have only
 * one instance of this class and its constructor is not public.
 *
 * $Author$
 * $Revision$
 */
 
class Util
{
	friend class Tango::AutoTangoMonitor;
	friend class Tango::ApiUtil;
public:

/**@name Singleton related methods
 * These methods follow the singleton design pattern (only one instance
 * of a class) */
//@{

/**
 * Create and get the singleton object reference.
 *
 * This method returns a reference to the object of the Util class.
 * If the class singleton object has not been created, it will be 
 * instanciated
 *
 * @param argc The process command line argument number
 * @param argv The process commandline arguments
 * @return The Util object reference
 */
	static Util *init(int argc,char *argv[]);
#ifdef _TG_WINDOWS_
/**
 * Create and get the singleton object reference.
 *
 * This method returns a reference to the object of the Util class.
 * If the class singleton object has not been created, it will be 
 * instanciated. This method must be used only for non-console mode windows 
 * device server
 *
 * @param AppInst The application instance
 * @param CmdShow The display window flag
 * @return The Util object reference
 */
        TANGO_IMP_EXP static Util *init(HINSTANCE AppInst,int CmdShow);
#endif
	
/**
 * Get the singleton object reference.
 *
 * This method returns a reference to the object of the Util class.
 * If the class has not been initialised with it's init method, this method
 * print a message and abort the device server process
 *
 * @return The Util object reference
 */
	TANGO_IMP_EXP static Util *instance(bool exit = true);
//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The class destructor.
 */
	~Util();
//@}

/**@name Get/Set instance data */
//@{
/**
 * Get a reference to the CORBA ORB
 *
 * This is a CORBA _duplicate of the original reference
 *
 * @return The CORBA ORB
 */		
	CORBA::ORB_ptr get_orb() {return CORBA::ORB::_duplicate(orb);}
	
/**
 * Get a reference to the CORBA Portable Object Adapter (POA)
 *
 * This is a CORBA _dupilcate of the original reference to the object POA.
 * For classical device server, thisis the root POA. For no database device
 * server, this is a specific POA with the USER_ID policy.
 *
 * @return The CORBA root POA
 */
	PortableServer::POA_ptr get_poa() {return PortableServer::POA::_duplicate(_poa);}

//
// process output control methods
//

/**
 * Set the process trace level.
 *
 * @param level The new process level
 */	
	void set_trace_level(int level) {_tracelevel = level;}
	
/**
 * Get the process trace level.
 *
 * @return The process trace level
 */
	int get_trace_level() {return _tracelevel;}

#ifndef TANGO_HAS_LOG4TANGO
/**
 * Set the process trace output .
 *
 * @param level The new process output
 */	
	void set_trace_output(string &trace) {trace_output = trace;}
	
/**
 * Get the process trace output.
 *
 * @return The process trace output
 */	
	string &get_trace_output() {return trace_output;}	

/**
 * Get the temporary process output print stream
 *
 * @return The process output print stream
 */	
	TangoSys_Cout &get_out() {return cout_tmp;}
	
/**
 * Set the process trace output stream.
 *
 * @param in The new process output stream
 */	
	void set_trace_output_stream(ofstream *in) {file_stream = in;}
	
/**
 * Get the process output print stream
 *
 * @return The process output print stream
 */
	ofstream *get_trace_output_stream() {return file_stream;}
#endif //TANGO_HAS_LOG4TANGO

/**
 * Get the device server instance name.
 *
 * @return The device server instance name
 */	
	string &get_ds_inst_name() {return ds_instance_name;}
	
/**
 * Get the device server executable name.
 *
 * @return The device server executable name
 */
	string &get_ds_exec_name() {return ds_exec_name;}
	
/**
 * Get the device server name.
 *
 * The device server name is the device server executable name/the device
 * server instance name
 * @return The device server name
 */
	string &get_ds_name() {return ds_name;}

/**
 * Get the host name where the device server process is running.
 *
 * @return The host name
 */	
	string &get_host_name() {return hostname;}
	
/**
 * Get the device server process identifier as a String
 *
 * @return The device server process identifier as a string
 */
	string &get_pid_str() {return pid_str;}
	
/**
 * Get the device server process identifier
 *
 * @return The device server process identifier
 */
	TangoSys_Pid get_pid() {return pid;}
	
/**
 * Get the TANGO library version number.
 *
 * @return The Tango library release number coded in 3 digits
 * (for instance 550,551,552,600,....)
 */	
	long get_tango_lib_release();
	
/**
 * Get the IDL TANGO version.
 *
 * @return The device server version
 */	
	string &get_version_str() {return version_str;}
	
/**
 * Get the device server version.
 *
 * @return The device server version
 */	
	string &get_server_version() {return server_version;}

/**
 * Set the device server version.
 *
 * @param vers The device server version
 */	
	void set_server_version(const char *vers) {server_version = vers;}
	
/**
 * Set the DeviceClass list pointer
 *
 * @param list The DeviceClass ptr vector address
 */
 	void set_class_list(vector<DeviceClass *> *list) {cl_list_ptr = list;cl_list = *list;}

/**
 * Add a DeviceClass to the DeviceClass list pointer
 *
 * @param cl The DeviceClass ptr
 */
 	void add_class_to_list(DeviceClass * cl) {cl_list.push_back(cl);}
		
/**
 * Get the DeviceClass list pointer
 *
 * @return The DeviceClass ptr vector address
 */	
	const vector<DeviceClass *> *get_class_list() {return &cl_list;}
	
/**
 * Set the serialization model
 *
 * @param ser The new serialization model. The serialization model must be one
 * of BY_DEVICE, BY_CLASS, BY_PROCESS or NO_SYNC
 */	
	void set_serial_model(SerialModel ser) {ext->ser_model = ser;}
	
/**
 * Get the serialization model
 *
 * @return The serialization model. This serialization model is one of
 * BY_DEVICE, BY_CLASS, BY_PROCESS or NO_SYNC
 */	
	SerialModel get_serial_model() {return ext->ser_model;}
/**
 * Get a reference to the TANGO EventSupplier object
 *
 * @return The EventSupplier object
 */
	EventSupplier *get_event_supplier() {return ext->event_supplier;}
//@}

/**@Miscellaneous methods */
//@{
/**
 * Check if the device server process is in its starting phase
 *
 * @return A boolean set to true if the server is in its starting phase.
 */
	bool is_svr_starting() {return ext->svr_starting;}
/**
 * Check if the device server process is in its shutting down sequence
 *
 * @return A boolean set to true if the server is in its shutting down phase.
 */
	bool is_svr_shutting_down() {return ext->svr_stopping;}
//@}
	

	void set_interceptors(Interceptors *in) {ext->inter = in;}
	Interceptors *get_interceptors() {return ext->inter;}
	
	vector<string> &get_cmd_line_name_list() {return ext->cmd_line_name_list;}
	TangoMonitor &get_heartbeat_monitor() {return ext->poll_mon;}
	PollThCmd &get_heartbeat_shared_cmd() {return ext->shared_data;}
	bool poll_status() {return ext->poll_on;}
	void poll_status(bool status) {ext->poll_on = status;}

//
// Some methods are duplicated here (with different names). It is for compatibility reason
//
	
	void polling_configure();
	PollThread *get_polling_thread_object() {return ext->heartbeat_th;}
	PollThread *get_heartbeat_thread_object() {return ext->heartbeat_th;}
	void clr_poll_th_ptr() {ext->heartbeat_th = NULL;}
	void clr_heartbeat_th_ptr() {ext->heartbeat_th = NULL;}
	int get_polling_thread_id() {return ext->heartbeat_th_id;}
	int get_heartbeat_thread_id() {return ext->heartbeat_th_id;}
	void stop_heartbeat_thread();
	string &get_svr_port_num() {return ext->svr_port_num;}
	
	void create_event_supplier();
	
	void *get_py_interp() {return ext->py_interp;}
	void set_py_interp(void *ptr) {ext->py_interp = ptr;}

	bool is_py_ds() {return ext->py_ds;}
	void set_py_ds() {ext->py_ds=true;}
	
	bool is_py_dbg() {return ext->py_dbg;}
	void set_py_dbg() {ext->py_dbg=true;}

	void set_py_lock_creator(CreatePyLock *py) {ext->cr_py_lock = py;}
	CreatePyLock *get_py_lock_creator() {return ext->cr_py_lock;}
	
	DbServerCache *get_db_cache() {return ext->db_cache;}
	void unvalidate_db_cache() {if (ext->db_cache!=NULL){delete ext->db_cache;ext->db_cache = NULL;}}
	
	void set_svr_starting(bool val) {ext->svr_starting = val;}
	void set_svr_shutting_down(bool val) {ext->svr_stopping = val;}
	
	vector<string> &get_polled_dyn_attr_names() {return ext->polled_dyn_attr_names;}
	vector<string> &get_full_polled_att_list() {return ext->polled_att_list;}
	string &get_dyn_att_dev_name() {return ext->dyn_att_dev_name;}
	vector<string> &get_all_dyn_attr_names() {return ext->all_dyn_attr;}
	
	int get_db_svr_version() {return ext->db_svr_version;}
	void set_db_svr_version();
	
	void clean_attr_polled_prop();
	void clean_dyn_attr_prop();
	
	int create_poll_thread(const char *dev_name,bool startup,int smallest_upd = -1);
	void stop_all_polling_threads();
	vector<PollingThreadInfo *> &get_polling_threads_info() {return ext->poll_ths;}
	PollingThreadInfo *get_polling_thread_info_by_id(int);
	int get_polling_thread_id_by_name(const char *);
	void check_pool_conf(DServer *,unsigned long);
	int check_dev_poll(vector<string> &,vector<string> &,DeviceImpl *);
	void split_string(string &,char,vector<string> &);
	void upd_polling_prop(vector<DevDbUpd> &,DServer *);
	int get_th_polled_devs(string &,vector<string> &);
	void get_th_polled_devs(long,vector<string> &);
	void build_first_pool_conf();
	bool is_dev_already_in_pool_conf(string &,vector<string>&,int);
	vector<string> &get_poll_pool_conf() {return ext->poll_pool_conf;}
	int get_dev_entry_in_pool_conf(string &);
	void remove_dev_from_polling_map(string &dev_name);
	void remove_polling_thread_info_by_id(int);
	
	bool is_server_event_loop_set() {if (ext->ev_loop_func != NULL)return true;else return false;}
	void set_shutdown_server(bool val) {ext->shutdown_server = val;}
	
	void shutdown_server();
	
	SubDevDiag &get_sub_dev_diag() {return ext->sub_dev_diag;}
	
/**@name Database related methods */
//@{
/**
 * Connect the process to the TANGO database.
 *
 * If the connection to the database failed, a message is displayed on the
 * screen and the process is aborted
 */
	void connect_db();

/**
 * Reread the file database
 */ 
	void reset_filedatabase();
		
/**
 * Get a reference to the TANGO database object
 *
 * @return The database object
 */
	Database *get_database() {return db;}
		
/**
 * Unregister a device server process from the TANGO database.
 *
 * If the database call fails, a message is displayed on the screen and the
 * process is aborted
 */	
	void unregister_server();
//@}

/** @name Device reference related methods */
//@{
/**
 * Get the list of device references for a given TANGO class.
 *
 * Return the list of references for all devices served by one implementation
 * of the TANGO device pattern implemented in the  process
 *
 * @param class_name The TANGO device class name
 * @return The device reference list
 * @exception DevFailed If in the device server process there is no TANGO 
 * device pattern implemented the TANGO device class given as parameter
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	vector<DeviceImpl *> &get_device_list_by_class(const string &class_name);
	
/**
 * Get the list of device references for a given TANGO class.
 *
 * Return the list of references for all devices served by one implementation
 * of the TANGO device pattern implemented in the  process
 *
 * @param class_name The TANGO device class name
 * @return The device reference list
 * @exception DevFailed If in the device server process there is no TANGO 
 * device pattern implemented the TANGO device class given as parameter
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	vector<DeviceImpl *> &get_device_list_by_class(const char *class_name);
	
/**
 * Get a device reference from its name
 *
 * @param dev_name The TANGO device name
 * @return The device reference
 * @exception DevFailed If in the device is not served by one device pattern
 * implemented in this process.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	DeviceImpl *get_device_by_name(const string &dev_name);
	
/**
 * Get a device reference from its name
 *
 * @param dev_name The TANGO device name
 * @return The device reference
 * @exception DevFailed If in the device is not served by one device pattern
 * implemented in this process.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	DeviceImpl *get_device_by_name(const char *dev_name);
	
/**
 * Get a reference to the dserver device attached to the device server process
 *
 * @return A reference to the dserver device
 */
	DServer *get_dserver_device();
	
/**
 * Get DeviceList from name.
 *
 * It is possible to use a wild card ('*') in the name parameter
 *  (e.g. "*", "/tango/tangotest/n*", ...)
 *
 * @param name The device name
 * @return The DeviceClass ptr vector address
 */	
	vector<DeviceImpl *> get_device_list(const string &name);
//@}

/** @name Device pattern related methods */
//@{
/**
 * Initialise all the device server pattern(s) embedded in a device server
 * process.
 *
 * @exception DevFailed If the device pattern initialistaion failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void server_init(bool with_window = false);
	
/**
 * Run the CORBA event loop
 *
 * This method runs the CORBA event loop. For UNIX or Linux operating system,
 * this method does not return. For Windows in a non-console mode, 
 * this method start a thread which enter the CORBA event loop.
 */
 	void server_run();
	
/**
 * Cleanup a Tango device server process before exit
 *
 * This method cleanup a Tango device server and relinquish all computer
 * resources before the process exit
 */
 	void server_cleanup();
 	
/**
 * Set the server event loop
 *
 * This method registers an event loop function in a Tango server.
 * This function will be called by the process main thread in an infinite loop
 * The process will not use the classical ORB blocking event loop. 
 * It is the user responsability to code this function in a way that it implements
 * some kind of blocking in order not to load the computer CPU
 * 
 * @param f_ptr The event loop function pointer. This function will not receive
 * any argument. It returns a boolean value. If this boolean is set to true,
 * the device server process exit.
 */
 	void server_set_event_loop(bool (*f_ptr)()) {ext->ev_loop_func = f_ptr;}
//@}


/**@name Class data members */
//@{
/**
 * The process trace level
 */	
	static int		_tracelevel;
/**
 * The database use flag (Use with extreme care). Implemented for device
 * server started without database usage.
 */
	TANGO_IMP static bool	_UseDb;

	TANGO_IMP static bool	_FileDb;
/**
 * A daemon process flag. If this flag is set to true, the server
 * process will not exit if it not able to connect to the database.
 * Instead, it will loop until the connection suceeds. The default
 * value is false.
 */
 	TANGO_IMP static bool	_daemon;
/**
 * The loop sleeping time in case of the _daemon flag set to true.
 * This sleeping time is the number of seconds the process will
 * sleep before it tries again to connect to the database. The default
 * value is 60 seconds.
 */
 	TANGO_IMP static long	_sleep_between_connect;

	
//@}	

#ifdef _TG_WINDOWS_
/**@name Windows specific methods */
//@{
/**
 * Get the console window instance
 *
 * @return The device server graphical interface console window instance
 */
 
	HWND			get_console_window();
	
/**
 * Get the main window instance
 *
 * @return The device server graphical interface main window instance
 */
	HWND			get_ds_main_window();

/**
 * Get a pointer to the debug object
 *
 * @return A pointer to the debug object
 */
	CoutBuf			*get_debug_object();

	TANGO_IMP static bool	_service;
/**
 * Get the text displayed on main server window.
 *
 * @return The text to be displayed
 */	
	string &get_main_window_text() {return main_win_text;}

/**
 * Set the text displayed on main server window.
 *
 * @param 	txt 	The new text to be displayed at the bottom of the
 * main window
 */	
	void set_main_window_text(string &txt) {main_win_text = txt;}
//@}
#endif

protected:
/**
 * Constructs a newly allocated Util object.
 *
 * This constructor is protected following the singleton pattern
 *
 * @param argc The process command line argument number
 * @param argv The process commandline arguments
 *
 */
	Util(int argc,char *argv[]);
#ifdef _TG_WINDOWS_
/**
 * Constructs a newly allocated Util object for Windows non-console
 * device server.
 *
 * This constructor is protected following the singleton pattern
 *
 * @param AppInst The applicationinstance
 * @param CmdShow The display window flag
 *
 */
	Util(HINSTANCE AppInst,int CmdShow);
#endif
		
private:
	TANGO_IMP static Util	*_instance;
	static bool				_constructed;
#ifdef _TG_WINDOWS_
	static bool				_win;
	int						argc;
	char					**argv;
	int						nCmd;
	CoutBuf					*pcb;
	W32Win					*ds_window;
	string					main_win_text;
	bool					go;
	TangoMonitor			mon;

	void build_argc_argv();
	void install_cons_handler();
	
	class ORBWin32Loop: public omni_thread	
	{
		Util *util;
	public:
		ORBWin32Loop(Util *u):util(u) {}
		virtual ~ORBWin32Loop() {}
		
		void *run_undetached(void *);
		void start() {start_undetached();}
	private:
		void wait_for_go();
	};
	friend class ORBWin32Loop;
	ORBWin32Loop *loop_th;
#endif

	CORBA::ORB_var 			orb;
	PortableServer::POA_var _poa;
	
	string					ds_instance_name;	// The instance name
	string					ds_exec_name;		// The server exec. name
	string					ds_name;			// The server name
	
	string					hostname;			// The host name
	string					pid_str;			// The process PID (as string)
	TangoSys_Pid			pid;				// The process PID

	string					version_str;		// Tango version
	string					server_version;		// Device server version

	string      			database_file_name;

#ifndef TANGO_HAS_LOG4TANGO	
	string			trace_output;
	TangoSys_Cout		cout_tmp;
	ofstream		*file_stream;
#endif //TANGO_HAS_LOG4TANGO

	Database				*db;			// The db proxy
	
	void effective_job(int,char *[]);
	void create_CORBA_objects();
	void misc_init();
	void init_host_name();
	void server_already_running();	
	void print_usage(char *);
	static void print_err_message(const char *,Tango::MessBoxType type = Tango::STOP);
	void print_err_message(const string &mess,Tango::MessBoxType type = Tango::STOP)
	{
		Util::print_err_message(mess.c_str(),type);
	}
	void check_args(int, char *[]);
	void display_help_message();
	DeviceImpl *find_device_name_core(string &);

	bool  							display_help;	// display help message flag
	const vector<DeviceClass *>		*cl_list_ptr;	// Ptr to server device class list	
	UtilExt							*ext;			// Class extension
	vector<DeviceClass *>			cl_list;		// Full class list ptr

public:

/** @name Polling related methods */
//@{
/**
 * Trigger polling for polled command.
 *
 * This method send the order to the polling thread to poll one object
 * registered with an update period defined as "externally triggerred"  
 *
 * @param dev The TANGO device
 * @param name The command name which must be polled
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void trigger_cmd_polling(DeviceImpl *dev,const string &name);
	
/**
 * Trigger polling for polled attribute.
 *
 * This method send the order to the polling thread to poll one object
 * registered with an update period defined as "externally triggerred"  
 *
 * @param dev The TANGO device
 * @param name The attribute name which must be polled
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void trigger_attr_polling(DeviceImpl *dev,const string &name);

/**
 * Fill polling buffer for polled attribute.
 *
 * This method fills the polling buffer for one polled attribute
 * registered with an update period defined as "externally triggerred" 
 * (polling period set to 0) 
 *
 * @param dev The TANGO device
 * @param att_name The attribute name which must be polled
 * @param data The data stack with one element for each history element
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	template <typename T>
	void fill_attr_polling_buffer(DeviceImpl *dev,
				      string &att_name,
				      AttrHistoryStack<T>  &data)
	{
//
// Check that the device is polled
//

		if (dev->is_polled() == false)
		{
			TangoSys_OMemStream o;
			o << "Device " << dev->get_name() << " is not polled" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}
	
//
// Attribute name in lower case letters and check that it is marked as polled
//

		string obj_name(att_name);
		transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
		
		dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);
	
//
// Get a reference on the Attribute object
//

		Tango::Attribute &att = dev->get_device_attr()->get_attr_by_name(att_name.c_str());
		Tango::WAttribute *w_att_ptr = NULL;
		Tango::AttrWriteType w_type = att.get_writable();
		if (w_type == Tango::READ_WRITE)
			w_att_ptr = &(dev->get_device_attr()->get_w_attr_by_name(att_name.c_str()));

//
// Check that it is not a WRITE only attribute
//

		if (w_type == Tango::WRITE)
		{
			TangoSys_OMemStream o;
			o << "Attribute " << att_name;
			o << " of device " << dev->get_name() << " is WRITE only" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}

//
// If the attribute is not READ_WRITE, the ptr to written part should always be NULL
//

		unsigned long nb_elt = data.length();
		if (w_type != READ_WRITE)
		{
			for (int i = 0;i < nb_elt;i++)
			{
				if ((data.get_data())[i].wr_ptr != NULL)
				{
					TangoSys_OMemStream o;
					o << "The attribute " << att_name;
					o << " for device " << dev->get_name();
					o << " is not a READ_WRITE attribute. You can't set the attribute written part.";
					o << "It is defined for record number " << i + 1 << ends;

					Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
				   							(const char *)"Util::fill_attr_polling_buffer");
				}	
			}
		}

//
// For device implementing IDL 3, DevEncoded data type is not supported
//
		
		long idl_vers = dev->get_dev_idl_version();
		if ((att.get_data_type() == DEV_ENCODED) && (idl_vers == 3))
		{
			TangoSys_OMemStream o;
			o << "The attribute " << att_name;
			o << " for device " << dev->get_name();
			o << " is of type DEV_ENCODED. Your device supports only IDL V3.";
			o << " DEV_ENCODED data type is supported starting with IDL V4" << ends;

			Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");			
		}

//
// DevEncoded data type is not supported for spectrum or image attribute
// Paranoid code? This case should never happens!
//

		if ((att.get_data_type() == DEV_ENCODED) && 
			(att.get_data_format() != Tango::SCALAR) &&
			(w_type == Tango::READ_WRITE))
		{
			for (int i = 0;i < nb_elt;i++)
			{
				if ((data.get_data())[i].wr_ptr != NULL)
				{
					TangoSys_OMemStream o;
					o << "The attribute " << att_name;
					o << " for device " << dev->get_name();
					o << " is of type DEV_ENCODED. Only Scalar attribute are supported for DEV_ENCODED";
					o << "It is defined for record number " << i + 1 << ends;

					Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
				   							(const char *)"Util::fill_attr_polling_buffer");
				}	
			}
		}

//
// Check that the device IDL is at least 3
//
		
		if (idl_vers <= 2)
		{
			TangoSys_OMemStream o;
			o << "The device " << dev->get_name() << " is too old to support this feature. ";
			o << "Please update your device to IDL 3 or more" << ends;

			Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");	
		}			
		
//
// Check that history is not larger than polling buffer
//

		unsigned long nb_poll = dev->get_attr_poll_ring_depth(att_name);

		if (nb_elt > nb_poll)
		{
			TangoSys_OMemStream o;
			o << "The polling buffer depth for attribute " << att_name;
			o << " for device " << dev->get_name();
			o << " is only " << nb_poll;
			o << " which is less than " << nb_elt << "!" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}
	
//
// A loop on each record
//
	
		unsigned long i;	
		Tango::DevFailed *save_except;
		Tango::AttributeValueList_3 *back_3;
		Tango::AttributeValueList_4 *back_4;
		bool attr_failed;
		
		struct timeval zero,when;
		zero.tv_sec = zero.tv_usec = 0;

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release
// the monitor in the loop
//
		
		dev->get_poll_monitor().get_monitor();
		
//
// The loop for each element
//
		
		for (i = 0;i < nb_elt;i++)
		{
			save_except = NULL;
			back_3 = NULL;
			back_4 = NULL;
			attr_failed = false;

			if ((data.get_data())[i].err.length() != 0)
			{
				attr_failed = true;
				try
				{
					save_except = new Tango::DevFailed((data.get_data())[i].err);
				}
				catch (bad_alloc)
				{
					dev->get_poll_monitor().rel_monitor();					
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_attr_polling_buffer");
				}		
			}
			else
			{
			
//
// Allocate memory for the AttributeValueList sequence
//

				try
				{
					if (idl_vers == 4)
					{
						back_4 = new Tango::AttributeValueList_4(1);
						back_4->length(1);
						(*back_4)[0].value.union_no_data(true);
					}
					else
					{
						back_3 = new Tango::AttributeValueList_3(1);
						back_3->length(1);
					}
				}
				catch (bad_alloc)
				{
					dev->get_poll_monitor().rel_monitor();
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_attr_polling_buffer");
				}

//
// Init name,date and quality factor
//

				Tango::AttrQuality qu = (data.get_data())[i].qual;
				if (idl_vers==4)
				{
					(*back_4)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
					(*back_4)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
					(*back_4)[0].time.tv_nsec = 0;

					(*back_4)[0].quality = qu;
					(*back_4)[0].name = CORBA::string_dup(att_name.c_str());

					(*back_4)[0].w_dim.dim_x = 0;
					(*back_4)[0].w_dim.dim_y = 0;
					(*back_4)[0].r_dim.dim_x = 0;
					(*back_4)[0].r_dim.dim_y = 0;
					
					(*back_4)[0].data_format = att.get_data_format();
				}
				else
				{
					(*back_3)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
					(*back_3)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
					(*back_3)[0].time.tv_nsec = 0;

					(*back_3)[0].quality = qu;
					(*back_3)[0].name = CORBA::string_dup(att_name.c_str());

					(*back_3)[0].w_dim.dim_x = 0;
					(*back_3)[0].w_dim.dim_y = 0;
					(*back_3)[0].r_dim.dim_x = 0;
					(*back_3)[0].r_dim.dim_y = 0;
				}
				if ((qu == Tango::ATTR_VALID) || 
				    (qu == Tango::ATTR_ALARM) ||
				    (qu == Tango::ATTR_WARNING) ||  
				    (qu == Tango::ATTR_CHANGING))
				{
				
//
// Set Attribute object value
//

					att.set_value((T *)(data.get_data())[i].ptr,
						      (data.get_data())[i].x,
						      (data.get_data())[i].y,
						      (data.get_data())[i].release);
					att.set_date((data.get_data())[i].t_val);			      
					att.set_quality(qu,false);
					      
//
// Init remaining fields in AttributeValueList
//

					if (w_type == Tango::READ_WITH_WRITE)
						dev->get_device_attr()->add_write_value(att);
					else if (w_type == Tango::READ_WRITE)
					{
						if ((data.get_data())[i].wr_ptr != NULL)
						{
							w_att_ptr->set_write_value((T *)(data.get_data())[i].wr_ptr,
													(data.get_data())[i].wr_x,
													(data.get_data())[i].wr_y);
							dev->get_device_attr()->add_write_value(att);
				
							if ((data.get_data())[i].release == true)
							{
								if (att.get_data_format() == Tango::SCALAR)
									delete (data.get_data())[i].wr_ptr;
								else
									delete [] (data.get_data())[i].wr_ptr;
							}
						}
						else
						{
							dev->get_device_attr()->add_write_value(att);
						}
					}
						
//
// Insert data into the AttributeValue object
//

					dev->data_into_net_object(att,back_3,back_4,0,w_type,true);
					
//
// Init remaining fields
//

					if (idl_vers == 4)
					{
						(*back_4)[0].r_dim.dim_x = (data.get_data())[i].x;
						(*back_4)[0].r_dim.dim_y = (data.get_data())[i].y;
							
						if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
						{
							WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
							(*back_4)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
							(*back_4)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
						}
					}
					else
					{
						(*back_3)[0].r_dim.dim_x = (data.get_data())[i].x;
						(*back_3)[0].r_dim.dim_y = (data.get_data())[i].y;

						if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
						{
							WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
							(*back_3)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
							(*back_3)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
						}
					}
				}
			}

//
// Fill one slot of polling buffer
//

			try
			{
				vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

				if (attr_failed == false)
				{
					if (idl_vers == 4)
					{
						when.tv_sec  = (*back_4)[0].time.tv_sec - DELTA_T;
						when.tv_usec = (*back_4)[0].time.tv_usec;
						(*ite)->insert_data(back_4,when,zero);
					}
					else
					{
						when.tv_sec  = (*back_3)[0].time.tv_sec - DELTA_T;
						when.tv_usec = (*back_3)[0].time.tv_usec;
						(*ite)->insert_data(back_3,when,zero);
					}
				}
				else
				{
					when = (data.get_data())[i].t_val;
					when.tv_sec = when.tv_sec - DELTA_T;
					(*ite)->insert_except(save_except,when,zero);
				}
			}
			catch (Tango::DevFailed &)
			{
				if (attr_failed == false)
					if (idl_vers == 4)
						delete back_4;
				else
						delete back_3;
				else
					delete save_except;
			}

		}
		
		dev->get_poll_monitor().rel_monitor();
	}

/**
 * Fill polling buffer for polled command.
 *
 * This method fills the polling buffer for one polled command
 * registered with an update period defined as "externally triggerred" 
 * (polling period set to 0) 
 *
 * @param dev The TANGO device
 * @param cmd_name The command name which must be polled
 * @param data The data stack with one element for each history element
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	template <typename T>
	void fill_cmd_polling_buffer(DeviceImpl *dev,
				     string &cmd_name,
				     CmdHistoryStack<T>  &data)
	{

//
// Check that the device is polled
//

		if (dev->is_polled() == false)
		{
			TangoSys_OMemStream o;
			o << "Device " << dev->get_name() << " is not polled" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_cmd_polling_buffer");
		}
	
//
// Command name in lower case letters and check that it is marked as polled
//

		string obj_name(cmd_name);
		transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
		
		dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
			
//
// Check that history is not larger than polling buffer
//

		long nb_elt = data.length();
		long nb_poll = dev->get_cmd_poll_ring_depth(cmd_name);

		if (nb_elt > nb_poll)
		{
			TangoSys_OMemStream o;
			o << "The polling buffer depth for command " << cmd_name;
			o << " for device " << dev->get_name();
			o << " is only " << nb_poll;
			o << " which is less than " << nb_elt << "!" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_cmd_polling_buffer");
		}

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release
// the monitor in the loop
//
				
		dev->get_poll_monitor().get_monitor();
				
//
// A loop on each record
//
	
		long i;	
		Tango::DevFailed *save_except;
		bool cmd_failed;
		CORBA::Any *any_ptr;
		
		struct timeval zero,when;
		zero.tv_sec = zero.tv_usec = 0;

		for (i = 0;i < nb_elt;i++)
		{
			save_except = NULL;
			cmd_failed = false;

			if ((data.get_data())[i].err.length() != 0)
			{
				cmd_failed = true;
				try
				{
					save_except = new Tango::DevFailed((data.get_data())[i].err);
				}
				catch (bad_alloc)
				{
					dev->get_poll_monitor().rel_monitor();
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_cmd_polling_buffer");
				}		
			}
			else
			{
			
//
// Allocate memory for the Any object
//

				try
				{
					any_ptr = new CORBA::Any();
				}
				catch (bad_alloc)
				{
					dev->get_poll_monitor().rel_monitor();
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_cmd_polling_buffer");
				}
				
//
// Set command value in Any object
// If the Any insertion is by pointer, with omniORB, it copy data into the Any
// and delete memory. Therefore, no need to delete memory if release is
// true. If release is false, uses the insertion by reference which only
// copy data.
//

				T *tmp_ptr = (data.get_data())[i].ptr;
				if ((data.get_data())[i].release == true)
					(*any_ptr) <<= tmp_ptr;
				else
					(*any_ptr) <<= (*tmp_ptr);
					      
			}

//
// Fill one slot of polling buffer
//

			try
			{
				vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
				when.tv_sec = (data.get_data())[i].t_val.tv_sec - DELTA_T;
				when.tv_usec = (data.get_data())[i].t_val.tv_usec;
				if (cmd_failed == false)
					(*ite)->insert_data(any_ptr,when,zero);
				else
					(*ite)->insert_except(save_except,when,zero);
			}
			catch (Tango::DevFailed &)
			{
				if (cmd_failed == false)
					delete any_ptr;
				else
					delete save_except;
			}

		}
		
		dev->get_poll_monitor().rel_monitor();
	}
	
/**
 * Set the polling threads pool size
 *
 * @param thread_nb The maximun number of threads in the polling threads pool
 */	
	void set_polling_threads_pool_size(long thread_nb) {ext->poll_pool_size = thread_nb;}
	
/**
 * Get the polling threads pool size
 *
 * @return The maximun number of threads in the polling threads pool
 */	
	long get_polling_threads_pool_size() {return ext->poll_pool_size;}
//@}

};

//+-------------------------------------------------------------------------
//
// function : 		return_empty_any
// 
// description : 	Return from a command when the command does not have
//			any output argument
//
// arguments : 		in : - cmd : The command name
//
//--------------------------------------------------------------------------

/**
 * Create and return an empty CORBA Any object.
 *
 * Create an empty CORBA Any object. Could be used by command which does
 * not return anything to the client. This method also prints a message on
 * screen (level 4) before it returns
 *
 * @param cmd The cmd name which use this empty Any. Only used to create the
 * thrown exception (in case of) and in the displayed message
 * @return The empty CORBA Any
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
inline CORBA::Any *return_empty_any(const char *cmd)
{

	CORBA::Any *out_any;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		TangoSys_MemStream o;
		
		o << cmd << "::execute";
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		//cout3 << "Bad allocation while in " << cmd << "::execute()" << endl;
#endif
		Tango::Except::throw_exception((const char *)"API_MemoryAllocation",
					     (const char *)"Can't allocate memory in server",
					     o.str());
	}
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	//cout4 << "Leaving " << cmd << "::execute()" << endl;	
#endif
	return(out_any);
	
}

inline DbDevice *DeviceImpl::get_db_device()
{
	if (Tango::Util::_UseDb == false)
	{
		TangoSys_OMemStream desc_mess;
		desc_mess << "Method not available for device ";
		desc_mess << device_name;
		desc_mess << " which is a non database device";
		
		Except::throw_exception((const char *)"API_NonDatabaseDevice",
					desc_mess.str(),
					(const char *)"DeviceImpl::get_db_device");
	}
	
	return db_dev;
}

void clear_att_dim(Tango::AttributeValue_3 &att_val);
void clear_att_dim(Tango::AttributeValue_4 &att_val);

//-----------------------------------------------------------------------
//
//			Polling threads pool related class/struct
//
//-----------------------------------------------------------------------

struct PollingThreadInfo
{
	int									thread_id;			// The polling thread identifier
	PollThread							*poll_th;			// The polling thread object
	PollThCmd							shared_data;		// The shared buffer
	TangoMonitor						poll_mon;			// The monitor
	vector<string>						polled_devices;		// Polled devices for this thread
	int									nb_polled_objects;	// Polled objects number in this thread
	int 								smallest_upd;		// Smallest thread update period
	vector<DevVarLongStringArray *> 	v_poll_cmd;			// Command(s) to send 
	
	PollingThreadInfo():poll_mon("Polling_thread_mon"),nb_polled_objects(0),smallest_upd(0)
	{shared_data.cmd_pending = false;shared_data.trigger=false;}
};

struct DevDbUpd
{
	unsigned long 	class_ind;
	unsigned long	dev_ind;
	int				mod_prop;
};

//------------------------------------------------------------------------
//
//			Python device server classes
//
//-----------------------------------------------------------------------


//
// For thread creation interceptor (Python device servers)
//

void create_PyPerThData(omni::omniInterceptors::createThread_T::info_T &);


class PyData: public omni_thread::value_t
{
public:
	PyData():rec_state(false),rec_status(false)
	{
		device_name = "No associated device name!";
		
		try
		{
			Util *tg = Util::instance(false);
			CreatePyLock *Creator = tg->get_py_lock_creator();
			PerTh_py_lock = Creator->create();
		}
		catch(Tango::DevFailed &) {PerTh_py_lock=NULL;}
	}
	
	~PyData()
	{
		if (PerTh_py_lock != NULL)
			delete PerTh_py_lock;
	}

	DevVarCharArray				PerTh_dvca;
	DevVarShortArray			PerTh_dvsha;
	DevVarLongArray				PerTh_dvla;
	DevVarFloatArray			PerTh_dvfa;
	DevVarDoubleArray			PerTh_dvda;
	DevVarUShortArray			PerTh_dvusa;
	DevVarULongArray			PerTh_dvula;
	DevVarStringArray 			PerTh_dvsa;
	DevVarLongStringArray		PerTh_dvlsa;
	DevVarDoubleStringArray		PerTh_dvdsa;
	DevVarLong64Array			PerTh_dvl64a;
	DevVarULong64Array			PerTh_dvul64a;
	DevVarEncodedArray			PerTh_dvea;
	
	string						PerTh_string;
	DevFailed					PerTh_df;
	vector<string>				PerTh_vec_str;
	vector<double>				PerTh_vec_db;
	DevErrorList				PerTh_del;
	
	bool						rec_state;
	bool						rec_status;
	
	// name of the associated device to a thread
	// used to sub device referencing
	string 						device_name;
	
	PyLock						*PerTh_py_lock;
};


class AutoPyLock
{
public:
	AutoPyLock();
	~AutoPyLock();
};

long _convert_tango_lib_release(const char *vers_str);

} // End of Tango namespace
		     
#endif /* UTILS */
