static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               PollRing.cpp
//
// description :        C++ source code for the RingElt and PollRing
//			classes. These classes are used to implement the 
//			tango polling ring buffer. There is one
//			polling ring for each Tango command or attribute
//			polled.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.8  2007/04/20 14:41:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.7  2007/04/16 14:57:42  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.6  2007/03/06 08:19:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.5  2005/01/13 09:28:32  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 3.3.2.4  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.3.2.3  2004/09/15 06:47:16  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.3.2.2  2004/08/10 09:07:52  taurel
// - Fix bug when reading attribute history and attribute quality factor
//   set to INVALID
//
// Revision 3.3.2.1  2004/07/15 15:04:06  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.3  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.2  2003/08/21 07:24:37  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.1.2.1  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.1  2003/05/28 14:55:10  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.0  2003/03/25 16:44:07  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:32  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:23  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:11  taurel
// See Tango WEB pages for list of changes
//
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <pollring.h>


#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		RingElt::RingElt 
// 
// description : 	Constructor for the RingElt class.
//			This constructor simply set the internal value to their
//			default
//
//--------------------------------------------------------------------------

RingElt::RingElt()
{
	when.tv_sec = when.tv_usec = 0;
	cmd_result = NULL;
	attr_value = NULL;
	attr_value_3 = NULL;
	except = NULL;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::PollRing 
// 
// description : 	Two constructors for the PollRing class. The first one
//			does not take any argument and construct a black box
//			with the default depth.
//			The second one create a black box with a depth defined 
//			by the argument.
//
// argument : in : 	- max_size : The black box depth
//
//--------------------------------------------------------------------------

PollRing::PollRing():ring(DefaultPollRingDepth)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = DefaultPollRingDepth;
}

PollRing::PollRing(long max_size):ring(max_size)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = max_size;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::~PollRing 
// 
// description : 	The class destructor. It frees all the memory allocated
//			to store command/attribute result
//
//--------------------------------------------------------------------------

