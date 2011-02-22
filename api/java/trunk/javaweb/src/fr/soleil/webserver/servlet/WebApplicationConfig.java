package fr.soleil.webserver.servlet;

import fr.soleil.tangoweb.data.TangoWebBeanList;
import fr.soleil.tangoweb.data.TangoWebRewriteRules;

public class WebApplicationConfig {
	private static WebApplicationConfig singleton = null;
	private TangoWebBeanList m_beanList = null;
	private TangoWebRewriteRules m_rules = null;
	
	public static void createSingleton()
	{
		singleton = new WebApplicationConfig();
	}
	
	public static WebApplicationConfig getSingleton()
	{
		if(singleton == null)
			createSingleton();
		return singleton;
	}

	public TangoWebBeanList getBeanList() {
		return m_beanList;
	}

	public void setBeanList(TangoWebBeanList list) {
		m_beanList = list;
	}

	public TangoWebRewriteRules getRules() {
		return m_rules;
	}

	public void setRules(TangoWebRewriteRules m_rules) {
		this.m_rules = m_rules;
	}
}
