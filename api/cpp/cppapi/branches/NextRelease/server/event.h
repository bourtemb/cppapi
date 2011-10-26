////////////////////////////////////////////////////////////////////////////////
///
///  file       event.h
///
/// 	        C++ include file for implementing the TANGO event server and
///		client singleton classes - EventSupplier and EventConsumer.
///             These classes are used to send events from the server
///             to the notification service and to receive events from
///             the notification service.
///
/// 		author(s) : A.Gotz (goetz@esrf.fr)
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011
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
///
/// 		original : 7 April 2003
///
/// 		$Revision$
///
/// 		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _EVENTAPI_H
#define _EVENTAPI_H

#include <attribute.h>
#include <except.h>
#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>
#include <omnithread.h>
#include <map>

#include <readers_writers_lock.h>

#include <zmq.hpp>


namespace Tango
{

#ifndef _USRDLL
extern "C"
{
#endif
void leavefunc();
void client_leavefunc();
#ifndef _USRDLL
}
#endif

#define 	CONF_TYPE_EVENT			"attr_conf"
#define		DATA_READY_TYPE_EVENT	"data_ready"
#define     ALL_EVENTS				0

/********************************************************************************
 * 																				*
 * 						EventData class											*
 * 																				*
 *******************************************************************************/

class EventData
{
public :
	EventData(DeviceProxy *dev,string &nam,string &evt,Tango::DeviceAttribute *attr_value_in,
	DevErrorList &errors_in);
	~EventData();
	EventData(const EventData &);
	EventData & operator=(const EventData &);

	DeviceProxy *device;
	string attr_name;
	string event;
	DeviceAttribute *attr_value;
	bool err;
	DevErrorList errors;
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}
private:
	void set_time();
};


/********************************************************************************
 * 																				*
 * 						EventDataList class										*
 * 																				*
 *******************************************************************************/
class EventDataList:public vector<EventData *>
{
public:
	EventDataList(): vector<EventData *>(0) {};
	~EventDataList()
		{
		if (size() > 0)
			{
			EventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}
			}
		}
	void clear()
		{
		if (size() > 0)
			{
			EventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}

			this->vector<EventData *>::clear();
			}
		}
};


/********************************************************************************
 * 																				*
 * 						AttrConfEventData class									*
 * 																				*
 *******************************************************************************/

class AttrConfEventData
{
public :
	AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
	                  Tango::AttributeInfoEx *attr_conf_in,
	                  DevErrorList &errors_in);
	~AttrConfEventData();
	AttrConfEventData(const AttrConfEventData &);
	AttrConfEventData & operator=(const AttrConfEventData &);

	DeviceProxy *device;
	string attr_name;
	string event;
	AttributeInfoEx *attr_conf;
	bool err;
	DevErrorList errors;
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}
private:
	void set_time();
};



/********************************************************************************
 * 																				*
 * 						AttrConfEventDataList class								*
 * 																				*
 *******************************************************************************/
class AttrConfEventDataList:public vector<AttrConfEventData *>
{
public:
	AttrConfEventDataList(): vector<AttrConfEventData *>(0) {};
	~AttrConfEventDataList()
		{
		if (size() > 0)
			{
			AttrConfEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}
			}
		}
	void clear()
		{
		if (size() > 0)
			{
			AttrConfEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}

			this->vector<AttrConfEventData *>::clear();
			}
		}
};

/********************************************************************************
 * 																				*
 * 						DataReadyEventData class								*
 * 																				*
 *******************************************************************************/

class DataReadyEventData
{
public :
	DataReadyEventData(DeviceProxy *,AttDataReady *,string &evt,DevErrorList &);
	~DataReadyEventData() {};
	DataReadyEventData(const DataReadyEventData &);
	DataReadyEventData & operator=(const DataReadyEventData &);

	DeviceProxy 	*device;
	string 			attr_name;
	string			event;
	int 			attr_data_type;
	int 			ctr;

	bool 			err;
	DevErrorList 	errors;

	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal 	reception_date;
	Tango::TimeVal &get_date() {return reception_date;}
private:
	void set_time();
};

/********************************************************************************
 * 																				*
 * 						DataReadyEventDataList class							*
 * 																				*
 *******************************************************************************/

