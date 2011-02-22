package fr.esrf.TangoApi;

import fr.esrf.Tango.DevFailed;
import fr.esrf.Tango.DevInfo;
import fr.esrf.Tango.DevInfo_3;
import fr.esrf.Tango.DevSource;
import fr.esrf.Tango.Device;
import fr.esrf.TangoApi.CommandInfo;
import fr.esrf.TangoApi.DeviceData;
import fr.esrf.TangoApi.IConnectionDAO;
import fr.esrf.TangoApi.TangoUrl;
import fr.esrf.webapi.IWebImpl;
import fr.esrf.webapi.WebServerClientUtil;

public class ConnectionDAOWebImpl implements IConnectionDAO, IWebImpl {
	private String m_strHost = null;

	private String m_strPort = null;

	private boolean m_bTransparent_reconnection = true;

	private Object[] classParam = null;

	public ConnectionDAOWebImpl() {
	}

	public void init() {
		classParam = new Object[] {};
	}

	public void init(String strHost, String strPort) {
		classParam = new Object[] { strHost, strPort };
		m_strHost = strHost;
		m_strPort = strPort;
	}

	public void init(String strHost, String strPort, boolean bAuto_reconnect) {
		classParam = new Object[] { strHost, strPort, bAuto_reconnect };
		m_strHost = strHost;
		m_strPort = strPort;
		m_bTransparent_reconnection = bAuto_reconnect;
	}

	public void init(String devname) throws DevFailed {
		classParam = new Object[] { devname };
	}

	public void init(String devname, boolean check_access) throws DevFailed {
		classParam = new Object[] { devname, check_access };
	}

	public void init(String devname, String param, int src) throws DevFailed {
		classParam = new Object[] { devname, param, src };
	}

	public void init(String devname, String host, String port) throws DevFailed {
		classParam = new Object[] { devname, host, port };
		m_strHost = host;
		m_strPort = port;		
	}

	// ==========================================================================
	/**
	 * return the name of connection (host:port)
	 */
	// ==========================================================================
	public String get_tango_host() throws DevFailed {
		return m_strHost + ":" + m_strPort;
	}

	public String getHost() {
		return m_strHost;
	}

	public void setHost(String host) {
		m_strHost = host;
	}

	public String getPort() {
		return m_strPort;
	}

	public void setPort(String port) {
		m_strPort = port;
	}

	public Device get_device() {
		try {
			return (Device) WebServerClientUtil.getResponse(this, classParam, "get_device", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
	}

	/**
	 * Execute a ping command to a device server.
	 * 
	 * @return the elapsed time for ping command in microseconds.
	 */
	// ===========================================================
	public long ping() throws DevFailed {
		return (Long) WebServerClientUtil.getResponse(this, new Object[] { m_strHost, m_strPort, m_bTransparent_reconnection }, "ping", new Object[] {}, new Class[] {});
	}

	public void build_connection() throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "build_connection", new Object[] {}, new Class[] {});

	}