PollRing::~PollRing()
{
	cout4 << "In PollRing destructor" << endl;
	
	long nb = ring.size();

	for (long i = 0;i < nb;i++)
	{
		delete ring[i].cmd_result;
		delete ring[i].except;
		delete ring[i].attr_value;
		delete ring[i].attr_value_3;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::insert_data
// 
// description : 	These methods insert a new element in the ring buffer
//			when its real data
//
// argument : in : 	- any_ptr : The Any returned by the command
//			- t : The date
//
//--------------------------------------------------------------------------


void PollRing::insert_data(CORBA::Any *any_ptr,struct timeval &t)
{
	
//
// Insert data in the ring
//

	delete(ring[insert_elt].cmd_result);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;
	
	ring[insert_elt].cmd_result = any_ptr;	
	ring[insert_elt].when = t;
			
//
// Manage insert and read indexes
//

	inc_indexes();
}

void PollRing::insert_data(Tango::AttributeValueList *attr_val,struct timeval &t)
{
	
//
// Insert data in the ring
//

	delete(ring[insert_elt].attr_value);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;
		
	ring[insert_elt].attr_value = attr_val;	
	ring[insert_elt].when = t;
			
//
// Manage insert and read indexes
//

	inc_indexes();
}

void PollRing::insert_data(Tango::AttributeValueList_3 *attr_val,struct timeval &t)
{
	
//
// Insert data in the ring
//

	delete(ring[insert_elt].attr_value_3);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;
		
	ring[insert_elt].attr_value_3 = attr_val;	
	ring[insert_elt].when = t;
			
//
// Manage insert and read indexes
//

	inc_indexes();
}
//-------------------------------------------------------------------------
//
// method : 		PollRing::insert_except
// 
// description : 	This method insert a new element in the ring buffer
//			when this element is an exception
//
// argument : in : 	- ex : The exception to be stored
//			- t : The date
//
//--------------------------------------------------------------------------


void PollRing::insert_except(Tango::DevFailed *ex,struct timeval &t)
{
	
//
// Insert data in the ring
//

	delete(ring[insert_elt].except);
	if (ring[insert_elt].attr_value != NULL)
	{
		delete(ring[insert_elt].attr_value);
		ring[insert_elt].attr_value = NULL;
	}
	if (ring[insert_elt].cmd_result != NULL)
	{
		delete(ring[insert_elt].cmd_result);
		ring[insert_elt].cmd_result = NULL;
	}
		
	ring[insert_elt].except = ex;	
	ring[insert_elt].when = t;
			
//
// Manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::inc_indexes
// 
// description : 	This private method increment the indexes used to acces
//			the box itself. This is necessary because the box must
//			be managed as a circular buffer
//
//--------------------------------------------------------------------------


void PollRing::inc_indexes()
{
	insert_elt++;
	if (insert_elt == max_elt)
		insert_elt = 0;
		
	if (nb_elt != max_elt)
		nb_elt++;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_delta_t
// 
// description : 	This method computes the delta time between records 
//			in the ring buffer
//
// argument : 	in :	- nb : The number of delta t to be computed
//		out :	- res : A reference to the vector where result should
//				be stored
//
//--------------------------------------------------------------------------

void PollRing::get_delta_t(vector<double> &res,long nb)
{

//
// Throw exception if nothing in ring
//

	if (nb_elt < 2)
	{
		Except::throw_exception((const char *)"API_PollRingBufferEmpty",
				        (const char *)"Not enough data stored yet in polling ring buffer",
				        (const char *)"PollRing::get_delta_t");
	}

//
// Get the buffer starting point
//
	
	long read_index,prev_read;
	if (insert_elt == 0)
	{
		read_index = max_elt - 1;
		prev_read = read_index - 1;
	}
	else
	{
		read_index = insert_elt - 1;
		if (read_index == 0)
			prev_read = max_elt - 1;
		else
			prev_read = read_index - 1;
	}

//
// Clear the result vector
//

	res.clear();

//
// Compute how many delta can be computed
//

	if (nb_elt <= nb)
		nb = nb_elt - 1;
		
//
// The delta t computing loop
//

	long i;		
	for (i = 0;i < nb;i++)
	{
		double t_ref = (double)ring[read_index].when.tv_sec + ((double)ring[read_index].when.tv_usec / 1000000);
		double t_prev = (double)ring[prev_read].when.tv_sec + ((double)ring[prev_read].when.tv_usec / 1000000);
			
		res.push_back(t_ref - t_prev);
		prev_read--;
		if (prev_read < 0)
			prev_read = max_elt - 1;
		read_index--;
		if (read_index < 0)
			read_index = max_elt - 1;
	}	
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_insert_date
// 
// description : 	This method returns the date of the last insert in the
//			ring buffer
//
//--------------------------------------------------------------------------

struct timeval PollRing::get_last_insert_date()
{
	if (insert_elt == 0)
		return ring[max_elt - 1].when;
	else
		return ring[insert_elt - 1].when;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::is_last_an_error
// 
// description : 	This method returns a boolean set to true if the last
//			data recorded into the ring buffer was an exception
//
//--------------------------------------------------------------------------

bool PollRing::is_last_an_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
			return false;
		else
			return true;
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
			return false;
		else
			return true;
	}
}

bool PollRing::is_last_cmd_an_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except != NULL)
			return true;
		else
			return false;
	}
	else
	{
		if (ring[insert_elt - 1].except != NULL)
			return true;
		else
			return false;
	}
}

bool PollRing::is_last_attr_an_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except != NULL)
			return true;
		else
		{
			if ((*(ring[max_elt - 1].attr_value_3))[0].err_list.length() != 0)
				return true;
			else
				return false;
		}
	}
	else
	{
		if (ring[insert_elt - 1].except != NULL)
			return true;
		else
		{
			if ((*(ring[insert_elt - 1].attr_value_3))[0].err_list.length() != 0)
				return true;
			else
				return false;
		}
	}
}


//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_except
// 
// description : 	This method returns the exception recently stored in
//			the ring buffer.
//
//--------------------------------------------------------------------------

