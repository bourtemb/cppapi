////////////////////////////////////////////////////////////////////////////////
///
///  file       eventsupplier.h
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


class EventSupplier : public POA_CosNotifyComm::StructuredPushSupplier
{
public :

	TANGO_IMP_EXP static EventSupplier *create(CORBA::ORB_var,string,Database*,string &,Util *);
	void connect();
	void disconnect_structured_push_supplier();
	void disconnect_from_notifd();
	void subscription_change(const CosNotification::EventTypeSeq& added,const CosNotification::EventTypeSeq& deled);

	void push_heartbeat_event();
	string &get_event_channel_ior() {return event_channel_ior;}
	void set_svr_port_num(string &);

protected :

	EventSupplier(CORBA::ORB_var,
		CosNotifyChannelAdmin::SupplierAdmin_var,
		CosNotifyChannelAdmin::ProxyID,
		CosNotifyChannelAdmin::ProxyConsumer_var,
		CosNotifyChannelAdmin::StructuredProxyPushConsumer_var,
		CosNotifyChannelAdmin::EventChannelFactory_var,
		CosNotifyChannelAdmin::EventChannel_var,
		string &);

private :
	static EventSupplier 									*_instance;
	CosNotifyChannelAdmin::EventChannel_var 				eventChannel;
	CosNotifyChannelAdmin::SupplierAdmin_var 				supplierAdmin;
	CosNotifyChannelAdmin::ProxyID 							proxyId;
	CosNotifyChannelAdmin::ProxyConsumer_var 				proxyConsumer;
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var 	structuredProxyPushConsumer;
	CosNotifyChannelAdmin::EventChannelFactory_var 			eventChannelFactory;
	CORBA::ORB_var 											orb_;

	inline int timeval_diff(TimeVal before, TimeVal after)
	{
		return ((after.tv_sec-before.tv_sec)*1000000 + after.tv_usec - before.tv_usec);
	}
	int 		heartbeat_period;
	int 		subscription_timeout;
	string 		event_channel_ior;
	string 		fqdn_prefix;

	void get_attribute_value(AttributeValue attr_value, LastAttrValue &curr_attr_value);
	void reconnect_notifd();
	TANGO_IMP_EXP static void connect_to_notifd(NotifService &,CORBA::ORB_var &,string &,Database *,string &,Util *);

	// Added a mutex to synchronize the access to
	//	detect_and_push_change_event_3	and
	// detect_and_push_archive_event_3 which are used
	// from different threads
	omni_mutex		event_mutex;

	// Added a mutex to synchronize the access to
	//	push_event_3 which is used
	// from different threads
	omni_mutex		push_mutex;

	// Added a mutex to synchronize the access to
	//	detect_event_3 which is used
	// from different threads
	omni_mutex		detect_mutex;

public :
	void push_att_data_ready_event(DeviceImpl *,const string &,long,DevLong);
	void detect_and_push_events_3(DeviceImpl *,AttributeValue_3 *,AttributeValue_4 *,DevFailed *,string &,struct timeval *);


/////////////////////////////////////////////////////////////////////////////
//
//			WARNING
//			-------
//
////////////////////////////////////////////////////////////////////////////

// Unfortunately, with Suse 9.3 with gcc 3.3.4 and binutils 2.15.94,
// it was not possible to generate the shared libs of Tango
// (linker error messages).
// To solve this problem, it was necessary to duplicate these methods
// instead of simply using a template method. This generates a code
// duplicate but...... (all methods finishing with xxxx_3)
//
// Test with gcc 3.4.5 with binutils 2.16.1 did not solve the problems
// We need to check with gcc 4.x.x if we could remove this code duplicate
// The other methods are in eventsupplier.cpp
//
//
//--------------------------------------------------------------------------


	template <typename T>
	void detect_and_push_events(DeviceImpl *device_impl,T &attr_value,DevFailed *except,string &attr_name);

//------------------ Change event ---------------------------

	bool detect_change_3(Attribute &attr,AttributeValue_3 *curr_attr_value,AttributeValue_4 *curr_attr_value_4,bool archive,
				  double &delta_change_rel,double &delta_change_abs,DevFailed *except,bool &force_change,DeviceImpl *dev);

	template <typename T>
	bool detect_change(Attribute &attr,T &curr_attr_value,bool archive,double &delta_change_rel,double &delta_change_abs,
				  DevFailed *except,bool &force_change,DeviceImpl *dev);

//------------------ Detect, push change event --------------

	void detect_and_push_change_event_3(DeviceImpl *device_impl,AttributeValue_3 *attr_value,AttributeValue_4 *attr_value_4,
						 Attribute &attr,string &attr_name,DevFailed *except,bool user_push = false);

	template <typename T>
	void detect_and_push_change_event(DeviceImpl *device_impl,T &attr_value,Attribute &attr,string &attr_name,DevFailed *except);

//------------------ Detect, push archive event --------------

	void detect_and_push_archive_event_3(DeviceImpl *device_impl,AttributeValue_3 *attr_value,AttributeValue_4 *attr_value_4,
						  Attribute &attr,string &attr_name,DevFailed *except,struct timeval *,bool user_push = false);

	template <typename T>
	void detect_and_push_archive_event(DeviceImpl *device_impl,T &attr_value,Attribute &attr,string &attr_name,DevFailed *except);

//------------------ Detect, push periodic event -------------

	void detect_and_push_periodic_event_3(DeviceImpl *device_impl,AttributeValue_3 *attr_value,AttributeValue_4 *attr_value_4,
					    Attribute &attr,string &attr_name,DevFailed *except,struct timeval *);

	template <typename T>
	void detect_and_push_periodic_event(DeviceImpl *device_impl,T &attr_value,Attribute &attr,string &attr_name,DevFailed *except);

//------------------ Push event -------------------------------

	template <typename T>
	void push_event(DeviceImpl *device_impl,string event_type,vector<string> &filterable_names,vector<double> &filterable_data,
		       	vector<string> &filterable_names_lg,vector<long> &filterable_data_lg,T &attr_value,string &attr_name,DevFailed *except);

	void push_event_3(DeviceImpl *device_impl,string event_type,vector<string> &filterable_names,vector<double> &filterable_data,
		       	vector<string> &filterable_names_lg,vector<long> &filterable_data_lg,AttributeValue_3 *attr_value,AttributeValue_4 *attr_value_4,
		       	string &attr_name,DevFailed *except);

//------------------- Miscellaneous event ---------------------

	template <typename T>
	void detect_and_push_quality_change_event(DeviceImpl *device_impl,T &attr_value,Attribute &attr,string &attr_name,DevFailed *except);

	template <typename T>
	void push_att_conf_events(DeviceImpl *device_impl,T &attr_conf,DevFailed *except,string &attr_name);

};

} // End of namespace

// Add template methods definitions

#include <eventsupplier.tpp>

#endif // _EVENT_SUPPLIER_API_H