	public void checkIfTaco(String cmdname) throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "checkIfTaco", new Object[] { cmdname }, new Class[] { String.class });

	}

	public void checkIfTango(String cmdname) throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "checkIfTango", new Object[] { cmdname }, new Class[] { String.class });
	}

	public void dev_import() throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "dev_import", new Object[] {}, new Class[] {});
	}

	public void throw_dev_failed(Exception e, String command, boolean from_inout_cmd) throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "throw_dev_failed", new Object[] { e, command, from_inout_cmd }, new Class[] { Exception.class, String.class, boolean.class });
	}

	public String adm_name() throws DevFailed {
		return (String) WebServerClientUtil.getResponse(this, classParam, "adm_name", new Object[] {}, new Class[] {});
	}

	public String[] black_box(int length) throws DevFailed {
		return (String[]) WebServerClientUtil.getResponse(this, classParam, "adm_name", new Object[] { length }, new Class[] { int.class });
	}

	public DeviceData command_inout(String command, DeviceData argin) throws DevFailed {
		return (DeviceData) WebServerClientUtil.getResponse(this, classParam, "command_inout", new Object[] { command, argin }, new Class[] { String.class, DeviceData.class });
	}

	public DeviceData command_inout(String command) throws DevFailed {
		return (DeviceData) WebServerClientUtil.getResponse(this, classParam, "command_inout", new Object[] { command }, new Class[] { String.class });
	}

	public CommandInfo[] command_list_query() throws DevFailed {
		return (CommandInfo[]) WebServerClientUtil.getResponse(this, classParam, "command_list_query", new Object[] {}, new Class[] {});
	}

	public String description() throws DevFailed {
		return (String) WebServerClientUtil.getResponse(this, classParam, "description", new Object[] {}, new Class[] {});
	}

	public int getAccessControl() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "getAccessControl", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return 0;
		}
	}

	public int get_idl_version() throws DevFailed {
		 try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "get_idl_version", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return 0;
		}
	}

	public String get_ior() throws DevFailed {
		return (String) WebServerClientUtil.getResponse(this, classParam, "get_ior", new Object[] {}, new Class[] {});
	}

	public String get_name() {
		try {
			return (String) WebServerClientUtil.getResponse(this, classParam, "get_name", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return null;
		}
	}

	public DevSource get_source() throws DevFailed {
		return (DevSource) WebServerClientUtil.getResponse(this, classParam, "get_source", new Object[] {}, new Class[] {});
	}

	public int get_timeout() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "get_timeout", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return 0;
		}
	}

	public int get_timeout_millis() throws DevFailed {
		return (Integer) WebServerClientUtil.getResponse(this, classParam, "get_timeout_millis", new Object[] {}, new Class[] {});
	}

	public boolean get_transparency_reconnection() {
		try {
			return (Boolean) WebServerClientUtil.getResponse(this, classParam, "get_transparency_reconnection", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return false;
		}
	}

	public DevInfo info() throws DevFailed {
		return (DevInfo) WebServerClientUtil.getResponse(this, classParam, "info", new Object[] {}, new Class[] {});
	}

	public DevInfo_3 info_3() throws DevFailed {
		return (DevInfo_3) WebServerClientUtil.getResponse(this, classParam, "info_3", new Object[] {}, new Class[] {});
	}

	public boolean isAllowedCommand(String cmd) throws DevFailed {
		return (Boolean) WebServerClientUtil.getResponse(this, classParam, "isAllowedCommand", new Object[] { cmd }, new Class[] { String.class });
	}

	public boolean is_taco() {
		try {
			return (Boolean) WebServerClientUtil.getResponse(this, classParam, "is_taco", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return false;
		}
	}

	public void setAccessControl(int access) {
		try {
			WebServerClientUtil.getResponse(this, classParam, "setAccessControl", new Object[] { access }, new Class[] { int.class });
		} catch (DevFailed e) {
			e.printStackTrace();
		}
	}

	public void set_source(DevSource new_src) throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "set_source", new Object[] { new_src }, new Class[] { DevSource.class });
	}

	public void set_timeout_millis(int millis) throws DevFailed {
		WebServerClientUtil.getResponse(this, classParam, "set_timeout_millis", new Object[] { millis }, new Class[] { int.class });
	}

	public void set_transparency_reconnection(boolean val) {
		try {
			WebServerClientUtil.getResponse(this, classParam, "set_transparency_reconnection", new Object[] { val }, new Class[] { boolean.class });
		} catch (DevFailed e) {
			e.printStackTrace();
		}
	}

	public TangoUrl getUrl() {
		try {
			return (TangoUrl) WebServerClientUtil.getResponse(this, classParam, "getUrl", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return null;
		}
	}

	public int getAccess() {
		try {
			return (Integer) WebServerClientUtil.getResponse(this, classParam, "getAccess", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return 0;
		}
	}

	public String getDevname() {
		try {
			return (String) WebServerClientUtil.getResponse(this, classParam, "getDevname", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return null;
		}
	}

	public boolean isCheck_access() {
		try {
			return (Boolean) WebServerClientUtil.getResponse(this, classParam, "isCheck_access", new Object[] {}, new Class[] {});
		} catch (DevFailed e) {
			e.printStackTrace();
			return false;
		}
	}

	public void setAccess(int access) {
		try {
			WebServerClientUtil.getResponse(this, classParam, "setAccess", new Object[] { access }, new Class[] { int.class });
		} catch (DevFailed e) {
			e.printStackTrace();
		}
	}

	protected void finalize() throws Throwable {
		// call the WebServerClientUtil to call the remove of the object in the
		// server
		WebServerClientUtil.removeObject(this);
		super.finalize();
	}

	public Object[] getClassParam() {
		return classParam;
	}

	public void setClassParam(Object[] classParam) {
		this.classParam = classParam;
	}
}