Tango::DevFailed *PollRing::get_last_except()
{
	if (insert_elt == 0)
		return ring[max_elt - 1].except;
	else
		return ring[insert_elt - 1].except;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_attr_error
// 
// description : 	This method returns the error stack for polled
//			attribute belonging to device implementing IDL
//			release 3 and more.
//
//--------------------------------------------------------------------------

Tango::DevErrorList &PollRing::get_last_attr_error()
{
	if (insert_elt == 0)
		return (*(ring[max_elt - 1].attr_value_3))[0].err_list;
	else
		return (*(ring[insert_elt - 1].attr_value_3))[0].err_list;
}
	
//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_data
// 
// description : 	This method returns the exception recently stored in
//			the ring buffer.
//
//--------------------------------------------------------------------------

CORBA::Any *PollRing::get_last_cmd_result()
{
	CORBA::Any *tmp_any;
	
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			tmp_any = ring[max_elt - 1].cmd_result;
			return new CORBA::Any(tmp_any->type(),
					      const_cast<void *>(tmp_any->value()),
					      false);
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			tmp_any = ring[insert_elt - 1].cmd_result;
			return new CORBA::Any(tmp_any->type(),
					      const_cast<void *>(tmp_any->value()),
					      false);
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}
	
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_attr_value
// 
// description : 	This method returns the exception recently stored in
//			the ring buffer.
//
//--------------------------------------------------------------------------

Tango::AttributeValue &PollRing::get_last_attr_value()
{	
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			return (*(ring[max_elt - 1].attr_value))[0];
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			return (*(ring[insert_elt - 1].attr_value))[0];
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}
	
}

Tango::AttributeValue_3 &PollRing::get_last_attr_value_3()
{	
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			return (*(ring[max_elt - 1].attr_value_3))[0];
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			return (*(ring[insert_elt - 1].attr_value_3))[0];
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}
	
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_cmd_history
// 
// description : 	This method get command history from the ring buffer
//
// argument : in :	- n : record number
//			- ptr : Pointer to the sequence where command result
//				should be stored
//
//--------------------------------------------------------------------------

void PollRing::get_cmd_history(long n,Tango::DevCmdHistoryList *ptr)
{
	long i;
	
//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;
	
	long seq_index = n - 1;

//
// Read buffer
//	
			
	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].time.tv_sec = ring[index].when.tv_sec + DELTA_T;
		(*ptr)[seq_index].time.tv_usec = ring[index].when.tv_usec;
		(*ptr)[seq_index].time.tv_nsec = 0;
		
		if (ring[index].except == NULL)
		{
			(*ptr)[seq_index].cmd_failed = false;
			(*ptr)[seq_index].value = *(ring[index].cmd_result);
			(*ptr)[seq_index].errors.length(0);
		}
		else
		{
			(*ptr)[seq_index].cmd_failed = true;
			(*ptr)[seq_index].errors = ring[index].except->errors;
		}
			
		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}
		
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_attr_history
// 
// description : 	This method get attribute history from the ring buffer
//
// argument : in :	- n : record number
//			- ptr : Pointer to the sequence where attribute result
//				should be stored
//			- type : The attribute data type
//
//--------------------------------------------------------------------------

