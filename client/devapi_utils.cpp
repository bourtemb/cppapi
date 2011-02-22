static const char *RcsId = "$Id$";

//
// devapi_utils.cpp 	- C++ source code file for TANGO device api 
//
// programmer(s)	-Emmanuel Taurel(taurel@esrf.fr)
//
// original 		- November 2007
//
// log			- $Log$
// log			- Revision 1.1.2.1  2007/11/16 14:10:59  taurel
// log			- - Added a new IDL interface (Device_4)
// log			- - Added a new way to get attribute history from polling buffer (must faster)
// log			-

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

                                                    
using namespace CORBA;

namespace Tango
{


//-----------------------------------------------------------------------------
//
// DeviceProxy::from_hist4_2_AttHistory()
//
// Convert the attribute history as returned by a IDL 4 device to the classical 
// DeviceAttributeHistory format
//
//-----------------------------------------------------------------------------

void DeviceProxy::from_hist4_2_AttHistory(DevAttrHistory_4_var &hist_4,vector<DeviceAttributeHistory> *ddh)
{

//
// Check received data validity
//

	if ((hist_4->quals.length() != hist_4->quals_array.length()) ||
		(hist_4->r_dims.length() != hist_4->r_dims_array.length()) ||
		(hist_4->w_dims.length() != hist_4->w_dims_array.length()) ||
		(hist_4->errors.length() != hist_4->errors_array.length()))
	{
		Tango::Except::throw_exception((const char *)"API_WrongHistoryDataBuffer",
									   (const char *)"Data buffer received from server is not valid !",
									   (const char *)"DeviceProxy::from_hist4_2_AttHistory");
	}

//
// Get history depth
//

	unsigned int h_depth = hist_4->dates.length();

//
// Copy date and name in each History list element
//

	unsigned int loop;
	for (loop = 0;loop < h_depth;loop++)
	{
		(*ddh)[loop].time = hist_4->dates[loop];
		(*ddh)[loop].name = hist_4->name.in();
	}

//
// Copy the attribute quality factor
//

	unsigned int k;

	for (loop = 0;loop < hist_4->quals.length();loop++)
	{
		int nb_elt = hist_4->quals_array[loop].nb_elt;
		int start = hist_4->quals_array[loop].start;

		for (k = 0;k < nb_elt;k++)
			(*ddh)[start - k].quality = hist_4->quals[loop];
	}

//
// Copy read dimension
//

	for (loop = 0;loop < hist_4->r_dims.length();loop++)
	{
		int nb_elt = hist_4->r_dims_array[loop].nb_elt;
		int start = hist_4->r_dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].dim_x = hist_4->r_dims[loop].dim_x;
			(*ddh)[start - k].dim_y = hist_4->r_dims[loop].dim_y;
		}
	}

//
// Copy write dimension
//

	for (loop = 0;loop < hist_4->w_dims.length();loop++)
	{
		int nb_elt = hist_4->w_dims_array[loop].nb_elt;
		int start = hist_4->w_dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].ext->w_dim_x = hist_4->w_dims[loop].dim_x;
			(*ddh)[start - k].ext->w_dim_y = hist_4->w_dims[loop].dim_y;
		}
	}

//
// Copy errors
//

	for (loop = 0;loop < hist_4->errors.length();loop++)
	{
		int nb_elt = hist_4->errors_array[loop].nb_elt;
		int start = hist_4->errors_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].failed(true);
			(*ddh)[start - k].ext->err_list->length(hist_4->errors[loop].length());
			for (unsigned int g = 0;g < hist_4->errors[loop].length();g++)
			{
				(*ddh)[start - k].ext->err_list[g] = (hist_4->errors[loop])[g];
			}
		}
	}

