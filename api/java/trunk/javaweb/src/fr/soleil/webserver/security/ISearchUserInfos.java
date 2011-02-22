package fr.soleil.webserver.security;

import fr.soleil.services.security.commons.data.User;


/**
 * 
 * This interface provides the folowing methods :
 * - get the UID of a user
 * - get all the groups a user belongs to
 * - get all the projects a user belongs to
 * 
 * All this informations are searched in a LDAP directory
 * @author MOULHAUD
 */
public interface ISearchUserInfos
{
	/**
	 * Get a user giving it UID number
	 * @param iUIDNumber {@link Integer} : the user uid number
	 * @return {@link User}
	 */
	public User getUser(Integer iUIDNumber) throws Exception;
	
	/**
	 * Get a user giving its UID string after validate the password
	 * @param strUID String : The uid
	 * @param strPassword String : The user password
	 * @return {@link User}
	 */

	public User getUser(String strUID,String strPassword) throws Exception;

	/**
	 * Get a user giving it UID
	 * @param iUID {@link String} : the user uid
	 * @return {@link User}
	 */
	public User getUser(String iUID) throws Exception;

	/**
	 * Method that allows to control that the user belongs to the LDAP directory
	 * 
	 * @param strPassword
	 * @param connection
	 * @return boolean. True if ok. False otherwise
	 */	
	public boolean controlPassword(String strUserDN, String strPassword) throws LDAPException;
	
	
}
