static const char *RcsId = "$Header$";

//+============================================================================
//
// file :               Command.cpp
//
// description :        C++ source code for the Command and templCommand classes.
//			The Command class is the root class for all derived 
//			Command classes. The TemplCommand class is a template
//			command class use for command which does take input 
//			nor outout parameters.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:29  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <new>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Command::Command 
// 
// description : 	constructors for abstract class Command
//
//--------------------------------------------------------------------------

Command::Command(const char *s,
		 Tango::CmdArgType in,
		 Tango::CmdArgType out):name(s),in_type(in),out_type(out)
{
}

Command::Command(string &s,
		 Tango::CmdArgType in,
		 Tango::CmdArgType out):name(s),in_type(in),out_type(out)
{
}

Command::Command(const char *s,
		 Tango::CmdArgType in,
		 Tango::CmdArgType out,
		 const char *in_desc,
		 const char *out_desc):name(s),in_type(in),out_type(out)
{
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
}

Command::Command(string &s,
		 Tango::CmdArgType in,
		 Tango::CmdArgType out,
		 string &in_desc,
		 string &out_desc):name(s),in_type(in),out_type(out),
		 		       in_type_desc(in_desc),
				       out_type_desc(out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		Command::extract()
// 
// description : 	Command extract methods. These are very simple methods
//			but overloaded many times for all Tango types.
//
//-----------------------------------------------------------------------------

void Command::throw_bad_type(const char *type)
{
	TangoSys_OMemStream o;
	
	o << "Incompatible command argument type, expected type is : Tango::" << type << ends;
	Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
			      o.str(),
			      (const char *)"Command::extract()");
}

void Command::extract(const CORBA::Any &in,Tango::DevBoolean &data)
{
	if ((in >>= CORBA::Any::to_boolean(data)) == false)
		throw_bad_type("DevBoolean");
}

void Command::extract(const CORBA::Any &in,Tango::DevShort &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevShort");
}

void Command::extract(const CORBA::Any &in,Tango::DevLong &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevLong");
}

void Command::extract(const CORBA::Any &in,Tango::DevFloat &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevFloat");
}

void Command::extract(const CORBA::Any &in,Tango::DevDouble &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevDouble");
}

void Command::extract(const CORBA::Any &in,Tango::DevUShort &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevUShort");
}

void Command::extract(const CORBA::Any &in,Tango::DevULong &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevULong");
}

void Command::extract(const CORBA::Any &in,Tango::DevString &data)
{
	if ((in >>= const_cast<const char *&>(data)) == false)
		throw_bad_type("DevString");
}

void Command::extract(const CORBA::Any &in,const char *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("ConstDevString");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarCharArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarCharArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarShortArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarShortArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarLongArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarLongArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarFloatArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarFloatArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarDoubleArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarDoubleArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarUShortArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarUShortArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarULongArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarULongArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarStringArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarStringArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarLongStringArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarLongStringArray");
}

void Command::extract(const CORBA::Any &in,const Tango::DevVarDoubleStringArray *&data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevVarDoubleStringArray");
}

void Command::extract(const CORBA::Any &in,Tango::DevState &data)
{
	if ((in >>= data) == false)
		throw_bad_type("DevState");
}

//+----------------------------------------------------------------------------
//
// method : 		Command::insert()
// 
// description : 	Command insert methods. These are very simple methods
//			but overloaded many times for all Tango types.
//
//-----------------------------------------------------------------------------

void Command::alloc_any(CORBA::Any *&any_ptr)
{
	try
	{
		any_ptr = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"Command::alloc_any()");
	}
}

CORBA::Any *Command::insert()
{
	CORBA::Any *out_any;
	alloc_any(out_any);	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevBoolean data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	CORBA::Any::from_boolean tmp(data);
	
	(*out_any) <<= tmp;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevShort data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}
	
CORBA::Any *Command::insert(Tango::DevLong data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevFloat data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevDouble data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevUShort data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}
	
CORBA::Any *Command::insert(Tango::DevULong data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevString data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;
	delete [] data;
		
	return out_any;
}

CORBA::Any *Command::insert(const char *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarCharArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarCharArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarShortArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarShortArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarLongArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarLongArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarFloatArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarFloatArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarDoubleArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarDoubleArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarUShortArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarUShortArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarULongArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarULongArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarStringArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarStringArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarLongStringArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarLongStringArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarDoubleStringArray *data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevVarDoubleStringArray &data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;		
	return out_any;
}

CORBA::Any *Command::insert(Tango::DevState data)
{
	CORBA::Any *out_any;
	alloc_any(out_any);
	
	(*out_any) <<= data;	
	return out_any;
}


//+-------------------------------------------------------------------------
//
// method : 		TempCommand class constructors
// 
// description : 	instance constructor
//
//--------------------------------------------------------------------------