void PollRing::get_attr_history(long n,Tango::DevAttrHistoryList *ptr,long type)
{
	long i;
	
//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;
	
	long seq_index = n - 1;

//
// Read buffer
//	
			
	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].value.time.tv_sec = ring[index].when.tv_sec + DELTA_T;
		(*ptr)[seq_index].value.time.tv_usec = ring[index].when.tv_usec;
		(*ptr)[seq_index].value.time.tv_nsec = 0;
		
		if (ring[index].except == NULL)
		{
			(*ptr)[seq_index].attr_failed = false;
			(*ptr)[seq_index].value.quality = (*ring[index].attr_value)[0].quality;
			(*ptr)[seq_index].value.dim_x = (*ring[index].attr_value)[0].dim_x;
			(*ptr)[seq_index].value.dim_y = (*ring[index].attr_value)[0].dim_y;
			(*ptr)[seq_index].value.name = CORBA::string_dup((*ring[index].attr_value)[0].name);
			
			if ((*ptr)[seq_index].value.quality != Tango::ATTR_INVALID)
			{
				const Tango::DevVarDoubleArray *tmp_db;
				Tango::DevVarDoubleArray *new_tmp_db;
				const Tango::DevVarShortArray *tmp_sh;
				Tango::DevVarShortArray *new_tmp_sh;
				const Tango::DevVarLongArray *tmp_lg;
				Tango::DevVarLongArray *new_tmp_lg;
				const Tango::DevVarLong64Array *tmp_lg64;
				Tango::DevVarLong64Array *new_tmp_lg64;
				const Tango::DevVarStringArray *tmp_str;
				Tango::DevVarStringArray *new_tmp_str;
				const Tango::DevVarFloatArray *tmp_fl;
				Tango::DevVarFloatArray *new_tmp_fl;
				const Tango::DevVarBooleanArray *tmp_boo;
				Tango::DevVarBooleanArray *new_tmp_boo;
				const Tango::DevVarUShortArray *tmp_ush;
				Tango::DevVarUShortArray *new_tmp_ush;
				const Tango::DevVarCharArray *tmp_uch;
				Tango::DevVarCharArray *new_tmp_uch;
				const Tango::DevVarULongArray *tmp_ulg;
				Tango::DevVarULongArray *new_tmp_ulg;
				const Tango::DevVarULong64Array *tmp_ulg64;
				Tango::DevVarULong64Array *new_tmp_ulg64;
				const Tango::DevVarStateArray *tmp_state;
				Tango::DevVarStateArray *new_tmp_state;
				
				switch (type)
				{
				case Tango::DEV_SHORT :
					(*ring[index].attr_value)[0].value >>= tmp_sh;
					new_tmp_sh = new DevVarShortArray(
							tmp_sh->length(),
							tmp_sh->length(),
							const_cast<short *>(tmp_sh->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_sh;
					break;

				case Tango::DEV_DOUBLE :					
					(*ring[index].attr_value)[0].value >>= tmp_db;		
					new_tmp_db = new DevVarDoubleArray(
							tmp_db->length(),
							tmp_db->length(),
							const_cast<double *>(tmp_db->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_db;
					break;
					
				case Tango::DEV_LONG :
					(*ring[index].attr_value)[0].value >>= tmp_lg;
					new_tmp_lg = new DevVarLongArray(tmp_lg->length(),tmp_lg->length(),
										const_cast<DevLong *>(tmp_lg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg;
					break;
					
				case Tango::DEV_LONG64 :
					(*ring[index].attr_value)[0].value >>= tmp_lg64;
					new_tmp_lg64 = new DevVarLong64Array(tmp_lg64->length(),tmp_lg64->length(),
										const_cast<DevLong64 *>(tmp_lg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg64;
					break;

				case Tango::DEV_STRING :
					(*ring[index].attr_value)[0].value >>= tmp_str;
					new_tmp_str = new DevVarStringArray(
							tmp_str->length(),
							tmp_str->length(),
							const_cast<char **>(tmp_str->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_str;
					break;
					
				case Tango::DEV_FLOAT :
					(*ring[index].attr_value)[0].value >>= tmp_fl;
					new_tmp_fl = new DevVarFloatArray(
							tmp_fl->length(),
							tmp_fl->length(),
							const_cast<float *>(tmp_fl->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_fl;
					break;

				case Tango::DEV_BOOLEAN :					
					(*ring[index].attr_value)[0].value >>= tmp_boo;		
					new_tmp_boo = new DevVarBooleanArray(
							tmp_boo->length(),
							tmp_boo->length(),
							const_cast<DevBoolean *>(tmp_boo->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_boo;
					break;
					
				case Tango::DEV_USHORT :
					(*ring[index].attr_value)[0].value >>= tmp_ush;
					new_tmp_ush = new DevVarUShortArray(
							tmp_ush->length(),
							tmp_ush->length(),
							const_cast<DevUShort *>(tmp_ush->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_ush;
					break;

				case Tango::DEV_UCHAR :
					(*ring[index].attr_value)[0].value >>= tmp_uch;
					new_tmp_uch = new DevVarCharArray(
							tmp_uch->length(),
							tmp_uch->length(),
							const_cast<DevUChar *>(tmp_uch->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_uch;
					break;
					
				case Tango::DEV_ULONG :
					(*ring[index].attr_value)[0].value >>= tmp_ulg;
					new_tmp_ulg = new DevVarULongArray(tmp_ulg->length(),tmp_ulg->length(),
										const_cast<DevULong *>(tmp_ulg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg;
					break;
					
				case Tango::DEV_ULONG64 :
					(*ring[index].attr_value)[0].value >>= tmp_ulg64;
					new_tmp_ulg64 = new DevVarULong64Array(tmp_ulg64->length(),tmp_ulg64->length(),
										const_cast<DevULong64 *>(tmp_ulg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg64;
					break;
					
				case Tango::DEV_STATE :
					(*ring[index].attr_value)[0].value >>= tmp_state;
					new_tmp_state = new DevVarStateArray(tmp_state->length(),tmp_state->length(),
										const_cast<DevState *>(tmp_state->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_state;
					break;
				}
			}
			(*ptr)[seq_index].errors.length(0);
		}
		else
		{
			(*ptr)[seq_index].attr_failed = true;
			(*ptr)[seq_index].errors = ring[index].except->errors;

			(*ptr)[seq_index].value.quality = Tango::ATTR_INVALID;
			(*ptr)[seq_index].value.dim_x = 0;
			(*ptr)[seq_index].value.dim_y = 0;
		}
			
		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}
		
}

void PollRing::get_attr_history(long n,Tango::DevAttrHistoryList_3 *ptr,long type)
{
	long i;
	
//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;
	
	long seq_index = n - 1;

//
// Read buffer
//	
			
	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].value.time.tv_sec = ring[index].when.tv_sec + DELTA_T;
		(*ptr)[seq_index].value.time.tv_usec = ring[index].when.tv_usec;
		(*ptr)[seq_index].value.time.tv_nsec = 0;

		if (ring[index].except == NULL)
		{		
			(*ptr)[seq_index].value.err_list = (*ring[index].attr_value_3)[0].err_list;
			(*ptr)[seq_index].value.quality = (*ring[index].attr_value_3)[0].quality;
			(*ptr)[seq_index].value.r_dim = (*ring[index].attr_value_3)[0].r_dim;
			(*ptr)[seq_index].value.w_dim = (*ring[index].attr_value_3)[0].w_dim;
			(*ptr)[seq_index].value.name = CORBA::string_dup((*ring[index].attr_value_3)[0].name);		
		

			(*ptr)[seq_index].attr_failed = false;

			if ((*ptr)[seq_index].value.quality != Tango::ATTR_INVALID)
			{
				const Tango::DevVarDoubleArray *tmp_db;
				Tango::DevVarDoubleArray *new_tmp_db;
				const Tango::DevVarShortArray *tmp_sh;
				Tango::DevVarShortArray *new_tmp_sh;
				const Tango::DevVarLongArray *tmp_lg;
				Tango::DevVarLongArray *new_tmp_lg;
				const Tango::DevVarLong64Array *tmp_lg64;
				Tango::DevVarLong64Array *new_tmp_lg64;
				const Tango::DevVarStringArray *tmp_str;
				Tango::DevVarStringArray *new_tmp_str;
				const Tango::DevVarFloatArray *tmp_fl;
				Tango::DevVarFloatArray *new_tmp_fl;
				const Tango::DevVarBooleanArray *tmp_boo;
				Tango::DevVarBooleanArray *new_tmp_boo;
				const Tango::DevVarUShortArray *tmp_ush;
				Tango::DevVarUShortArray *new_tmp_ush;
				const Tango::DevVarCharArray *tmp_uch;
				Tango::DevVarCharArray *new_tmp_uch;
				const Tango::DevVarULongArray *tmp_ulg;
				Tango::DevVarULongArray *new_tmp_ulg;
				const Tango::DevVarULong64Array *tmp_ulg64;
				Tango::DevVarULong64Array *new_tmp_ulg64;
				const Tango::DevVarStateArray *tmp_state;
				Tango::DevVarStateArray *new_tmp_state;
				

				switch (type)
				{
				case Tango::DEV_SHORT :
					(*ring[index].attr_value_3)[0].value >>= tmp_sh;
					new_tmp_sh = new DevVarShortArray(
							tmp_sh->length(),
							tmp_sh->length(),
							const_cast<short *>(tmp_sh->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_sh;
					break;

				case Tango::DEV_DOUBLE :					
					(*ring[index].attr_value_3)[0].value >>= tmp_db;		
					new_tmp_db = new DevVarDoubleArray(
							tmp_db->length(),
							tmp_db->length(),
							const_cast<double *>(tmp_db->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_db;
					break;

				case Tango::DEV_LONG :
					(*ring[index].attr_value_3)[0].value >>= tmp_lg;
					new_tmp_lg = new DevVarLongArray(tmp_lg->length(),tmp_lg->length(),
							const_cast<DevLong *>(tmp_lg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg;
					break;
					
				case Tango::DEV_LONG64 :
					(*ring[index].attr_value_3)[0].value >>= tmp_lg64;
					new_tmp_lg64 = new DevVarLong64Array(tmp_lg64->length(),tmp_lg64->length(),
							const_cast<DevLong64 *>(tmp_lg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg64;
					break;

				case Tango::DEV_STRING :
					(*ring[index].attr_value_3)[0].value >>= tmp_str;
					new_tmp_str = new DevVarStringArray(
							tmp_str->length(),
							tmp_str->length(),
							const_cast<char **>(tmp_str->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_str;
					break;
					
				case Tango::DEV_FLOAT :
					(*ring[index].attr_value_3)[0].value >>= tmp_fl;
					new_tmp_fl = new DevVarFloatArray(
							tmp_fl->length(),
							tmp_fl->length(),
							const_cast<float *>(tmp_fl->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_fl;
					break;

				case Tango::DEV_BOOLEAN :					
					(*ring[index].attr_value_3)[0].value >>= tmp_boo;		
					new_tmp_boo = new DevVarBooleanArray(
							tmp_boo->length(),
							tmp_boo->length(),
							const_cast<DevBoolean *>(tmp_boo->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_boo;
					break;

				case Tango::DEV_USHORT :
					(*ring[index].attr_value_3)[0].value >>= tmp_ush;
					new_tmp_ush = new DevVarUShortArray(
							tmp_ush->length(),
							tmp_ush->length(),
							const_cast<DevUShort *>(tmp_ush->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_ush;
					break;

				case Tango::DEV_UCHAR :
					(*ring[index].attr_value_3)[0].value >>= tmp_uch;
					new_tmp_uch = new DevVarCharArray(
							tmp_uch->length(),
							tmp_uch->length(),
							const_cast<DevUChar *>(tmp_uch->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_uch;
					break;

				case Tango::DEV_ULONG :
					(*ring[index].attr_value_3)[0].value >>= tmp_ulg;
					new_tmp_ulg = new DevVarULongArray(tmp_ulg->length(),tmp_ulg->length(),
							const_cast<DevULong *>(tmp_ulg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg;
					break;
					
				case Tango::DEV_ULONG64 :
					(*ring[index].attr_value_3)[0].value >>= tmp_ulg64;
					new_tmp_ulg64 = new DevVarULong64Array(tmp_ulg64->length(),tmp_ulg64->length(),
							const_cast<DevULong64 *>(tmp_ulg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg64;
					break;
										
				case Tango::DEV_STATE :
				{
					CORBA::TypeCode_var ty;
					ty = (*ring[index].attr_value_3)[0].value.type();
			
					if (ty->kind() == CORBA::tk_enum)
					{
						Tango::DevState tmp_ds;
						(*ring[index].attr_value_3)[0].value >>= tmp_ds;
						(*ptr)[seq_index].value.value <<= tmp_ds;
					}
					else
					{
						(*ring[index].attr_value_3)[0].value >>= tmp_state;
						new_tmp_state = new DevVarStateArray(tmp_state->length(),tmp_state->length(),
									const_cast<DevState *>(tmp_state->get_buffer()),false);
						(*ptr)[seq_index].value.value <<= new_tmp_state;
					}
					break;
				}
				}
			}
		}
		else			
		{
			(*ptr)[seq_index].attr_failed = true;
			(*ptr)[seq_index].value.err_list = ring[index].except->errors;
			(*ptr)[seq_index].value.quality = Tango::ATTR_INVALID;
			clear_att_dim((*ptr)[seq_index].value);
		}
			
						
		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}
		
}

} // End of Tango namespace
