package fr.soleil.webserver.servlet;

import java.io.IOException;
import java.io.InputStream;
import java.io.NotSerializableException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.text.NumberFormat;
import java.util.Set;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import fr.soleil.tangoweb.data.TangoWebBeanList;
import fr.soleil.tangoweb.data.TangoWebRewriteRules;
import fr.soleil.util.UtilLogger;
import fr.soleil.util.exception.WebSecurityException;
import fr.soleil.util.parameter.FileParameterLoaderImpl;
import fr.soleil.util.parameter.ParameterManager;
import fr.soleil.util.serialized.WebReflectRequest;
import fr.soleil.util.serialized.WebRequest;
import fr.soleil.util.serialized.WebResponse;
import fr.soleil.webserver.config.XMLConfigLoader;
import fr.soleil.webserver.security.ISearchUserInfos;

/**
 * Servlet which process request from the TWIST CLient.
 * @author BARBA-ROSSA
 *
 */
public class ActionServlet extends HttpServlet
{
	private static final long serialVersionUID = 1L;
	
	// temp first version of security
	private static ISearchUserInfos  securityManager = null;
	
	/**
	 * Initialize the application context such as Server's logger 
	 */
	public void init() throws ServletException
	{
		super.init();
		
		try
		{
			FileParameterLoaderImpl loader = new FileParameterLoaderImpl();
			ParameterManager.setLoader(loader);
		} catch (Exception e)
		{
			throw new ServletException();
		}

        // we load the configuration
		try {
			TangoWebBeanList beanList = XMLConfigLoader.loadSerializationConfig("serializationConfig.xml");
			WebApplicationConfig.getSingleton().setBeanList(beanList);
			
			TangoWebRewriteRules ruleList = XMLConfigLoader.loadRulesConfig("serializationConfig.xml");
			WebApplicationConfig.getSingleton().setRules(ruleList);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.setProperty("TANGO_HOST", System.getenv("TANGO_HOST"));
		System.setProperty("TANGO_WEB_SERVER", System.getenv("TANGO_WEB_SERVER"));
		
		// securityManager = new SearchUserInfosDefaultImpl();
		// need to implement a configuration parameter for create an instance of a selected security module
		// fr.soleil.services.security.serveur.SearchUserInfosDefaultImpl
		
		// need to create a system to call a class for loading other parameters
		
		try
		{
			// We load the server properties
			FileParameterLoaderImpl fileLoader = new FileParameterLoaderImpl();
			ParameterManager.setLoader(fileLoader);
			fileLoader.readFile("", "server", true);
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
		// we load the security module 
		LoadISearchUserInfos();
			
		//System.setProperty("TANGO_HOST","monteverdi::20000");
		/*
		// for MAMBO we create instance
        ArchivingManagerFactory.getImpl(ArchivingManagerFactory.REAL_IMPL_TYPE_GLOBAL_START_CALL);
        AttributeManagerFactory.getImpl( AttributeManagerFactory.BUFFERED_FORMATS_AND_DOMAINS );
        ExtractingManagerFactory.getImpl( ExtractingManagerFactory.BASIC );
        */

		
        // we get the MAMBO connection parameters
        // D:\Mambo hdbmanager hdbmanager tdbmanager tdbmanager expert
		/*
		try
		{
	        Mambo.setHDBuser(ParameterManager.getStringParameter("","HDB_LOGIN"));//args[ 1 ];
	        Mambo.setHDBpassword(ParameterManager.getStringParameter("","HDB_PASSWORD"));//args[ 2 ];
	        Mambo.setTDBuser(ParameterManager.getStringParameter("","TDB_LOGIN"));//args[ 3 ];
	        Mambo.setTDBpassword(ParameterManager.getStringParameter("","TDB_PASSWORD"));//args[ 4 ];
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		*/
	}
	
	private void LoadISearchUserInfos()
	{
		try
		{
			boolean securityActivated = ParameterManager.getBooleanParameter("","SECURITY_ACTIVATED");
			if(securityActivated)
			{
				String securityClassName = ParameterManager.getStringParameter("","SECURITY_CLASS");
				
		        // we get the class definition object
		        Class c = Class.forName(securityClassName);
	
		        Constructor constructor = c.getConstructor(new Class[]{});
		        securityManager = (ISearchUserInfos) constructor.newInstance(new Object[]{});
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	/**
	 * Execute the client request
	 * @param request
	 * @param response
	 * @throws ServletException
	 * @throws IOException
	 */
	protected void processCall(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
			// the response content type is only java serialized object. 
			// We don't use the servlet's post method for other case.
			response.setContentType("application/x-java-serialized-object");
			
			// we get the input stream
			InputStream in = request.getInputStream();
			// We get the object sending by the client in the input stream
			ObjectInputStream inputFromApplet = new ObjectInputStream(in);
			WebRequest webRequest = null;
			WebResponse webResponse = null;
			WebReflectRequest reflectRequest = null;
			String strAction = null;
			String strMethod = null;
			try{
				request.getSession().setAttribute("authenticated",true); //temp			
				
				// We read the object and we put it into a WebRequest instance
				webRequest = (WebRequest) inputFromApplet.readObject();

				// We execute the action
				//
				ProcessAction processAction  = (ProcessAction)request.getSession().getAttribute("ProcessAction");
				if(processAction == null)
				{
					processAction = new ProcessAction(request.getSession().getId());
					request.getSession().setAttribute("ProcessAction", processAction);
				}
				
				Object[] arguments =webRequest.getArguments();
				reflectRequest = (WebReflectRequest)arguments[0];
				strAction = reflectRequest.getAction();
				strMethod = reflectRequest.getMethod();
				
				if("Storage.remove".equalsIgnoreCase(strAction))
				{
					ProcessActionCounter.getSingleton().addRemoveCounter(webRequest.getApplication());
					processAction.removeObject(reflectRequest.getObjectID());
				} 
				else if("Security.authenticate".equalsIgnoreCase(strAction))
				{
					// here we call the services to authenticate the user
					String strLogin = (String)reflectRequest.getClassParam()[0];
					String strpassWord = (String)reflectRequest.getClassParam()[1];
					boolean authenticated = false;
					// FOR DEV : we simply check if login is equal to password.
			/*
 					if(securityManager.getUser(strLogin, strpassWord)!=null)
					{
						authenticated = true;
						request.getSession().setAttribute("authenticated", authenticated);				
					}
					else
						request.getSession().setAttribute("authenticated", authenticated);
 			*/
					if(strLogin.equals(strpassWord))
					{
						authenticated = true;
						request.getSession().setAttribute("authenticated", authenticated);
					}
					else
					{
						request.getSession().setAttribute("authenticated", authenticated);
						throw new WebSecurityException(WebSecurityException.INVALID_USER);
					}			
 				}
				else
				{
					ProcessActionCounter.getSingleton().addAccessCounter(webRequest.getApplication());
					// we check if the user is already authenticated
					if(request.getSession().getAttribute("authenticated") == null)
						throw new WebSecurityException(WebSecurityException.USER_NOT_CONNECTED);
					
					boolean authenticated =  (Boolean)request.getSession().getAttribute("authenticated");
					if(authenticated)
					{
						// we call the action.
						if(reflectRequest.getObjectID() == null)
						{
							ProcessActionCounter.getSingleton().addMethodCounter(strAction+"."+strMethod);
							ProcessActionCounter.getSingleton().addClassCounter(strAction);
							ProcessActionCounter.getSingleton().addClassUseCounter(strAction);
						}
						else
						{
							ProcessActionCounter.getSingleton().addMethodCounter(strAction+"."+strMethod);
							ProcessActionCounter.getSingleton().addClassUseCounter(strAction);			
						}
						
						// we execute the action
						webResponse =  processAction.processRequest(reflectRequest);
					}
					else
						throw new WebSecurityException(WebSecurityException.USER_NOT_CONNECTED);
				}					
				// -----------------------------------------------------------------------------------------
				// -----------------------------------------------------------------------------------------
				
				UtilLogger.logger.addInfoLog("WebRequest Action : " + webRequest.getAction());
				// We call the process action object to get the result
				
			} catch (Exception e) {
				
				System.out.println(e.getMessage()+ " : " + strAction+"."+strMethod);
				// if we have an exception we send it to the client
				webResponse = new WebResponse();
				webResponse.setResult(new Object[]{e});
				UtilLogger.logger.addERRORLog("ActionServlet :" + e.getMessage());
			}
			
			try
			{
			// The output stream get the webresponse and send it to the client
			// The webresponse object and ALL of it's CHILD must be seralizable
			OutputStream outstr = response.getOutputStream();
			ObjectOutputStream oos = new ObjectOutputStream(outstr);
			// we put the response into the stream
			oos.writeObject(webResponse);
			// we send the response
			oos.flush();
			// a Test we send à String
			oos.writeObject("Test: second envoie vers le client en gardant la connection ouverte ... ");
			oos.flush();
			// and close the stream, it's very important.
			oos.close();
			UtilLogger.logger.addInfoLog("We send the result...");
			}catch(NotSerializableException ioe)
			{
				System.err.print("Erreur " + ioe.getMessage());
				System.err.print("Action :" + reflectRequest.getAction());
				System.err.print(", Method " + reflectRequest.getMethod());
				System.err.print(", ObjectID " + reflectRequest.getObjectID());
				throw ioe;
			}
	}
	
	
	/**
	 * For the moment the Get method is only use the check if the servlet running
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
	{
		response.getWriter().println("SERVLET RUNNING : " + System.currentTimeMillis());
		response.getWriter().println("");
		
		Set keySet = ProcessActionCounter.getSingleton().getAccessCounter().keySet();
		Object[] keys = keySet.toArray();
		String key = null;
		int access = 0;
		int remove = 0;
		for(int i = 0;i < keys.length;i++)
		{
			key = (String)keys[i];
			access = ProcessActionCounter.getSingleton().getAccessCounter().get(key);
			if(ProcessActionCounter.getSingleton().getRemoveCounter().containsKey(key))
				remove = ProcessActionCounter.getSingleton().getRemoveCounter().get(key);
			else
				remove = 0;
			response.getWriter().println(key+":"+access+":"+remove);
		}
		long totalMemory = Runtime.getRuntime().totalMemory();
		long freeMemory = Runtime.getRuntime().freeMemory();
		long usedMemory = totalMemory-freeMemory;
		response.getWriter().println("Total Memory : "+ NumberFormat.getNumberInstance().format(totalMemory) );
		response.getWriter().println("Free Memory : "+ NumberFormat.getNumberInstance().format(freeMemory));
		response.getWriter().println("Used Memory : "+ NumberFormat.getNumberInstance().format(usedMemory));
		response.getWriter().println("");
		response.getWriter().println("Classes uses :");

		keySet = ProcessActionCounter.getSingleton().getClassUseCounter().keySet();
		keys = keySet.toArray();
		for(int i = 0;i < keys.length;i++)
		{
			key = (String)keys[i];
			if(ProcessActionCounter.getSingleton().getClassCounter().containsKey(key))
				access = ProcessActionCounter.getSingleton().getClassCounter().get(key);
			else
				access = 0;
			if(ProcessActionCounter.getSingleton().getClassUseCounter().containsKey(key))
				remove = ProcessActionCounter.getSingleton().getClassUseCounter().get(key);
			else
				remove = 0;
			response.getWriter().println(key+":"+access+":"+remove);
		}
		response.getWriter().println("");
		response.getWriter().println("Method uses :");
		
		keySet = ProcessActionCounter.getSingleton().getMethodCounter().keySet();
		keys = keySet.toArray();		
		for(int i = 0;i < keys.length;i++)
		{
			key = (String)keys[i];
			access = ProcessActionCounter.getSingleton().getMethodCounter().get(key);
			response.getWriter().println(key+":"+access);
		}		
	}
	
	/**
	 * We receive in post method only the client request
	 */
	protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException
	{
		processCall(req, resp);
	}	
}
