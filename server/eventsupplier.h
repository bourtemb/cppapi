////////////////////////////////////////////////////////////////////////////////
///
///  file       eventsupplier.h
///
/// 	        C++ include file for implementing the TANGO event server and
///		        client singleton classes - EventSupplier and EventConsumer.
///             These classes are used to send events from the server
///             to the notification service and to receive events from
///             the notification service.
///
/// 		author(s) : A.Gotz (goetz@esrf.fr)
///
/// 		original : 7 April 2003
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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
///
/// 		$Revision$
///
/// 		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _EVENT_SUPPLIER_API_H
#define _EVENT_SUPPLIER_API_H

#include <except.h>

#if defined (_TG_WINDOWS_) && defined (_USRDLL) && !defined(_TANGO_LIB)
#define USE_stub_in_nt_dll
#endif

#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>

#include <zmq.hpp>

#if defined (_TG_WINDOWS_) && defined (_USRDLL) && !defined(_TANGO_LIB)
#undef USE_stub_in_nt_dll
#endif

#include <eventconsumer.h>

#include <omnithread.h>
#ifndef _TG_WINDOWS_
#include <sys/time.h>
#endif
#include <math.h>


namespace Tango
{

typedef struct _NotifService
{
	CosNotifyChannelAdmin::SupplierAdmin_var 				SupAdm;
	CosNotifyChannelAdmin::ProxyID 							pID;
	CosNotifyChannelAdmin::ProxyConsumer_var 				ProCon;
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var 	StrProPush;
	CosNotifyChannelAdmin::EventChannelFactory_var 			EveChaFac;
	CosNotifyChannelAdmin::EventChannel_var 				EveCha;
	string													ec_ior;
} NotifService;

//---------------------------------------------------------------------
//
//              EventSupplier base class
//
//---------------------------------------------------------------------

class EventSupplier
{
public :
    EventSupplier(Util *);
	virtual ~EventSupplier() {}

	void push_att_data_ready_event(DeviceImpl *,const string &,long,DevLong);

    struct AttributeData
    {
        const AttributeValue      *attr_val;
        const AttributeValue_3    *attr_val_3;
        const AttributeValue_4    *attr_val_4;
        const AttributeConfig_2   *attr_conf_2;
        const AttributeConfig_3   *attr_conf_3;
        const AttDataReady        *attr_dat_ready;
    };

	SendEventType detect_and_push_events(DeviceImpl *,struct AttributeData &,DevFailed *,string &,struct timeval *);

//------------------ Change event ---------------------------

	bool detect_change(Attribute &,struct AttributeData &,bool,double &,double &,DevFailed *,bool &,DeviceImpl *);

//------------------ Detect, push change event --------------

	bool detect_and_push_change_event(DeviceImpl *,struct AttributeData &,Attribute &,string &,DevFailed *,bool user_push = false);

//------------------ Detect, push archive event --------------

	bool detect_and_push_archive_event(DeviceImpl *,struct AttributeData &,Attribute &,string &,DevFailed *,struct timeval *,bool user_push = false);

//------------------ Detect, push periodic event -------------

	bool detect_and_push_periodic_event(DeviceImpl *,struct AttributeData &,Attribute &,string &,DevFailed *,struct timeval *);

//------------------ Push event -------------------------------

	virtual void push_event(DeviceImpl *,string,vector<string> &,vector<double> &,vector<string> &,vector<long> &,struct AttributeData &,string &,DevFailed *) = 0;
	virtual void push_heartbeat_event() = 0;

//------------------- Attribute conf change event ---------------------

	void push_att_conf_events(DeviceImpl *device_impl,AttributeData &,DevFailed *,string &);

	omni_mutex &get_push_mutex() {return push_mutex;}
	string &get_fqdn_prefix() {return fqdn_prefix;}

	bool get_one_subscription_cmd() {return one_subscription_cmd;}
	void set_one_subscription_cmd(bool val) {one_subscription_cmd = val;}

protected :
	inline int timeval_diff(TimeVal before, TimeVal after)
	{
		return ((after.tv_sec-before.tv_sec)*1000000 + after.tv_usec - before.tv_usec);
	}

	static string 		    fqdn_prefix;

	// Added a mutex to synchronize the access to
	//	detect_and_push_change_event	and
	// detect_and_push_archive_event which are used
	// from different threads
	static omni_mutex		event_mutex;

	// Added a mutex to synchronize the access to
	//	push_event which is used
	// from different threads
	static omni_mutex		push_mutex;

	// Added a mutex to synchronize the access to
	//	detect_event which is used
	// from different threads
	static omni_mutex		detect_mutex;

private:
	bool        one_subscription_cmd;
};

//---------------------------------------------------------------------
//
//              NotifdEventSupplier class
//
//---------------------------------------------------------------------

class NotifdEventSupplier : public EventSupplier, public POA_CosNotifyComm::StructuredPushSupplier
{
public :

	TANGO_IMP_EXP static NotifdEventSupplier *create(CORBA::ORB_var,string,Util *);
	void connect();
	void disconnect_structured_push_supplier();
	void disconnect_from_notifd();
	void subscription_change(const CosNotification::EventTypeSeq& added,const CosNotification::EventTypeSeq& deled);

