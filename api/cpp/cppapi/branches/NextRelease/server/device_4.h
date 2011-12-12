//=============================================================================
//
// file :		Device.h
//
// description :	Include for the Device root classes.
//			Three classes are declared in this file :
//				The Device class
//				The DeviceClass class
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//
//=============================================================================

#ifndef _DEVICE_4_H
#define _DEVICE_4_H

#include <tango.h>

namespace Tango
{


//=============================================================================
//
//			The Device_4Impl class
//
//
// description :	This class is derived directly from the Tango::Device_skel
//			class generated by CORBA. It represents the CORBA
//			servant which will be accessed by the client.
//			It implements all the methods
//			and attributes defined in the IDL interface for Device.
//
//=============================================================================


/**
 * Base class for all TANGO device since version 4.
 *
 * This class inherits from DeviceImpl class which itself inherits from
 * CORBA classes where all the network layer is implemented.
 * This class has been created since release 7 of Tango library where the IDL
 * Tango module has been modified in order to create a Device_4 interface
 * which inherits from the original Device interface
 *
 * $Author$
 * $Revision$
 */

class Device_4Impl : public virtual POA_Tango::Device_4,
		     public Device_3Impl
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Device_4Impl object from its name.
 *
 * The device description field is set to <i>A Tango device</i>. The device
 * state is set to unknown and the device status is set to
 * <b>Not Initialised</b>
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 *
 */
	Device_4Impl(DeviceClass *device_class,string &dev_name);

/**
 * Constructs a newly allocated Device_4Impl object from its name and its description.
 *
 * The device
 * state is set to unknown and the device status is set to
 * <i>Not Initialised</i>
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 * @param	desc	The device description
 *
 */
	Device_4Impl(DeviceClass *device_class,string &dev_name,string &desc);

/**
 * Constructs a newly allocated Device_4Impl object from all its creation
 * parameters.
 *
 * The device is constructed from its name, its description, an original state
 * and status
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 * @param	desc 		The device description
 * @param	dev_state 	The device initial state
 * @param	dev_status	The device initial status
 *
 */
	Device_4Impl(DeviceClass *device_class,
	           string &dev_name,string &desc,
	           Tango::DevState dev_state,string &dev_status);

/**
 * Constructs a newly allocated Device_4Impl object from all its creation
 * parameters with some default values.
 *
 * The device is constructed from its name, its description, an original state
 * and status. This constructor defined default values for the description,
 * state and status parameters. The default device description is <i>A TANGO device</i>.
 * The default device state is <i>UNKNOWN</i> and the default device status
 * is <i>Not initialised</i>.
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 * @param	desc	The device desc
 * @param	dev_state 	The device initial state
 * @param	dev_status	The device initial status
 *
 */
	Device_4Impl(DeviceClass *device_class,
	           const char *dev_name,const char *desc = "A TANGO device",
	           Tango::DevState dev_state = Tango::UNKNOWN,
	           const char *dev_status = StatusNotSet);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The device desctructor.
 */
	virtual ~Device_4Impl() {delete ext_4;}
//@}


/**@name CORBA operation methods
 * Method defined to implement TANGO device CORBA operation */
//@{

/**
 * Read attribute value history.
 *
 * Invoked when the client request the read_attribute_history_3 CORBA operation.
 * This operation allows a client to retrieve attribute value history for
 * polled attribute. The depth of the history is limited to the depth of
 * the device server internal polling buffer.
 * It returns to the client one DevAttrHistory structure for each record.
 *
 * @param name The attribute name
 * @param n The record number.
 * @return A DevAttrHistory_4 structure. This strucure contains all the element
 * allowing a client to retrieve for each history record the attribute value,
 * the date and in case of the attribute
 * returns an error when it was read, the DevErrors data.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevAttrHistory_4">here</a>
 * to read <b>DevAttrHistory</b> structure definition.
 * @exception DevFailed Thrown if the attribute does not exist or is not polled.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual Tango::DevAttrHistory_4 *read_attribute_history_4(const char* name,
								  CORBA::Long n)
	throw(Tango::DevFailed, CORBA::SystemException);

/**
 * Read command value history.
 *
 * Invoked when the client request the command_inout_history_2 CORBA operation.
 * This operation allows a client to retrieve command return value history for
 * polled command. The depth of the history is limited to the depth of
 * the device server internal polling buffer.
 * It returns to the client one DevCmdHistory structure for each record.
 *
 * @param command The command name
 * @param n The record number.
 * @return A DevCmdHistory_4 structure. This strucure contains all the element
 * allowing a client to retrieve for each history record the command return value
 * (in an Any), the date and in case of the command returns an error when it was read, the
 * DevErrors data.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevCmdHistory_4">here</a>
 * to read <b>DevCmdHistory</b> structure definition.
 * @exception DevFailed Thrown if the attribute does not exist or is not polled.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	virtual Tango::DevCmdHistory_4 *command_inout_history_4(const char* command,
								CORBA::Long n)
	throw(Tango::DevFailed, CORBA::SystemException);

/**
 * Execute a command.
 *
 * It's the master method executed when a "command_inout_4" CORBA operation is
 * requested by a client. It checks the device lock, updates the device black-box, call the
 * TANGO command handler and returned the output Any
 *
 * @param in_cmd The command name
 * @param in_data The command input data packed in a CORBA Any
 * @param source The data source. This parameter is new in Tango release 2. It
 * allows a client to choose the data source between the device itself or the
 * data cache for polled command.
 * @param cl_ident The client identificator. This parameter is new in release 4.
 * It allows device locking feature implemented in Tango V7
 * @return The command output data packed in a CORBA Any object
 * @exception DevFailed Re-throw of the exception thrown by the command_handler
 * method.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual CORBA::Any *command_inout_4(const char *in_cmd,
					    const CORBA::Any &in_data,
					    Tango::DevSource source,
					    const Tango::ClntIdent &cl_ident)
	throw (Tango::DevFailed, CORBA::SystemException);

/**
 * Read attribute(s) value.
 *
 * Invoked when the client request the read_attributes_2 CORBA operation.
 * It returns to the client one AttributeValue structure for each wanted
 * attribute.
 *
 * @param names The attribute(s) name list
 * @param source The data source. This parameter is new in Tango release 2. It
 * allows a client to choose the data source between the device itself or the
 * data cache for polled attribute.
 * @param cl_ident The client identificator. This parameter is new in release 4.
 * It allows device locking feature implemented in Tango V7
 * @return A sequence of AttributeValue_4 structure. One structure is initialised
 * for each wanted attribute with the attribute value, the date and the attribute
 * value quality. Click <a href="../../../tango_idl/idl_html/_Tango.html#AttributeValue_4">here</a>
 * to read <b>AttributeValue</b> structure definition.
 * @exception DevFailed Thrown if the attribute does not exist.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual Tango::AttributeValueList_4 *read_attributes_4(const Tango::DevVarStringArray& names,
								     Tango::DevSource source,
									 const Tango::ClntIdent &cl_ident)
								     throw (Tango::DevFailed, CORBA::SystemException);

/**
 * Write attribute(s) value.
 *
 * Invoked when the client request the write_attributes CORBA operation.
 * It sets the attribute(s) with the new value(s) passed as parameter.
 *
 * @param values The attribute(s) new value(s). One structure is initialised
 * for each wanted attribute with the attribute value. The attribute quality and
 * date are not used by this method.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#AttributeValue_4">here</a>
 * to read <b>AttributeValue_4</b> structure definition.
 * @param cl_ident The client identificator. This parameter is new in release 4.
 * It allows device locking feature implemented in Tango V7
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual void write_attributes_4(const Tango::AttributeValueList_4 &values,
    									const Tango::ClntIdent &cl_ident)
    			throw (Tango::MultiDevFailed, Tango::DevFailed,CORBA::SystemException);

/**
 * Set attribute(s) configuration.
 *
 * Invoked when the client request the set_attribute_config_3 CORBA operation.
 * It updates the device attribute configuration actually used by the device but
 * this method also updates the Tango database. One structure of the
 * AttributeConfig_3 type is needed for each attribute to update configuration.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#AttributeConfig_3">here</a>
 * to read <b>AttributeConfig_3</b> structure specification.
 *
 * @param new_conf The attribute(s) new configuration structure sequence
 * @param cl_ident The client identificator. This parameter is new in release 4.
 * It allows device locking feature implemented in Tango V7
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual void set_attribute_config_4(const Tango::AttributeConfigList_3& new_conf,
    										const Tango::ClntIdent &cl_ident)
    		throw (Tango::DevFailed, CORBA::SystemException);

/**
 * Write then read attribute(s) value.
 *
 * Invoked when the client request the write_read_attributes CORBA operation.
 * It sets the attribute(s) with the new value(s) passed as parameter
 * and then read the attribute(s) and return the read value to the caller.
 *
 * @param values The attribute(s) new value(s). One structure is initialised
 * for each wanted attribute with the attribute value. The attribute quality and
 * date are not used by this method.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#AttributeValue_4">here</a>
 * to read <b>AttributeValue</b> structure definition.
 * @param cl_ident The client identificator. This parameter is new in release 4.
 * It allows device locking feature implemented in Tango V7
 * @return A sequence of AttributeValue_4 structure. One structure is initialised
 * for each wanted attribute with the attribute value, the date and the attribute
 * value quality. Click <a href="../../../tango_idl/idl_html/_Tango.html#AttributeValue_4">here</a>
 * to read <b>AttributeValue_4</b> structure definition.
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual Tango::AttributeValueList_4* write_read_attributes_4(const Tango::AttributeValueList_4 &values,
    										  const Tango::ClntIdent &cl_ident)
    		throw (Tango::MultiDevFailed,Tango::DevFailed, CORBA::SystemException);

//@}

public:

private:
    class Device_4ImplExt
    {
    public:
        Device_4ImplExt() {}
        ~Device_4ImplExt() {}
    };

	Device_4ImplExt				*ext_4;

};

} // End of Tango namespace

#endif // _DEVICE_4_H