//
// Get data type and data ptr
//

	const Tango::DevVarDoubleArray *tmp_db;
	const Tango::DevVarShortArray *tmp_sh;
	const Tango::DevVarLongArray *tmp_lg;
	const Tango::DevVarLong64Array *tmp_lg64;
	const Tango::DevVarStringArray *tmp_str;
	const Tango::DevVarFloatArray *tmp_fl;
	const Tango::DevVarBooleanArray *tmp_boo;
	const Tango::DevVarUShortArray *tmp_ush;
	const Tango::DevVarCharArray *tmp_uch;
	const Tango::DevVarULongArray *tmp_ulg;
	const Tango::DevVarULong64Array *tmp_ulg64;
	const Tango::DevVarStateArray *tmp_state;

	long data_type = -1;
	unsigned int seq_size;
	
	CORBA::TypeCode_var ty = hist_4->value.type();
	if (ty->kind() != tk_null)
	{	
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();

		switch (ty_seq->kind())
		{
			case tk_long:
			data_type = DEV_LONG;
			hist_4->value >>= tmp_lg;
			seq_size = tmp_lg->length();
			break;
			
			case tk_longlong:
			data_type = DEV_LONG64;
			hist_4->value >>= tmp_lg64;
			seq_size = tmp_lg64->length();
			break;
				
			case tk_short:
			data_type = DEV_SHORT;
			hist_4->value >>= tmp_sh;
			seq_size = tmp_sh->length();
			break;
			
			case tk_double:
			data_type = DEV_DOUBLE;
			hist_4->value >>= tmp_db;
			seq_size = tmp_db->length();
			break;
			
			case tk_string:
			data_type = DEV_STRING;
			hist_4->value >>= tmp_str;
			seq_size = tmp_str->length();
			break;
				
			case tk_float:	
			data_type = DEV_FLOAT;
			hist_4->value >>= tmp_fl;
			seq_size = tmp_fl->length();
			break;
			
			case tk_boolean:
			data_type = DEV_BOOLEAN;
			hist_4->value >>= tmp_boo;
			seq_size = tmp_boo->length();
			break;
			
			case tk_ushort:
			data_type = DEV_USHORT;
			hist_4->value >>= tmp_ush;
			seq_size = tmp_ush->length();
			break;
			
			case tk_octet:
			data_type = DEV_UCHAR;
			hist_4->value >>= tmp_uch;
			seq_size = tmp_uch->length();
			break;
	
			case tk_ulong:
			data_type = DEV_ULONG;
			hist_4->value >>= tmp_ulg;
			seq_size = tmp_ulg->length();
			break;
					
			case tk_ulonglong:
			data_type = DEV_ULONG64;
			hist_4->value >>= tmp_ulg64;
			seq_size = tmp_ulg64->length();
			break;
			
			case tk_enum:
			data_type = DEV_STATE;
			hist_4->value >>= tmp_state;
			seq_size = tmp_state->length();
			break;
		}
	}
		
//
// Copy data
//


	int base = seq_size;
	for (loop = 0;loop < h_depth;loop++)
	{

		if (((*ddh)[loop].failed() == true) || ((*ddh)[loop].quality == Tango::ATTR_INVALID))
			continue;

//
// Get the data length for this record
//
		
		int ind = h_depth - (loop + 1);

		int r_dim_x = (*ddh)[loop].dim_x;
		int r_dim_y = (*ddh)[loop].dim_y;
		int w_dim_x = (*ddh)[loop].ext->w_dim_x;
		int w_dim_y = (*ddh)[loop].ext->w_dim_y;

		int data_length;
		(r_dim_y == 0) ? data_length = r_dim_x : data_length = r_dim_x * r_dim_y;
		(w_dim_y == 0) ? data_length += w_dim_x : data_length += (w_dim_x * w_dim_y);

//
// Real copy now
//

		int ll;
		switch (data_type)
		{
			case DEV_SHORT:
			(*ddh)[loop].ShortSeq = new DevVarShortArray();
			(*ddh)[loop].ShortSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ShortSeq[ll] = (*tmp_sh)[(base - data_length) + ll];
			break;

			case DEV_LONG:
			(*ddh)[loop].LongSeq = new DevVarLongArray();
			(*ddh)[loop].LongSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].LongSeq[ll] = (*tmp_lg)[(base - data_length) + ll];
			break;
			
			case DEV_LONG64:
			(*ddh)[loop].ext->Long64Seq = new DevVarLong64Array();
			(*ddh)[loop].ext->Long64Seq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ext->Long64Seq[ll] = (*tmp_lg64)[(base - data_length) + ll];
			break;
			
			case DEV_FLOAT:
			(*ddh)[loop].FloatSeq = new DevVarFloatArray();
			(*ddh)[loop].FloatSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].FloatSeq[ll] = (*tmp_fl)[(base - data_length) + ll];
			break;
			
			case DEV_DOUBLE:
			(*ddh)[loop].DoubleSeq = new DevVarDoubleArray();
			(*ddh)[loop].DoubleSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].DoubleSeq[ll] = (*tmp_db)[(base - data_length) + ll];
			break;
			
			case DEV_STRING:
			(*ddh)[loop].StringSeq = new DevVarStringArray();
			(*ddh)[loop].StringSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].StringSeq[ll] = (*tmp_str)[(base - data_length) + ll];
			break;
			
			case DEV_BOOLEAN:
			(*ddh)[loop].BooleanSeq = new DevVarBooleanArray();
			(*ddh)[loop].BooleanSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].BooleanSeq[ll] = (*tmp_boo)[(base - data_length) + ll];
			break;
			
			case DEV_USHORT:
			(*ddh)[loop].UShortSeq = new DevVarUShortArray();
			(*ddh)[loop].UShortSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].UShortSeq[ll] = (*tmp_ush)[(base - data_length) + ll];
			break;
			
			case DEV_UCHAR:
			(*ddh)[loop].UCharSeq = new DevVarUCharArray();
			(*ddh)[loop].UCharSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].UCharSeq[ll] = (*tmp_uch)[(base - data_length) + ll];
			break;
			
			case DEV_ULONG:
			(*ddh)[loop].ext->ULongSeq = new DevVarULongArray();
			(*ddh)[loop].ext->ULongSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ext->ULongSeq[ll] = (*tmp_ulg)[(base - data_length) + ll];
			break;
			
			case DEV_ULONG64:
			(*ddh)[loop].ext->ULong64Seq = new DevVarULong64Array();
			(*ddh)[loop].ext->ULong64Seq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ext->ULong64Seq[ll] = (*tmp_ulg64)[(base - data_length) + ll];
			break;
			
			case DEV_STATE:
			(*ddh)[loop].ext->StateSeq = new DevVarStateArray();
			(*ddh)[loop].ext->StateSeq->length(data_length);
		
			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ext->StateSeq[ll] = (*tmp_state)[(base - data_length) + ll];
			break;
		}

		base = base - ll;

	}

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::from_hist4_2_DataHistory()
//
// Convert the command history as returned by a IDL 4 device to the classical 
// DeviceDataHistory format
//
//-----------------------------------------------------------------------------

