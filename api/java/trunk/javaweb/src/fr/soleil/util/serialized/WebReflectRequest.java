package fr.soleil.util.serialized;

public class WebReflectRequest implements java.io.Serializable {
	private String m_strAction = null;
	private Object[]  m_aClassParam = null;
	private String m_strMethod = null;
	private Object[]  m_aMethodParam = null;
	private String m_objectID = null;
	private Class[] m_aMethodParamClass = null;
	
	public WebReflectRequest()
	{
	}

	public WebReflectRequest(String strAction, Object[] strClassParam, String strMethod, Object[] aParameter, Class[] aParameterClass)
	{
		m_strAction = strAction;
		m_aClassParam = strClassParam;
		m_strMethod = strMethod;
		m_aMethodParam = aParameter;
		m_objectID = null;
		m_aMethodParamClass = aParameterClass;
	}

	public WebReflectRequest(String objectID, String actionName, String strMethod, Object[] aParameter, Class[] aParameterClass)
	{
		m_strAction = actionName;
		m_aClassParam = null;
		m_strMethod = strMethod;
		m_aMethodParam = aParameter;
		m_objectID = objectID;
		m_aMethodParamClass = aParameterClass;
	}
	
	public Object[] getClassParam() {
		return m_aClassParam;
	}
	public void setClassParam(Object[] classParam) {
		m_aClassParam = classParam;
	}
	public Object[] getMethodParam() {
		return m_aMethodParam;
	}
	public void setMethodParam(Object[] methodParam) {
		m_aMethodParam = methodParam;
	}
	public Class[] getMethodParamClass() {
		return m_aMethodParamClass;
	}
	public void setMethodParamClass(Class[] methodParamClass) {
		m_aMethodParamClass = methodParamClass;
	}
	public String getObjectID() {
		return m_objectID;
	}
	public void setObjectID(String objectID) {
		this.m_objectID = objectID;
	}
	public String getAction() {
		return m_strAction;
	}
	public void setAction(String strAction) {
		this.m_strAction = strAction;
	}
	public String getMethod() {
		return m_strMethod;
	}
	public void setMethod(String strMethod) {
		this.m_strMethod = strMethod;
	}

}