TemplCommand::TemplCommand(const char *s,
			   void (DeviceImpl::*f)())
			   :exe_ptr(f)
{
	name = s;
	allowed_ptr = NULL;
	in_type = out_type = Tango::DEV_VOID;
}

TemplCommand::TemplCommand(const char *s,
			   void (DeviceImpl::*f)(),
			   bool (DeviceImpl::*a)(const CORBA::Any &))
			   :exe_ptr(f),allowed_ptr(a)
{
	name = s;
	in_type = out_type = Tango::DEV_VOID;
}

TemplCommand::TemplCommand(string &s,
			   void (DeviceImpl::*f)())
			   :exe_ptr(f)
{
	name = s;
	in_type = out_type = Tango::DEV_VOID;
	allowed_ptr = NULL;
}

TemplCommand::TemplCommand(string &s,
			   void (DeviceImpl::*f)(),
			   bool (DeviceImpl::*a)(const CORBA::Any &))
			   :exe_ptr(f),allowed_ptr(a)
{
	name = s;
	in_type = out_type = Tango::DEV_VOID;
}

TemplCommand::TemplCommand(const char *s,
			   void (DeviceImpl::*f)(),
			   const char *in_desc,const char *out_desc)
			   :exe_ptr(f)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	in_type = out_type = Tango::DEV_VOID;
}

TemplCommand::TemplCommand(const char *s,
			   void (DeviceImpl::*f)(),
			   bool (DeviceImpl::*a)(const CORBA::Any &),
			   const char *in_desc,const char *out_desc)
			   :exe_ptr(f),allowed_ptr(a)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	in_type = out_type = Tango::DEV_VOID;
}

TemplCommand::TemplCommand(string &s,
			   void (DeviceImpl::*f)(),
			   bool (DeviceImpl::*a)(const CORBA::Any &),
			   string &in_desc,string &out_desc)
			   :exe_ptr(f),allowed_ptr(a)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	in_type = out_type = Tango::DEV_VOID;
}

TemplCommand::TemplCommand(string &s,
			   void (DeviceImpl::*f)(),
			   string &in_desc,string &out_desc)
			   :exe_ptr(f)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	in_type = out_type = Tango::DEV_VOID;
	allowed_ptr = NULL;
}


//+-------------------------------------------------------------------------
//
// method : 		set_type
// 
// description : 	Set the Command class type data according to the type
//			of the object passed as parameters
//
// input : - data_type : reference to the type_info object of the parameter
//	   - type : reference to the Command class type data
//
//--------------------------------------------------------------------------

