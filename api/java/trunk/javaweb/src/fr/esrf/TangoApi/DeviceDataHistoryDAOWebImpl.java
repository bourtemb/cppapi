//+======================================================================
// $Source$
//
// Project:   Tango
//
// Description:  java source code for the TANGO clent/server API.
//
// $Author$
//
// $Revision$
//
// $Log$
// Revision 3.11  2005/12/02 09:52:32  pascal_verdier
// java import have been optimized.
//
// Revision 3.10  2005/11/29 05:33:29  pascal_verdier
// extractUChar() and extractUCharArray() methods added.
//
// Revision 3.9  2005/06/13 09:05:18  pascal_verdier
// Minor bugs fixed.
//
// Revision 3.8  2004/12/16 10:16:44  pascal_verdier
// Missing TANGO 5 features added.
//
// Revision 3.7  2004/12/07 09:30:29  pascal_verdier
// Exception classes inherited from DevFailed added.
//
// Revision 3.6  2004/05/14 14:21:33  pascal_verdier
// Add timeout at runtime.
// Some little bugs fixed.
//
// Revision 3.5  2004/03/12 13:15:21  pascal_verdier
// Using JacORB-2.1
//
// Revision 3.0  2003/04/29 08:03:28  pascal_verdier
// Asynchronous calls added.
// Logging related methods.
// little bugs fixed.
//
// Revision 2.0  2003/01/09 14:00:37  verdier
// jacORB is now the ORB used.
//
// Revision 1.8  2002/06/26 09:02:17  verdier
// tested with atkpanel on a TACO device
//
// Revision 1.7  2002/04/09 12:21:51  verdier
// IDL 2 implemented.
//
//
// Copyright 2001 by European Synchrotron Radiation Facility, Grenoble, France
//               All Rights Reversed
//-======================================================================

package fr.esrf.TangoApi;


import fr.esrf.Tango.*;
import fr.esrf.TangoDs.Except;
import fr.esrf.TangoDs.TangoConst;
import fr.esrf.webapi.IWebImpl;
import fr.esrf.webapi.WebServerClientUtil;

import org.omg.CORBA.Any;
import org.omg.CORBA.TCKind;
import org.omg.CORBA.TypeCode;

/**
 * Class Description: This class manage data object for Tango device history
 * Data access. <Br>
 * <Br>
 * <Br>
 * <b> Usage example: </b> <Br>
 * <ul>
 * <i> DeviceDataHistory[] histo = dev.command_history("ReadCurrent", 10); <Br>
 * for (int i=0 ; i < histo.length ; i++) <Br> { <Br>
 * <ul>
 * Date d = new Date(histo[i].getTime()); <Br>
 * double[] values = histo[i].extractDoubleArray(); <Br>
 * </ul> } <Br>
 * </ul>
 * </i>
 * 
 * @author verdier
 * @version $Revision$
 */

public class DeviceDataHistoryDAOWebImpl implements IDeviceDataHistoryDAO, IWebImpl {
	private Object[] classParam = null;

	/**
	 * History from command history.
	 */
	public static final int COMMAND = 0;

	/**
	 * History from attribute history.
	 */
	public static final int ATTRIBUTE = 1;

	/**
	 * Data source DeviceDataHistory.COMMAND or DeviceDataHistory.ATTRIBUTE
	 */
	public int source;

	/**
	 * Command/Attribute name.
	 */
	public String name;

	/**
	 * true if command/attribute failed.
	 */
	public boolean failed;

	/**
	 * Error list if any in reading Command/Attribute.
	 */
	public DevError[] errors;

	// ===========================================
	/**
	 * Constructor from a DevCmdHistory.
	 */
	// ===========================================
	public void init(String cmdname, DevCmdHistory cmd_hist) throws DevFailed {
		classParam = new Object[] { cmdname, cmd_hist };
	}

	// ===========================================
	/**
	 * Constructor from an AttributeValue.
	 */
	// ===========================================
	public void init(DevAttrHistory att_histo) throws DevFailed {
		classParam = new Object[] { att_histo };
	}

	// ===========================================
	/**
	 * Constructor from an AttributeValue for Device_3Impl.
	 */
	// ===========================================
	public void init(DevAttrHistory_3 att_histo) throws DevFailed {
		classParam = new Object[] { att_histo };
	}