class DataReadyEventDataList:public vector<DataReadyEventData *>
{
public:
	DataReadyEventDataList(): vector<DataReadyEventData *>(0) {};
	~DataReadyEventDataList()
	{
		if (size() > 0)
		{
			DataReadyEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
			{
				delete (*vpos);
			}
		}
	}
	void clear()
	{
		if (size() > 0)
		{
			DataReadyEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
			{
				delete (*vpos);
			}

			this->vector<DataReadyEventData *>::clear();
		}
	}
}
;


/********************************************************************************
 * 																				*
 * 						EventQueue class										*
 * 																				*
 *******************************************************************************/
class EventQueue
{
public:
	EventQueue();
	EventQueue(long max_size);
	~EventQueue();

	void insert_event(EventData         	*new_event);
	void insert_event(AttrConfEventData 	*new_event);
	void insert_event(DataReadyEventData 	*new_event);

	int      size();
	TimeVal get_last_event_date();
	bool     is_empty() {if (event_buffer.size() == 0) return true;else return false;}

	void get_events(EventDataList         	&event_list);
	void get_events(AttrConfEventDataList 	&event_list);
	void get_events(DataReadyEventDataList	&event_list);
	void get_events(CallBack *cb);

private:
	void inc_indexes();

	vector<EventData *>         	event_buffer;
	vector<AttrConfEventData *> 	conf_event_buffer;
	vector<DataReadyEventData *>	ready_event_buffer;

	long	max_elt;
	long	insert_elt;
	long	nb_elt;

	omni_mutex	modification_mutex;
};



/********************************************************************************
 * 																				*
 * 						KeepAliveThCmd class									*
 * 																				*
 *******************************************************************************/

class KeepAliveThCmd:public omni_mutex
{
public :
	KeepAliveThCmd():cond(this) {};

	bool				cmd_pending;	// The new command flag
	KeepAliveCmdCode	cmd_code;		// The command code
	omni_condition 		cond;
};


/********************************************************************************
 * 																				*
 * 						Map structures      									*
 * 																				*
 *******************************************************************************/

typedef struct event_not_connected
{
	DeviceProxy 					*device;
	string 							attribute;
	EventType 						event_type;
	string 							event_name;
	int 						 	event_id;
	CallBack 						*callback;
	EventQueue                  	*ev_queue;
	vector<string> 					filters;
	time_t 							last_heartbeat;
} EventNotConnected;

typedef struct event_subscribe
{
	EventQueue						*ev_queue;
	CallBack						*callback;
	int								id;
} EventSubscribeStruct;

//------------------------ Event Callback related info --------------------------------------

typedef struct event_callback_base
{
 	DeviceProxy 					*device;
	string 							attr_name;
	string 							event_name;
	string 							channel_name;
	string                          fully_qualified_event_name;
	time_t 							last_subscribed;
	TangoMonitor					*callback_monitor;
	vector<EventSubscribeStruct>	callback_list;
} EventCallBackBase;

typedef struct event_callback_zmq
{
    DevLong                         device_idl;
}EventCallBackZmq;

typedef struct event_callback: public EventCallBackBase, public EventCallBackZmq
{
	string 							filter_constraint;
	CosNotifyFilter::FilterID 		filter_id;
	bool 							filter_ok;
} EventCallBackStruct;

//------------------------ Event Channel related info --------------------------------------

typedef struct event_channel_base
{
 	DeviceProxy 					*adm_device_proxy;
	string 							full_adm_name;
	time_t 							last_subscribed;
	time_t 							last_heartbeat;
	bool 							heartbeat_skipped;
	TangoMonitor					*channel_monitor;
	ChannelType                     channel_type;
} EventChannelBase;

typedef struct channel_struct: public EventChannelBase
{
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var structuredProxyPushSupplier;
	CosNotifyFilter::FilterID 		heartbeat_filter_id;
	string 							notifyd_host;
	bool 							notifd_failed;
	long 							has_notifd_closed_the_connection;
} EventChannelStruct;

typedef std::map<std::string,EventChannelStruct>::iterator EvChanIte;
typedef std::map<std::string,EventCallBackStruct>::iterator EvCbIte;


/********************************************************************************
 * 																				*
 * 						EventConsumer class										*
 * 																				*
 *******************************************************************************/


