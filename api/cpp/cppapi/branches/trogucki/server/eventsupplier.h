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

//#include <zmq.hpp>

#if defined (_TG_WINDOWS_) && defined (_USRDLL) && !defined(_TANGO_LIB)
#undef USE_stub_in_nt_dll
#endif


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
    EventSupplier(Database*,string &);

    void set_svr_port_num(string &);

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

	void detect_and_push_events(DeviceImpl *,struct AttributeData &,DevFailed *,string &,struct timeval *);

//------------------ Change event ---------------------------

	bool detect_change(Attribute &,struct AttributeData &,bool,double &,double &,DevFailed *,bool &,DeviceImpl *);

//------------------ Detect, push change event --------------

	void detect_and_push_change_event(DeviceImpl *,struct AttributeData &,Attribute &,string &,DevFailed *,bool user_push = false);

//------------------ Detect, push archive event --------------

	void detect_and_push_archive_event(DeviceImpl *,struct AttributeData &,Attribute &,string &,DevFailed *,struct timeval *,bool user_push = false);

//------------------ Detect, push periodic event -------------

	void detect_and_push_periodic_event(DeviceImpl *,struct AttributeData &,Attribute &,string &,DevFailed *,struct timeval *);

//------------------ Push event -------------------------------

	virtual void push_event(DeviceImpl *,string,vector<string> &,vector<double> &,vector<string> &,vector<long> &,struct AttributeData &,string &,DevFailed *) = 0;
	virtual void push_heartbeat_event() = 0;

//------------------- Attribute conf change event ---------------------

	void push_att_conf_events(DeviceImpl *device_impl,AttributeData &,DevFailed *,string &);

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
};

//---------------------------------------------------------------------
//
//              NotifdEventSupplier class
//
//---------------------------------------------------------------------

class NotifdEventSupplier : public EventSupplier, public POA_CosNotifyComm::StructuredPushSupplier
{
public :

	TANGO_IMP_EXP static NotifdEventSupplier *create(CORBA::ORB_var,string,Database*,string &,Util *);
	void connect();
	void disconnect_structured_push_supplier();
	void disconnect_from_notifd();
	void subscription_change(const CosNotification::EventTypeSeq& added,const CosNotification::EventTypeSeq& deled);

	void push_heartbeat_event();
	string &get_event_channel_ior() {return event_channel_ior;}

//------------------ Push event -------------------------------

	void push_event(DeviceImpl *,string,vector<string> &,vector<double> &,vector<string> &,vector<long> &,struct AttributeData &,string &,DevFailed *);

protected :

	NotifdEventSupplier(CORBA::ORB_var,
		CosNotifyChannelAdmin::SupplierAdmin_var,
		CosNotifyChannelAdmin::ProxyID,
		CosNotifyChannelAdmin::ProxyConsumer_var,
		CosNotifyChannelAdmin::StructuredProxyPushConsumer_var,
		CosNotifyChannelAdmin::EventChannelFactory_var,
		CosNotifyChannelAdmin::EventChannel_var,
		string &,
		Database *,
		string &);

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
	TANGO_IMP_EXP static void connect_to_notifd(NotifService &,CORBA::ORB_var &,string &,Database *,string &,Util *);
};


//---------------------------------------------------------------------
//
//              ZmqEventSupplier class
//
//---------------------------------------------------------------------

class ZmqEventSupplier : public EventSupplier
{
public :
	TANGO_IMP_EXP static ZmqEventSupplier *create(Database *,string &,string &);

//------------------ Push event -------------------------------

	void push_heartbeat_event();
	void push_event(DeviceImpl *,string,vector<string> &,vector<double> &,vector<string> &,vector<long> &,struct AttributeData &,string &,DevFailed *);

	string &get_heartbeat_endpoint() {return heartbeat_endpoint;}

protected :
	ZmqEventSupplier(Database *,string &,string &);

private :
	static ZmqEventSupplier 	*_instance;

//	zmq::context_t              zmq_context;            // ZMQ context
//	zmq::socket_t               *heartbeat_pub_sock;    // heartbeat publisher socket
//	zmq::message_t              *heartbeat_mess;        // heartbeat message
	string                      heartbeat_endpoint;     // heartbeat publisher endpoint

//	void tango_bind(zmq::socket_t *,string &);
};

} // End of namespace

#endif // _EVENT_SUPPLIER_API_H