void DeviceProxy::from_hist4_2_DataHistory(DevCmdHistory_4_var &hist_4,vector<DeviceDataHistory> *ddh)
{

//
// Check received data validity
//

	if ((hist_4->dims.length() != hist_4->dims_array.length()) ||
		(hist_4->errors.length() != hist_4->errors_array.length()))
	{
		Tango::Except::throw_exception((const char *)"API_WrongHistoryDataBuffer",
									   (const char *)"Data buffer received from server is not valid !",
									   (const char *)"DeviceProxy::from_hist4_2_DataHistory");
	}

//
// Get history depth
//

	unsigned int h_depth = hist_4->dates.length();

//
// Copy date in each history list element
//

	unsigned int loop,k;
	for (loop = 0;loop < h_depth;loop++)
	{
		(*ddh)[loop].set_date(hist_4->dates[loop]);
	}

//
// Copy errors
//

	for (loop = 0;loop < hist_4->errors.length();loop++)
	{
		int nb_elt = hist_4->errors_array[loop].nb_elt;
		int start = hist_4->errors_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].failed(true);
			DevErrorList_var del(&hist_4->errors[loop]);
			(*ddh)[start - k].errors(del);
			del._retn();
		}
	}

//
// Create a temporary sequence with record dimension
//

	Tango::AttributeDimList ad(h_depth);
	ad.length(h_depth);
	
	for (loop = 0;loop < hist_4->dims.length();loop++)
	{
		int nb_elt = hist_4->dims_array[loop].nb_elt;
		int start = hist_4->dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			ad[start - k].dim_x = hist_4->dims[loop].dim_x;
			ad[start - k].dim_y = hist_4->dims[loop].dim_y;
		}
	}
	