class EventConsumer
{

	typedef void    (*EventCallbackFunction)(string event_name,string event_type,Tango::DeviceAttribute *attr_value);

public :
	EventConsumer(ApiUtil *ptr);
	virtual ~EventConsumer() {}

	int connect_event(DeviceProxy *,const string &,EventType,CallBack *,EventQueue *,const vector<string> &,string &,int event_id = 0);
	void connect(DeviceProxy *,string &,DeviceData &,string &);

	void shutdown();
	void shutdown_keep_alive_thread();
    ChannelType get_event_system_for_event_id(int);
	virtual void cleanup_EventChannel_map() = 0;

	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   CallBack *callback, const vector<string> &filters, bool stateless = false);
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   int event_queue_size, const vector<string> &filters, bool stateless = false);
	void unsubscribe_event(int event_id);

	// methods to access data in event queues

	void get_events (int event_id, EventDataList &event_list);
	void get_events (int event_id, AttrConfEventDataList &event_list);
	void get_events (int event_id, DataReadyEventDataList &event_list);
	void get_events (int event_id, CallBack *cb);
	int  event_queue_size(int event_id);
	TimeVal get_last_event_date(int event_id);
	bool is_event_queue_empty(int event_id);


	static KeepAliveThCmd                                   cmd;
	static EventConsumerKeepAliveThread 	                *keep_alive_thread;

protected :
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   CallBack *callback, EventQueue *ev_queue,
					   const vector<string> &filters, bool stateless = false);
	friend class EventConsumerKeepAliveThread;
	void attr_to_device(const AttributeValue *,const AttributeValue_3 *,long,DeviceAttribute *);
	void attr_to_device(const AttributeValue_4 *,DeviceAttribute *);
	void conf_to_info(AttributeConfig_2 &,AttributeInfoEx **);

	static map<std::string,std::string> 					device_channel_map;     // key - device_name, value - channel name
	static map<std::string,EventChannelStruct> 				channel_map;            // key - channel_name, value - Event Channel info
	static map<std::string,EventCallBackStruct> 			event_callback_map;     // key - callback_key, value - Event CallBack info
	static ReadersWritersLock 								map_modification_lock;

	static vector<EventNotConnected> 						event_not_connected;
	static int 												subscribe_event_id; 	// unique event id
	static vector<string> 									env_var_fqdn_prefix;

	static omni_mutex										ev_consumer_inst_mutex;

	string													device_name;
	string 													att_name_lower;
	string													callback_key;

	int add_new_callback(EvCbIte &,CallBack *,EventQueue *,int);
	void get_fire_sync_event(DeviceProxy *,CallBack *,EventQueue *,EventType,string &,const string &,EventCallBackStruct &);

	virtual void connect_event_channel(string &,Database *,bool,DeviceData &) = 0;
    virtual void disconnect_event_channel(string &channel_name) {}
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &) = 0;
    virtual void disconnect_event(string &) {}

    virtual void get_subscription_command_name(string &) = 0;
    virtual void set_channel_type(EventChannelStruct &) = 0;
};

/********************************************************************************
 * 																				*
 * 						NotifdEventConsumer class								*
 * 																				*
 *******************************************************************************/


class NotifdEventConsumer : public POA_CosNotifyComm::StructuredPushConsumer ,
                            public EventConsumer ,
                            public omni_thread
{
public :
	static NotifdEventConsumer *create();
    TANGO_IMP_EXP static void cleanup() {if (_instance != NULL){_instance=NULL;}}

	void push_structured_event(const CosNotification::StructuredEvent&);
	virtual void cleanup_EventChannel_map();

	void disconnect_structured_push_consumer();
	void offer_change(const CosNotification::EventTypeSeq &,const CosNotification::EventTypeSeq &);

	CORBA::ORB_var 					orb_;

protected :
	NotifdEventConsumer(ApiUtil *ptr);
	virtual void connect_event_channel(string &,Database *,bool,DeviceData &);
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &);

    virtual void get_subscription_command_name(string &cmd) {cmd="EventSubscriptionChange";}
    virtual void set_channel_type(EventChannelStruct &ecs) {ecs.channel_type = NOTIFD;}

