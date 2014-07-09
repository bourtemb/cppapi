static const char *RcsId = "$Id$\n$Name$";

//+==================================================================================================================
//
// file :               Pipe.cpp
//
// description :        C++ source code for the Pipe class. The Pipe class is the root class for all derived
//						Pipe classes.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2014
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventsupplier.h>

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::Pipe
//
// description :
//		Constructor for abstract class Pipe
//
//-----------------------------------------------------------------------------------------------------------------

Pipe::Pipe(const string &_name,const string &_desc,const string &_label,Tango::DispLevel _level,PipeWriteType _pwt)
:name(_name),desc(_desc),label(_label),disp_level(_level),writable(_pwt),ext(new PipeExt)
{
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);

	pipe_serial_model = PIPE_BY_KERNEL;

	event_subscription = 0;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_time
//
// description :
//		Set the date in the pipe object
//
//--------------------------------------------------------------------------------------------------------------------

void Pipe::set_time()
{
#ifdef _TG_WINDOWS_
	struct _timeb t;
	_ftime(&t);

	when.tv_sec = (CORBA::Long)t.time;
	when.tv_usec = (CORBA::Long)(t.millitm * 1000);
	when.tv_nsec = 0;
#else
	struct timezone tz;
	struct timeval tv;
	gettimeofday(&tv,&tz);

	when.tv_sec = (CORBA::Long)tv.tv_sec;
	when.tv_usec = (CORBA::Long)tv.tv_usec;
	when.tv_nsec = 0;
#endif
}

//+-------------------------------------------------------------------------
//
// method : 		Pipe::set_pipe_serial_method
//
// description : 	Set pipe serialization method
//
//--------------------------------------------------------------------------

void Pipe::set_pipe_serial_model(PipeSerialModel ser_model)
{
	if (ser_model == Tango::PIPE_BY_USER)
	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->get_serial_model() != Tango::BY_DEVICE)
		{
			Except::throw_exception(API_PipeNotAllowed,
				      	  "Pipe serial model by user is not allowed when the process is not in BY_DEVICE serialization model",
				      	  "Pipe::set_pipe_serial_model");
		}
	}

	pipe_serial_model=ser_model;
}

Pipe &Pipe::operator[](const string &_na)
{
	the_blob.operator[](_na);
	return *this;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::fire_event
//
// description :
//		Fire a pipe event
//
// arguments:
// 		in :
//			- dev : Device pointer
//			- except : Pointer to the DevFailed exception
//
//---------------------------------------------------------------------------------------------------------------------


void Pipe::fire_event(DeviceImpl *dev,DevFailed *except)
{
	cout4 << "Pipe::fire_event() entering ..." << endl;

//
// Check if it is needed to send an event
//

	time_t now;
	time_t delta_subscription;

	now = time(NULL);
	delta_subscription = now - event_subscription;

	if (delta_subscription >= EVENT_RESUBSCRIBE_PERIOD)
		return;

//
// Get the event supplier, and simply return if not created
//

	ZmqEventSupplier *event_supplier_zmq = NULL;

	Tango::Util *tg = Util::instance();
	event_supplier_zmq = tg->get_zmq_event_supplier();

	if (event_supplier_zmq == NULL)
	{
		return;
	}

//
// Create the structure used to send data to event system
//

	EventSupplier::SuppliedEventData ad;
	::memset(&ad,0,sizeof(ad));

//
// Fire event
//

	vector<string> f_names;
	vector<double> f_data;
	vector<string> f_names_lg;
	vector<long> f_data_lg;

	event_supplier_zmq->push_event(dev,"pipe",f_names,f_data,f_names_lg,f_data_lg,ad,name,except,true);
}

#ifdef _TG_WINDOWS_
void Pipe::fire_event(DeviceImpl *_dev,DevicePipeBlob *_dat)
{
	struct _timeb now_win;
	struct timeval now;

	_ftime(&now_win);
	now.tv_sec = (unsigned long)now_win.time;
	now.tv_usec = (long)now_win.millitm * 1000;

	fire_event(_dev,_dat,now);
}
#endif // _TG_WINDOWS_


void Pipe::fire_event(DeviceImpl *dev,DevicePipeBlob *p_data,struct timeval &t)
{
	cout4 << "Pipe::fire_event() entering ..." << endl;

//
// Check if it is needed to send an event
//

	time_t now;
	time_t delta_subscription;

	now = time(NULL);
	delta_subscription = now - event_subscription;

	if (delta_subscription >= EVENT_RESUBSCRIBE_PERIOD)
		return;

//
// Get the event supplier, and simply return if not created
//

	ZmqEventSupplier *event_supplier_zmq = NULL;

	Tango::Util *tg = Util::instance();
	event_supplier_zmq = tg->get_zmq_event_supplier();

	if (event_supplier_zmq == NULL)
	{
		return;
	}

//
// Create the structure used to send data to event system
//

	EventSupplier::SuppliedEventData ad;
	::memset(&ad,0,sizeof(ad));

//
// Init the structure sent with the event
//

	ad.pipe_val = new DevPipeData();
	ad.pipe_val->name = CORBA::string_dup(name.c_str());

	::memset(&(ad.pipe_val->time),0,sizeof(ad.pipe_val->time));
	ad.pipe_val->time.tv_sec = t.tv_sec;
	ad.pipe_val->time.tv_usec = t.tv_usec;


	const string &bl_name = p_data->get_name();
	if (bl_name.size() != 0)
		ad.pipe_val->data_blob.name = bl_name.c_str();

	DevVarPipeDataEltArray *tmp_ptr = p_data->get_insert_data();
	if (tmp_ptr == Tango_nullptr)
	{
		Except::throw_exception(API_PipeNoDataElement,"No data in DevicePipeBlob!","Pipe::fire_event()");
	}

	CORBA::ULong max,len;
	max = tmp_ptr->maximum();
	len = tmp_ptr->length();
	ad.pipe_val->data_blob.blob_data.replace(max,len,tmp_ptr->get_buffer((CORBA::Boolean)true),true);

//
// Fire event
//

	vector<string> f_names;
	vector<double> f_data;
	vector<string> f_names_lg;
	vector<long> f_data_lg;

	event_supplier_zmq->push_event(dev,"pipe",f_names,f_data,f_names_lg,f_data_lg,ad,name,NULL,true);

	p_data->reset_insert_ctr();
	delete tmp_ptr;

	delete ad.pipe_val;
}

} // End of Tango namespace