//
// Get data ptr and data size
//

	const Tango::DevVarDoubleArray *tmp_db;
	const Tango::DevVarShortArray *tmp_sh;
	const Tango::DevVarLongArray *tmp_lg;
	const Tango::DevVarLong64Array *tmp_lg64;
	const Tango::DevVarStringArray *tmp_str;
	const Tango::DevVarFloatArray *tmp_fl;
	const Tango::DevVarBooleanArray *tmp_boo;
	const Tango::DevVarUShortArray *tmp_ush;
	const Tango::DevVarCharArray *tmp_uch;
	const Tango::DevVarULongArray *tmp_ulg;
	const Tango::DevVarULong64Array *tmp_ulg64;
	const Tango::DevVarStateArray *tmp_state;
	const Tango::DevVarLongStringArray *tmp_lg_str;
	const Tango::DevVarDoubleStringArray *tmp_db_str;

	unsigned int seq_size;
	unsigned int seq_size_str;
	unsigned int seq_size_num;

	switch (hist_4->cmd_type)
	{
		case DEV_LONG:
		case DEVVAR_LONGARRAY:
		hist_4->value >>= tmp_lg;
		seq_size = tmp_lg->length();
		break;
		
		case DEV_LONG64:
		case DEVVAR_LONG64ARRAY:
		hist_4->value >>= tmp_lg64;
		seq_size = tmp_lg64->length();
		break;
			
		case DEV_SHORT:
		case DEVVAR_SHORTARRAY:
		hist_4->value >>= tmp_sh;
		seq_size = tmp_sh->length();
		break;
		
		case DEV_DOUBLE:
		case DEVVAR_DOUBLEARRAY:
		hist_4->value >>= tmp_db;
		seq_size = tmp_db->length();
		break;
		
		case DEV_STRING:
		case DEVVAR_STRINGARRAY:
		hist_4->value >>= tmp_str;
		seq_size = tmp_str->length();
		break;
			
		case DEV_FLOAT:
		case DEVVAR_FLOATARRAY:
		hist_4->value >>= tmp_fl;
		seq_size = tmp_fl->length();
		break;
		
		case DEV_BOOLEAN:
		hist_4->value >>= tmp_boo;
		seq_size = tmp_boo->length();
		break;
		
		case DEV_USHORT:
		case DEVVAR_USHORTARRAY:
		hist_4->value >>= tmp_ush;
		seq_size = tmp_ush->length();
		break;
		
		case DEVVAR_CHARARRAY:
		hist_4->value >>= tmp_uch;
		seq_size = tmp_uch->length();
		break;

		case DEV_ULONG:
		case DEVVAR_ULONGARRAY:
		hist_4->value >>= tmp_ulg;
		seq_size = tmp_ulg->length();
		break;
				
		case DEV_ULONG64:
		case DEVVAR_ULONG64ARRAY:
		hist_4->value >>= tmp_ulg64;
		seq_size = tmp_ulg64->length();
		break;
		
		case DEV_STATE:
		hist_4->value >>= tmp_state;
		seq_size = tmp_state->length();
		break;
		
		case DEVVAR_LONGSTRINGARRAY:
		hist_4->value >>= tmp_lg_str;
		seq_size_str = tmp_lg_str->svalue.length();
		seq_size_num = tmp_lg_str->lvalue.length();
		break;
		
		case DEVVAR_DOUBLESTRINGARRAY:
		hist_4->value >>= tmp_db_str;
		seq_size_str = tmp_db_str->svalue.length();
		seq_size_num = tmp_db_str->dvalue.length();
		break;
	}