void TemplCommand::set_type(const type_info &data_type,Tango::CmdArgType &type)
{
	if (data_type == typeid(void))
	{
		cout4 << "Command : " << name << ", Type is void" << endl;
		type = Tango::DEV_VOID;
	}	
	else if (data_type == typeid(Tango::DevBoolean))
	{
		cout4 << "Command : " << name << ", Type is a boolean" << endl;
		type = Tango::DEV_BOOLEAN;
	}
	else if (data_type == typeid(Tango::DevShort))
	{
		cout4 << "Command : " << name << ", Type is a short" << endl;
		type = Tango::DEV_SHORT;
	}
	else if (data_type == typeid(Tango::DevLong))
	{
		cout4 << "Command : " << name << ", Type is a long" << endl;
		type = Tango::DEV_LONG;
	}
	else if (data_type == typeid(Tango::DevFloat))
	{
		cout4 << "Command : " << name << ", Type is a float" << endl;
		type = Tango::DEV_FLOAT;
	}
	else if (data_type == typeid(Tango::DevDouble))
	{
		cout4 << "Command : " << name << ", Type is a double" << endl;
		type = Tango::DEV_DOUBLE;
	}
	else if (data_type == typeid(Tango::DevUShort))
	{
		cout4 << "Command : " << name << ", Type is an unsigned short" << endl;
		type = Tango::DEV_USHORT;
	}
	else if (data_type == typeid(Tango::DevULong))
	{
		cout4 << "Command : " << name << ", Type is an unsigned long" << endl;
		type = Tango::DEV_ULONG;
	}
	else if (data_type == typeid(Tango::DevString))
	{
		cout4 << "Command : " << name << ", Type is a string" << endl;
		type = Tango::DEV_STRING;
	}
	else if ((data_type == typeid(Tango::DevVarCharArray)) || 
		 (data_type == typeid(const Tango::DevVarCharArray *)) ||
		 (data_type == typeid(Tango::DevVarCharArray *)))
	{
		cout4 << "Command : " << name << ", Type is a char array" << endl;
		type = Tango::DEVVAR_CHARARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarShortArray)) ||
		 (data_type == typeid(const Tango::DevVarShortArray *)) ||
		 (data_type == typeid(Tango::DevVarShortArray *)))
	{
		cout4 << "Command : " << name << ", Type is a short array" << endl;
		type = Tango::DEVVAR_SHORTARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarLongArray)) ||
		 (data_type == typeid(const Tango::DevVarLongArray *)) ||
		 (data_type == typeid(Tango::DevVarLongArray *)))
	{
		cout4 << "Command : " << name << ", Type is a long array" << endl;
		type = Tango::DEVVAR_LONGARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarFloatArray)) ||
		 (data_type == typeid(const Tango::DevVarFloatArray *)) ||
		 (data_type == typeid(Tango::DevVarFloatArray *)))
	{
		cout4 << "Command : " << name << ", Type is a float array" << endl;
		type = Tango::DEVVAR_FLOATARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarDoubleArray)) ||
		 (data_type == typeid(const Tango::DevVarDoubleArray *)) ||
		 (data_type == typeid(Tango::DevVarDoubleArray *)))
	{
		cout4 << "Command : " << name << ", Type is a double array" << endl;
		type = Tango::DEVVAR_DOUBLEARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarUShortArray)) ||
		 (data_type == typeid(const Tango::DevVarUShortArray *)) ||
		 (data_type == typeid(Tango::DevVarUShortArray *)))
	{
		cout4 << "Command : " << name << ", Type is a unsigned short array" << endl;
		type = Tango::DEVVAR_USHORTARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarULongArray)) ||
		 (data_type == typeid(const Tango::DevVarULongArray *)) ||
		 (data_type == typeid(Tango::DevVarULongArray *)))
	{
		cout4 << "Command : " << name << ", Type is a undigned long array" << endl;
		type = Tango::DEVVAR_ULONGARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarStringArray)) ||
		 (data_type == typeid(const Tango::DevVarStringArray *)) ||
		 (data_type == typeid(Tango::DevVarStringArray *)))
	{
		cout4 << "Command : " << name << ", Type is a string array" << endl;
		type = Tango::DEVVAR_STRINGARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarLongStringArray)) ||
		 (data_type == typeid(const Tango::DevVarLongStringArray *)) ||
		 (data_type == typeid(Tango::DevVarLongStringArray *)))
	{
		cout4 << "Command : " << name << ", Type is a long + string array" << endl;
		type = Tango::DEVVAR_LONGSTRINGARRAY;
	}
	else if ((data_type == typeid(Tango::DevVarDoubleStringArray)) ||
		 (data_type == typeid(const Tango::DevVarDoubleStringArray *)) ||
		 (data_type == typeid(Tango::DevVarDoubleStringArray *)))
	{
		cout4 << "Command : " << name << ", Type is a double + string array" << endl;
		type = Tango::DEVVAR_DOUBLESTRINGARRAY;
	}
	else if (data_type == typeid(Tango::DevState))
	{
		cout4 << "Command : " << name << ", Type is a DevState" << endl;
		type = Tango::DEV_STATE;
	}
	else
	{
		cout4 << "Command : " << name << ", Unknown type" << endl;
		TangoSys_OMemStream o;
		
		o << "Command " << name << " defined with an unsuported type"  << ends;
		Except::throw_exception((const char *)"API_CmdArgumentTypeNotSupported",
				      o.str(),(const char *)"TemplCommand::set_type");
	}	
}

//+-------------------------------------------------------------------------
//
// method : 		is_allowed
// 
// description : 	Check if the command is allowed. If the pointer to 
//			DeviceImpl class method "allowed_ptr" is null, the 
//			default mode id used (command always executed). 
//			Otherwise, the method is executed
//
// input : - dev_ptr : pointer to the device on which the command must be
//		       executed
//	   - in_any : Incoming command data
//
// This method returns a boolean set to true if the command is allowed
//
//--------------------------------------------------------------------------

bool TemplCommand::is_allowed(DeviceImpl *dev_ptr,const CORBA::Any &in_any)
{
#ifdef __hpux
	if (allowed_ptr == NULL)
#else
	if (allowed_ptr == (bool (DeviceImpl::*)(const CORBA::Any &))NULL)
#endif
		return true;
	else
		return ((dev_ptr->*allowed_ptr)(in_any));
}

//+-------------------------------------------------------------------------
//
// method : 		execute
// 
// description : 	Execute the method associated with the command
//			(stored in the exe_ptr data)
//
// input : - dev_ptr : pointer to the device on which the command must be
//		       executed
//	   - in_any : Incoming command data
//
// This method returns a pointer to an CORBA::Any object with the command outing
// data.
//
//--------------------------------------------------------------------------

CORBA::Any *TemplCommand::execute(DeviceImpl *dev_ptr,const CORBA::Any &in_any)
{

//
// Execute the command associated method
//

	(dev_ptr->*exe_ptr)();
	return insert();
}

} // End of Tango namespace
