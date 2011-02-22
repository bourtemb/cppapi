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

/********************************************************************************
 * 																				*
 * 						EventData class											*
 * 																				*
 *******************************************************************************/

class EventData
{
public :

	EventData(DeviceProxy *dev,string &nam,string &evt,Tango::DeviceAttribute *attr_value_in,DevErrorList &errors_in) :
	device(dev),attr_name(nam),event(evt),attr_value(attr_value_in),errors(errors_in)
	{if (errors.length()==0) err=false;else err = true;}
		
	~EventData();
	EventData(const EventData &);
	EventData & operator=(const EventData &);
	
	DeviceProxy *device;
	string attr_name;
	string event;
	DeviceAttribute *attr_value;
	bool err;
	DevErrorList errors;
};

/********************************************************************************
 * 																				*
 * 						AttrConfEventData class									*
 * 																				*
 *******************************************************************************/
 
class AttrConfEventData
{
public :

	AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,Tango::AttributeInfoEx *attr_conf_in,DevErrorList &errors_in) :
	device(dev),attr_name(nam),event(evt),attr_conf(attr_conf_in),errors(errors_in)
	{if (errors.length()==0) err=false;else err = true;}
		
	~AttrConfEventData();
	AttrConfEventData(const AttrConfEventData &);
	AttrConfEventData & operator=(const AttrConfEventData &);
	
	DeviceProxy *device;
	string attr_name;
	string event;
	AttributeInfoEx *attr_conf;
	bool err;
	DevErrorList errors;
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
	
	bool			cmd_pending;	// The new command flag
	KeepAliveCmdCode	cmd_code;	// The command code
	omni_condition 		cond;
};

typedef struct event_not_connected
{
	DeviceProxy 				*device;
	string 						attribute;
	EventType 					event_type;
	string 						event_name;
	int 						event_id;
	CallBack 					*callback;
	vector<string> 				filters;
	int 						id;
   	time_t 						last_heartbeat;
} EventNotConnected;


typedef struct event_callback
{
	DeviceProxy 				*device;
	string 						attr_name;
	string 						event_name;
	string 						channel_name;
	string 						filter_constraint;
	time_t 						last_subscribed;
	CallBack 					*callback;
	CosNotifyFilter::FilterID 	filter_id;
	bool 						filter_ok;
	int 						id;
	TangoMonitor				*callback_monitor;
} EventCallBackStruct;

typedef struct channel_struct
{
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var structuredProxyPushSupplier;
	DeviceProxy 				*adm_device_proxy;
	string 						full_adm_name;
	time_t 						last_subscribed;
	time_t 						last_heartbeat;
	bool 						heartbeat_skipped;
	CosNotifyFilter::FilterID 	heartbeat_filter_id;
	string 						notifyd_host;
	bool 						notifd_failed;
	long 						has_notifd_closed_the_connection;
	TangoMonitor				*channel_monitor;
} EventChannelStruct;

	
/********************************************************************************
 * 																				*
 * 						EventConsumer class										*
 * 																				*
 *******************************************************************************/
 
 	
class EventConsumer : public POA_CosNotifyComm::StructuredPushConsumer ,
	      	      	  public omni_thread
{

	typedef void    (*EventCallbackFunction)(string event_name,string event_type,Tango::DeviceAttribute *attr_value);

public :
    EventConsumer(const EventConsumer&);
	static EventConsumer *create();
    void operator=(const EventConsumer&);
	virtual void push_structured_event(const CosNotification::StructuredEvent&);
	void connect(DeviceProxy* device_proxy);
	void disconnect_structured_push_consumer();
	void disconnect_from_notifd();	
	void offer_change(const CosNotification::EventTypeSeq& added,
                      const CosNotification::EventTypeSeq& deled);
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event, 
	                   CallBack *callback, const vector<string> &filters, bool stateless = false);
	int connect_event  (DeviceProxy *device, const string &attribute, EventType event, 
	                   CallBack *callback, const vector<string> &filters, 
							 string &event_name, int event_id = 0);
	void unsubscribe_event(int event_id);
	void unsubscribe_device(DeviceProxy *device);
	void cleanup_EventChannel_map();
	TANGO_IMP_EXP static void cleanup()
	{if (_instance != NULL){_instance=NULL;}}
	
	CORBA::ORB_var 			orb_;
	KeepAliveThCmd			cmd;
	EventConsumerKeepAliveThread 	*keep_alive_thread;

protected :
		
	friend class EventConsumerKeepAliveThread;
	void attr_to_device(const AttributeValue *,const AttributeValue_3 *,long,DeviceAttribute *);
	void conf_to_info(AttributeConfig_2 &,AttributeInfoEx **);

	EventConsumer(ApiUtil *ptr);

private :

	TANGO_IMP static EventConsumer *_instance;

	void *run_undetached(void *arg);
	void connect_event_channel(string &channel_name,Database *,bool);
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::ConsumerAdmin_var consumerAdmin;
	CosNotifyChannelAdmin::ProxyID proxyId;
	CosNotifyChannelAdmin::ProxySupplier_var proxySupplier;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var
            	structuredProxyPushSupplier;
	CosNotifyChannelAdmin::EventChannelFactory_var eventChannelFactory;
	
	map<std::string,std::string> device_channel_map;
	map<std::string,EventCallBackStruct> event_callback_map;
	map<std::string,EventChannelStruct> channel_map;
	ReadersWritersLock map_modification_lock;

	vector<EventNotConnected> event_not_connected;
	int subscribe_event_id; // unique event id		
};

typedef std::map<std::string,EventChannelStruct>::iterator EvChanIte;
typedef std::map<std::string,EventCallBackStruct>::iterator EvCbIte;

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

};

} // End of namespace


#endif // _EVENTAPI_H