//
// Copy data
//

	int base = seq_size;
	int base_str = seq_size_str;
	int base_num = seq_size_num;
	
	for (loop = 0;loop < h_depth;loop++)
	{

		if ((*ddh)[loop].failed() == true)
			continue;

//
// Get the data length for this record
//
		
		int ind = h_depth - (loop + 1);

		int data_length = ad[loop].dim_x;
		int data_num_length = ad[loop].dim_y;

//
// Real copy now
//

		int ll;
		switch (hist_4->cmd_type)
		{
			case Tango::DEV_SHORT:
			{
				Tango::DevShort tmp_data = (*tmp_sh)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_SHORTARRAY:
			{
				const CORBA::Short *c_seq_buff = tmp_sh->get_buffer();
				CORBA::Short *seq_buff = const_cast<CORBA::Short*>(c_seq_buff);
				Tango::DevVarShortArray ShortSeq = DevVarShortArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= ShortSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_LONG:
			{
				Tango::DevLong tmp_data = (*tmp_lg)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_LONGARRAY:
			{
				const CORBA::Long *c_seq_buff = tmp_lg->get_buffer();
				CORBA::Long *seq_buff = const_cast<CORBA::Long*>(c_seq_buff);
				Tango::DevVarLongArray LongSeq = DevVarLongArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= LongSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_FLOAT:
			{
				Tango::DevFloat tmp_data = (*tmp_fl)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_FLOATARRAY:
			{
				const CORBA::Float *c_seq_buff = tmp_fl->get_buffer();
				CORBA::Float *seq_buff = const_cast<CORBA::Float*>(c_seq_buff);
				Tango::DevVarFloatArray FloatSeq = DevVarFloatArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= FloatSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_DOUBLE:
			{
				Tango::DevDouble tmp_data = (*tmp_db)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_DOUBLEARRAY:
			{
				const CORBA::Double *c_seq_buff = tmp_db->get_buffer();
				CORBA::Double *seq_buff = const_cast<CORBA::Double*>(c_seq_buff);
				Tango::DevVarDoubleArray DoubleSeq = DevVarDoubleArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= DoubleSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_LONG64:
			{
				Tango::DevLong64 tmp_data = (*tmp_lg64)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_LONG64ARRAY:
			{
				const CORBA::LongLong *c_seq_buff = tmp_lg64->get_buffer();
				CORBA::LongLong *seq_buff = const_cast<CORBA::LongLong*>(c_seq_buff);
				Tango::DevVarLong64Array Long64Seq = DevVarLong64Array(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= Long64Seq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_STRING:
			{
				Tango::ConstDevString tmp_data = (*tmp_str)[base - 1].in();
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_STRINGARRAY:
			{
				const Tango::ConstDevString *c_seq_buff = tmp_str->get_buffer();
				char **seq_buff = const_cast<char **>(c_seq_buff);
				Tango::DevVarStringArray StrSeq = DevVarStringArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= StrSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_BOOLEAN:
			{
				Tango::DevBoolean tmp_data = (*tmp_str)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CORBA::Any::from_boolean(tmp_data);
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_USHORT:
			{
				Tango::DevUShort tmp_data = (*tmp_ush)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CORBA::Any::from_boolean(tmp_data);
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_USHORTARRAY:
			{
				const Tango::DevUShort *c_seq_buff = tmp_ush->get_buffer();
				Tango::DevUShort *seq_buff = const_cast<Tango::DevUShort*>(c_seq_buff);
				Tango::DevVarUShortArray UshSeq = DevVarUShortArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= UshSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_ULONG:
			{
				Tango::DevULong tmp_data = (*tmp_ulg)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CORBA::Any::from_boolean(tmp_data);
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_ULONGARRAY:
			{
				const Tango::DevULong *c_seq_buff = tmp_ulg->get_buffer();
				Tango::DevULong *seq_buff = const_cast<Tango::DevULong*>(c_seq_buff);
				Tango::DevVarULongArray UlgSeq = DevVarULongArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= UlgSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_ULONG64:
			{
				Tango::DevULong64 tmp_data = (*tmp_ulg64)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CORBA::Any::from_boolean(tmp_data);
				(*ddh)[loop].any = any_ptr;
			}
			break;
		
			case Tango::DEVVAR_ULONG64ARRAY:
			{
				const Tango::DevULong64 *c_seq_buff = tmp_ulg64->get_buffer();
				Tango::DevULong64 *seq_buff = const_cast<Tango::DevULong64*>(c_seq_buff);
				Tango::DevVarULong64Array Ulg64Seq = DevVarULong64Array(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= Ulg64Seq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEVVAR_CHARARRAY:
			{
				const Tango::DevUChar *c_seq_buff = tmp_uch->get_buffer();
				Tango::DevUChar *seq_buff = const_cast<Tango::DevUChar*>(c_seq_buff);
				Tango::DevVarCharArray CharSeq = DevVarCharArray(data_length,data_length,&(seq_buff[base - data_length]),false);
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CharSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEV_STATE:
			{
				Tango::DevState tmp_data = (*tmp_state)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEVVAR_LONGSTRINGARRAY:
			{
				Tango::DevVarLongStringArray *dvlsa = new Tango::DevVarLongStringArray();
				dvlsa->svalue.length(data_length);
				dvlsa->lvalue.length(data_num_length);

				unsigned int ll;
				for (ll = 0;ll < data_length;ll++)
					dvlsa->svalue[ll] = tmp_lg_str->svalue[(base_str - data_length) + ll];
				for (ll = 0;ll < data_num_length;ll++)
					dvlsa->lvalue[ll] = tmp_lg_str->lvalue[(base_num - data_num_length) + ll];
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= dvlsa;
				(*ddh)[loop].any = any_ptr;
			}
			break;
			
			case Tango::DEVVAR_DOUBLESTRINGARRAY:
			{
				Tango::DevVarDoubleStringArray *dvdsa = new Tango::DevVarDoubleStringArray();
				dvdsa->svalue.length(data_length);
				dvdsa->dvalue.length(data_num_length);

				unsigned int ll;
				for (ll = 0;ll < data_length;ll++)
					dvdsa->svalue[ll] = tmp_db_str->svalue[(base_str - data_length) + ll];
				for (ll = 0;ll < data_num_length;ll++)
					dvdsa->dvalue[ll] = tmp_db_str->dvalue[(base_num - data_num_length) + ll];
			
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= dvdsa;
				(*ddh)[loop].any = any_ptr;
			}
			break;
		}

		if ((hist_4->cmd_type == DEVVAR_LONGSTRINGARRAY) || (hist_4->cmd_type == DEVVAR_DOUBLESTRINGARRAY))
		{
			base_str = base_str - data_length;
			base_num = base_num - data_num_length;
		}
		else
			base = base - data_length;

	}
}


} // End of namespace
