//=============================================================================
//
// file :               LockThread.h
//
// description :        Include for the LockThread object. This class implements
//                      the locking thread
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.1.2.1  2007/12/19 15:53:08  taurel
// - Still some work going on for the locking feature
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _LOCKTHREAD_H
#define _LOCKTHREAD_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The LockThCmd structure
//
// description :	This structure is used to shared data between the locking
//			thread and the main thread.
//
//=============================================================================

struct LockThCmd
{
	bool			cmd_pending;	// The new command flag
	LockCmdCode		cmd_code;		// The command code
	string			dev_name;		// The device name
	DevLong			lock_validity;	// The lock validity
	bool			suicide;		// The suicide flag
};

struct LockedDevice
{
	string			dev_name;		// The locked device name
	DevLong			validity;		// The locked device validity
	
	bool operator<(LockedDevice &arg) {return validity < arg.validity;}
};

enum LockCmdType
{
	LOCK_TIME_OUT = 0,
	LOCK_COMMAND,
};

//=============================================================================
//
//			The LockThread class
//
// description :	Class to store all the necessary information for the
//			locking thread. It's run() method is the thread code
//
//=============================================================================

class TangoMonitor;

class LockThread: public omni_thread
{
public:
	LockThread(LockThCmd &,TangoMonitor &,DeviceProxy *,string &,DevLong);
	
	void run(void *);
	
	void execute_cmd();
	void one_more_lock();
	void unlock_all_devs();
	void update_th_period();
	void compute_sleep_time(bool);
	LockCmdType get_command(DevLong);

protected:	
	LockThCmd				&shared_cmd;
	TangoMonitor			&p_mon;

	LockThCmd				local_cmd;
	DevLong					sleep;
	
	vector<LockedDevice>	locked_devices;
	vector<string>			re_lock_cmd_args;
	DevLong					period;
	DevLong 				period_ms;
	DeviceProxy				*admin_proxy;
	
	struct timeval			next_work;
};
	
} // End of Tango namespace

#endif /* _LOCKTHREAD_ */