	// ===========================================
	/**
	 * Return attribute time value.
	 */
	// ===========================================
	public TimeVal getTimeVal() {
		try {
			return (TimeVal) WebServerClientUtil.getResponse(this, classParam, "getTimeVal", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * Return attribute time value in seconds since EPOCH.
	 */
	// ===========================================
	public long getTimeValSec() {
		try {
			return (Long) WebServerClientUtil.getResponse(this, classParam, "getTimeValSec", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * return time in milliseconds since EPOCH to build a Date class.
	 */
	// ===========================================
	public long getTime() {
		try {
			return (Long) WebServerClientUtil.getResponse(this, classParam, "getTime", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * return AttrQuality if from attribute.
	 */
	// ===========================================
	public AttrQuality getAttrQuality() throws DevFailed {
		return (AttrQuality) WebServerClientUtil.getResponse(this, classParam, "getAttrQuality", new Object[] {}, new Class[] {});

	}

	// ===========================================
	/**
	 * Return attribute dim_x if from attribute.
	 */
	// ===========================================
	public int getDimX() throws DevFailed {
		return (Integer) WebServerClientUtil.getResponse(this, classParam, "getDimX", new Object[] {}, new Class[] {});

	}

	// ===========================================
	/**
	 * Return attribute dim_y if from attribute.
	 */
	// ===========================================
	public int getDimY() throws DevFailed {
		return (Integer) WebServerClientUtil.getResponse(this, classParam, "getDimY", new Object[] {}, new Class[] {});

	}

	// ********** Extract Methods for basic types *********************

	// ===========================================
	/**
	 * extract method for a CORBA Any.
	 */
	// ===========================================
	public Any extractAny() {
		try {
			return (Any) WebServerClientUtil.getResponse(this, classParam, "extractAny", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a boolean.
	 */
	// ===========================================
	public boolean extractBoolean() {
		try {
			return (Boolean) WebServerClientUtil.getResponse(this, classParam, "extractBoolean", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return false;
		}
	}

	// ===========================================
	/**
	 * extract method for an unsigned char.
	 * 
	 * @return the extracted value.
	 */
	// ===========================================
	public short extractUChar() throws DevFailed {
		return (Short) WebServerClientUtil.getResponse(this, classParam, "extractUChar", new Object[] {}, new Class[] {});

	}

	// ===========================================
	/**
	 * extract method for a short.
	 */
	// ===========================================
	public short extractShort() {
		try {
			return (Short) WebServerClientUtil.getResponse(this, classParam, "extractShort", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * extract method for an unsigned short.
	 */
	// ===========================================
	public short extractUShort() {
		try {
			return (Short) WebServerClientUtil.getResponse(this, classParam, "extractUShort", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * extract method for a long.
	 */
	// ===========================================
	public int extractLong() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "extractLong", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * extract method for an unsigned long.
	 */
	// ===========================================
	public int extractULong() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "extractULong", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * extract method for a float.
	 */
	// ===========================================
	public float extractFloat() {
		try {
			return (Float) WebServerClientUtil.getResponse(this, classParam, "extractFloat", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * extract method for a double.
	 */
	// ===========================================
	public double extractDouble() {
		try {
			return (Double) WebServerClientUtil.getResponse(this, classParam, "extractDouble", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * extract method for a String.
	 */
	// ===========================================
	public String extractString() {
		try {
			return (String) WebServerClientUtil.getResponse(this, classParam, "extractString", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a DevState.
	 */
	// ===========================================
	public DevState extractDevState() {
		try {
			return (DevState) WebServerClientUtil.getResponse(this, classParam, "extractDevState", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ********** Extract Methods for sequence types *********************

	// ===========================================
	/**
	 * extract method for a byte Array.
	 */
	// ===========================================
	public boolean[] extractBooleanArray() {
		try {
			return (boolean[]) WebServerClientUtil.getResponse(this, classParam, "extractBooleanArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a byte Array.
	 */
	// ===========================================
	public byte[] extractByteArray() {
		try {
			return (byte[]) WebServerClientUtil.getResponse(this, classParam, "extractByteArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for an unsigned char Array.
	 * 
	 * @return the extracted value.
	 */
	// ===========================================
	public short[] extractUCharArray() throws DevFailed {
		return (short[]) WebServerClientUtil.getResponse(this, classParam, "extractUCharArray", new Object[] {}, new Class[] {});

	}

	// ===========================================
	/**
	 * extract method for a short Array.
	 */
	// ===========================================
	public short[] extractShortArray() {
		try {
			return (short[]) WebServerClientUtil.getResponse(this, classParam, "extractShortArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for an unsigned short Array.
	 */
	// ===========================================
	public short[] extractUShortArray() {
		try {
			return (short[]) WebServerClientUtil.getResponse(this, classParam, "extractUShortArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a long Array.
	 */
	// ===========================================
	public int[] extractLongArray() {
		try {
			return (int[]) WebServerClientUtil.getResponse(this, classParam, "extractLongArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for an unsigned long Array.
	 */
	// ===========================================
	public int[] extractULongArray() {
		try {
			return (int[]) WebServerClientUtil.getResponse(this, classParam, "extractULongArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a float Array.
	 */
	// ===========================================
	public float[] extractFloatArray() {
		try {
			return (float[]) WebServerClientUtil.getResponse(this, classParam, "extractFloatArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a double Array.
	 */
	// ===========================================
	public double[] extractDoubleArray() {
		try {
			return (double[]) WebServerClientUtil.getResponse(this, classParam, "extractDoubleArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a String Array.
	 */
	// ===========================================
	public String[] extractStringArray() {
		try {
			return (String[]) WebServerClientUtil.getResponse(this, classParam, "extractStringArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a DevVarLongStringArray.
	 */
	// ===========================================
	public DevVarLongStringArray extractLongStringArray() {
		try {
			return (DevVarLongStringArray) WebServerClientUtil.getResponse(this, classParam, "extractLongStringArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * extract method for a DevVarDoubleStringArray.
	 */
	// ===========================================
	public DevVarDoubleStringArray extractDoubleStringArray() {
		try {
			return (DevVarDoubleStringArray) WebServerClientUtil.getResponse(this, classParam, "extractDoubleStringArray", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * Returns the attribute errors list
	 */
	// ===========================================
	public DevError[] getErrStack() {
		try {
			return (DevError[]) WebServerClientUtil.getResponse(this, classParam, "getErrStack", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * Returns the attribute type
	 */
	// ===========================================
	public TypeCode type() {
		try {
			return (TypeCode) WebServerClientUtil.getResponse(this, classParam, "type", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	/**
	 * Return attribute name.
	 * 
	 * @throws DevFailed
	 *             in case of read_attribute failed
	 */
	// ===========================================
	public String getName() {
		try {
			return (String) WebServerClientUtil.getResponse(this, classParam, "getName", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return null;
		}
	}

	// ===========================================
	// ===========================================
	// TODO remove javadoc

	// ===========================================
	/**
	 * Return number of data read.
	 * 
	 * @throws DevFailed
	 *             in case of read_attribute failed
	 */
	// ===========================================
	public int getNbRead() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "getNbRead", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * Return number of data written.
	 * 
	 * @throws DevFailed
	 *             in case of read_attribute failed
	 */
	// ===========================================
	public int getNbWritten() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "getNbWritten", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * Return attribute written dim_x.
	 * 
	 * @throws DevFailed
	 *             in case of read_attribute failed
	 */
	// ===========================================
	public int getWrittenDimX() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "getWrittenDimX", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * Return attribute written dim_y.
	 * 
	 * @throws DevFailed
	 *             in case of read_attribute failed
	 */
	// ===========================================
	public int getWrittenDimY() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "getWrittenDimY", new Object[] {}, new Class[] {});

		} catch (DevFailed dfe) {
			dfe.printStackTrace();
			return 0;
		}
	}

	// ===========================================
	/**
	 * Returns attribute Tango type.
	 */
	// ===========================================
	public int getType() throws DevFailed {
		return (Integer) WebServerClientUtil.getResponse(this, classParam, "getType", new Object[] {}, new Class[] {});

	}

	public Object[] getClassParam() {
		return classParam;
	}

	public void setClassParam(Object[] classParam) {
		this.classParam = classParam;
	}
}