	void push_heartbeat_event();
	string &get_event_channel_ior() {return event_channel_ior;}
    void file_db_svr();

//------------------ Push event -------------------------------

	virtual void push_event(DeviceImpl *,string,vector<string> &,vector<double> &,vector<string> &,vector<long> &,struct AttributeData &,string &,DevFailed *);

protected :

	NotifdEventSupplier(CORBA::ORB_var,
		CosNotifyChannelAdmin::SupplierAdmin_var,
		CosNotifyChannelAdmin::ProxyID,
		CosNotifyChannelAdmin::ProxyConsumer_var,
		CosNotifyChannelAdmin::StructuredProxyPushConsumer_var,
		CosNotifyChannelAdmin::EventChannelFactory_var,
		CosNotifyChannelAdmin::EventChannel_var,
		string &,
		Util *);

private :
	static NotifdEventSupplier 								*_instance;
	CosNotifyChannelAdmin::EventChannel_var 				eventChannel;
	CosNotifyChannelAdmin::SupplierAdmin_var 				supplierAdmin;
	CosNotifyChannelAdmin::ProxyID 							proxyId;
	CosNotifyChannelAdmin::ProxyConsumer_var 				proxyConsumer;
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var 	structuredProxyPushConsumer;
	CosNotifyChannelAdmin::EventChannelFactory_var 			eventChannelFactory;
	CORBA::ORB_var 											orb_;

	string 		event_channel_ior;

	void reconnect_notifd();
	TANGO_IMP_EXP static void connect_to_notifd(NotifService &,CORBA::ORB_var &,string &,Util *);
};


//---------------------------------------------------------------------
//
//              ZmqEventSupplier class
//
//---------------------------------------------------------------------

#define     LARGE_DATA_THRESHOLD    2048
#define     LARGE_DATA_THRESHOLD_ENCODED   LARGE_DATA_THRESHOLD * 4

#ifndef HAS_LAMBDA_FUNC
template <typename A1,typename A2,typename R>
struct WantedClient : public binary_function<A1,A2,R>
{
	R operator() (A1 conn_client, A2 client) const
	{
		return conn_client.clnt == client;
	}
};

template <typename A1,typename A2,typename R>
struct OldClient : public binary_function<A1,A2,R>
{
	R operator() (A1 conn_client, A2 ti) const
	{
        if (ti > (conn_client.date + 500))
        {
            return true;
        }
        else
            return false;
	}
};
#endif

class ZmqEventSupplier : public EventSupplier
{
public :
	TANGO_IMP_EXP static ZmqEventSupplier *create(Util *);
	virtual ~ZmqEventSupplier();

//------------------ Push event -------------------------------

	void push_heartbeat_event();
	virtual void push_event(DeviceImpl *,string,vector<string> &,vector<double> &,vector<string> &,vector<long> &,struct AttributeData &,string &,DevFailed *);

	string &get_heartbeat_endpoint() {return heartbeat_endpoint;}
	string &get_event_endpoint() {return event_endpoint;}

    void create_event_socket();
    void create_mcast_event_socket(string &,string &,int,bool);
    bool is_event_mcast(string &);
    string &get_mcast_event_endpoint(string &);
    void init_event_cptr(string &event_name);

    bool update_connected_client(client_addr *);
    void set_double_send() {double_send=true;double_send_heartbeat=true;}

protected :
	ZmqEventSupplier(Util *);

private :
	static ZmqEventSupplier 	*_instance;

    struct McastSocketPub
    {
        string                  endpoint;
        zmq::socket_t           *pub_socket;
        bool                    local_client;
    };

    struct ConnectedClient
    {
        client_addr             clnt;
        time_t                  date;
    };

	zmq::context_t              zmq_context;            // ZMQ context
	zmq::socket_t               *heartbeat_pub_sock;    // heartbeat publisher socket
	zmq::socket_t               *event_pub_sock;        // events publisher socket
	map<string,McastSocketPub>  event_mcast;            // multicast socket(s)

	string                      heartbeat_endpoint;     // heartbeat publisher endpoint
	string                      host_ip;                // Host IP address
	string                      heartbeat_event_name;   // The event name used for the heartbeat
	ZmqCallInfo                 heartbeat_call;         // The heartbeat call info
    cdrMemoryStream             heartbeat_call_cdr;     //
    TangoCdrMemoryStream        data_call_cdr;
    string                      event_name;

    zmq::message_t              endian_mess;            // Zmq message for host endianness
    zmq::message_t              endian_mess_2;          //
    zmq::message_t              heartbeat_call_mess;    //
    zmq::message_t              heartbeat_call_mess_2;  //

	unsigned char               host_endian;            // the host endianess
	bool                        heartbeat_name_init;

	bool                        ip_specified;           // The user has specified an IP address
	string                      user_ip;                // The specified IP address

	string                      event_endpoint;         // event publisher endpoint

	map<string,unsigned int>    event_cptr;             // event counter map

	list<ConnectedClient>       con_client;             // Connected clients
	bool                        double_send;            // Double send flag
	bool                        double_send_heartbeat;

	void tango_bind(zmq::socket_t *,string &);
	unsigned char test_endian();
    void create_mcast_socket(string &,int,McastSocketPub &);
};

} // End of namespace

#endif // _EVENT_SUPPLIER_API_H