private :

	TANGO_IMP static NotifdEventConsumer 					*_instance;

	CosNotifyChannelAdmin::EventChannel_var 				eventChannel;
	CosNotifyChannelAdmin::ConsumerAdmin_var 				consumerAdmin;
	CosNotifyChannelAdmin::ProxyID 							proxyId;
	CosNotifyChannelAdmin::ProxySupplier_var 				proxySupplier;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var	structuredProxyPushSupplier;
	CosNotifyChannelAdmin::EventChannelFactory_var 			eventChannelFactory;

	void *run_undetached(void *arg);
};


/********************************************************************************
 * 																				*
 * 						ZmqEventConsumer class  								*
 * 																				*
 *******************************************************************************/


class ZmqEventConsumer : public EventConsumer ,
                         public omni_thread
{
public :
	static ZmqEventConsumer *create();
    TANGO_IMP_EXP static void cleanup() {if (_instance != NULL){_instance=NULL;}}

	virtual void cleanup_EventChannel_map();

	enum UserDataEventType
	{
	    ATT_CONF = 0,
	    ATT_READY,
	    ATT_VALUE
	};

protected :
	ZmqEventConsumer(ApiUtil *ptr);
	virtual void connect_event_channel(string &,Database *,bool,DeviceData &);
    virtual void disconnect_event_channel(string &channel_name);
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &);
    virtual void disconnect_event(string &);

    virtual void get_subscription_command_name(string &cmd) {cmd="ZmqEventSubscriptionChange";}
    virtual void set_channel_type(EventChannelStruct &ecs) {ecs.channel_type = ZMQ;}

private :
	TANGO_IMP static ZmqEventConsumer       *_instance;
	zmq::context_t                          zmq_context;            // ZMQ context
	zmq::socket_t                           *heartbeat_sub_sock;    // heartbeat subscriber socket
	zmq::socket_t                           *control_sock;          // control socket
	zmq::socket_t                           *event_sub_sock;        // event subscriber socket

	map<string,zmq::socket_t *>             event_mcast;            // multicast socket(s)
	vector<string>                          connected_pub;          //
	vector<string>                          connected_heartbeat;    //

    AttributeValue_var                      av;
    AttributeValue_3_var                    av3;
    AttributeValue_4_var                    av4;
    AttributeConfig_2_var                   ac2;
    AttributeConfig_3_var                   ac3;
    AttDataReady_var                        adr;
    DevErrorList_var                        del;

	void *run_undetached(void *arg);
	void push_heartbeat_event(string &);
    void push_zmq_event(string &,unsigned char,zmq::message_t &,bool);
    bool process_ctrl(zmq::message_t &);
    void process_heartbeat(zmq::message_t &,zmq::message_t &,zmq::message_t &);
    void process_event(zmq::message_t &,zmq::message_t &,zmq::message_t &,zmq::message_t &);
};

/********************************************************************************
 * 																				*
 * 						EventConsumerKeepAliveThread class						*
 * 																				*
 *******************************************************************************/

class EventConsumerKeepAliveThread : public omni_thread
{

public :

    EventConsumerKeepAliveThread(const EventConsumer&);
	EventConsumerKeepAliveThread(KeepAliveThCmd &cmd):shared_cmd(cmd){};
	void start() {start_undetached();}

protected :
	KeepAliveThCmd		&shared_cmd;

private :
	void *run_undetached(void *arg);
	bool reconnect_to_channel(EvChanIte &,EventConsumer *);
	void reconnect_to_event(EvChanIte &,EventConsumer *);
	void re_subscribe_event(EvCbIte &,EvChanIte &);
    void stateless_subscription_failed(vector<EventNotConnected>::iterator &,DevFailed &,time_t &);
};

/********************************************************************************
 * 																				*
 * 						DelayedEventUnsubThread class							*
 * 																				*
 *******************************************************************************/

class DelayedEventUnsubThread: public omni_thread
{
public:
	DelayedEventUnsubThread(EventConsumer *ec,int id,TangoMonitor *m):omni_thread(),event_id(id),ev_cons(ec),the_mon(m) {}

	void run(void *);

private:
	int 			event_id;
	EventConsumer 	*ev_cons;
	TangoMonitor	*the_mon;
};

} // End of namespace


#endif // _EVENTAPI_H
