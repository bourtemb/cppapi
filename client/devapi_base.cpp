static const char *RcsId = "$Id$\n$Name$";

//
// devapi_base.cpp 	- C++ source code file for TANGO device api 
//
// programmer(s)	- Andy Gotz (goetz@esrf.fr)
//
// original 		- March 2001
//
// Copyright (C) :      2001,2002,2003,2004,2005,2006,2007,2008,2009
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
// log			- $Log$
// log			- Revision 3.64.2.2  2009/06/17 08:14:42  taurel
// log			- - The DeviceProxy::set_timeout_millis() method is now stateless
// log			-
// log			- Revision 3.64.2.1  2009/06/12 10:45:50  taurel
// log			- - Fix bug in DeviceProxy::get_logging_level() method
// log			-
// log			- Revision 3.64  2009/04/20 13:25:50  taurel
// log			- - Fix bug in case of default constructed DeviceProxy and alias() method
// log			- - Add some ctors from "const char *" to make programmer's life easier
// log			-
// log			- Revision 3.63  2009/03/30 15:02:58  taurel
// log			- - Fix last bugs before Tango 7 ??
// log			-
// log			- Revision 3.62  2009/03/27 12:20:34  taurel
// log			- - Fix bug in the methods to get environment variable from file
// log			-
// log			- Revision 3.61  2009/03/20 11:52:06  taurel
// log			- - Add tangorc files management (for env. variables)
// log			-
// log			- Revision 3.60  2009/03/17 12:07:24  taurel
// log			- - Forget the UtilSingletonNotCreated exception in case of CS started
// log			- with control access
// log			-
// log			- Revision 3.59  2009/03/13 09:32:27  taurel
// log			- - Small changes to fix Windows VC8 warnings in Warning level 3
// log			-
// log			- Revision 3.58  2009/03/02 15:55:51  taurel
// log			- - Ported to Windows
// log			-
// log			- Revision 3.57  2009/02/27 13:24:43  taurel
// log			- - Small changes for Solaris
// log			-
// log			- Revision 3.56  2009/02/26 07:47:27  taurel
// log			- - The attribute data format is now transferred within the AttributeValue_4 structure
// log			-
// log			- Revision 3.55  2009/02/25 07:04:10  taurel
// log			- - Change signature of the DeviceProxy::get_attribute_config() method
// log			-
// log			- Revision 3.54  2009/02/23 14:44:45  taurel
// log			- - Add a check on wildcard character number fo rthe DeviceProxy::get_property_list() method
// log			-
// log			- Revision 3.53  2009/02/23 14:27:53  taurel
// log			- - Added a DeviceProxy::get_property_list() method
// log			-
// log			- Revision 3.52  2009/02/19 12:18:09  taurel
// log			- - Changes for Solaris and 64 bits compilation
// log			-
// log			- Revision 3.51  2009/02/03 15:07:31  jensmeyer
// log			- Added sub device rgistration when creating a DeviceProxy in a device
// log			- server.
// log			-
// log			- Revision 3.50  2009/01/30 09:17:34  taurel
// log			- - End of first implementation of Data Ready event
// log			-
// log			- Revision 3.49  2009/01/29 16:24:26  taurel
// log			- - Commit after merge with branch Release_6_1_1-bugfixes
// log			-
// log			- Revision 3.48  2009/01/23 12:39:50  taurel
// log			- - Add some code for the DeviceUnlocked exception in case a locked device
// log			- is unlocked by the locking back door
// log			- Revision 3.32.2.2  2008/10/20 11:29:21  taurel
// log			- - Fix bug in DeviceProxy::adm_name() method in case od device running
// log			- in  Tango host specified in the device name
// log			-
// log			- Revision 3.47  2009/01/21 12:45:15  taurel
// log			- - Change CopyRights for 2009
// log			-
// log			- Revision 3.46  2009/01/08 14:56:46  taurel
// log			- - The read_attribute_4 also transfer the client authentification
// log			-
// log			- Revision 3.45  2008/12/17 09:54:44  taurel
// log			- - First implementation of attributes sent on the wire using IDL Union
// log			- instead of IDL Any
// log			-
// log			- Revision 3.44  2008/11/18 09:31:13  taurel
// log			- - Change in some error message to be ATK compatible
// log			- (since XXX ms...)
// log			-
// log			- Revision 3.43  2008/10/06 15:02:16  taurel
// log			- - Changed the licensing info from GPL to LGPL
// log			-
// log			- Revision 3.42  2008/10/02 16:09:25  taurel
// log			- - Add some licensing information in each files...
// log			-
// log			- Revision 3.41  2008/10/01 12:01:04  jensmeyer
// log			- Changed method name event_queue_is_empty() to is_event_queue_empty()
// log			-
// log			- Revision 3.40  2008/09/23 14:38:28  taurel
// log			- - Commit after the end of DevEncoded data type implementation
// log			- - The new test suite is also now running fine
// log			-
// log			- Revision 3.39  2008/09/15 12:29:08  jensmeyer
// log			- Added an eventqueue reading method to call the usual callback method
// log			- when reading event data from the queue.
// log			-
// log			- Revision 3.38  2008/09/15 08:05:24  jensmeyer
// log			- Added methods for the event queue handling to the DeviceProxy and AttributeProxy classes.
// log			-
// log			- Revision 3.37  2008/09/04 07:35:37  taurel
// log			- - Fix bug in memorized attributes
// log			- - Changes for the new IDL 4
// log			-
// log			- Revision 3.36  2008/09/01 14:25:38  taurel
// log			- - Fix some bugs in locking implementation
// log			- - The Tango::Callback class dtor is now defined as virtual as it should be
// log			-
// log			- Revision 3.35  2008/06/14 11:28:07  taurel
// log			- - DevEncoded attribute data type implementation work going on
// log			-
// log			- Revision 3.34  2008/06/10 07:50:29  taurel
// log			- - Fix client threadig issue when first device proxy instance created
// log			- in threads which are not the main thread
// log			- - Add code for the DevEncoded attribute data type
// log			-
// log			- Revision 3.33  2008/05/20 12:42:29  taurel
// log			- - Commit after merge with release 7 branch
// log			- Revision 3.32.2.1  2008/07/17 12:27:14  taurel
// log			- - Fix memory leak in read_attributes() in case of exception
// log			-
// log			- Revision 3.32  2008/03/26 16:48:38  taurel
// log			- - Clarify some exception messages
// log			- Revision 3.26.2.8  2008/05/20 06:14:19  taurel
// log			- - Last commit before merge with trunk
// log			-
// log			- Revision 3.31  2008/02/28 12:18:36  jensmeyer
// log			- Corrected error messages during initialisation and deleted printouts.
// log			- Revision 3.26.2.7  2008/02/07 15:56:58  taurel
// log			- - First implementation of the Controlled Access done
// log			-
// log			- Revision 3.30  2008/01/22 08:46:09  jensmeyer
// log			- Deleted unix timing for database testing in Connection::connect().
// log			- Revision 3.26.2.6  2008/01/03 16:04:23  taurel
// log			- - Some changes in locking feature implementation
// log			-
// log			- Revision 3.29  2008/01/18 14:44:40  jensmeyer
// log			- Added a new subscribe_event method to the DeviceProxy and
// log			- AttributeProxy classes which takes an option for stateless
// log			- event subscription.
// log			- Revision 3.26.2.5  2007/12/20 14:27:18  taurel
// log			- - Some more work on locking
// log			-
// log			- Revision 3.28  2008/01/15 12:25:51  taurel
// log			- - Add a retry on database connection
// log			- Revision 3.26.2.4  2007/12/19 15:53:08  taurel
// log			- - Still some work going on for the locking feature
// log			-
// log			- Revision 3.27  2007/12/12 10:15:46  taurel
// log			- - Db calls during DS startup has a separate timeout and some retries
// log			- Revision 3.26.2.3  2007/11/22 12:35:50  taurel
// log			- - First part of the device locking implementation
// log			-
// log			- Revision 3.26.2.2  2007/11/20 14:39:12  taurel
// log			- - Add the new way to retrieve command history from polling buffer
// log			- implemented in Tango V7
// log			-
// log			- Revision 3.26.2.1  2007/11/16 14:10:55  taurel
// log			- - Added a new IDL interface (Device_4)
// log			- - Added a new way to get attribute history from polling buffer (must faster)
// log			-
// log			- Revision 3.26  2007/10/26 11:24:10  taurel
// log			- - Add some print-out for test purposes
// log			-
// log			- Revision 3.25  2007/10/16 08:25:47  taurel
// log			- - Add management of the TC connection establishment timeout for DB access
// log			- - Add DB server cache in DS used during DS startup sequence
// log			- - Comment out the sleep time during DS startup sequence
// log			-
// log			- Revision 3.24  2007/04/20 14:38:33  taurel
// log			- - Ported to Windows 64 bits x64 architecture
// log			-
// log			- Revision 3.23  2007/04/16 14:55:16  taurel
// log			- - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// log			- - Ported to omniORB4.1
// log			- - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// log			- - Added a new filterable field in the archive event
// log			-
// log			- Revision 3.22  2007/03/06 08:20:45  taurel
// log			- - Added 64 bits data types for 64 bits computer...
// log			-
// log			- Revision 3.21  2007/02/16 10:38:53  taurel
// log			- - Implementing a new types of event on the Attribute configuration
// log			-
// log			- Revision 3.20  2007/02/14 14:12:17  taurel
// log			- - Fix bug in asynchronous calls reconnection
// log			-
// log			- Revision 3.19  2006/08/10 15:08:12  taurel
// log			- - Fix a bug in event system when using alias to create the DeviceProxy object
// log			- and with a device name with lower and upper case letters
// log			-
// log			- Revision 3.18  2006/06/15 07:03:22  taurel
// log			- - Fix bug in reconnection in case of admin device Init, Restart or RestartDevice command
// log			-
// log			- Revision 3.17  2006/05/30 11:17:56  jensmeyer
// log			- Modifications on push_event functionality for MS Windows.
// log			- Mainly modifications for timing and time stamps.
// log			-
// log			- Revision 3.16  2006/05/23 12:02:06  jensmeyer
// log			- Added a reconnection delay of 1000ms not to send to many unnecessary
// log			- requestes to the database. Modified Connection::reconnect().
// log			-
// log			- Revision 3.15  2006/05/18 07:28:45  taurel
// log			- - Fix some bugs and warnings detected by VC8 on Windows
// log			-
// log			- Revision 3.14  2006/04/13 13:18:46  taurel
// log			- Several changes:
// log			- - Add get/set_transparency_reconnection in AttributeProxy class
// log			- - Add a AttributeProxy ctor from an already contruxted DeviceProxy
// log			- - Fix bug in assignement operator and copy ctor of the DeviceProxy class
// log			- - Add a default ctor for the DeviceProxy class
// log			-
// log			- Revision 3.13  2005/10/03 15:50:04  jensmeyer
// log			- Corrected archive period reading.
// log			-
// log			- Revision 3.12  2005/05/10 16:09:13  taurel
// log			- - Fix windows VC7 warnings
// log			-
// log			- Revision 3.11  2005/03/14 09:48:12  taurel
// log			- - Fix some bugs in filedatabase (Change in the data transferred between client and
// log			-   server).
// log			- - Fix bug in event re-connection
// log			- - Add event support even for device server started with the -file command line option
// log			-
// log			- Revision 3.10  2005/01/13 08:36:36  taurel
// log			- - Merge trunk with Release_5_0 from brach Release_5_branch
// log			-
// log			- Revision 3.9.2.12  2004/12/06 14:35:49  taurel
// log			- - FIx some compatibility bugs for attribute config
// log			- - Some cleaning of the filedatabase files
// log			-
// log			- Revision 3.9.2.11  2004/11/22 13:53:12  taurel
// log			- - Add a compatibility method in DeviceProxy class for V5 client
// log			-   requesting attribute config to V4 server
// log			- - Some minor changes
// log			-
// log			- Revision 3.9.2.10  2004/11/16 13:10:04  taurel
// log			- - Fix bug in attribute_list_query() for client compiled with Tango V5
// log			-   talking to a server V4
// log			-
// log			- Revision 3.9.2.9  2004/11/15 13:15:56  taurel
// log			- - Fix bug in DeviceProxy::adm_name() method in case of non-database
// log			-   device server
// log			-
// log			- Revision 3.9.2.8  2004/11/10 13:08:11  taurel
// log			- - Changes in teh COnnection class ctor from boolean
// log			-
// log			- Revision 3.9.2.7  2004/11/04 09:51:29  taurel
// log			- - Add compatibility with old database server for the DbGetDeviceAttributeProperty
// log			-   and associate commands
// log			- - Some minor fixes to pass test suite
// log			-
// log			- Revision 3.9.2.6  2004/10/27 05:58:10  taurel
// log			- - Some minor changes to compile on all our supported platforms
// log			-
// log			- Revision 3.9.2.5  2004/10/22 11:23:17  taurel
// log			- Added warning alarm
// log			- Change attribute config. It now includes alarm and event parameters
// log			- Array attribute property now supported
// log			- subscribe_event throws exception for change event if they are not correctly configured
// log			- Change in the polling thread: The event heartbeat has its own work in the work list
// log			- Also add some event_unregister
// log			- Fix order in which classes are destructed
// log			- Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// log			- Change in the CORBA info call for the device type
// log			-
// log			- Revision 3.9.2.4  2004/10/05 13:58:10  maxchiandone
// log			- First upload for filedatabase.
// log			-
// log			- Revision 3.9.2.3  2004/09/15 06:44:43  taurel
// log			- - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// log			- - It is also possible to read state and status as attributes
// log			- - Fix bug in Database::get_class_property() method (missing ends insertion)
// log			- - Fix bug in admin device DevRestart command (device name case problem)
// log			-
// log			- Revision 3.9.2.2  2004/08/26 07:33:02  taurel
// log			- - Implement a way to directly fills command or attribute polling buffer
// log			-
// log			- Revision 3.9.2.1  2004/08/10 09:10:09  taurel
// log			- - It's now possible to create a DeviceProxy or AttributeProxy instance
// log			-   with a name syntax like //host:port/xxxx. It is needed for Java API
// log			-   coherency
// log			-
// log			- Revision 3.9  2004/07/07 08:39:56  taurel
// log			-
// log			- - Fisrt commit after merge between Trunk and release 4 branch
// log			- - Add EventData copy ctor, asiignement operator and dtor
// log			- - Add Database and DeviceProxy::get_alias() method
// log			- - Add AttributeProxy ctor from "device_alias/attribute_name"
// log			- - Exception thrown when subscribing two times for exactly yhe same event
// log			-
// log			- Revision 3.8  2003/08/21 07:22:02  taurel
// log			- - End of the implementation of the new way to transfer data for read and
// log			-   write attributes (better use of exception)
// log			- - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// log			- - Added DeviceAttribute ctors from "const char *"
// log			- - Enable writing of spectrum and image attributes
// log			- - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
// log			-   attribute writing
// log			- - Attribute date automatically set in case of attribute quality factor set to INVALID
// log			- - Change in the polling thread discarding element algo. to support case of polling
// log			-   several cmd/atts at the same polling period with cmd/attr having a long response time
// log			- - Take cmd/attr execution time into account in the "Data not updated since" polling
// log			-   status string
// log			- - Split "str().c_str()" code in two lines of code. It was the reason of some problem
// log			-   on Windows device server
// log			- - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
// log			-   to send trigger to the polling thread
// log			-
// log			- Revision 3.7  2003/07/03 07:37:56  taurel
// log			- - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// log			- - Handle this new IDL release in DeviceProxy class
// log			- - New exception methods in DeviceAttribute class
// log			- - New way to get data out of DeviceAttribute object
// log			- - Fix bugs in DeviceProxy copy constructor and assignement operator
// log			- - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// log			- - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// log			- - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// log			- - Add a server_cleanup() method in the Util class
// log			- - Win32 : Update debug menu in the server graphical window to support logging feature
// log			- - Win32 : Display library CVS tag in the "Help->About" sub-window
// log			-
// log			- Revision 3.6.2.9  2004/05/17 15:24:33  taurel
// log			- - DeviceProxy ctor used with string does not "lowercase" device name
// log			-   anymore
// log			-
// log			- Revision 3.6.2.8  2004/03/19 15:19:50  taurel
// log			- - Comment out the connect(this) call in DeviceProxy::unsubscribe_event() method
// log			- - Fix bug in db interface with stream usage (once more)
// log			-
// log			- Revision 3.6.2.7  2004/03/02 07:40:23  taurel
// log			- - Fix compiler warnings (gcc used with -Wall)
// log			- - Fix bug in DbDatum insertion operator fro vectors
// log			- - Now support "modulo" as periodic filter
// log			-
// log			- Revision 3.6.2.6  2004/02/06 11:57:04  taurel
// log			- - Many changes in the event system
// log			-
// log			- Revision 3.6.2.5  2004/01/20 08:30:07  taurel
// log			- -First commit after merge with the event branch and work on the AttributeProxy class
// log			- - Fix bug in the stream "clear()" method usage when used with gcc 3.3
// log			-
// log			- Revision 3.6.2.4  2003/12/10 16:07:39  taurel
// log			- Last commit before merging with the event branch.
// log			-
// log			- Revision 3.6.2.3  2003/10/16 11:58:17  taurel
// log			- - Fix some memory leaks in DeviceProxy::is_polled() and in
// log			- Database::get_device_exported() methods
// log			- - Add some bug fixes in Database::get_device_attribute_property(),
// log			- Database::put_class_attribute_property()
// log			-
// log			- Revision 3.6.2.2  2003/10/02 14:53:42  taurel
// log			- Forgot to copy the Connection class copy constructor from Main trunck !
// log			-
// log			- Revision 3.6.2.1  2003/09/18 14:07:41  taurel
// log			- Fixes some bugs:
// log			-  - Bug fix in DeviceProxy copy constructor and assignement operator
// log			-  - Change the way how DeviceProxy::write_attribute() is coded
// log			-  - Added DeviceAttribute ctors from "const char *"
// log			-  - Split "str().c_str()" in two lines of code. It was the reason of some
// log			-    problems using Windows VC6
// log			-
// log			- Revision 3.6  2003/05/28 14:42:56  taurel
// log			- Add (conditionaly) autoconf generated include file
// log			-
// log			- Revision 3.5  2003/05/16 08:48:02  taurel
// log			- Many changes for release 3.0.1. The most important ones are :
// log			- - Timeout are backs
// log			- - Multiple db servers (change in TANGO_HOST syntax)
// log			- - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// log			- - Attributes name stored in blackbox
// log			- - Remove check if a class is created without any device
// log			- - It's now possible to create a DeviceProxy from its alias name
// log			- - Command, attribute names are case insensitive
// log			- - Change parameters of some DeviceProxy logging methods
// log			- - Change parameters of DeviceProxy asynchronous replies calls
// log			- - New serialization model in device server (no serialization model)
// log			- - Win32 (2000) device server service does not exit at loggoff anymore
// log			- - Miscellaneous bug fixes
// log			-
// log			- Revision 3.4  2003/04/28 08:34:26  nleclercq
// log			- Changed add/remove_logging_target parameter to type::name
// log			-
// log			- Revision 3.3  2003/04/08 15:08:57  taurel
// log			- - Fix bug in reconnection algo.
// log			-
// log			- Revision 3.2  2003/04/03 15:21:53  taurel
// log			- Added methods to print DeviceData, DeviceAttribute, DeviceDataHistory
// log			- and DeviceAttributeHistory instance
// log			-
// log			- Revision 3.1  2003/04/02 12:22:07  taurel
// log			- Miscellaneous changes to :
// log			-  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
// log			-  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
// log			-  - Timeout are bcks (omniORB 4.0.1)
// log			-  - Fix bug when killing a device server via the kill command of the admin device
// log			-    (Solaris only)
// log			-  - Cleanup device server code to have only one ORB and one db connection within a server
// log			-
// log			- Revision 3.0  2003/03/25 16:30:42  taurel
// log			- Change revision number to 3.0 before release 3.0.0 of Tango lib
// log			-
// log			- Revision 2.8  2003/03/25 16:28:12  taurel
// log			- Fix a comment spelling mistake !!!
// log			-
// log			- Revision 2.7  2003/03/20 08:54:54  taurel
// log			- Updated to support asynchronous calls
// log			-
// log			- Revision 2.6  2003/03/11 17:52:00  nleclercq
// log			- Switch from log4cpp to log4tango
// log			-
// log			- Revision 2.5  2003/02/17 15:02:01  taurel
// log			- Added the new Tango logging stuff (Thanks Nicolas from Soleil)
// log			-
// log			- Revision 2.4  2003/01/09 12:00:33  taurel
// log			- - Ported to gcc 3.2
// log			- - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// log			- - Replace some ORB * by ORB_ptr
// log			- - Use CORBA::ORB::is_nil() instead of comparing to NULL
// log			-
// log			- Revision 2.3  2002/12/16 11:58:36  taurel
// log			- - Change the underlying ORB fom ORBacus to omniORB
// log			- - New method get_device_list() in Util class
// log			- - Util::get_class_list() takes DServer device into account
// log			- - Util::get_device_by_name() takes DSErver device into account
// log			- - Util::get_device_list_by_class() takes DServer device into account
// log			- - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
// log			-
// log			- Revision 2.2  2002/10/14 09:32:43  taurel
// log			- Fix bugs in devapi_base.cpp file :
// log			- - In read_attribute and read_attributes method of the DeviceProxy class
// log			-   Do not create sequence the same way if the call is local or remote.
// log			- - Add reconnection in the Connection::set_timeout_millis method
// log			- - Add flags to the Connection::set_timeout_millis method
// log			- - Fix bug in the DeviceProxy constructor when device is not marked as exported
// log			-   in the database. The constructor was not stateless in this case.
// log			-
// log			- Revision 2.1  2002/08/12 12:43:24  taurel
// log			- Fix bug in DeviceProxy::write_attributes method when writing several
// log			- attributes in one call. (File devapi_base.cpp)
// log			-
// log			- Revision 2.0  2002/06/28 13:43:08  taurel
// log			- Lot of changes since last releases :
// log			- 	- Database object managed as a singleton per control system
// log			- 	- Support all tango device naming syntax (using TANGO_HOST env.
// log			-  	  variable, without env variable and non database device)
// log			- 	- No more copy during read_attribute and command_inout
// log			- 	- Added some missing methods
// log			- 	- Build an exception class hierarchy
// log			- 	- Added correct management of device time-out
// log			- 	- Support all Tango device interface release 2 features
// log			- 	  (data/attribute comming from polling buffer, polling related methods,
// log			- 	   command/attribute result history)
// log			-

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#include <process.h>
#else
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#endif /* _TG_WINDOWS_ */

#include <time.h>
#include <signal.h>

#include <algorithm>
                                                    
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// ConnectionExt::assignement operator
//
//-----------------------------------------------------------------------------

ConnectionExt &ConnectionExt::operator=(const ConnectionExt &rval)
{
	tr_reco  = rval.tr_reco;
	device_3 = rval.device_3;
	
	prev_failed    = rval.prev_failed;
	prev_failed_t0 = rval.prev_failed_t0;

	return *this;
}


//-----------------------------------------------------------------------------
//
// Connection::Connection() - constructor to manage a connection to a device
//
//-----------------------------------------------------------------------------

Connection::Connection(ORB *orb_in):pasyn_ctr(0),pasyn_cb_ctr(0),
				    timeout(CLNT_TIMEOUT),
				    version(0),source(Tango::CACHE_DEV)
{

	ext = new ConnectionExt();

//
// Some default init for access control
//
	
	check_acc = true;
	access = ACCESS_READ;
	
//
// If the proxy is created from inside a device server, use the server orb
//

	ApiUtil *au = ApiUtil::instance();		
	if ((orb_in == NULL) && (CORBA::is_nil(au->get_orb()) == true)) 
	{
		if (au->in_server() == true)
			ApiUtil::instance()->set_orb(Util::instance()->get_orb());
		else
			ApiUtil::instance()->create_orb();
	}
	else
	{
		if (orb_in != NULL)
			au->set_orb(orb_in);
	}
}

Connection::Connection(bool dummy)
{
	if (dummy)
	{
		ext = NULL;
	}
}

//-----------------------------------------------------------------------------
//
// Connection::~Connection() - destructor to destroy connection to TANGO device
//
//-----------------------------------------------------------------------------

Connection::~Connection()
{
	if (ext != NULL)
		delete ext;
}

//-----------------------------------------------------------------------------
//
// Connection::Connection() - copy constructor
//
//-----------------------------------------------------------------------------

Connection::Connection(const Connection &sou)
{
	dbase_used = sou.dbase_used;
	from_env_var = sou.from_env_var;	
	host = sou.host;
	port = sou.port;
	port_num = port_num;
	
	db_host = sou.db_host;
	db_port = sou.db_port;
	db_port_num = sou.db_port_num;
	
	ior = sou.ior;
	pasyn_ctr = sou.pasyn_ctr;
	pasyn_cb_ctr = sou.pasyn_cb_ctr;
		
	device = sou.device;
	if (sou.version >= 2)
		device_2 = sou.device_2;
	
	timeout = sou.timeout;	
	connection_state = sou.connection_state;
	version = sou.version;
	source = sou.source;
	
	check_acc = sou.check_acc;
	access = sou.access;

	if (sou.ext != NULL)
	{
		ext = new ConnectionExt();
		*ext = *(sou.ext);
	}
	else
		ext = NULL;
}

//-----------------------------------------------------------------------------
//
// Connection::connect() - method to create connection to a TANGO device 
//		using it's stringified CORBA reference i.e. IOR or corbaloc
//
//-----------------------------------------------------------------------------

void Connection::connect(string &corba_name)
{

	bool retry = true;
	long db_retries = DB_START_PHASE_RETRIES;
					
	while (retry == true)
	{	
		try
		{

        //gettimeofday(&start,NULL);		
//
// Narrow CORBA string name to CORBA object
// First, try as a Device_3, then as a Device_2 and finally as a Device
//

			Object_var obj;
			obj = ApiUtil::instance()->get_orb()->string_to_object(corba_name.c_str());

			ext->device_4 = Device_4::_narrow(obj);
		
			if (CORBA::is_nil(ext->device_4))
			{
				ext->device_3 = Device_3::_narrow(obj);

				if (CORBA::is_nil(ext->device_3))
				{
					device_2 = Device_2::_narrow(obj);
					if (CORBA::is_nil(device_2))
					{
						device = Device::_narrow(obj);
						if (CORBA::is_nil(device))
						{
							cerr << "Can't build connection to object " << corba_name <<  endl;			
							connection_state = CONNECTION_NOTOK;

							TangoSys_OMemStream desc;
							desc << "Failed to connect to device " << dev_name();
							desc << " (device nil after _narrowing)" << ends;
							ApiConnExcept::throw_exception((const char*)"API_CantConnectToDevice",
                        			       			desc.str(),
                        			       			(const char*)"Connection::connect()");
						}
						else
						{
							version = 1;
						}
					}
					else
					{
						version = 2;
						device = Device_2::_duplicate(device_2);
					}
				}
				else
				{
					version = 3;
					device_2 = Device_3::_duplicate(ext->device_3);
					device = Device_3::_duplicate(ext->device_3);			
				}
			}
			else
			{
				version = 4;
				ext->device_3 = Device_4::_duplicate(ext->device_4);
				device_2 = Device_4::_duplicate(ext->device_4);
				device = Device_4::_duplicate(ext->device_4);
			}
			retry = false;
		
//		
// Mark the connection as OK and set timeout to its value
// (The default is 3 seconds)
//

			connection_state = CONNECTION_OK;
			if (timeout != CLNT_TIMEOUT)
				set_timeout_millis(timeout);
		}
		catch (CORBA::SystemException &ce)
		{
			TangoSys_OMemStream desc;
			TangoSys_MemStream reason;
			bool db_connect = false;

			desc << "Failed to connect to ";

			string::size_type pos = corba_name.find(':');
			if (pos == string::npos)
			{
				desc << "device " << dev_name() << ends;
				reason << "API_CantConnectToDevice" << ends;
			}
			else
			{
				string prot = corba_name.substr(0,pos);
				if (prot == "corbaloc")
				{
					string::size_type tmp = corba_name.find('/');
					if (tmp != string::npos)
					{
						string dev = corba_name.substr(tmp + 1);
						if (dev == "database")
						{
							desc << "database on host ";
							desc << db_host << " with port ";
							desc << db_port << ends;
							reason << "API_CantConnectToDatabase" << ends;
							db_retries--;
							if (db_retries != 0)
								db_connect = true;
						}
						else
						{
							CORBA::SystemException *se;
							desc << "device " << dev_name() << ends;
							if ((se = CORBA::OBJECT_NOT_EXIST::_downcast(&ce)) != 0)
								reason << "API_DeviceNotDefined" << ends;
							else if ((se = CORBA::TRANSIENT::_downcast(&ce)) != 0)
								reason << "API_ServerNotRunning" << ends;
							else
								reason << "API_CantConnectToDevice" << ends;
						}
					}
					else
					{
						desc << "device " << dev_name() << ends;
						reason << "API_CantConnectToDevice" << ends;
					}
				}
				else
				{
					desc << "device " << dev_name() << ends;
					reason << "API_CantConnectToDevice" << ends;
				}
			}

			if (db_connect == false)
			{						
				ApiConnExcept::re_throw_exception(ce,reason.str(),desc.str(),
					        	  (const char *)"Connection::connect");
			}
		}
	}
}


//-----------------------------------------------------------------------------
//
// Connection::reconnect() - reconnect to a CORBA object
//
//-----------------------------------------------------------------------------

void Connection::reconnect(bool db_used)
{
	struct timeval now;
#ifndef _TG_WINDOWS_
	gettimeofday(&now, NULL);
#else
	struct _timeb now_win;
	_ftime(&now_win);
	now.tv_sec = (unsigned long)now_win.time;
	now.tv_usec = (long)now_win.millitm * 1000;
#endif /* _TG_WINDOWS_ */

	double	t = (double)now.tv_sec + ((double)now.tv_usec / 1000000);
	double	delay = t - ext->prev_failed_t0;

	if (connection_state != CONNECTION_OK)
	{
		//	Do not reconnect if to soon
		if ( (ext->prev_failed == true) && delay < (RECONNECTION_DELAY / 1000) )
		{
			TangoSys_OMemStream desc;
			desc << "Failed to connect to device " << dev_name() << endl;
			desc << "The connection request was delayed." << endl;
			desc << "The last connection request was done less than " << RECONNECTION_DELAY << " ms ago" << ends;
			
			Tango::Except::throw_exception ( (const char *)"API_CantConnectToDevice",
													desc.str(),
													(const char *)"Connection::reconnect");
		}
	}

	try
	{	
		if (connection_state != CONNECTION_OK)
		{	
			string corba_name;
			if (db_used == true)
			{
				corba_name = get_corba_name(check_acc);
				if (check_acc == false)
				{
					ApiUtil *au = ApiUtil::instance();
					int db_num;
					if (get_from_env_var() == true)
						db_num = au->get_db_ind();
					else
						db_num = au->get_db_ind(get_db_host(),get_db_port_num());
					(au->get_db_vect())[db_num]->clear_access_except_errors();
				}
			}
			else
				corba_name = build_corba_name();

			connect(corba_name);
		}

//
// Try to ping the device. With omniORB, it is possible that the first
// real access to the device is done when a call to one of the interface
// operation is done. Do it now.
//
		try
		{
			device->ping();
			
			ext->prev_failed_t0 = t;
			ext->prev_failed    = false;
			
			//cout << "connection delay = " << delay << endl;
		}
		
		catch (CORBA::SystemException &ce)
		{
			connection_state = CONNECTION_NOTOK;
		
			TangoSys_OMemStream desc;
			desc << "Failed to connect to device " << dev_name() << ends;
								
			ApiConnExcept::re_throw_exception(ce,
						  (const char *)"API_CantConnectToDevice",
						  desc.str(),
					     (const char *)"Connection::reconnect");
		}
	}
	catch (DevFailed &)
	{
		ext->prev_failed    = true;
		ext->prev_failed_t0 = t;
		
		throw;
	}
}

//-----------------------------------------------------------------------------
//
// Connection::get_env_var() - Get an environment variable
//
// This method get an environment variable value from different source.
// which are (orderd by piority)
//
// 1 - A real environmrnt variable
// 2 - A file ".tangorc" in the user home directory
// 3 - A file "/etc/tangorc"
//
// in :	- env_var_name : The environment variable name
//
// out : - env_var : The string initialised with the env. variable value
//
// This method returns 0 of the env. variable is found. Otherwise, it returns -1
//
//-----------------------------------------------------------------------------

#ifndef _TG_WINDOWS_
int Connection::get_env_var(const char *env_var_name,string &env_var)
{
	int ret = -1;
	char *env_c_str;

//
// try to get it as a classical env. variable
//
	
	env_c_str = getenv(env_var_name);
	
	if (env_c_str == NULL)
	{
		uid_t user_id = geteuid();

		struct passwd pw;
		struct passwd *pw_ptr;
		char buffer[1024];

		if (getpwuid_r(user_id,&pw,buffer,sizeof(buffer),&pw_ptr) != 0)
		{
			return ret;
		}

		if (pw_ptr == NULL)
		{
			return ret;
		}

//
// Try to get it from the user hone dir file
//
	
		string home_file(pw.pw_dir);
		home_file = home_file + "/" + USER_ENV_VAR_FILE;

		int local_ret;
		string local_env_var;	
		local_ret = get_env_var_from_file(home_file,env_var_name,local_env_var);

		if (local_ret == 0)
		{
			env_var = local_env_var;
			ret = 0;
		}	
		else
		{
		
//
// Try to get it from a host defined file
//

			home_file = HOST_ENV_VAR_FILE;	
			local_ret = get_env_var_from_file(home_file,env_var_name,local_env_var);
			if (local_ret == 0)
			{
				env_var = local_env_var;
				ret = 0;
			}
		}
	}
	else
	{
		env_var = env_c_str;
		ret = 0;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// Connection::get_env_var_from_file() - Get an environment variable from a file
//
// in :	- env_var : The environment variable name
//		- f_name : The file name
//
// out : - ret_env_var : The string initialised with the env. variable value
//
// This method returns 0 of the env. variable is found. Otherwise, it returns -1
//
//-----------------------------------------------------------------------------

int Connection::get_env_var_from_file(string &f_name,const char *env_var,string &ret_env_var)
{
    ifstream inFile;
	string file_line;
	string var(env_var);
	int ret = -1;
    
    inFile.open(f_name.c_str());
    if (!inFile)
	{
        return ret;
    }
 
	transform(var.begin(),var.end(),var.begin(),::tolower);
	   
    while (!inFile.eof())
	{
		getline(inFile,file_line);
		transform(file_line.begin(),file_line.end(),file_line.begin(),::tolower);
		
		if (file_line.find(var) != string::npos)
		{
			string::size_type pos;
			if ((pos = file_line.find('=')) != string::npos)
			{
				string tg_host = file_line.substr(pos + 1);
				string::iterator end_pos = remove(tg_host.begin(),tg_host.end(),' ');
				tg_host.erase(end_pos,tg_host.end());

				ret_env_var = tg_host;
				ret = 0;
				break;
			}
		}
    }
    
    inFile.close();
    return ret;
}
#endif

//-----------------------------------------------------------------------------
//
// Connection::get_fqdn() - Get a host fqdn (Fully Qualified domain name)
//
// This method gets the host fully qualified domain name (from DNS) and
// modified the passes string accordingly
//
// in :	- host : The host name
//
//-----------------------------------------------------------------------------

void Connection::get_fqdn(string &host)
{
	struct hostent *he;
	he = gethostbyname(host.c_str());

	if (he != NULL)
	{
		string na(he->h_name);
		string::size_type pos = na.find('.');
		if (pos == string::npos)
		{
			char **p;
			for (p = he->h_aliases;*p != 0;++p)
			{
				string al(*p);
				pos = al.find('.');
				if (pos != string::npos)
				{
					host = al;
					break;
				}					
			}
		}
		else
			host = na;
	}
}

//-----------------------------------------------------------------------------
//
// Connection::get_timeout_millis() - public method to get timeout on a TANGO device
//
//-----------------------------------------------------------------------------

int Connection::get_timeout_millis()
{
	return(timeout);
}


//-----------------------------------------------------------------------------
//
// Connection::set_timeout_millis() - public method to set timeout on a TANGO device
//
//-----------------------------------------------------------------------------

void Connection::set_timeout_millis(int millisecs)
{

	timeout = millisecs;
	
	try
	{
		if (connection_state != CONNECTION_OK)
			reconnect(dbase_used);
	
		omniORB::setClientCallTimeout(device,millisecs);

		if (version == 4)
		{
			omniORB::setClientCallTimeout(ext->device_4,millisecs);
			omniORB::setClientCallTimeout(ext->device_3,millisecs);
			omniORB::setClientCallTimeout(device_2,millisecs);
		}
		else if (version == 3)
		{
			omniORB::setClientCallTimeout(ext->device_3,millisecs);
			omniORB::setClientCallTimeout(device_2,millisecs);
		}	
		else if (version == 2)
		{
			omniORB::setClientCallTimeout(device_2,millisecs);
		}
	}
	catch (Tango::DevFailed &) {}
}


//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//
//-----------------------------------------------------------------------------


DeviceData Connection::command_inout(string &command)
{
	DeviceData data_in;

	return(command_inout(command,data_in));
}

//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//
//-----------------------------------------------------------------------------


DeviceData Connection::command_inout(string &command, DeviceData &data_in)
{
	
//
// We are using a pointer to an Any as the return value of the command_inout
// call. This is because the assignament to the Any_var any in the
// DeviceData object in faster in this case (no copy).
// Don't forget that the any_var in the DeviceData takes ownership of the
// memory allocated 
//

	DeviceData data_out;
	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);
			
//
// Manage control access in case the access right
// is READ_ONLY. We need to check if the command is a
// "READ" command or not
//
					
			if (access == ACCESS_READ)
			{
				int db_num;
				ApiUtil *au = ApiUtil::instance();
						
				vector<Database *> & v_d = au->get_db_vect();
				Database *db;
				if (v_d.size() == 0)
					db = static_cast<Database *>(this);
				else
				{
					if (get_from_env_var() == true)
						db_num = au->get_db_ind();
					else
						db_num = au->get_db_ind(get_db_host(),get_db_port_num());
					db = v_d[db_num];
/*					if (db->is_control_access_checked() == false)
						db = static_cast<Database *>(this);*/
				}

//
// If the command is not allowed, throw exception
// Also throw exception if it was not possible to get the list
// of allowed commands from the control access service
//
// The ping rule is simply to send to the client correct
// error message in case of re-connection
//
						
				string d_name = dev_name();
				if (db->is_command_allowed(d_name,command) == false)
				{
					try
					{
						device->ping();
					}
					catch(...) {connection_state = CONNECTION_NOTOK;throw;}
							
					DevErrorList &e = db->get_access_except_errors();
/*					if (e.length() != 0)
					{
						DevFailed df(e);
						throw df;
					}*/
							
					TangoSys_OMemStream desc;
					if (e.length() == 0)
						desc << "Command " << command << " on device " << dev_name() << " is not authorized" << ends;
					else
						desc << "Command " << command << " on device " << dev_name() << " is not authorized because an error occurs while talking to the Controlled Access Service" << ends;
							
					NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
													  (const char *)"Connection::command_inout()");
				}
			}
				
//
// Now, try to execute the command
//		
			
			CORBA::Any *received;
			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());
				
				received = ext->device_4->command_inout_4(command.c_str(),data_in.any,source,ci);
			}
			else if (version >= 2)
				received = device_2->command_inout_2(command.c_str(),data_in.any,source);
			else
				received = device->command_inout(command.c_str(),data_in.any);

			ctr = 2;
			data_out.any = received;
		}
		catch (Tango::ConnectionFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout on device " << dev_name();
			desc << ", command " << command << ends;
			ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        	desc.str(), (const char*)"Connection::command_inout()");
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout on device " << dev_name();
			desc << ", command " << command << ends;
			
			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"Connection::command_inout()");
			else
				Except::re_throw_exception(e,(const char*)"API_CommandFailed",
                        	desc.str(), (const char*)"Connection::command_inout()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT_CMD(trans);		
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT_CMD(one);
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_inout on device " << dev_name();
				desc << ", command " << command << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"Connection::command_inout()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT_CMD(comm);
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_inout on device " << dev_name();
				desc << ", command " << command << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"Connection::command_inout()");
			}
		}   
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout on device " << dev_name();
			desc << ", command " << command << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                        			   desc.str(),
						   (const char*)"Connection::command_inout()");
		}
	}
	
	return data_out;
}

//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//				 using low level CORBA types
//
//-----------------------------------------------------------------------------


CORBA::Any_var Connection::command_inout(string &command, CORBA::Any &any)
{		
	int ctr = 0;
	
	while (ctr < 2)
	{
		try
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			
//
// Manage control access in case the access right
// is READ_ONLY. We need to check if the command is a
// "READ" command or not
//
					
			if (access == ACCESS_READ)
			{
				int db_num;
				ApiUtil *au = ApiUtil::instance();
						
				vector<Database *> & v_d = au->get_db_vect();
				Database *db;
				if (v_d.size() == 0)
					db = static_cast<Database *>(this);
				else
				{
					if (get_from_env_var() == true)
						db_num = au->get_db_ind();
					else
						db_num = au->get_db_ind(get_db_host(),get_db_port_num());
					db = v_d[db_num];
/*					if (db->is_control_access_checked() == false)
						db = static_cast<Database *>(this);*/
				}

//
// If the command is not allowed, throw exception
// Also throw exception if it was not possible to get the list
// of allowed commands from the control access service
//
// The ping rule is simply to send to the client correct
// error message in case of re-connection
//
						
				string d_name = dev_name();
				if (db->is_command_allowed(d_name,command) == false)
				{
					try
					{
						device->ping();
					}
					catch(...) {connection_state = CONNECTION_NOTOK;throw;}
							
					DevErrorList &e = db->get_access_except_errors();
/*					if (e.length() != 0)
					{
						DevFailed df(e);
						throw df;
					}*/
							
					TangoSys_OMemStream desc;
					if (e.length() == 0)
						desc << "Command " << command << " on device " << dev_name() << " is not authorized" << ends;
					else
						desc << "Command " << command << " on device " << dev_name() << " is not authorized because an error occurs while talking to the Controlled Access Service" << ends;
							
					NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
													  (const char *)"Connection::command_inout()");
				}
			}
							
						
			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());
				
				return (ext->device_4->command_inout_4(command.c_str(),any,source,ci));
			}
			else if (version >= 2)
				return (device_2->command_inout_2(command.c_str(),any,source));
			else
				return (device->command_inout(command.c_str(),any));
			ctr = 2;
		}
		catch (Tango::ConnectionFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout on device " << dev_name();
			desc << ", command " << command << ends;
			ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        	desc.str(), (const char*)"Connection::command_inout()");
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout on device " << dev_name();
			desc << ", command " << command << ends;
			
			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"Connection::command_inout()");
			else
				Except::re_throw_exception(e,(const char*)"API_CommandFailed",
                        	desc.str(), (const char*)"Connection::command_inout()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT_CMD(trans);
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT_CMD(one);
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_inout on device " << dev_name();
				desc << ", command " << command << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"Connection::command_inout()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT_CMD(comm);
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_inout on device " << dev_name();
				desc << ", command " << command << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"Connection::command_inout()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout on device " << dev_name();
			desc << ", command " << command << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                        			   desc.str(),
						   (const char*)"Connection::command_inout()");
		}
	}
//
// Just to make VC++ quiet (will never reach this code !)
//

	CORBA::Any_var tmp;
	return tmp;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - constructor for device proxy object 
//
//-----------------------------------------------------------------------------

DeviceProxy::DeviceProxy (string &name, CORBA::ORB *orb) : Connection(orb),
							   is_alias(false),
							   adm_device(NULL),
							   lock_ctr(0),db_dev(NULL),
							   ext_proxy(NULL)
{
	real_constructor(name,true);
}

DeviceProxy::DeviceProxy (const char *na, CORBA::ORB *orb) : Connection(orb),
							     is_alias(false),
							     adm_device(NULL),
							     lock_ctr(0),db_dev(NULL),
							     ext_proxy(NULL)
{
	string name(na);
	real_constructor(name,true);
}

DeviceProxy::DeviceProxy (string &name, bool need_check_acc,CORBA::ORB *orb) : Connection(orb),
								 is_alias(false),
								 adm_device(NULL),
								 lock_ctr(0),db_dev(NULL),
							     ext_proxy(NULL)
{
	real_constructor(name,need_check_acc);
}

DeviceProxy::DeviceProxy (const char *na, bool need_check_acc,CORBA::ORB *orb) : Connection(orb),
								 is_alias(false),
								 adm_device(NULL),
								 lock_ctr(0),db_dev(NULL),
							     ext_proxy(NULL)
{
	string name(na);
	real_constructor(name,need_check_acc);
}

void DeviceProxy::real_constructor (string &name,bool need_check_acc)
{

//
// parse device name
//

	parse_name(name);
	string corba_name;
	bool exported = true;
		
	if (dbase_used == true)
	{
		try
		{
			if (from_env_var == true)
			{	
				ApiUtil *ui = ApiUtil::instance();
				db_dev = new DbDevice(device_name);
				int ind = ui->get_db_ind();
				db_host = (ui->get_db_vect())[ind]->get_db_host();
				db_port = (ui->get_db_vect())[ind]->get_db_port();
				db_port_num = (ui->get_db_vect())[ind]->get_db_port_num();
			}
			else
			{
				db_dev = new DbDevice(device_name,db_host,db_port);
			}
		}
		catch (Tango::DevFailed &e)
		{
			if (strcmp(e.errors[0].reason.in(),"API_TangoHostNotSet") == 0)
			{
				cerr << e.errors[0].desc.in() << endl;
			}
			throw;
		}

		try
		{		
			corba_name = get_corba_name(need_check_acc);
		}
		catch (Tango::DevFailed &dfe)
		{
			if (strcmp(dfe.errors[0].reason,"DB_DeviceNotDefined") == 0)
			{
				delete db_dev;
				TangoSys_OMemStream desc;
				desc << "Can't connect to device " << device_name << ends;
				ApiConnExcept::re_throw_exception(dfe,
						(const char *)"API_DeviceNotDefined",
					 	desc.str(),
						(const char *)"DeviceProxy::DeviceProxy");
			}
			else if (strcmp(dfe.errors[0].reason,"API_DeviceNotExported") == 0)
				exported = false;
		}
	}
	else
	{
		corba_name = build_corba_name();
	}

//
// Implement stateless new() i.e. even if connect fails continue
// If the DeviceProxy was created using device alias, ask for the real
// device name.
//

	try
	{
		if (exported == true)
		{
			connect(corba_name);
			
			if (is_alias == true)
			{
				CORBA::String_var real_name = device->name();
				device_name = real_name.in();
				transform(device_name.begin(),device_name.end(),device_name.begin(),::tolower);
				db_dev->set_name(device_name);
			}
		}
	}
	catch (Tango::ConnectionFailed &dfe)
	{
		connection_state = CONNECTION_NOTOK;
		if (dbase_used == false)
		{
			if (strcmp(dfe.errors[1].reason,"API_DeviceNotDefined") == 0)
				throw;
		}
	}
	catch (CORBA::SystemException &) 
	{
		connection_state = CONNECTION_NOTOK;
		if (dbase_used == false)
			throw;
	}
	
//
// For non-database device , try to ping them. It's the only way to know that 
// the device is not defined
//

	if (dbase_used == false)
	{
		try
		{
			ping();
		}
		catch (Tango::ConnectionFailed &dfe)
		{
			if (strcmp(dfe.errors[1].reason,"API_DeviceNotDefined") == 0)
				throw;
		}
	}
	
	
// get the name of the asscociated device when connecting
// inside a device server

	try
	{
		ApiUtil *ui = ApiUtil::instance();
		if (ui->in_server() == true)
		{
			Tango::Util *tg  = Tango::Util::instance(false);
			tg->get_sub_dev_diag().register_sub_device (tg->get_sub_dev_diag().get_associated_device(), name);
		}
	}
	catch (Tango::DevFailed &e)
	{
		if (::strcmp(e.errors[0].reason.in(),"API_UtilSingletonNotCreated") != 0)
			throw;
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - copy constructor
//
//-----------------------------------------------------------------------------

DeviceProxy::DeviceProxy(const DeviceProxy &sou):Connection(sou)
{
	
//
// Copy DeviceProxy members
//
	
	device_name = sou.device_name;
	alias_name = sou.alias_name;
	is_alias = sou.is_alias;
	adm_dev_name = sou.adm_dev_name;
	lock_ctr = sou.lock_ctr;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
				db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
			else
				db_dev = new DbDevice(device_name);
		}
		else
		{
			db_dev = new DbDevice(device_name,db_host,db_port);
		}
	}

//
// Copy adm device pointer
//
		
	if (sou.adm_device == NULL)
		adm_device = NULL;
	else
	{
		adm_device = new DeviceProxy(sou.adm_device->dev_name().c_str());
	}
	
//
// Copy extension class
//

	if (sou.ext_proxy == NULL)
		ext_proxy = NULL;
	else
	{
		ext_proxy = new DeviceProxyExt();
		*ext_proxy = *(sou.ext_proxy);
	}

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - assignement operator 
//
//-----------------------------------------------------------------------------

DeviceProxy &DeviceProxy::operator=(const DeviceProxy &rval)
{

//
// First Connection class members
//
	if (dbase_used == true)
		delete db_dev;
	dbase_used = rval.dbase_used;
	from_env_var = rval.from_env_var;
	host = rval.host;
	port = rval.port;
	port_num = rval.port_num;
	db_host = rval.db_host;
	db_port = rval.db_port;
	db_port_num = rval.db_port_num;
	ior = rval.ior;
	pasyn_ctr = rval.pasyn_ctr;
	pasyn_cb_ctr = rval.pasyn_cb_ctr;
	device = rval.device;
	device_2 = rval.device_2;
	timeout = rval.timeout;
	connection_state = rval.connection_state;
	version = rval.version;
	source = rval.source;
	if (ext != NULL)
		delete ext;
	if (rval.ext != NULL)
	{
		ext = new ConnectionExt();
		*ext = *rval.ext;
	}
	else
		ext = NULL;
	
//
// Now DeviceProxy members
//
	
	device_name = rval.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
				db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
			else
				db_dev = new DbDevice(device_name);
		}
		else
		{
			db_dev = new DbDevice(device_name,db_host,db_port);
		}
	}
	
	if (adm_device != NULL)
		delete adm_device;
	if (rval.adm_device != NULL)
	{
		adm_device = new DeviceProxy(rval.adm_device->dev_name().c_str());
	}
	else
		adm_device = NULL;
	
	if (ext_proxy != NULL)
		delete ext_proxy;
	if (rval.ext_proxy != NULL)
	{
		ext_proxy = new DeviceProxyExt;
		*ext_proxy = *(rval.ext_proxy);
	}
	else
		ext_proxy = NULL;
		
	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::parse_name() - Parse device name according to Tango device
//			       name syntax
//
// in :	- full_name : The device name
//
//-----------------------------------------------------------------------------

void DeviceProxy::parse_name(string &full_name)
{
	string name_wo_prot;
	string name_wo_db_mod;
	string dev_name,object_name;
	
//
// Device name in lower case letters
//

	string full_name_low(full_name);	
	transform(full_name_low.begin(),full_name_low.end(),full_name_low.begin(),::tolower);
		
//
// Try to find protocol specification in device name and analyse it
//

	string::size_type pos = full_name_low.find(PROT_SEP);
	if (pos == string::npos)
	{
		if (full_name_low.size() > 2)
		{
			if ((full_name_low[0] == '/') && (full_name_low[1] == '/'))
				name_wo_prot = full_name_low.substr(2);
			else
				name_wo_prot = full_name_low;
		}
		else
			name_wo_prot = full_name_low;
	}
	else
	{
		string protocol = full_name_low.substr(0,pos);

		if (protocol == TANGO_PROTOCOL)
		{
			name_wo_prot = full_name_low.substr(pos + 3);
		}		
		else if (protocol == TACO_PROTOCOL)
		{		
			TangoSys_OMemStream desc;
			desc << "Taco protocol is not supported" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
			exit(-1);
		}
		else
		{		
			TangoSys_OMemStream desc;
			desc << protocol;
			desc << " protocol is an unsupported protocol" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
		}			
	}

//
// Try to find database database modifier and analyse it
//

	pos = name_wo_prot.find(MODIFIER);
	if (pos != string::npos)
	{
		string mod = name_wo_prot.substr(pos + 1);
		
		if (mod == DBASE_YES)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos)); 
			dbase_used = true;
		}
		else if (mod == DBASE_NO)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos));
			dbase_used = false;
		}
		else
		{
			//cerr << mod << " is a non supported database modifier!" << endl;

			TangoSys_OMemStream desc;
			desc << mod;
			desc << " modifier is an unsupported db modifier" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedDBaseModifier",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
		}
	}
	else
	{
		name_wo_db_mod = name_wo_prot;
		dbase_used = true;
	}

	if (dbase_used == false)
	{
	
//
// Extract host name and port number
//

		pos = name_wo_db_mod.find(HOST_SEP);
		if (pos == string::npos)
		{
			TangoSys_OMemStream desc;
			desc << "Host and port not correctly defined in device name " << full_name << ends;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
		}
		
		host = name_wo_db_mod.substr(0,pos);
		string::size_type tmp = name_wo_db_mod.find(PORT_SEP);
		if (tmp == string::npos)
		{
			TangoSys_OMemStream desc;
			desc << "Host and port not correctly defined in device name " << full_name << ends;
		
			ApiWrongNameExcept::throw_exception((const char*)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
		}
		port = name_wo_db_mod.substr(pos + 1,tmp - pos - 1);
		TangoSys_MemStream s;
		s << port << ends;
		s >> port_num;
		device_name = name_wo_db_mod.substr(tmp + 1);
		
//
// Check device name syntax (domain/family/member). Alias are forbidden without
// using the db
//

		tmp = device_name.find(DEV_NAME_FIELD_SEP);
		if (tmp == string::npos)
		{
			TangoSys_OMemStream desc;
			desc << "Wrong device name syntax (domain/family/member) in " << full_name << endl;
			desc << "Rem: Alias are forbidden when not using a database" << ends;
			
			ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
		}
		string::size_type prev_sep = tmp;
		tmp = device_name.find(DEV_NAME_FIELD_SEP,tmp + 1);
		if ((tmp == string::npos) || (tmp == prev_sep + 1))
		{
			TangoSys_OMemStream desc;
			desc << "Wrong device name syntax (domain/family/member) in " << full_name << endl;
			desc << "Rem: Alias are forbidden when not using a database" << ends;
			
			ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
		}
		prev_sep = tmp;
		tmp = device_name.find(DEV_NAME_FIELD_SEP,tmp + 1);
		if (tmp != string::npos)
		{
			TangoSys_OMemStream desc;
			desc << "Wrong device name syntax (domain/family/member) in " << full_name << endl;
			desc << "Rem: Alias are forbidden when not using a database" << ends;
			
			ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
		}
				
		db_host = db_port = NOT_USED;
		db_port_num = 0;
				 
	}
	else
	{
	
//
// Search if host and port are specified
//

		pos = name_wo_db_mod.find(PORT_SEP);
		if (pos == string::npos)
		{

//
// It could be an alias name, check its syntax
//

			pos = name_wo_db_mod.find(HOST_SEP);
			if (pos != string::npos)
			{
				TangoSys_OMemStream desc;
				desc << "Wrong alias name syntax in " << full_name << " (: is not allowed in alias name)" << ends;
						
				ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
			}
			
			pos = name_wo_db_mod.find(RES_SEP);
			if (pos != string::npos)
			{
				TangoSys_OMemStream desc;
				desc << "Wrong alias name syntax in " << full_name << " (-> is not allowed in alias name)" << ends;
			
				ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
			}
			
//
// Alias name syntax OK
//

			alias_name = device_name = name_wo_db_mod;
			is_alias = true;
			from_env_var = true;
			port_num = 0;
			host = FROM_IOR;
			port = FROM_IOR;
		}
		else
		{
			string bef_sep = name_wo_db_mod.substr(0,pos);
			string::size_type tmp = bef_sep.find(HOST_SEP);
			if (tmp == string::npos)
			{
			
//
// There is at least one / in dev name but it is not a TANGO_HOST definition. 
// A correct dev name must have 2 /. Check this. An alias cannot have any /
//

				if (pos == 0)
				{
					TangoSys_OMemStream desc;
					desc << "Wrong device name syntax (domain/family/member) in " << full_name << ends;

					ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
				}
				
				string::size_type prev_sep = pos;
				pos = name_wo_db_mod.find(DEV_NAME_FIELD_SEP,pos + 1);
				if ((pos == string::npos) || (pos == prev_sep + 1))
				{
				
					TangoSys_OMemStream desc;
					desc << "Wrong device name syntax (domain/family/member) in " << full_name << ends;
			
					ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
				}
				
				prev_sep = pos;
				pos = name_wo_db_mod.find(DEV_NAME_FIELD_SEP,prev_sep + 1);
				if (pos != string::npos)
				{
					TangoSys_OMemStream desc;
					desc << "Wrong device name syntax (domain/family/member) in " << full_name << ends;
			
					ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
				}
										
				device_name = name_wo_db_mod;
				from_env_var = true;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
			else
			{
				string tmp_host(bef_sep.substr(0,tmp));
				if (tmp_host.find('.') == string::npos)
					get_fqdn(tmp_host);
				db_host = tmp_host;
				db_port = bef_sep.substr(tmp + 1);
				TangoSys_MemStream s;
				s << db_port << ends;
				s >> db_port_num;
				object_name = name_wo_db_mod.substr(pos + 1);
				
//
// We should now check if the object name is a device name or an alias
//

				pos = object_name.find(DEV_NAME_FIELD_SEP);
				if (pos == string::npos)
				{
				
//
// It is an alias. Check its syntax
//

					pos = object_name.find(HOST_SEP);
					if (pos != string::npos)
					{
						TangoSys_OMemStream desc;
						desc << "Wrong alias name syntax in " << full_name << " (: is not allowed in alias name)" << ends;
					
						ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
					}
			
					pos = object_name.find(RES_SEP);
					if (pos != string::npos)
					{					
						TangoSys_OMemStream desc;
						desc << "Wrong alias name syntax in " << full_name << " (-> is not allowed in alias name)" << ends;

						ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
					}
					alias_name = device_name = object_name;
					is_alias = true;
			
//
// Alias name syntax OK, but is it really an alias defined in db ?
//
				}
				else
				{
				
//
// It's a device name. Check its syntax.
// There is at least one / in dev name but it is not a TANGO_HOST definition. 
// A correct dev name must have 2 /. Check this. An alias cannot have any /
//

					string::size_type prev_sep = pos;
					pos = object_name.find(DEV_NAME_FIELD_SEP,pos + 1);
					if ((pos == string::npos) || (pos == prev_sep + 1))
					{
						TangoSys_OMemStream desc;
						desc << "Wrong device name syntax (domain/family/member) in " << full_name << ends;
							
						ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
					}

					prev_sep = pos;
					pos = object_name.find(DEV_NAME_FIELD_SEP,prev_sep + 1);
					if (pos != string::npos)
					{
						TangoSys_OMemStream desc;
						desc << "Wrong device name syntax (domain/family/member) in " << full_name << ends;
					
						ApiWrongNameExcept::throw_exception((const char *)"API_WrongDeviceNameSyntax",
						desc.str(),
						(const char *)"DeviceProxy::parse_name()");
					}
								
					device_name = object_name;				
				}
				from_env_var = false;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
		}

	}
		
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_corba_name() - return IOR for device from database
//
//-----------------------------------------------------------------------------

string DeviceProxy::get_corba_name(bool need_check_acc)
{
	DbDevImportInfo import_info;

	import_info = db_dev->import_device();

	if (import_info.exported != 1)
	{
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;		
		desc << "Device " << device_name << " is not exported (hint: try starting the device server)" << ends;
		ApiConnExcept::throw_exception((const char*)"API_DeviceNotExported",
					       desc.str(),
					       (const char*)"DeviceProxy::get_corba_name()");
	}

//
// Get device access right
//

	if (need_check_acc == true)
		access = db_dev->check_access_control();
	else
		check_acc = false;
				
	return(import_info.ior);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::build_corba_name() - build corba name for non database device
//				     server. In this case, corba name uses
//				     the "corbaloc" naming schema
//
//-----------------------------------------------------------------------------

string DeviceProxy::build_corba_name()
{

	string db_corbaloc = "corbaloc:iiop:";
	db_corbaloc = db_corbaloc + host + ":" + port;
	db_corbaloc = db_corbaloc + "/" + device_name;

	return db_corbaloc;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::reconnect() - Call the reconnection method and in case
//			      the device has been created from its alias,
//		              get its real name.
//
//-----------------------------------------------------------------------------

void DeviceProxy::reconnect(bool db_used)
{
	Connection::reconnect(db_used);

	if (connection_state == CONNECTION_OK)
	{	
		if (is_alias == true)
		{
			CORBA::String_var real_name = device->name();
			device_name = real_name.in();
			transform(device_name.begin(),device_name.end(),device_name.begin(),::tolower);
			db_dev->set_name(device_name);
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::import_info() - return import info for device from database
//
//-----------------------------------------------------------------------------

DbDevImportInfo DeviceProxy::import_info()
{
	DbDevImportInfo import_info;
	
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					        desc.str(),
					        (const char *)"DeviceProxy::import_info");
	}
	else
	{
		import_info = db_dev->import_device();
	}

	return(import_info);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::~DeviceProxy() - destructor to destroy proxy to TANGO device
//
//-----------------------------------------------------------------------------

DeviceProxy::~DeviceProxy()
{

	if (dbase_used == true)
		delete db_dev;

//
// If the device is locked, unlock it whatever the lock counter is
//

	if (ApiUtil::_is_instance_null() == false)
	{
		if (lock_ctr > 0)
		{
			try
			{
				unlock(true);
			}
			catch(...) {}
		}
	}

//
// Delete memory
//
	
	if (adm_device != NULL)
		delete adm_device;
		
	if (ext_proxy != NULL)
		delete ext_proxy;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::ping() - ping TANGO device and return time elapsed in microseconds
//
//-----------------------------------------------------------------------------

int DeviceProxy::ping()
{
	int elapsed;

#ifndef _TG_WINDOWS_
	struct timeval before, after;

	gettimeofday(&before, NULL);
#else
	struct _timeb before, after;

	_ftime(&before);
#endif /* _TG_WINDOWS_ */

	int ctr = 0;
	
	while (ctr < 2)
	{
		try
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			device->ping();
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","ping");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","ping");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute ping on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::ping()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","ping");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute ping on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::ping()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;			
			desc << "Failed to execute ping on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                        	desc.str(),
						   (const char*)"DeviceProxy::ping()");
		}
	}
#ifndef _TG_WINDOWS_
	gettimeofday(&after, NULL);
	elapsed = (after.tv_sec-before.tv_sec)*1000000;
	elapsed = (after.tv_usec-before.tv_usec) + elapsed;
#else
	_ftime(&after);
	elapsed = (after.time-before.time)*1000000;
	elapsed = (after.millitm-before.millitm)*1000 + elapsed;
#endif /* _TG_WINDOWS_ */

	return(elapsed);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::name() - return TANGO device name as string
//
//-----------------------------------------------------------------------------

string DeviceProxy::name()
{
	string na;
	int ctr = 0;
	
	while (ctr < 2)
	{
		try
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			CORBA::String_var n = device->name();
			ctr = 2;
			na = n;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","name");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","name");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute name() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::name()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","name");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute name() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::name()");
			}
		}   
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute name() on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::name()");
		}
	}

	return(na);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::alias() - return TANGO device alias (if any)
//
//-----------------------------------------------------------------------------

string DeviceProxy::alias()
{
	if (alias_name.size() == 0)
	{
		Database *db = this->get_device_db();
		if (db != NULL)
			db->get_alias(device_name,alias_name);
		else
		{
			Tango::Except::throw_exception((const char *)"DB_AliasNotDefined",
	   						               (const char *)"No alias found for your device",
							               (const char *)"DeviceProxy::alias()");
		}
	}
	
	return alias_name;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::state() - return TANGO state of device
//
//-----------------------------------------------------------------------------

DevState DeviceProxy::state()
{
	DevState sta = Tango::UNKNOWN;
	int ctr = 0;

	while (ctr < 2)
	{	
		try
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			sta = device->state();
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &transp)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(transp,"DeviceProxy","state");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","state");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute state() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::state()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","state");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute state() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::state()");
			}
		}
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute state() on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                    				      desc.str(),
						      (const char*)"DeviceProxy::state()");
		}
	}
	
	return sta;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::status() - return TANGO status of device
//
//-----------------------------------------------------------------------------

string DeviceProxy::status()
{
	string status_str;
	int ctr = 0;

	while (ctr < 2)
	{
		try
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			CORBA::String_var st = device->status();
			ctr = 2;
			status_str = st;
		}
		catch (CORBA::TRANSIENT &transp)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(transp,"DeviceProxy","status");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","status");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute status() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::status()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","status");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute status() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::status()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute status() on device (CORBA exception)" << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::status()");
		}
	}

	return(status_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::adm_name() - return TANGO admin name of device
//
//-----------------------------------------------------------------------------

string DeviceProxy::adm_name()
{
	string adm_name_str;
	int ctr = 0;

	while (ctr < 2)
	{
		try
		{
			if (connection_state != CONNECTION_OK)
				reconnect(dbase_used);

			CORBA::String_var st = device->adm_name();
			ctr = 2;
			adm_name_str = st;
			
			if (dbase_used == false)
			{
				string prot("tango://");
				prot = prot + host + ':' + port + '/';
				adm_name_str.insert(0,prot);
				adm_name_str.append("#dbase=no");
			}
			else if (from_env_var == false)
			{
				string prot("tango://");
				prot = prot + db_host + ':' + db_port + '/';
				adm_name_str.insert(0,prot);
			}
		}
		catch (CORBA::TRANSIENT &transp)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(transp,"DeviceProxy","adm_name");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","adm_name");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute adm_name() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::adm_name()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","adm_name");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute adm_name() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::adm_name()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute adm_name() on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::adm_name()");
		}
	}

	return(adm_name_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::description() - return TANGO device description as string
//
//-----------------------------------------------------------------------------

string DeviceProxy::description()
{
	string description_str;
	int ctr = 0;

	while (ctr < 2)
	{
		try
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			CORBA::String_var st = device->description();
			ctr = 2;
			description_str = st;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","description");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","description");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute description() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::description()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","description");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute description() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::description()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute description() on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::description()");
		}
	}

	return(description_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::black_box() - return the list of the last n commands exectued on
//		this TANGO device 
//
//-----------------------------------------------------------------------------
 
vector<string> *DeviceProxy::black_box(int last_n_commands) 
{
	DevVarStringArray_var last_commands;
	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			last_commands = device->black_box(last_n_commands);
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","black_box");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","black_box");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute black_box on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::black_box()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","black_box");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute black_box on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::black_box()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute black_box on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                      				      desc.str(),
						      (const char*)"DeviceProxy::black_box()");
		}
	}

	vector<string> *last_commands_vector = new(vector<string>);
	last_commands_vector->resize(last_commands->length());

	for (unsigned int i=0; i<last_commands->length(); i++)
	{
		(*last_commands_vector)[i] = last_commands[i];
	}

	return(last_commands_vector);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::info() - return information about this device
//
//-----------------------------------------------------------------------------
 
DeviceInfo const &DeviceProxy::info() 
{
	DevInfo_var dev_info;
	DevInfo_3_var dev_info_3;
	int ctr = 0;

	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version >= 3)
			{
				dev_info_3 = ext->device_3->info_3();

				_info.dev_class = dev_info_3->dev_class;
				_info.server_id = dev_info_3->server_id;
				_info.server_host = dev_info_3->server_host;
				_info.server_version = dev_info_3->server_version;
				_info.doc_url = dev_info_3->doc_url;
				_info.dev_type = dev_info_3->dev_type;
			}
			else
			{
				dev_info = device->info();
				
				_info.dev_class = dev_info->dev_class;
				_info.server_id = dev_info->server_id;
				_info.server_host = dev_info->server_host;
				_info.server_version = dev_info->server_version;
				_info.doc_url = dev_info->doc_url;
				_info.dev_type = DescNotSet;
			}
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","info");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","info");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute info() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::info()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","info");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute info() on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::info()");
			}
		}
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute info() on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                        			   desc.str(),
						   (const char*)"DeviceProxy::info()");
		}
	}

	return(_info);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::command_query() - return the description for the specified 
//		command implemented for this TANGO device 
//
//-----------------------------------------------------------------------------
 
CommandInfo DeviceProxy::command_query(string cmd) 
{
	CommandInfo command_info;
	DevCmdInfo_var cmd_info;
	DevCmdInfo_2_var cmd_info_2;
	int ctr = 0;

	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version == 1)
			{
				cmd_info = device->command_query(cmd.c_str());

				command_info.cmd_name = cmd_info->cmd_name;
				command_info.cmd_tag = cmd_info->cmd_tag;
				command_info.in_type = cmd_info->in_type;
				command_info.out_type = cmd_info->out_type;
				command_info.in_type_desc = cmd_info->in_type_desc;
				command_info.out_type_desc = cmd_info->out_type_desc;
				command_info.disp_level = Tango::OPERATOR;
			}
			else
			{
				cmd_info_2 = device_2->command_query_2(cmd.c_str());

				command_info.cmd_name = cmd_info_2->cmd_name;
				command_info.cmd_tag = cmd_info_2->cmd_tag;
				command_info.in_type = cmd_info_2->in_type;
				command_info.out_type = cmd_info_2->out_type;
				command_info.in_type_desc = cmd_info_2->in_type_desc;
				command_info.out_type_desc = cmd_info_2->out_type_desc;
				command_info.disp_level = cmd_info_2->level;
			}
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","command_query");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","command_query");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_query on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::command_query()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","command_query");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_query on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::command_query()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_query on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                        			   desc.str(),
						   (const char*)"DeviceProxy::command_query()");
		}
	}


	
	return(command_info);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::command_list_query() - return the list of commands implemented for this TANGO device 
//
//-----------------------------------------------------------------------------
 
CommandInfoList *DeviceProxy::command_list_query() 
{
	CommandInfoList *command_info_list = new CommandInfoList();
	DevCmdInfoList_var cmd_info_list;
	DevCmdInfoList_2_var cmd_info_list_2;
	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version == 1)
			{	
				cmd_info_list = device->command_list_query();

				command_info_list->resize(cmd_info_list->length());

				for (unsigned int i=0; i < cmd_info_list->length(); i++)
				{
					(*command_info_list)[i].cmd_name = cmd_info_list[i].cmd_name;
					(*command_info_list)[i].cmd_tag = cmd_info_list[i].cmd_tag;
					(*command_info_list)[i].in_type = cmd_info_list[i].in_type;
					(*command_info_list)[i].out_type = cmd_info_list[i].out_type;
					(*command_info_list)[i].in_type_desc = cmd_info_list[i].in_type_desc;
					(*command_info_list)[i].out_type_desc = cmd_info_list[i].out_type_desc;
					(*command_info_list)[i].disp_level = Tango::OPERATOR;
				}
			}
			else
			{
				cmd_info_list_2 = device_2->command_list_query_2();

				command_info_list->resize(cmd_info_list_2->length());

				for (unsigned int i=0; i < cmd_info_list_2->length(); i++)
				{
					(*command_info_list)[i].cmd_name = cmd_info_list_2[i].cmd_name;
					(*command_info_list)[i].cmd_tag = cmd_info_list_2[i].cmd_tag;
					(*command_info_list)[i].in_type = cmd_info_list_2[i].in_type;
					(*command_info_list)[i].out_type = cmd_info_list_2[i].out_type;
					(*command_info_list)[i].in_type_desc = cmd_info_list_2[i].in_type_desc;
					(*command_info_list)[i].out_type_desc = cmd_info_list_2[i].out_type_desc;
					(*command_info_list)[i].disp_level = cmd_info_list_2[i].level;
				}		
			}
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","command_list_query");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","command_list_query");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_list_query on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::command_list_query()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","command_list_query");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute command_list_query on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::command_list_query()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_list_query on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::command_list_query()");
		}
	}



	return(command_info_list);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property(string &property_name, DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property");
	}
	else
	{
		db_data.resize(1);
		db_data[0] = DbDatum(property_name);

		db_dev->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property(vector<string> &property_names, DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property");
	}
	else
	{
		db_data.resize(property_names.size());
		for (unsigned int i=0; i<property_names.size(); i++)
		{
			db_data[i] = DbDatum(property_names[i]);
		}

		db_dev->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property");
	}
	else
	{
		db_dev->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::put_property() - put a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::put_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::put_property");
	}
	else
	{
		db_dev->put_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::delete_property(string &property_name) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::delete_property");
	}
	else
	{
		DbData db_data;

		db_data.push_back(DbDatum(property_name));

		db_dev->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::delete_property(vector<string> &property_names) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::delete_property");
	}
	else
	{
		DbData db_data;

		for (unsigned int i=0; i<property_names.size(); i++)
		{
			db_data.push_back(DbDatum(property_names[i]));
		}

		db_dev->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::delete_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::delete_property");
	}
	else
	{
		db_dev->delete_property(db_data);
	}

	return;
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property_list() - get a list of property names from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property_list(const string &wildcard,vector<string> &prop_list) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property_list");
	}
	else
	{
		int num = 0;
#ifdef __SUNPRO_CC
		count(wildcard.begin(),wildcard.end(),'*',num);
#else
		num = count(wildcard.begin(),wildcard.end(),'*');
#endif
		if (num > 1)
		{
			ApiWrongNameExcept::throw_exception((const char*)"API_WrongWildcardUsage",
						(const char *)"Only one wildcard character (*) allowed!",
						(const char *)"DeviceProxy::get_property_list");
		}
		db_dev->get_property_list(wildcard,prop_list);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_config() - return a list of attributes
//
//-----------------------------------------------------------------------------
 
AttributeInfoList *DeviceProxy::get_attribute_config(vector<string>& attr_string_list) 
{
	AttributeConfigList_var attr_config_list;
	AttributeConfigList_2_var attr_config_list_2;
	AttributeInfoList *dev_attr_config = new AttributeInfoList();
	DevVarStringArray attr_list;
	int ctr = 0;
	
	attr_list.length(attr_string_list.size());
	for (unsigned int i=0; i<attr_string_list.size(); i++)
	{
		if (attr_string_list[i] == AllAttr)
		{
			if (version >= 3)
				attr_list[i] = string_dup(AllAttr_3);
			else
				attr_list[i] = string_dup(AllAttr);
		}
		else if (attr_string_list[i] == AllAttr_3)
		{
			if (version < 3)
				attr_list[i] = string_dup(AllAttr);
			else
				attr_list[i] = string_dup(AllAttr_3);
		}
		else
			attr_list[i] = string_dup(attr_string_list[i].c_str());
	}
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version == 1)
			{
				attr_config_list = device->get_attribute_config(attr_list);

				dev_attr_config->resize(attr_config_list->length());

				for (unsigned int i=0; i<attr_config_list->length(); i++)
				{
					(*dev_attr_config)[i].name = attr_config_list[i].name;
					(*dev_attr_config)[i].writable = attr_config_list[i].writable;
					(*dev_attr_config)[i].data_format = attr_config_list[i].data_format;
					(*dev_attr_config)[i].data_type = attr_config_list[i].data_type;
					(*dev_attr_config)[i].max_dim_x = attr_config_list[i].max_dim_x;
					(*dev_attr_config)[i].max_dim_y = attr_config_list[i].max_dim_y;
					(*dev_attr_config)[i].description = attr_config_list[i].description;
					(*dev_attr_config)[i].label = attr_config_list[i].label;
					(*dev_attr_config)[i].unit = attr_config_list[i].unit;
					(*dev_attr_config)[i].standard_unit = attr_config_list[i].standard_unit;
					(*dev_attr_config)[i].display_unit = attr_config_list[i].display_unit;
					(*dev_attr_config)[i].format = attr_config_list[i].format;
					(*dev_attr_config)[i].min_value = attr_config_list[i].min_value;
					(*dev_attr_config)[i].max_value = attr_config_list[i].max_value;
					(*dev_attr_config)[i].min_alarm = attr_config_list[i].min_alarm;
					(*dev_attr_config)[i].max_alarm = attr_config_list[i].max_alarm;
					(*dev_attr_config)[i].writable_attr_name = attr_config_list[i].writable_attr_name;
					(*dev_attr_config)[i].extensions.resize(attr_config_list[i].extensions.length());
					for (unsigned int j=0; j<attr_config_list[i].extensions.length(); j++)
					{
						(*dev_attr_config)[i].extensions[j] = attr_config_list[i].extensions[j];
					}
					(*dev_attr_config)[i].disp_level = Tango::OPERATOR;
				}
			}
			else
			{
				attr_config_list_2 = device_2->get_attribute_config_2(attr_list);

				dev_attr_config->resize(attr_config_list_2->length());

				for (unsigned int i=0; i<attr_config_list_2->length(); i++)
				{
					(*dev_attr_config)[i].name = attr_config_list_2[i].name;
					(*dev_attr_config)[i].writable = attr_config_list_2[i].writable;
					(*dev_attr_config)[i].data_format = attr_config_list_2[i].data_format;
					(*dev_attr_config)[i].data_type = attr_config_list_2[i].data_type;
					(*dev_attr_config)[i].max_dim_x = attr_config_list_2[i].max_dim_x;
					(*dev_attr_config)[i].max_dim_y = attr_config_list_2[i].max_dim_y;
					(*dev_attr_config)[i].description = attr_config_list_2[i].description;
					(*dev_attr_config)[i].label = attr_config_list_2[i].label;
					(*dev_attr_config)[i].unit = attr_config_list_2[i].unit;
					(*dev_attr_config)[i].standard_unit = attr_config_list_2[i].standard_unit;
					(*dev_attr_config)[i].display_unit = attr_config_list_2[i].display_unit;
					(*dev_attr_config)[i].format = attr_config_list_2[i].format;
					(*dev_attr_config)[i].min_value = attr_config_list_2[i].min_value;
					(*dev_attr_config)[i].max_value = attr_config_list_2[i].max_value;
					(*dev_attr_config)[i].min_alarm = attr_config_list_2[i].min_alarm;
					(*dev_attr_config)[i].max_alarm = attr_config_list_2[i].max_alarm;
					(*dev_attr_config)[i].writable_attr_name = attr_config_list_2[i].writable_attr_name;
					(*dev_attr_config)[i].extensions.resize(attr_config_list_2[i].extensions.length());
					for (unsigned int j=0; j<attr_config_list_2[i].extensions.length(); j++)
					{
						(*dev_attr_config)[i].extensions[j] = attr_config_list_2[i].extensions[j];
					}
					(*dev_attr_config)[i].disp_level = attr_config_list_2[i].level;
				}
			}
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","get_attribute_config");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","get_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute get_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::get_attribute_config()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","get_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute get_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::get_attribute_config()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute get_attribute_config on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                       				   desc.str(),
						   (const char*)"DeviceProxy::get_attribute_config()");
		}
		catch (Tango::DevFailed)
		{
			delete dev_attr_config;
			throw;
		}
	}

	return(dev_attr_config);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_config_ex() - return a list of attributes
//
//-----------------------------------------------------------------------------
 
AttributeInfoListEx *DeviceProxy::get_attribute_config_ex(vector<string>& attr_string_list) 
{
	AttributeConfigList_var attr_config_list;
	AttributeConfigList_2_var attr_config_list_2;
	AttributeConfigList_3_var attr_config_list_3;
	AttributeInfoListEx *dev_attr_config = new AttributeInfoListEx();
	DevVarStringArray attr_list;
	int ctr = 0;
	
	attr_list.length(attr_string_list.size());
	for (unsigned int i=0; i<attr_string_list.size(); i++)
	{
		if (attr_string_list[i] == AllAttr)
		{
			if (version >= 3)
				attr_list[i] = string_dup(AllAttr_3);
			else
				attr_list[i] = string_dup(AllAttr);
		}
		else if (attr_string_list[i] == AllAttr_3)
		{
			if (version < 3)
				attr_list[i] = string_dup(AllAttr);
			else
				attr_list[i] = string_dup(AllAttr_3);
		}
		else
			attr_list[i] = string_dup(attr_string_list[i].c_str());
	}

	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version == 1)
			{
				attr_config_list = device->get_attribute_config(attr_list);

				dev_attr_config->resize(attr_config_list->length());

				for (unsigned int i=0; i<attr_config_list->length(); i++)
				{
					(*dev_attr_config)[i].name = attr_config_list[i].name;
					(*dev_attr_config)[i].writable = attr_config_list[i].writable;
					(*dev_attr_config)[i].data_format = attr_config_list[i].data_format;
					(*dev_attr_config)[i].data_type = attr_config_list[i].data_type;
					(*dev_attr_config)[i].max_dim_x = attr_config_list[i].max_dim_x;
					(*dev_attr_config)[i].max_dim_y = attr_config_list[i].max_dim_y;
					(*dev_attr_config)[i].description = attr_config_list[i].description;
					(*dev_attr_config)[i].label = attr_config_list[i].label;
					(*dev_attr_config)[i].unit = attr_config_list[i].unit;
					(*dev_attr_config)[i].standard_unit = attr_config_list[i].standard_unit;
					(*dev_attr_config)[i].display_unit = attr_config_list[i].display_unit;
					(*dev_attr_config)[i].format = attr_config_list[i].format;
					(*dev_attr_config)[i].min_value = attr_config_list[i].min_value;
					(*dev_attr_config)[i].max_value = attr_config_list[i].max_value;
					(*dev_attr_config)[i].min_alarm = attr_config_list[i].min_alarm;
					(*dev_attr_config)[i].max_alarm = attr_config_list[i].max_alarm;
					(*dev_attr_config)[i].writable_attr_name = attr_config_list[i].writable_attr_name;
					(*dev_attr_config)[i].extensions.resize(attr_config_list[i].extensions.length());
					for (unsigned int j=0; j<attr_config_list[i].extensions.length(); j++)
					{
						(*dev_attr_config)[i].extensions[j] = attr_config_list[i].extensions[j];
					}
					(*dev_attr_config)[i].disp_level = Tango::OPERATOR;
				}
			}
			else if (version == 2)
			{
				attr_config_list_2 = device_2->get_attribute_config_2(attr_list);

				dev_attr_config->resize(attr_config_list_2->length());

				for (unsigned int i=0; i<attr_config_list_2->length(); i++)
				{
					(*dev_attr_config)[i].name = attr_config_list_2[i].name;
					(*dev_attr_config)[i].writable = attr_config_list_2[i].writable;
					(*dev_attr_config)[i].data_format = attr_config_list_2[i].data_format;
					(*dev_attr_config)[i].data_type = attr_config_list_2[i].data_type;
					(*dev_attr_config)[i].max_dim_x = attr_config_list_2[i].max_dim_x;
					(*dev_attr_config)[i].max_dim_y = attr_config_list_2[i].max_dim_y;
					(*dev_attr_config)[i].description = attr_config_list_2[i].description;
					(*dev_attr_config)[i].label = attr_config_list_2[i].label;
					(*dev_attr_config)[i].unit = attr_config_list_2[i].unit;
					(*dev_attr_config)[i].standard_unit = attr_config_list_2[i].standard_unit;
					(*dev_attr_config)[i].display_unit = attr_config_list_2[i].display_unit;
					(*dev_attr_config)[i].format = attr_config_list_2[i].format;
					(*dev_attr_config)[i].min_value = attr_config_list_2[i].min_value;
					(*dev_attr_config)[i].max_value = attr_config_list_2[i].max_value;
					(*dev_attr_config)[i].min_alarm = attr_config_list_2[i].min_alarm;
					(*dev_attr_config)[i].max_alarm = attr_config_list_2[i].max_alarm;
					(*dev_attr_config)[i].writable_attr_name = attr_config_list_2[i].writable_attr_name;
					(*dev_attr_config)[i].extensions.resize(attr_config_list_2[i].extensions.length());
					for (unsigned int j=0; j<attr_config_list_2[i].extensions.length(); j++)
					{
						(*dev_attr_config)[i].extensions[j] = attr_config_list_2[i].extensions[j];
					}
					(*dev_attr_config)[i].disp_level = attr_config_list_2[i].level;
				}
				
				get_remaining_param(dev_attr_config);
			}
			else
			{
				attr_config_list_3 = ext->device_3->get_attribute_config_3(attr_list);

				dev_attr_config->resize(attr_config_list_3->length());

				for (unsigned int i=0; i<attr_config_list_3->length(); i++)
				{
					unsigned int j;
					(*dev_attr_config)[i].name = attr_config_list_3[i].name;
					(*dev_attr_config)[i].writable = attr_config_list_3[i].writable;
					(*dev_attr_config)[i].data_format = attr_config_list_3[i].data_format;
					(*dev_attr_config)[i].data_type = attr_config_list_3[i].data_type;
					(*dev_attr_config)[i].max_dim_x = attr_config_list_3[i].max_dim_x;
					(*dev_attr_config)[i].max_dim_y = attr_config_list_3[i].max_dim_y;
					(*dev_attr_config)[i].description = attr_config_list_3[i].description;
					(*dev_attr_config)[i].label = attr_config_list_3[i].label;
					(*dev_attr_config)[i].unit = attr_config_list_3[i].unit;
					(*dev_attr_config)[i].standard_unit = attr_config_list_3[i].standard_unit;
					(*dev_attr_config)[i].display_unit = attr_config_list_3[i].display_unit;
					(*dev_attr_config)[i].format = attr_config_list_3[i].format;
					(*dev_attr_config)[i].min_value = attr_config_list_3[i].min_value;
					(*dev_attr_config)[i].max_value = attr_config_list_3[i].max_value;
					(*dev_attr_config)[i].min_alarm = attr_config_list_3[i].att_alarm.min_alarm;
					(*dev_attr_config)[i].max_alarm = attr_config_list_3[i].att_alarm.max_alarm;
					(*dev_attr_config)[i].writable_attr_name = attr_config_list_3[i].writable_attr_name;
					(*dev_attr_config)[i].extensions.resize(attr_config_list_3[i].extensions.length());
					for (j=0; j<attr_config_list_3[i].extensions.length(); j++)
					{
						(*dev_attr_config)[i].extensions[j] = attr_config_list_3[i].extensions[j];
					}
					for (j=0; j<attr_config_list_3[i].sys_extensions.length(); j++)
					{
						(*dev_attr_config)[i].sys_extensions[j] = attr_config_list_3[i].sys_extensions[j];
					}
					(*dev_attr_config)[i].disp_level = attr_config_list_3[i].level;
					
					(*dev_attr_config)[i].alarms.min_alarm = attr_config_list_3[i].att_alarm.min_alarm;
					(*dev_attr_config)[i].alarms.max_alarm = attr_config_list_3[i].att_alarm.max_alarm;
					(*dev_attr_config)[i].alarms.min_warning = attr_config_list_3[i].att_alarm.min_warning;
					(*dev_attr_config)[i].alarms.max_warning = attr_config_list_3[i].att_alarm.max_warning;					
					(*dev_attr_config)[i].alarms.delta_t = attr_config_list_3[i].att_alarm.delta_t;
					(*dev_attr_config)[i].alarms.delta_val = attr_config_list_3[i].att_alarm.delta_val;
					(*dev_attr_config)[i].alarms.extensions.resize(attr_config_list_3[i].att_alarm.extensions.length());
					for (j=0; j<attr_config_list_3[i].att_alarm.extensions.length(); j++)
					{
						(*dev_attr_config)[i].alarms.extensions[j] = attr_config_list_3[i].att_alarm.extensions[j];
					}
					
					(*dev_attr_config)[i].events.ch_event.rel_change = attr_config_list_3[i].event_prop.ch_event.rel_change;
					(*dev_attr_config)[i].events.ch_event.abs_change = attr_config_list_3[i].event_prop.ch_event.abs_change;
					(*dev_attr_config)[i].events.ch_event.extensions.resize(attr_config_list_3[i].event_prop.ch_event.extensions.length());
					for (j=0; j<attr_config_list_3[i].event_prop.ch_event.extensions.length(); j++)
					{
						(*dev_attr_config)[i].events.ch_event.extensions[j] = attr_config_list_3[i].event_prop.ch_event.extensions[j];
					}
					
					(*dev_attr_config)[i].events.per_event.period = attr_config_list_3[i].event_prop.per_event.period;
					(*dev_attr_config)[i].events.per_event.extensions.resize(attr_config_list_3[i].event_prop.per_event.extensions.length());
					for (j=0; j<attr_config_list_3[i].event_prop.per_event.extensions.length(); j++)
					{
						(*dev_attr_config)[i].events.per_event.extensions[j] = attr_config_list_3[i].event_prop.per_event.extensions[j];
					}
					
					(*dev_attr_config)[i].events.arch_event.archive_rel_change = attr_config_list_3[i].event_prop.arch_event.rel_change;
					(*dev_attr_config)[i].events.arch_event.archive_abs_change = attr_config_list_3[i].event_prop.arch_event.abs_change;
					(*dev_attr_config)[i].events.arch_event.archive_period = attr_config_list_3[i].event_prop.arch_event.period;
					(*dev_attr_config)[i].events.arch_event.extensions.resize(attr_config_list_3[i].event_prop.arch_event.extensions.length());
					for (j=0; j<attr_config_list_3[i].event_prop.arch_event.extensions.length(); j++)
					{
						(*dev_attr_config)[i].events.arch_event.extensions[j] = attr_config_list_3[i].event_prop.arch_event.extensions[j];
					}
				}
			}
			
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","get_attribute_config");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","get_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute get_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::get_attribute_config()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","get_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute get_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::get_attribute_config()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute get_attribute_config on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						   (const char*)"API_CommunicationFailed",
                       				   desc.str(),
						   (const char*)"DeviceProxy::get_attribute_config()");
		}
		catch (Tango::DevFailed)
		{
			delete dev_attr_config;
			throw;
		}
	}
		
	return(dev_attr_config);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_remaining_param() 
//
// For device implementing device_2, get attribute config param from db
// instead of getting them from device. The wanted parameters are the 
// warning alarm parameters, the RDS parameters and the event param.
// This method is called only for device_2 device
//
// In : dev_attr_config : ptr to attribute config vector returned
//			  to caller
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_remaining_param(AttributeInfoListEx *dev_attr_config) 
{

//
// Give a default value to all param.
//

	for (unsigned int loop = 0;loop < dev_attr_config->size();loop++)
	{
		(*dev_attr_config)[loop].alarms.min_alarm = (*dev_attr_config)[loop].min_alarm;
		(*dev_attr_config)[loop].alarms.max_alarm = (*dev_attr_config)[loop].max_alarm;
		(*dev_attr_config)[loop].alarms.min_warning = AlrmValueNotSpec;
		(*dev_attr_config)[loop].alarms.max_warning = AlrmValueNotSpec;
		(*dev_attr_config)[loop].alarms.delta_t = AlrmValueNotSpec;
		(*dev_attr_config)[loop].alarms.delta_val = AlrmValueNotSpec;
		(*dev_attr_config)[loop].events.ch_event.abs_change = AlrmValueNotSpec;
		(*dev_attr_config)[loop].events.ch_event.rel_change = AlrmValueNotSpec;
		(*dev_attr_config)[loop].events.per_event.period = AlrmValueNotSpec;
		(*dev_attr_config)[loop].events.arch_event.archive_abs_change = AlrmValueNotSpec;
		(*dev_attr_config)[loop].events.arch_event.archive_rel_change = AlrmValueNotSpec;
		(*dev_attr_config)[loop].events.arch_event.archive_period = AlrmValueNotSpec;
	}
	
//
// If device does not use db, simply retruns
//

	if (dbase_used == false)
	{
		return;
	}
	else
	{

//
// First get device class (if not already done)
//

		if (_info.dev_class.empty() == true)
		{
			this->info();
		}

//
// Get class attribute properties
//

		DbData db_data_class, db_data_device;
		unsigned int i,k;
		int j;
		for (i = 0;i < dev_attr_config->size();i++)
		{
			db_data_class.push_back(DbDatum((*dev_attr_config)[i].name));
			db_data_device.push_back(DbDatum((*dev_attr_config)[i].name));
		}
		db_dev->get_dbase()->get_class_attribute_property(_info.dev_class,db_data_class);

//
// Now get device attribute properties
//

		db_dev->get_attribute_property(db_data_device);
		
//
// Init remaining parameters from them retrieve at class level
//

		for (i = 0;i < db_data_class.size();i++)
		{
			long nb_prop;
			
			string &att_name = db_data_class[i].name;
			db_data_class[i] >> nb_prop;
			i++;
			
			for (j = 0;j < nb_prop;j++)
			{
			
//
// Extract prop value
//

				string prop_value;				
				string &prop_name = db_data_class[i].name;
				if (db_data_class[i].size() != 1)
				{
					vector<string> tmp;
					db_data_class[i] >> tmp;
					prop_value = tmp[0] + ", " + tmp[1];
				}
				else
					db_data_class[i] >> prop_value;
				i++;
				
//
// Store prop value in attribute config vector
//

				for (k = 0;k < dev_attr_config->size();k++)
				{
					if ((*dev_attr_config)[k].name == att_name)
					{
						if (prop_name == "min_warning")
							(*dev_attr_config)[k].alarms.min_warning = prop_value;
						else if (prop_name == "max_warning")
							(*dev_attr_config)[k].alarms.max_warning = prop_value;
						else if (prop_name == "delta_t")
							(*dev_attr_config)[k].alarms.delta_t = prop_value;
						else if (prop_name == "delta_val")
							(*dev_attr_config)[k].alarms.delta_val = prop_value;
						else if (prop_name == "abs_change")
							(*dev_attr_config)[k].events.ch_event.abs_change = prop_value;
						else if (prop_name == "rel_change")
							(*dev_attr_config)[k].events.ch_event.rel_change = prop_value;
						else if (prop_name == "period")
							(*dev_attr_config)[k].events.per_event.period = prop_value;
						else if (prop_name == "archive_abs_change")
							(*dev_attr_config)[k].events.arch_event.archive_abs_change = prop_value;
						else if (prop_name == "archive_rel_change")
							(*dev_attr_config)[k].events.arch_event.archive_rel_change = prop_value;
						else if (prop_name == "archive_period")
							(*dev_attr_config)[k].events.arch_event.archive_period = prop_value;
					}
				}					
			}
		}
		
//
// Init remaining parameters from them retrieve at device level
//

		for (i = 0;i < db_data_device.size();i++)
		{
			long nb_prop;
			
			string &att_name = db_data_device[i].name;
			db_data_device[i] >> nb_prop;
			i++;
			
			for (j = 0;j < nb_prop;j++)
			{
			
//
// Extract prop value
//

				string prop_value;				
				string &prop_name = db_data_device[i].name;
				if (db_data_device[i].size() != 1)
				{
					vector<string> tmp;
					db_data_device[i] >> tmp;
					prop_value = tmp[0] + ", " + tmp[1];
				}
				else
					db_data_device[i] >> prop_value;
				i++;
				
//
// Store prop value in attribute config vector
//

				for (k = 0;k < dev_attr_config->size();k++)
				{
					if ((*dev_attr_config)[k].name == att_name)
					{
						if (prop_name == "min_warning")
							(*dev_attr_config)[k].alarms.min_warning = prop_value;
						else if (prop_name == "max_warning")
							(*dev_attr_config)[k].alarms.max_warning = prop_value;
						else if (prop_name == "delta_t")
							(*dev_attr_config)[k].alarms.delta_t = prop_value;
						else if (prop_name == "delta_val")
							(*dev_attr_config)[k].alarms.delta_val = prop_value;
						else if (prop_name == "abs_change")
							(*dev_attr_config)[k].events.ch_event.abs_change = prop_value;
						else if (prop_name == "rel_change")
							(*dev_attr_config)[k].events.ch_event.rel_change = prop_value;
						else if (prop_name == "period")
							(*dev_attr_config)[k].events.per_event.period = prop_value;
						else if (prop_name == "archive_abs_change")
							(*dev_attr_config)[k].events.arch_event.archive_abs_change = prop_value;
						else if (prop_name == "archive_rel_change")
							(*dev_attr_config)[k].events.arch_event.archive_rel_change = prop_value;
						else if (prop_name == "archive_period")
							(*dev_attr_config)[k].events.arch_event.archive_period = prop_value;
					}
				}					
			}
		}	
	
	}
	
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_config() - return a single attribute config
//
//-----------------------------------------------------------------------------
 
AttributeInfoEx DeviceProxy::get_attribute_config(const string& attr_string) 
{
	vector<string> attr_string_list;
	AttributeInfoListEx *dev_attr_config_list;
	AttributeInfoEx dev_attr_config;

	attr_string_list.push_back(attr_string);
	dev_attr_config_list = get_attribute_config_ex(attr_string_list);

	dev_attr_config = (*dev_attr_config_list)[0];
	delete(dev_attr_config_list);
	
	return(dev_attr_config);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::set_attribute_config() - set config for a list of attributes
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::set_attribute_config(AttributeInfoList &dev_attr_list) 
{
	AttributeConfigList attr_config_list;
	DevVarStringArray attr_list;
	int ctr = 0;

	attr_config_list.length(dev_attr_list.size());

	for (unsigned int i=0; i<attr_config_list.length(); i++)
	{
		attr_config_list[i].name = dev_attr_list[i].name.c_str();
		attr_config_list[i].writable = dev_attr_list[i].writable;
		attr_config_list[i].data_format = dev_attr_list[i].data_format;
		attr_config_list[i].data_type = dev_attr_list[i].data_type;
		attr_config_list[i].max_dim_x = dev_attr_list[i].max_dim_x;
		attr_config_list[i].max_dim_y = dev_attr_list[i].max_dim_y;
		attr_config_list[i].description = dev_attr_list[i].description.c_str();
		attr_config_list[i].label = dev_attr_list[i].label.c_str();
		attr_config_list[i].unit = dev_attr_list[i].unit.c_str();
		attr_config_list[i].standard_unit = dev_attr_list[i].standard_unit.c_str();
		attr_config_list[i].display_unit = dev_attr_list[i].display_unit.c_str();
		attr_config_list[i].format = dev_attr_list[i].format.c_str();
		attr_config_list[i].min_value = dev_attr_list[i].min_value.c_str();
		attr_config_list[i].max_value = dev_attr_list[i].max_value.c_str();
		attr_config_list[i].min_alarm = dev_attr_list[i].min_alarm.c_str();
		attr_config_list[i].max_alarm = dev_attr_list[i].max_alarm.c_str();
		attr_config_list[i].writable_attr_name = dev_attr_list[i].writable_attr_name.c_str();
		attr_config_list[i].extensions.length(dev_attr_list[i].extensions.size());
		for (unsigned int j=0; j<dev_attr_list[i].extensions.size(); j++)
		{
			attr_config_list[i].extensions[j] = string_dup(dev_attr_list[i].extensions[j].c_str());
		}
	}
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			device->set_attribute_config(attr_config_list);
			ctr = 2;

		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
			{
				TangoSys_OMemStream desc;
				desc << "Failed to execute set_attribute_config on device " << device_name << ends;

				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::set_attribute_config()");
			}
			else
				throw;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","set_attribute_config");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","set_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute set_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::set_attribute_config()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","set_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute set_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::set_attribute_config()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute set_attribute_config on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::set_attribute_config()");
		}
	}


	return;
}

 
void DeviceProxy::set_attribute_config(AttributeInfoListEx &dev_attr_list) 
{
	AttributeConfigList attr_config_list;
	AttributeConfigList_3 attr_config_list_3;
	DevVarStringArray attr_list;
	int ctr = 0;
	unsigned int i,j;

	if (version >= 3)
	{
		attr_config_list_3.length(dev_attr_list.size());

		for (i=0; i<attr_config_list_3.length(); i++)
		{
			attr_config_list_3[i].name = dev_attr_list[i].name.c_str();
			attr_config_list_3[i].writable = dev_attr_list[i].writable;
			attr_config_list_3[i].data_format = dev_attr_list[i].data_format;
			attr_config_list_3[i].data_type = dev_attr_list[i].data_type;
			attr_config_list_3[i].max_dim_x = dev_attr_list[i].max_dim_x;
			attr_config_list_3[i].max_dim_y = dev_attr_list[i].max_dim_y;
			attr_config_list_3[i].description = dev_attr_list[i].description.c_str();
			attr_config_list_3[i].label = dev_attr_list[i].label.c_str();
			attr_config_list_3[i].unit = dev_attr_list[i].unit.c_str();
			attr_config_list_3[i].standard_unit = dev_attr_list[i].standard_unit.c_str();
			attr_config_list_3[i].display_unit = dev_attr_list[i].display_unit.c_str();
			attr_config_list_3[i].format = dev_attr_list[i].format.c_str();
			attr_config_list_3[i].min_value = dev_attr_list[i].min_value.c_str();
			attr_config_list_3[i].max_value = dev_attr_list[i].max_value.c_str();
			attr_config_list_3[i].writable_attr_name = dev_attr_list[i].writable_attr_name.c_str();
			attr_config_list_3[i].level = dev_attr_list[i].disp_level;
			attr_config_list_3[i].extensions.length(dev_attr_list[i].extensions.size());
			for (j=0; j<dev_attr_list[i].extensions.size(); j++)
			{
				attr_config_list_3[i].extensions[j] = string_dup(dev_attr_list[i].extensions[j].c_str());
			}
			for (j=0; j<dev_attr_list[i].sys_extensions.size(); j++)
			{
				attr_config_list_3[i].sys_extensions[j] = string_dup(dev_attr_list[i].sys_extensions[j].c_str());
			}
						
			attr_config_list_3[i].att_alarm.min_alarm = dev_attr_list[i].alarms.min_alarm.c_str();
			attr_config_list_3[i].att_alarm.max_alarm = dev_attr_list[i].alarms.max_alarm.c_str();
			attr_config_list_3[i].att_alarm.min_warning = dev_attr_list[i].alarms.min_warning.c_str();
			attr_config_list_3[i].att_alarm.max_warning = dev_attr_list[i].alarms.max_warning.c_str();
			attr_config_list_3[i].att_alarm.delta_t = dev_attr_list[i].alarms.delta_t.c_str();
			attr_config_list_3[i].att_alarm.delta_val = dev_attr_list[i].alarms.delta_val.c_str();
			for (j=0; j<dev_attr_list[i].alarms.extensions.size(); j++)
			{
				attr_config_list_3[i].att_alarm.extensions[j] = string_dup(dev_attr_list[i].alarms.extensions[j].c_str());
			}
			
			attr_config_list_3[i].event_prop.ch_event.rel_change = dev_attr_list[i].events.ch_event.rel_change.c_str();
			attr_config_list_3[i].event_prop.ch_event.abs_change = dev_attr_list[i].events.ch_event.abs_change.c_str();
			for (j=0; j<dev_attr_list[i].events.ch_event.extensions.size(); j++)
			{
				attr_config_list_3[i].event_prop.ch_event.extensions[j] = string_dup(dev_attr_list[i].events.ch_event.extensions[j].c_str());
			}
			
			attr_config_list_3[i].event_prop.per_event.period = dev_attr_list[i].events.per_event.period.c_str();
			for (j=0; j<dev_attr_list[i].events.per_event.extensions.size(); j++)
			{
				attr_config_list_3[i].event_prop.per_event.extensions[j] = string_dup(dev_attr_list[i].events.per_event.extensions[j].c_str());
			}
			
			attr_config_list_3[i].event_prop.arch_event.rel_change = dev_attr_list[i].events.arch_event.archive_rel_change.c_str();
			attr_config_list_3[i].event_prop.arch_event.abs_change = dev_attr_list[i].events.arch_event.archive_abs_change.c_str();
			attr_config_list_3[i].event_prop.arch_event.period = dev_attr_list[i].events.arch_event.archive_period.c_str();
			for (j=0; j<dev_attr_list[i].events.ch_event.extensions.size(); j++)
			{
				attr_config_list_3[i].event_prop.arch_event.extensions[j] = string_dup(dev_attr_list[i].events.arch_event.extensions[j].c_str());
			}						
		}
	}
	else
	{
		attr_config_list.length(dev_attr_list.size());

		for (i=0; i<attr_config_list.length(); i++)
		{
			attr_config_list[i].name = dev_attr_list[i].name.c_str();
			attr_config_list[i].writable = dev_attr_list[i].writable;
			attr_config_list[i].data_format = dev_attr_list[i].data_format;
			attr_config_list[i].data_type = dev_attr_list[i].data_type;
			attr_config_list[i].max_dim_x = dev_attr_list[i].max_dim_x;
			attr_config_list[i].max_dim_y = dev_attr_list[i].max_dim_y;
			attr_config_list[i].description = dev_attr_list[i].description.c_str();
			attr_config_list[i].label = dev_attr_list[i].label.c_str();
			attr_config_list[i].unit = dev_attr_list[i].unit.c_str();
			attr_config_list[i].standard_unit = dev_attr_list[i].standard_unit.c_str();
			attr_config_list[i].display_unit = dev_attr_list[i].display_unit.c_str();
			attr_config_list[i].format = dev_attr_list[i].format.c_str();
			attr_config_list[i].min_value = dev_attr_list[i].min_value.c_str();
			attr_config_list[i].max_value = dev_attr_list[i].max_value.c_str();
			attr_config_list[i].min_alarm = dev_attr_list[i].min_alarm.c_str();
			attr_config_list[i].max_alarm = dev_attr_list[i].max_alarm.c_str();
			attr_config_list[i].writable_attr_name = dev_attr_list[i].writable_attr_name.c_str();
			attr_config_list[i].extensions.length(dev_attr_list[i].extensions.size());
			for (j=0; j<dev_attr_list[i].extensions.size(); j++)
			{
				attr_config_list_3[i].extensions[j] = string_dup(dev_attr_list[i].extensions[j].c_str());
			}
		}
	}
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());
				
				ext->device_4->set_attribute_config_4(attr_config_list_3,ci);
			}
			else if (version == 3)
				ext->device_3->set_attribute_config_3(attr_config_list_3);
			else
				device->set_attribute_config(attr_config_list);
			ctr = 2;

		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
			{
				TangoSys_OMemStream desc;
				desc << "Failed to execute set_attribute_config on device " << device_name << ends;

				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::set_attribute_config()");
			}
			else
				throw;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","set_attribute_config");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","set_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute set_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::set_attribute_config()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","set_attribute_config");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute set_attribute_config on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::set_attribute_config()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute set_attribute_config on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::set_attribute_config()");
		}
	}

	return;
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::read_attributes() - return a list of attributes
//
//-----------------------------------------------------------------------------
 
vector<DeviceAttribute> *DeviceProxy::read_attributes(vector<string>& attr_string_list) 
{
	AttributeValueList_var attr_value_list;
	AttributeValueList_3_var attr_value_list_3;
	AttributeValueList_4_var attr_value_list_4;
	vector<DeviceAttribute> *dev_attr = new(vector<DeviceAttribute>);
	DevVarStringArray attr_list;

	unsigned int i;	
	attr_list.length(attr_string_list.size());
	for (i=0; i<attr_string_list.size(); i++)
	{
		attr_list[i] = string_dup(attr_string_list[i].c_str());
	}

	int ctr = 0;
		
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());

				attr_value_list_4 = ext->device_4->read_attributes_4(attr_list,source,ci);
			}
			else if (version == 3)
				attr_value_list_3 = ext->device_3->read_attributes_3(attr_list,source);		
			else if (version == 2)
				attr_value_list = device_2->read_attributes_2(attr_list,source);
			else
				attr_value_list = device->read_attributes(attr_list);

			ctr = 2;
		}
		catch (Tango::ConnectionFailed &e)
		{
			delete dev_attr;
			
			TangoSys_OMemStream desc;
			desc << "Failed to read_attributes on device " << device_name;
			desc << ", attributes ";
			int nb_attr = attr_string_list.size();
			for (int i = 0;i < nb_attr;i++)
			{
				desc << attr_string_list[i];
				if (i != nb_attr - 1)
					desc << ", ";
			}
			desc << ends;
            ApiConnExcept::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::read_attributes()");
		}
		catch (Tango::DevFailed &e)
		{
			delete dev_attr;
			
			TangoSys_OMemStream desc;
			desc << "Failed to read_attributes on device " << device_name;
			desc << ", attributes ";
			int nb_attr = attr_string_list.size();
			for (int i = 0;i < nb_attr;i++)
			{
				desc << attr_string_list[i];
				if (i != nb_attr - 1)
					desc << ", ";
			}
			desc << ends;
        	Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::read_attributes()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			if ((trans.minor() == omni::TRANSIENT_CallTimedout) || (ctr == 1))
				delete dev_attr;
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","read_attributes");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				if (ctr == 1)
					delete dev_attr;
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","read_attributes");
			}
			else
			{
				delete dev_attr;
				
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute read_attributes on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::read_attributes()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				if (ctr == 1)
					delete dev_attr;
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","read_attributes");
			}
			else
			{
				delete dev_attr;
				
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute read_attributes on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::read_attributes()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			delete dev_attr;
	
			connection_state = CONNECTION_NOTOK;
			TangoSys_OMemStream desc;
			desc << "Failed to execute read_attributes on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::read_attributes()");
		}
	}

	unsigned long nb_received;	
	if (version < 3)
		nb_received = attr_value_list->length();
	else if (version == 3)
		nb_received = attr_value_list_3->length();
	else
		nb_received = attr_value_list_4->length();
	dev_attr->resize(nb_received);

	for (i=0; i < nb_received; i++)
	{
		if (version >= 3)
		{
			if (version == 3)
				ApiUtil::attr_to_device(NULL,&(attr_value_list_3[i]),version,&(*dev_attr)[i]);
			else
				ApiUtil::attr_to_device(&(attr_value_list_4[i]),version,&(*dev_attr)[i]);
							
//
// Add an error in the error stack in case there is one
//
		
			long nb_except = (*dev_attr)[i].ext->err_list.in().length();
			if (nb_except != 0)
			{
				TangoSys_OMemStream desc;
				desc << "Failed to read_attributes on device " << device_name;
				desc << ", attribute " << (*dev_attr)[i].name << ends;
		
				(*dev_attr)[i].ext->err_list.inout().length(nb_except + 1);
				(*dev_attr)[i].ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
				(*dev_attr)[i].ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::read_attributes()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				(*dev_attr)[i].ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
				char *tmp_str = desc.str();
				(*dev_attr)[i].ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
				delete[] tmp_str;
#endif
				(*dev_attr)[i].ext->err_list[nb_except].severity = Tango::ERR;
			} 
		}
		else
		{
			ApiUtil::attr_to_device(&(attr_value_list[i]),NULL,version,&(*dev_attr)[i]);
		}							
	}

	return(dev_attr);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::read_attribute() - return a single attribute 
//
//-----------------------------------------------------------------------------
 

DeviceAttribute DeviceProxy::read_attribute(string& attr_string) 
{
	AttributeValueList_var attr_value_list;
	AttributeValueList_3_var attr_value_list_3;
	AttributeValueList_4_var attr_value_list_4;
	DeviceAttribute dev_attr;
	DevVarStringArray attr_list;
	int ctr = 0;

	attr_list.length(1);
	attr_list[0] = CORBA::string_dup(attr_string.c_str());

	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());
				attr_value_list_4 = ext->device_4->read_attributes_4(attr_list,source,ci);
			}
			else if (version == 3)
				attr_value_list_3 = ext->device_3->read_attributes_3(attr_list,source);
			else if (version == 2)
				attr_value_list = device_2->read_attributes_2(attr_list,source);
			else
				attr_value_list = device->read_attributes(attr_list);
			ctr = 2;
		}
		catch (Tango::ConnectionFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to read_attribute on device " << device_name;
			desc << ", attribute " << attr_string << ends;
			ApiConnExcept::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::read_attribute()");
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to read_attribute on device " << device_name;
			desc << ", attribute " << attr_string << ends;
			Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::read_attribute()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","read_attribute");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","read_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to read_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::read_attribute()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","read_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to read_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::read_attribute()");
			}
		} 
		catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;
			TangoSys_OMemStream desc;
			desc << "Failed to read_attribute on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::read_attribute()");
		}
	}

	if (version >= 3)
	{
		if (version == 3)
			ApiUtil::attr_to_device(NULL,&(attr_value_list_3[0]),version,&dev_attr);
		else
			ApiUtil::attr_to_device(&(attr_value_list_4[0]),version,&dev_attr);
				
//
// Add an error in the error stack in case there is one
//
		
		long nb_except = dev_attr.ext->err_list.in().length();
		if (nb_except != 0)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to read_attribute on device " << device_name;
			desc << ", attribute " << dev_attr.name << ends;
		
			dev_attr.ext->err_list.inout().length(nb_except + 1);
			dev_attr.ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
			dev_attr.ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::read_attribute()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			dev_attr.ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
			char *tmp_str = desc.str();
			dev_attr.ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
			delete[] tmp_str;
#endif
			dev_attr.ext->err_list[nb_except].severity = Tango::ERR;
		} 
	}
	else
	{
		ApiUtil::attr_to_device(&(attr_value_list[0]),NULL,version,&dev_attr);
	}

	return(dev_attr);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::write_attributes() - write a list of attributes
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::write_attributes(vector<DeviceAttribute>& attr_list) 
{	
	AttributeValueList attr_value_list;
	AttributeValueList_4 attr_value_list_4;
	
	if (version >= 4)
		attr_value_list_4.length(attr_list.size());
	else
		attr_value_list.length(attr_list.size());
	
	for (unsigned int i=0; i<attr_list.size(); i++)
	{
		if (version >= 4)
		{
			attr_value_list_4[i].name = attr_list[i].name.c_str();
			attr_value_list_4[i].quality = attr_list[i].quality;
			attr_value_list_4[i].data_format = attr_list[i].data_format;
			attr_value_list_4[i].time = attr_list[i].time;
			attr_value_list_4[i].w_dim.dim_x = attr_list[i].dim_x;
			attr_value_list_4[i].w_dim.dim_y = attr_list[i].dim_y;			
		}
		else
		{
			attr_value_list[i].name = attr_list[i].name.c_str();
			attr_value_list[i].quality = attr_list[i].quality;
			attr_value_list[i].time = attr_list[i].time;
			attr_value_list[i].dim_x = attr_list[i].dim_x;
			attr_value_list[i].dim_y = attr_list[i].dim_y;
		}
		
		if (attr_list[i].LongSeq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.long_att_value(attr_list[i].LongSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].LongSeq.in();	
			continue;
		}
		if (attr_list[i].ext->Long64Seq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.long64_att_value(attr_list[i].ext->Long64Seq.in());	
			else
				attr_value_list[i].value <<= attr_list[i].ext->Long64Seq.in();	
			continue;
		}
		if (attr_list[i].ShortSeq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.short_att_value(attr_list[i].ShortSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].ShortSeq.in();
			continue;
		}	
		if (attr_list[i].DoubleSeq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.double_att_value(attr_list[i].DoubleSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].DoubleSeq.in();
			continue;
		}
		if (attr_list[i].StringSeq.operator->() != NULL)
		{	
			if (version >= 4)
				attr_value_list_4[i].value.string_att_value(attr_list[i].StringSeq.in());
			else
				attr_value_list[i].value  <<= attr_list[i].StringSeq.in();
			continue;
		}
		if (attr_list[i].FloatSeq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.float_att_value(attr_list[i].FloatSeq.in());	
			else
				attr_value_list[i].value <<= attr_list[i].FloatSeq.in();	
			continue;
		}
		if (attr_list[i].BooleanSeq.operator->() != NULL)
		{	
			if (version >= 4)
				attr_value_list_4[i].value.bool_att_value(attr_list[i].BooleanSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].BooleanSeq.in();
			continue;
		}	
		if (attr_list[i].UShortSeq.operator->() != NULL)
		{	
			if (version >= 4)
				attr_value_list_4[i].value.ushort_att_value(attr_list[i].UShortSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].UShortSeq.in();
			continue;
		}
		if (attr_list[i].UCharSeq.operator->() != NULL)
		{	
			if (version >= 4)
				attr_value_list_4[i].value.uchar_att_value(attr_list[i].UCharSeq.in());
			else
				attr_value_list[i].value  <<= attr_list[i].UCharSeq.in();
			continue;
		}
		if (attr_list[i].ext->ULongSeq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.ulong_att_value(attr_list[i].ext->ULongSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].ext->ULongSeq.in();	
			continue;
		}
		if (attr_list[i].ext->ULong64Seq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.ulong64_att_value(attr_list[i].ext->ULong64Seq.in());	
			else
				attr_value_list[i].value <<= attr_list[i].ext->ULong64Seq.in();	
			continue;
		}
		if (attr_list[i].ext->StateSeq.operator->() != NULL)
		{
			if (version >= 4)
				attr_value_list_4[i].value.state_att_value(attr_list[i].ext->StateSeq.in());
			else
				attr_value_list[i].value <<= attr_list[i].ext->StateSeq.in();	
			continue;
		}
	}

	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

//
// Throw exception if caller not allowed to write_attribute
//
			 
			if (access == ACCESS_READ)
			{
				try
				{
					device->ping();
				}
				catch(...) {connection_state = CONNECTION_NOTOK;throw;}
				
				TangoSys_OMemStream desc;
				desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
			
				NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
											  	  (const char *)"DeviceProxy::write_attributes()");
			}
				
//
// Now, write the attribute(s)
//
			
			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());
				
				ext->device_4->write_attributes_4(attr_value_list_4,ci);
			}
			else if (version == 3)
				ext->device_3->write_attributes_3(attr_value_list);
			else	
				device->write_attributes(attr_value_list);
			ctr = 2;
		}
		catch (Tango::MultiDevFailed &e)
		{
			throw Tango::NamedDevFailedList(e,
						       device_name,
						       (const char *)"DeviceProxy::write_attributes",
						       (const char *)"API_AttributeFailed");
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to write_attributes on device " << device_name;
			desc << ", attributes ";
			int nb_attr = attr_value_list.length();
			for (int i = 0;i < nb_attr;i++)
			{
				desc << attr_value_list[i].name.in();
				if (i != nb_attr - 1)
					desc << ", ";
			}
			desc << ends;

			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::write_attribute()");
			else
                Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","write_attributes");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","write_attributes");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attributes()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","write_attributes");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attributes()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::write_attributes()");
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::write_attribute() - write a single attribute 
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::write_attribute(DeviceAttribute &dev_attr) 
{		
	AttributeValueList attr_value_list;
	AttributeValueList_4 attr_value_list_4;
		
	if (version >= 4)
	{
		attr_value_list_4.length(1);

		attr_value_list_4[0].name = dev_attr.name.c_str();
		attr_value_list_4[0].quality = dev_attr.quality;
		attr_value_list_4[0].data_format = dev_attr.data_format;
		attr_value_list_4[0].time = dev_attr.time;
		attr_value_list_4[0].w_dim.dim_x = dev_attr.dim_x;
		attr_value_list_4[0].w_dim.dim_y = dev_attr.dim_y;
		
		if (dev_attr.LongSeq.operator->() != NULL)
			attr_value_list_4[0].value.long_att_value(dev_attr.LongSeq.in());
		else if (dev_attr.ext->Long64Seq.operator->() != NULL)
			attr_value_list_4[0].value.long64_att_value(dev_attr.ext->Long64Seq.in());
		else if (dev_attr.ShortSeq.operator->() != NULL)
			attr_value_list_4[0].value.short_att_value(dev_attr.ShortSeq.in());
		else if (dev_attr.DoubleSeq.operator->() != NULL)
			attr_value_list_4[0].value.double_att_value(dev_attr.DoubleSeq.in());
		else if (dev_attr.StringSeq.operator->() != NULL)
			attr_value_list_4[0].value.string_att_value(dev_attr.StringSeq.in());
		else if (dev_attr.FloatSeq.operator->() != NULL)
			attr_value_list_4[0].value.float_att_value(dev_attr.FloatSeq.in());
		else if (dev_attr.BooleanSeq.operator->() != NULL)
			attr_value_list_4[0].value.bool_att_value(dev_attr.BooleanSeq.in());
		else if (dev_attr.UShortSeq.operator->() != NULL)
			attr_value_list_4[0].value.ushort_att_value(dev_attr.UShortSeq.in());
		else if (dev_attr.UCharSeq.operator->() != NULL)
			attr_value_list_4[0].value.uchar_att_value(dev_attr.UCharSeq.in());
		else if (dev_attr.ext->ULongSeq.operator->() != NULL)
			attr_value_list_4[0].value.ulong_att_value(dev_attr.ext->ULongSeq.in());
		else if (dev_attr.ext->ULong64Seq.operator->() != NULL)
			attr_value_list_4[0].value.ulong64_att_value(dev_attr.ext->ULong64Seq.in());
		else if (dev_attr.ext->StateSeq.operator->() != NULL)
			attr_value_list_4[0].value.state_att_value(dev_attr.ext->StateSeq.in());
		else if (dev_attr.ext->EncodedSeq.operator->() != NULL)
			attr_value_list_4[0].value.encoded_att_value(dev_attr.ext->EncodedSeq.in());
	}
	else
	{
		attr_value_list.length(1);

		attr_value_list[0].name = dev_attr.name.c_str();
		attr_value_list[0].quality = dev_attr.quality;
		attr_value_list[0].time = dev_attr.time;
		attr_value_list[0].dim_x = dev_attr.dim_x;
		attr_value_list[0].dim_y = dev_attr.dim_y;
		
		if (dev_attr.LongSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.LongSeq.in();
		else if (dev_attr.ext->Long64Seq.operator->() != NULL)
			 attr_value_list[0].value <<= dev_attr.ext->Long64Seq.in();
		else if (dev_attr.ShortSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.ShortSeq.in();
		else if (dev_attr.DoubleSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.DoubleSeq.in();
		else if (dev_attr.StringSeq.operator->() != NULL)
			attr_value_list[0].value  <<= dev_attr.StringSeq.in();
		else if (dev_attr.FloatSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.FloatSeq.in();
		else if (dev_attr.BooleanSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.BooleanSeq.in();
		else if (dev_attr.UShortSeq.operator->() != NULL)
			attr_value_list[0].value  <<= dev_attr.UShortSeq.in();
		else if (dev_attr.UCharSeq.operator->() != NULL)
			attr_value_list[0].value  <<= dev_attr.UCharSeq.in();
		else if (dev_attr.ext->ULongSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.ext->ULongSeq.in();
		else if (dev_attr.ext->ULong64Seq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.ext->ULong64Seq.in();
		else if (dev_attr.ext->StateSeq.operator->() != NULL)
			attr_value_list[0].value <<= dev_attr.ext->StateSeq.in();
	}

	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);
			
//
// Throw exception if caller not allowed to write_attribute
//
			 
			if (access == ACCESS_READ)
			{
				try
				{
					device->ping();
				}
				catch(...) {connection_state = CONNECTION_NOTOK;throw;}
				
				TangoSys_OMemStream desc;
				desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
			
				NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
											  	  (const char *)"DeviceProxy::write_attribute()");
			}

//
// Now, write the attribute(s)
//
			
			if (version >= 4)
			{
				ClntIdent ci;
				ApiUtil *au = ApiUtil::instance();
				ci.cpp_clnt(au->get_client_pid());
				
				ext->device_4->write_attributes_4(attr_value_list_4,ci);
			}
			else if (version == 3)
				ext->device_3->write_attributes_3(attr_value_list);
			else	
				device->write_attributes(attr_value_list);
			ctr = 2;

		}
		catch (Tango::MultiDevFailed &e)
		{

//
// Transfer this exception into a DevFailed exception
//

			Tango::DevFailed ex(e.errors[0].err_list);
			TangoSys_OMemStream desc;
			desc << "Failed to write_attribute on device " << device_name;
			desc << ", attribute ";
			desc << dev_attr.name;
			desc << ends;
                	Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");

		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to write_attribute on device " << device_name;
			desc << ", attribute ";
			desc << dev_attr.name;
			desc << ends;

			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::write_attribute()");
			else
            	Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","write_attribute()");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","write_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attribute()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","write_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attribute()");
			}
		}
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::write_attribute()");
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::write_attribute() - write attribute(s) using the CORBA data type 
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::write_attribute(const AttributeValueList &attr_val) 
{
		
	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);
			
//
// Throw exception if caller not allowed to write_attribute
//
 
			if (access == ACCESS_READ)
			{
				try
				{
					device->ping();
				}
				catch(...) {connection_state = CONNECTION_NOTOK;throw;}
				
				TangoSys_OMemStream desc;
				desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
	
				NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
									  	  		(const char *)"DeviceProxy::write_attribute()");
			}

//
// Now, write the attribute(s)
//
			

			if (version >= 3)
				ext->device_3->write_attributes_3(attr_val);
			else	
				device->write_attributes(attr_val);
			ctr = 2;

		}
		catch (Tango::MultiDevFailed &e)
		{

//
// Transfer this exception into a DevFailed exception
//

			Tango::DevFailed ex(e.errors[0].err_list);
			TangoSys_OMemStream desc;
			desc << "Failed to write_attribute on device " << device_name;
			desc << ", attribute ";
			desc << attr_val[0].name.in();
			desc << ends;
                	Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");

		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to write_attribute on device " << device_name;
			desc << ", attribute ";
			desc << attr_val[0].name.in();
			desc << ends;

			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::write_attribute()");
			else
                Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","write_attribute()");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","write_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attribute()");
			}
		}   
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","write_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attribute()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::write_attribute()");
		}
	}

	return;
}


void DeviceProxy::write_attribute(const AttributeValueList_4 &attr_val) 
{

//
// Check that the device supports IDL V4
//
	
	if (version < 4)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to write_attribute on device " << device_name;
		desc << ", attribute ";
		desc << attr_val[0].name.in();
		desc << ". The device does not support thi stype of data (Bad IDL release)";
		desc << ends;
		Tango::Except::throw_exception((const char*)"API_NotSupportedFeature",
                    	desc.str(), (const char*)"DeviceProxy::write_attribute()");
	}
	int ctr = 0;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);
			
//
// Throw exception if caller not allowed to write_attribute
//
 
			if (access == ACCESS_READ)
			{
				try
				{
					device->ping();
				}
				catch(...) {connection_state = CONNECTION_NOTOK;throw;}
				
				TangoSys_OMemStream desc;
				desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
	
				NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
									  	  		(const char *)"DeviceProxy::write_attribute()");
			}

//
// Now, write the attribute(s)
//
			
			ClntIdent ci;
			ApiUtil *au = ApiUtil::instance();
			ci.cpp_clnt(au->get_client_pid());
				
			ext->device_4->write_attributes_4(attr_val,ci);
			ctr = 2;

		}
		catch (Tango::MultiDevFailed &e)
		{

//
// Transfer this exception into a DevFailed exception
//

			Tango::DevFailed ex(e.errors[0].err_list);
			TangoSys_OMemStream desc;
			desc << "Failed to write_attribute on device " << device_name;
			desc << ", attribute ";
			desc << attr_val[0].name.in();
			desc << ends;
                	Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");

		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to write_attribute on device " << device_name;
			desc << ", attribute ";
			desc << attr_val[0].name.in();
			desc << ends;

			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::write_attribute()");
			else
                Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_attribute()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","write_attribute()");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","write_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attribute()");
			}
		}   
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","write_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_attribute()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::write_attribute()");
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_list() - get list of attributes
//
//-----------------------------------------------------------------------------
 
vector<string> *DeviceProxy::get_attribute_list() 
{
	vector<string> all_attr;
	AttributeInfoList * all_attr_config;
	vector<string> *attr_list = new vector<string>;

	all_attr.push_back(AllAttr_3);
	all_attr_config = get_attribute_config(all_attr);

	attr_list->resize(all_attr_config->size());
	for (unsigned int i=0; i<all_attr_config->size(); i++)
	{
		(*attr_list)[i] = (*all_attr_config)[i].name;

	}
	delete all_attr_config;

	return attr_list;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::attribute_list_query() - get list of attributes
//
//-----------------------------------------------------------------------------
 
AttributeInfoList *DeviceProxy::attribute_list_query() 
{
	vector<string> all_attr;
	AttributeInfoList *all_attr_config;

	all_attr.push_back(AllAttr_3);
	all_attr_config = get_attribute_config(all_attr);

	return all_attr_config;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::attribute_list_query_ex() - get list of attributes
//
//-----------------------------------------------------------------------------
 
AttributeInfoListEx *DeviceProxy::attribute_list_query_ex() 
{
	vector<string> all_attr;
	AttributeInfoListEx *all_attr_config;

	all_attr.push_back(AllAttr_3);
	all_attr_config = get_attribute_config_ex(all_attr);

	return all_attr_config;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::command_history() - get command history (only for polled command)
//
//-----------------------------------------------------------------------------
 
vector<DeviceDataHistory> *DeviceProxy::command_history(string &cmd_name,int depth) 
{
	if (version == 1)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << device_name;
		desc << " does not support command_history feature" << ends;
		ApiNonSuppExcept::throw_exception((const char *)"API_UnsupportedFeature",
						  desc.str(),
						  (const char *)"DeviceProxy::command_history");
	}

	DevCmdHistoryList *hist = NULL;
	DevCmdHistory_4_var hist_4 = NULL;

	int ctr = 0;
	
	while (ctr < 2)
	{		
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version <= 3)
				hist = device_2->command_inout_history_2(cmd_name.c_str(),depth);
			else
				hist_4 = ext->device_4->command_inout_history_4(cmd_name.c_str(),depth);
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","command_history");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","command_history");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Command_history failed on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::command_history()");
			}
		} 
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","command_history");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Command_history failed on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::command_history()");
			}
		} 
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Command_history failed on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::command_history()");
		}
	}

		
	vector<DeviceDataHistory> *ddh = new vector<DeviceDataHistory>;
	
	if (version <= 3)
	{
		int *ctr_ptr = new int;
		*ctr_ptr = 0;
		
		ddh->reserve(hist->length());
		
		for (unsigned int i = 0;i < hist->length();i++)
		{
			ddh->push_back(DeviceDataHistory(i,ctr_ptr,hist));
		}
	}
	else
	{
		ddh->reserve(hist_4->dates.length());
		for (unsigned int i = 0;i < hist_4->dates.length();i++)
			ddh->push_back(DeviceDataHistory());
		from_hist4_2_DataHistory(hist_4,ddh);
	}

	return ddh;

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::attribute_history() - get attribute history
//				      (only for polled attribute)
//
//-----------------------------------------------------------------------------
 
vector<DeviceAttributeHistory> *DeviceProxy::attribute_history(string &cmd_name,int depth) 
{
	if (version == 1)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << device_name;
		desc << " does not support attribute_history feature" << ends;
		ApiNonSuppExcept::throw_exception((const char *)"API_UnsupportedFeature",
						  desc.str(),
						  (const char *)"DeviceProxy::attribute_history");
	}

	DevAttrHistoryList_var hist;
	DevAttrHistoryList_3_var hist_3;
	DevAttrHistory_4_var hist_4;
		
	int ctr = 0;

	while (ctr < 2)
	{		
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);

			if (version == 2)
				hist = device_2->read_attribute_history_2(cmd_name.c_str(),depth);
			else
			{
				if (version == 3)
					hist_3 = ext->device_3->read_attribute_history_3(cmd_name.c_str(),depth);
				else
					hist_4 = ext->device_4->read_attribute_history_4(cmd_name.c_str(),depth);
			}
			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","attribute_history");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","attribute_history");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Attribute_history failed on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::attribute_history()");
			}
		}  
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","attribute_history");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Attribute_history failed on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::attribute_history()");
			}
		}  
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;
			TangoSys_OMemStream desc;
			desc << "Attribute_history failed on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::attribute_history()");
		}
	}
		
	vector<DeviceAttributeHistory> *ddh = new vector<DeviceAttributeHistory>;

	if (version == 2)
	{	
		ddh->reserve(hist->length());
		for (unsigned int i = 0;i < hist->length();i++)
		{
			ddh->push_back(DeviceAttributeHistory(i,hist));
		}
	}
	else if (version == 3)
	{
		ddh->reserve(hist_3->length());
		for (unsigned int i = 0;i < hist_3->length();i++)
		{
			ddh->push_back(DeviceAttributeHistory(i,hist_3));
		}
	}
	else
	{
		ddh->reserve(hist_4->dates.length());
		for (unsigned int i = 0;i < hist_4->dates.length();i++)
			ddh->push_back(DeviceAttributeHistory());
		from_hist4_2_AttHistory(hist_4,ddh);
	}
	
	return ddh;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::connect_to_adm_device() - get device polling status
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::connect_to_adm_device() 
{
	adm_dev_name = adm_name();
	
	adm_device = new DeviceProxy(adm_dev_name);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::polling_status() - get device polling status
//
//-----------------------------------------------------------------------------
 
vector<string> *DeviceProxy::polling_status() 
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DeviceData dout,din;
	string cmd("DevPollStatus");
	din.any <<= device_name.c_str();
	
//
// In case of connection failed error, do a re-try
//

	try
	{
		dout = adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		dout = adm_device->command_inout(cmd,din);
	}

	const DevVarStringArray *out_str;
	dout >> out_str;

	vector<string> *poll_stat = new vector<string>;
	poll_stat->reserve(out_str->length());
	
	for (unsigned int i = 0;i < out_str->length();i++)
	{
		string str = (*out_str)[i].in();
		poll_stat->push_back(str);
	}	
	return poll_stat;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_polled() - return true if the object "obj_name" is polled.
//			      In this case, the upd string is initialised with
//			      the polling period.
//
//-----------------------------------------------------------------------------
 
bool DeviceProxy::is_polled(polled_object obj, string &obj_name,string &upd) 
{
	bool ret = false;
	vector<string> *poll_str;
	
	poll_str = polling_status();
	if (poll_str->size() == 0)
	{
		delete poll_str;
		return ret;
	}

	string loc_obj_name(obj_name);
	transform(loc_obj_name.begin(),loc_obj_name.end(),loc_obj_name.begin(),::tolower);
		
	for (unsigned int i = 0;i < poll_str->size();i++)
	{
		string &tmp_str = (*poll_str)[i];
		string::size_type pos,end;
		pos = tmp_str.find(' ');
		pos++;
		end = tmp_str.find(' ',pos + 1);
		string obj_type = tmp_str.substr(pos, end - pos);
		if (obj_type == "command")
		{
			if (obj == Attr)
				continue;
		}
		else if (obj_type == "attribute")
		{
			if (obj == Cmd)
			{
				if ((loc_obj_name != "state") && (loc_obj_name != "status"))
					continue;
			}
		}
		
		pos = tmp_str.find('=');
		pos = pos + 2;
		end = tmp_str.find(". S",pos + 1);
		if (end == string::npos)
			end = tmp_str.find('\n',pos + 1);
		string name = tmp_str.substr(pos, end - pos);
		transform(name.begin(),name.end(),name.begin(),::tolower);

		if (name == loc_obj_name)
		{
		
//
// Now that it's found, search for its polling period
//

			pos = tmp_str.find("triggered",end);
			if (pos != string::npos)
			{
				ret = true;
				upd = "0";
				break;
			}
			else
			{
				pos = tmp_str.find('=',end);
				pos = pos + 2;
				end = tmp_str.find('\n',pos + 1);
				string per = tmp_str.substr(pos, end - pos);
				upd = per;
				ret = true;
				break;
			}
		}
	}
			
	delete poll_str;
	
	return  ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_command_poll_period() - Return command polling period
//					    (in mS)
//
//-----------------------------------------------------------------------------
 
int DeviceProxy::get_command_poll_period(string &cmd_name) 
{
	string poll_per;
	bool poll = is_polled(Cmd,cmd_name,poll_per);

	int ret;	
	if (poll == true)
	{
		TangoSys_MemStream stream;
		
		stream << poll_per << ends;
		stream >> ret;
	}
	else
		ret = 0;
		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_poll_period() - Return attribute polling period
//					    (in mS)
//
//-----------------------------------------------------------------------------
 
int DeviceProxy::get_attribute_poll_period(string &attr_name) 
{
	string poll_per;
	bool poll = is_polled(Attr,attr_name,poll_per);

	int ret;	
	if (poll == true)
	{
		TangoSys_MemStream stream;
		
		stream << poll_per << ends;
		stream >> ret;
	}
	else
		ret = 0;
		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::poll_command() - If object is already polled, just update its
//				 polling period. If object is not polled, add
//				 it to the list of polled objects
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::poll_command(string &cmd_name, int period) 
{
	string poll_per;
	bool poll = is_polled(Cmd,cmd_name,poll_per);
	
	DevVarLongStringArray in;
	in.lvalue.length(1);
	in.svalue.length(3);
	
	in.svalue[0] = CORBA::string_dup(device_name.c_str());
	in.svalue[1] = CORBA::string_dup("command");
	in.svalue[2] = CORBA::string_dup(cmd_name.c_str());
	in.lvalue[0] = period;
	
	if (poll == true)
	{
	
//
// If object is polled and the polling period is the same, simply retruns
//		
		TangoSys_MemStream stream;
		int per;
		
		stream << poll_per << ends;
		stream >> per;
		
		if ((per == period) || (per == 0))
			return;
		else
		{
	
//
// If object is polled, this is an update of the polling period
//

			DeviceData din;
			string cmd("UpdObjPollingPeriod");
			din.any <<= in;

			try
			{			
				adm_device->command_inout(cmd,din);
			}
			catch (Tango::CommunicationFailed &)
			{
				adm_device->command_inout(cmd,din);
			}

		}
	}
	else
	{
	
//
// This a AddObjPolling command
//

		DeviceData din;
		string cmd("AddObjPolling");
		din.any <<= in;

		try
		{			
			adm_device->command_inout(cmd,din);
		}
		catch (Tango::CommunicationFailed &)
		{
			adm_device->command_inout(cmd,din);
		}

	}
	
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::poll_attribute() - If object is already polled, just update its
//				 polling period. If object is not polled, add
//				 it to the list of polled objects
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::poll_attribute(string &attr_name, int period) 
{
	string poll_per;
	bool poll = is_polled(Attr,attr_name,poll_per);
	
	DevVarLongStringArray in;
	in.lvalue.length(1);
	in.svalue.length(3);
	
	in.svalue[0] = CORBA::string_dup(device_name.c_str());
	in.svalue[1] = CORBA::string_dup("attribute");
	in.svalue[2] = CORBA::string_dup(attr_name.c_str());
	in.lvalue[0] = period;
	
	if (poll == true)
	{
	
//
// If object is polled and the polling period is the same, simply retruns
//		
		TangoSys_MemStream stream;
		int per;
		
		stream << poll_per << ends;
		stream >> per;
		
		if ((per == period) || (per == 0))
			return;
		else
		{
	
//
// If object is polled, this is an update of the polling period
//

			DeviceData din;
			string cmd("UpdObjPollingPeriod");
			din.any <<= in;

			try
			{			
				adm_device->command_inout(cmd,din);
			}
			catch (Tango::CommunicationFailed &)
			{
				adm_device->command_inout(cmd,din);
			}

		}
	}
	else
	{
	
//
// This a AddObjPolling command
//

		DeviceData din;
		string cmd("AddObjPolling");
		din.any <<= in;
		
		try
		{	
			adm_device->command_inout(cmd,din);
		}
		catch (Tango::CommunicationFailed &)
		{
			adm_device->command_inout(cmd,din);
		}

	}
	
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_cmd_polled() - return true if the command is polled
//
//-----------------------------------------------------------------------------

bool DeviceProxy::is_command_polled(string &cmd_name)
{
	string upd;
	return is_polled(Cmd,cmd_name,upd);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_attribute_polled() - return true if the attribute is polled
//
//-----------------------------------------------------------------------------

bool DeviceProxy::is_attribute_polled(string &attr_name)
{
	string upd;
	return is_polled(Attr,attr_name,upd);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::stop_poll_command() - Stop polling a command
//
//-----------------------------------------------------------------------------

void DeviceProxy::stop_poll_command(string &cmd_name)
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DevVarStringArray in;
	in.length(3);
	
	in[0] = CORBA::string_dup(device_name.c_str());
	in[1] = CORBA::string_dup("command");
	in[2] = CORBA::string_dup(cmd_name.c_str());
	
	DeviceData din;
	string cmd("RemObjPolling");
	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd,din);
	}

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::stop_poll_attribute() - Stop polling attribute
//
//-----------------------------------------------------------------------------

void DeviceProxy::stop_poll_attribute(string &attr_name)
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DevVarStringArray in;
	in.length(3);
	
	in[0] = CORBA::string_dup(device_name.c_str());
	in[1] = CORBA::string_dup("attribute");
	in[2] = CORBA::string_dup(attr_name.c_str());
	
	DeviceData din;
	string cmd("RemObjPolling");
	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd,din);
	}

}

#ifdef TANGO_HAS_LOG4TANGO
//-----------------------------------------------------------------------------
//
// DeviceProxy::add_logging_target - Add a logging target
//
//-----------------------------------------------------------------------------
void DeviceProxy::add_logging_target(const string &target_type_name)
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DevVarStringArray in(2);
	in.length(2);

	in[0] = CORBA::string_dup(device_name.c_str());
	in[1] = CORBA::string_dup(target_type_name.c_str());
	
	DeviceData din;
	string cmd("AddLoggingTarget");
	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd,din);
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::remove_logging_target - Remove a logging target
//
//-----------------------------------------------------------------------------
void DeviceProxy::remove_logging_target (const string &target_type_name)
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DevVarStringArray in(2);
	in.length(2);
	
	in[0] = CORBA::string_dup(device_name.c_str());
	in[1] = CORBA::string_dup(target_type_name.c_str());
	
	DeviceData din;
	string cmd("RemoveLoggingTarget");
	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd,din);
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_logging_target - Returns the logging target list
//
//-----------------------------------------------------------------------------
vector<string> DeviceProxy::get_logging_target (void)
{
  	if (adm_device == NULL)
		connect_to_adm_device();

  	DeviceData din;
	din << device_name;

  	string cmd("GetLoggingTarget");

  	DeviceData dout;
  	DevVarStringArray_var logging_targets;
	try
	{			
		dout = adm_device->command_inout(cmd, din);
	}
	catch (Tango::CommunicationFailed &)
	{
		dout = adm_device->command_inout(cmd, din);
	}

  	vector<string> logging_targets_vec;

  	dout >> logging_targets_vec;

  	return logging_targets_vec;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_logging_level - Returns the current logging level
//
//-----------------------------------------------------------------------------

int DeviceProxy::get_logging_level (void)
{
  	if (adm_device == NULL)
		connect_to_adm_device();

  	string cmd("GetLoggingLevel");

  	DevVarStringArray in;
	in.length(1);
	in[0] = CORBA::string_dup(device_name.c_str());

  	DeviceData din;
  	din.any <<= in;

  	DeviceData dout;
	try
	{			
		dout = adm_device->command_inout(cmd, din);
	}
	catch (Tango::CommunicationFailed &)
	{
		dout = adm_device->command_inout(cmd, din);
	}

  	long level;
  	if ((dout >> level) == false)
	{
		const Tango::DevVarLongStringArray *lsarr;
		dout >> lsarr;

		string devnm = dev_name();
		std::transform( devnm.begin(), devnm.end(), devnm.begin(), ::tolower );

		for( int i = 0; i < lsarr->svalue.length(); i++ )
		{
			string nm( lsarr->svalue[i] );
			std::transform( nm.begin(), nm.end(), nm.begin(), ::tolower );

			if( devnm == nm )
			{
				level = lsarr->lvalue[i];
				break;
			}
		}
	}
  
	return (int)level; 
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::set_logging_level - Set the logging level
//
//-----------------------------------------------------------------------------

void DeviceProxy::set_logging_level (int level)
{
  	if (adm_device == NULL)
		connect_to_adm_device();

  	string cmd("SetLoggingLevel");

  	DevVarLongStringArray in;
	in.lvalue.length(1);
	in.lvalue[0] = level;
  	in.svalue.length(1);
	in.svalue[0] = CORBA::string_dup(device_name.c_str());

  	DeviceData din;
  	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd, din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd, din);
	}
}
#endif // TANGO_HAS_LOG4TANGO



//-----------------------------------------------------------------------------
//
// DeviceProxy::subscribe_event - Subscribe to an event
//                                Old interface for compatibility
//
//-----------------------------------------------------------------------------

int DeviceProxy::subscribe_event (const string &attr_name, EventType event, 
                                 CallBack *callback, const vector<string> &filters)
{
	return subscribe_event (attr_name, event, callback, filters, false);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::subscribe_event - Subscribe to an event
//                                Adds the statless flag for stateless
//                                event subscription.
//
//-----------------------------------------------------------------------------

int DeviceProxy::subscribe_event (const string &attr_name, EventType event, 
                                 CallBack *callback, const vector<string> &filters,
											bool stateless)
{
  	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		ApiUtil::instance()->create_event_consumer();
	}

	return ApiUtil::instance()->get_event_consumer()->subscribe_event(this, attr_name, 
	                          event, callback, filters, stateless);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::subscribe_event - Subscribe to an event with the usage of
//                                the event queue for data reception.
//                                
//                                Adds the statless flag for stateless
//                                event subscription.
//
//-----------------------------------------------------------------------------

int DeviceProxy::subscribe_event (const string &attr_name, EventType event, 
                                 int event_queue_size, const vector<string> &filters,
                                 bool stateless)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		ApiUtil::instance()->create_event_consumer();
	}

	return ApiUtil::instance()->get_event_consumer()->subscribe_event(this, attr_name, 
	                          event, event_queue_size, filters, stateless);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::unsubscribe_event - Unsubscribe to an event
//
//-----------------------------------------------------------------------------

void DeviceProxy::unsubscribe_event (int event_id)
{
  	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		return;
	}
	
	ApiUtil::instance()->get_event_consumer()->unsubscribe_event(event_id);
}



//-----------------------------------------------------------------------------
//
// method :       DeviceProxy::get_events()
// 
// description :  Return a vector with all events stored in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied! 
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void DeviceProxy::get_events (int event_id, EventDataList &event_list)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::get_events()");
	}
	
	ApiUtil::instance()->get_event_consumer()->get_events(event_id, event_list);
}


//-----------------------------------------------------------------------------
//
// method :       DeviceProxy::get_events()
// 
// description :  Return a vector with all attribute configuration events 
//                stored in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied! 
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void DeviceProxy::get_events (int event_id, AttrConfEventDataList &event_list)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::get_events()");
	}
	
	ApiUtil::instance()->get_event_consumer()->get_events(event_id, event_list);
}

void DeviceProxy::get_events (int event_id, DataReadyEventDataList &event_list)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::get_events()");
	}
	
	ApiUtil::instance()->get_event_consumer()->get_events(event_id, event_list);
}

//-----------------------------------------------------------------------------
//
// method :       DeviceProxy::get_events()
// 
// description :  Call the callback method for all events stored 
//                in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied! 
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void DeviceProxy::get_events (int event_id, CallBack *cb)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::get_events()");
	}
	
	ApiUtil::instance()->get_event_consumer()->get_events(event_id, cb);
}




//+----------------------------------------------------------------------------
//
// method :       DeviceProxy::event_queue_size()
// 
// description :  Returns the number of events stored in the event queue 
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------
int  DeviceProxy::event_queue_size(int event_id)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::event_queue_size()");
	}
	
	return (ApiUtil::instance()->get_event_consumer()->event_queue_size(event_id));
}

//+----------------------------------------------------------------------------
//
// method :       DeviceProxy::is_event_queue_empty()
// 
// description :  Returns true when the event queue is empty
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------	
bool DeviceProxy::is_event_queue_empty(int event_id)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::is_event_queue_empty()");
	}
	
	return (ApiUtil::instance()->get_event_consumer()->is_event_queue_empty(event_id));
}

//+----------------------------------------------------------------------------
//
// method :       DeviceProxy::get_last_event_date()
// 
// description :  Get the time stamp of the last inserted event
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------	
TimeVal DeviceProxy::get_last_event_date(int event_id)
{
	if (ApiUtil::instance()->get_event_consumer() == NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Could not find event consumer object, \n";
		desc << "probably no event subscription was done before!";
		desc << ends;
		Tango::Except::throw_exception(
						(const char*)"API_EventConsumer",
						desc.str(), 
						(const char*)"DeviceProxy::get_last_event_date()");
	}
	
	return (ApiUtil::instance()->get_event_consumer()->get_last_event_date(event_id));
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::get_device_db - get database
//
//-----------------------------------------------------------------------------

Database * DeviceProxy::get_device_db()
{
	if ((db_port_num != 0) && (db_dev != NULL))
		return db_dev->get_dbase();
	else
		return (Database *)NULL;
}

//-----------------------------------------------------------------------------
//
// clean_lock - Litle function installed in the list of function(s) to be called
// at exit time. It will clean all locking thread(s) and unlock locked device(s)
//
//-----------------------------------------------------------------------------

void clean_lock()
{
	if (ApiUtil::_is_instance_null() == false)
	{
		ApiUtil *au = ApiUtil::instance();
		au->clean_locking_threads();
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::lock - Lock the device
//
//-----------------------------------------------------------------------------

void DeviceProxy::lock(int lock_validity)
{
	
//
// Feature unavailable for device without database
//

	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Feature not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::lock");
	}

//
// Some checks on lock validity
//
	
	if (lock_validity < MIN_LOCK_VALIDITY)
	{
		TangoSys_OMemStream desc;		
		desc << "Lock validity can not be lower than " << MIN_LOCK_VALIDITY << " seconds" << ends;
		
		Except::throw_exception((const char*)"API_MethodArgument",desc.str(),
								(const char*)"DeviceProxy::lock");
	}

	if (lock_ctr != 0)
	{
		if (lock_validity != lock_valid)
		{
			TangoSys_OMemStream desc;
			
			desc << "Device " << device_name << " is already locked with another lock validity (";
			desc << lock_valid << " sec)" << ends;
			
			Except::throw_exception((const char*)"API_MethodArgument",desc.str(),
									(const char*)"DeviceProxy::lock");
		}
	}

//
// Check if the function to be executed atexit is already installed
//
	
	Tango::ApiUtil *au = ApiUtil::instance();
	if (au->is_lock_exit_installed() == false)
	{
		atexit(clean_lock);
		au->set_lock_exit_installed(true);
	}
	
//
// Connect to the admin device if not already done
//
	
	if (adm_device == NULL)
		connect_to_adm_device();

//
// Send command to admin device
//
		
  	string cmd("LockDevice");
  	DeviceData din;
  	DevVarLongStringArray sent_data;
  	sent_data.svalue.length(1);
  	sent_data.svalue[0] = CORBA::string_dup(device_name.c_str());
  	sent_data.lvalue.length(1);
  	sent_data.lvalue[0] = lock_validity;
  	din << sent_data;

	adm_device->command_inout(cmd, din);

//
// Increment locking counter
//
	
	lock_ctr++;
	lock_valid = lock_validity;
	
//
// Try to find the device's server admin device locking thread
// in the ApiUtil map.
// If the thread is not there, start one.
// If the thread is there, ask it to add the device to its list
// of locked devices
//

	int interupted;
	
	{
		omni_mutex_lock oml(au->lock_th_map);
		
		map<string,LockingThread>::iterator pos = au->lock_threads.find(adm_dev_name);
		if (pos == au->lock_threads.end())
		{
			create_locking_thread(au,lock_validity);
		}
		else
		{
			if (pos->second.shared->suicide == true)
			{
				delete pos->second.shared;
				delete pos->second.mon;
				au->lock_threads.erase(pos);
				
				create_locking_thread(au,lock_validity);
			}
			else
			{
			
				omni_mutex_lock sync(*(pos->second.mon));
				if (pos->second.shared->cmd_pending == true)
				{
					interupted = pos->second.mon->wait(DEFAULT_TIMEOUT);
						
					if ((pos->second.shared->cmd_pending == true) && (interupted == 0))
					{
						cout4 << "TIME OUT" << endl;
						Except::throw_exception((const char *)"API_CommandTimedOut",
								        		(const char *)"Locking thread blocked !!!",
								        		(const char *)"DeviceProxy::lock");
					}
				}
				pos->second.shared->cmd_pending = true;
				pos->second.shared->cmd_code = LOCK_ADD_DEV;
				pos->second.shared->dev_name = device_name;
				pos->second.shared->lock_validity = lock_valid;
	
				pos->second.mon->signal();
					
				cout4 << "Cmd sent to locking thread" << endl;
			
				while (pos->second.shared->cmd_pending == true)
				{
					interupted = pos->second.mon->wait(DEFAULT_TIMEOUT);
				
					if ((pos->second.shared->cmd_pending == true) && (interupted == 0))
					{
						cout4 << "TIME OUT" << endl;
						Except::throw_exception((const char *)"API_CommandTimedOut",
								        		(const char *)"Locking thread blocked !!!",
								        		(const char *)"DeviceProxy::lock");
					}				
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::unlock - Unlock the device
//
//-----------------------------------------------------------------------------

void DeviceProxy::unlock(bool force)
{
	
//
// Feature unavailable for device without database
//

	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Feature not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::unlock");
	}

	if (adm_device == NULL)
		connect_to_adm_device();
//
// Send command to admin device
//
		
  	string cmd("UnLockDevice");
  	DeviceData din,dout;
  	DevVarLongStringArray sent_data;
  	sent_data.svalue.length(1);
  	sent_data.svalue[0] = CORBA::string_dup(device_name.c_str());
  	sent_data.lvalue.length(1);
  	if (force == true)
  		sent_data.lvalue[0] = 1;
  	else
  		sent_data.lvalue[0] = 0;
  	din << sent_data;

//
// Send request to the DS admin device
//
  	
	dout = adm_device->command_inout(cmd, din);

//
// Decrement locking counter or replace it by the device global counter
// returned by the server
//
	
	Tango::DevLong glob_ctr;
	
	dout >> glob_ctr;
	lock_ctr--;
	if (glob_ctr != lock_ctr)
		lock_ctr = glob_ctr;
		
//
// Try to find the device's server admin device locking thread
// in the ApiUtil map.
// Ask the thread to remove the device to its list of locked devices
//

	if ((lock_ctr == 0) || (force == true))
	{
		Tango::ApiUtil *au = Tango::ApiUtil::instance();
		int interupted;
		
		{
			omni_mutex_lock oml(au->lock_th_map);
			map<string,LockingThread>::iterator pos = au->lock_threads.find(adm_dev_name);
			if (pos == au->lock_threads.end())
			{
//				TangoSys_OMemStream o;
            
//				o << "Can't find the locking thread for device " << device_name << " and admin device " << adm_dev_name << ends;
//				Tango::Except::throw_exception((const char *)"API_CantFindLockingThread",o.str(),
//                                           		(const char *)"DeviceProxy::unlock()");
			}
			else
			{
				if (pos->second.shared->suicide == true)
				{
					delete pos->second.shared;
					delete pos->second.mon;
					au->lock_threads.erase(pos);
				}
				else
				{
					omni_mutex_lock sync(*(pos->second.mon));
					if (pos->second.shared->cmd_pending == true)
					{
						interupted = pos->second.mon->wait(DEFAULT_TIMEOUT);
						
						if ((pos->second.shared->cmd_pending == true) && (interupted == 0))
						{
							cout4 << "TIME OUT" << endl;
							Except::throw_exception((const char *)"API_CommandTimedOut",
								        		(const char *)"Locking thread blocked !!!",
								        		(const char *)"DeviceProxy::unlock");
						}
					}
					pos->second.shared->cmd_pending = true;
					pos->second.shared->cmd_code = LOCK_REM_DEV;
					pos->second.shared->dev_name = device_name;
	
					pos->second.mon->signal();
					
					cout4 << "Cmd sent to locking thread" << endl;
					
					while (pos->second.shared->cmd_pending == true)
					{
						interupted = pos->second.mon->wait(DEFAULT_TIMEOUT);
						
						if ((pos->second.shared->cmd_pending == true) && (interupted == 0))
						{
							cout4 << "TIME OUT" << endl;
							Except::throw_exception((const char *)"API_CommandTimedOut",
										        		(const char *)"Locking thread blocked !!!",
										        		(const char *)"DeviceProxy::unlock");
						}				
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::create_locking_thread - Create and start a locking thread
//
//-----------------------------------------------------------------------------

void DeviceProxy::create_locking_thread(ApiUtil *au,DevLong dl)
{
	
	LockingThread lt;
	lt.mon = NULL;
	lt.l_thread = NULL;
	lt.shared = NULL;
	
	pair<map<string,LockingThread>::iterator,bool> status;
	status = au->lock_threads.insert(make_pair(adm_dev_name,lt));
	if (status.second == false)
	{
		TangoSys_OMemStream o;
		o << "Can't create the locking thread for device " << device_name << " and admin device " << adm_dev_name << ends;
		Tango::Except::throw_exception((const char *)"API_CantCreateLockingThread",o.str(),
	                               (const char *)"DeviceProxy::create_locking_thread()");
	}
	else
	{
		map<string,LockingThread>::iterator pos;
		
		pos = status.first;
		pos->second.mon = new TangoMonitor(adm_dev_name.c_str());
		pos->second.shared = new LockThCmd;
		pos->second.shared->cmd_pending = false;
		pos->second.shared->suicide = false;
		pos->second.l_thread = new LockThread(*pos->second.shared,*pos->second.mon,adm_device,device_name,dl);
	
		pos->second.l_thread->start();
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::locking_status - Return a device locking status as a string
//
//-----------------------------------------------------------------------------

string DeviceProxy::locking_status()
{
	vector<string> v_str;
	vector<DevLong> v_l;
	
	ask_locking_status(v_str,v_l);

	string str(v_str[0]);
	return str;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_locked - Check if a device is locked
//
// Returns true if locked, false otherwise
//
//-----------------------------------------------------------------------------

bool DeviceProxy::is_locked()
{
	vector<string> v_str;
	vector<DevLong> v_l;
	
	ask_locking_status(v_str,v_l);
	
	return (bool)v_l[0];
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_locked_by_me - Check if a device is locked by the caller
//
// Returns true if the caller is the locker, false otherwise
//
//-----------------------------------------------------------------------------

bool DeviceProxy::is_locked_by_me()
{
	vector<string> v_str;
	vector<DevLong> v_l;
	
	ask_locking_status(v_str,v_l);

	bool ret = false;
	
	if (v_l[0] == 0)
		ret = false;
	else
	{
#ifndef _TG_WINDOWS_
		if (getpid() != v_l[1])
#else
		if (_getpid() != v_l[1])
#endif
			ret = false;
		else if ((v_l[2] != 0) || (v_l[3] != 0) || (v_l[4] != 0) || (v_l[5] != 0))
			ret = false;
		else
		{
			string full_ip_str;
			get_locker_host(v_str[1],full_ip_str);

//
// Get the host address(es) and check if it is the same than the one sent by the server
//
			
			char h_name[80];
			int res = gethostname(h_name,80);
			if (res == 0)
			{
				netcalls_mutex.lock();
				struct hostent *my_addr = gethostbyname(h_name);
				if (my_addr != NULL)
				{
					struct in_addr **addr_list;
					
					addr_list = (struct in_addr **)my_addr->h_addr_list;
					for (int i = 0;addr_list[i] != NULL;i++)
					{
						char *my_ip_addr = inet_ntoa(*addr_list[i]);
						if (::strcmp(my_ip_addr,full_ip_str.c_str()) == 0)
						{
							ret = true;
							break;
						}
					}
					netcalls_mutex.unlock();
				}
				else
				{
					netcalls_mutex.unlock();
					Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
													   (const char *)"Can't retrieve my IP address (gethostbyname)!",
													   (const char *)"DeviceProxy::is_locked_by_me()");
				}
			}
			else
			{
				Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
												   (const char *)"Can't retrieve my name (gethostbyname)!",
												   (const char *)"DeviceProxy::is_locked_by_me()");
			}
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_locker - Get some info on the device locker if the device
// is locked
// This method returns true if the device is effectively locked.
// Otherwise, it returns false
//
//-----------------------------------------------------------------------------

bool DeviceProxy::get_locker(LockerInfo &lock_info)
{
	vector<string> v_str;
	vector<DevLong> v_l;
	
	ask_locking_status(v_str,v_l);
	
	if (v_l[0] == 0)
		return false;
	else
	{
		
//
// If the PID info coming from server is not 0, the locker is CPP
// Otherwise, it is Java
//
		
		if (v_l[1] != 0)
		{
			lock_info.ll = Tango::CPP;
			lock_info.li.LockerPid = v_l[1];
			
			lock_info.locker_class = "Not defined";
		}
		else
		{
			lock_info.ll = Tango::JAVA;
			for (int loop = 0;loop < 4;loop++)
				lock_info.li.UUID[loop] = v_l[2 + loop];
			
			string full_ip;
			get_locker_host(v_str[1],full_ip);
			
			lock_info.locker_class = v_str[2];
		}
		
//
// Add locker host name
//
		
		string full_ip;
		get_locker_host(v_str[1],full_ip);

//
// Convert locker IP address to its name
//
		
		struct hostent *ho;
#ifdef sun
		ulong_t ad;
		
		ad = inet_addr(full_ip.c_str());
		ho = gethostbyaddr((const char *)&ad,sizeof(ad),AF_INET);
#else
#ifdef _TG_WINDOWS_
		unsigned int addr;
		addr = inet_addr(full_ip.c_str());
		ho = gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
#else
		struct in_addr ad;

		inet_aton(full_ip.c_str(),&ad);
		ho = gethostbyaddr(&ad,sizeof(ad),AF_INET);
#endif
#endif
			
		if (ho != NULL)
			lock_info.locker_host = ho->h_name;
		else
			lock_info.locker_host = full_ip;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::ask_locking_status - Get the device locking status
//
//-----------------------------------------------------------------------------

void DeviceProxy::ask_locking_status(vector<string> &v_str,vector<DevLong> &v_l)
{
	
//
// Feature unavailable for device without database
//

	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Feature not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::locking_status");
	}
	
	if (adm_device == NULL)
		connect_to_adm_device();
		
//
// Send command to admin device
//
		
  	string cmd("DevLockStatus");
  	DeviceData din,dout;
  	din.any <<= device_name.c_str();

	dout = adm_device->command_inout(cmd, din);
	
//
// Extract data and return data to caller
//

	dout.extract(v_l,v_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_locker_host - Isolate from the host string as it is returned
// by omniORB, only the host IP address
//
//-----------------------------------------------------------------------------

void DeviceProxy::get_locker_host(string &f_addr,string &ip_addr)
{	
//
// The hostname is returned in the following format:
// "giop:tcp:160.103.5.157:32989" or "giop:tcp:[::ffff:160.103.5.157]:32989
// We need to isolate the IP address
//

	bool ipv6 = false;
	if (f_addr.find('[') != string::npos)
		ipv6 = true;
		
	string::size_type pos;
	if ((pos = f_addr.find(':')) == string::npos)
	{
		Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
										(const char *)"Locker IP address returned by server is unvalid",
										(const char *)"DeviceProxy::get_locker_host()");
	}
	pos++;
	if ((pos = f_addr.find(':',pos)) == string::npos)
	{
		Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
										(const char *)"Locker IP address returned by server is unvalid",
										(const char *)"DeviceProxy::get_locker_host()");
	}
	pos++;
	
	if (ipv6 == true)
	{
		pos = pos + 3;
		if ((pos = f_addr.find(':',pos)) == string::npos)
		{
			Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
										(const char *)"Locker IP address returned by server is unvalid",
										(const char *)"DeviceProxy::get_locker_host()");
		}
		pos++;
		string ip_str = f_addr.substr(pos);
		if ((pos = ip_str.find(']')) == string::npos)
		{
			Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
										(const char *)"Locker IP address returned by server is unvalid",
										(const char *)"DeviceProxy::get_locker_host()");
		}
		ip_addr = ip_str.substr(0,pos);		
	}
	else
	{
		string ip_str = f_addr.substr(pos);
		if ((pos = ip_str.find(':')) == string::npos)
		{
			Tango::Except::throw_exception((const char*)"API_WrongLockingStatus",
										(const char *)"Locker IP address returned by server is unvalid",
										(const char *)"DeviceProxy::get_locker_host()");
		}
		ip_addr = ip_str.substr(0,pos);	
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::write_read_attribute() - write then read a single attribute 
//
//-----------------------------------------------------------------------------
 
DeviceAttribute DeviceProxy::write_read_attribute(DeviceAttribute &dev_attr) 
{
	
//
// This call is available only for Devices implemented IDL V4
//
	
	if (version < 4)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << device_name;
		desc << " does not support write_read_attribute feature" << ends;
		ApiNonSuppExcept::throw_exception((const char *)"API_UnsupportedFeature",
						  desc.str(),
						  (const char *)"DeviceProxy::write_read_attribute");		
	}

//
// Data into the AttributeValue object
//
	
	AttributeValueList_4 attr_value_list;
	attr_value_list.length(1);

	attr_value_list[0].name = dev_attr.name.c_str();
	attr_value_list[0].quality = dev_attr.quality;
	attr_value_list[0].data_format = dev_attr.data_format;
	attr_value_list[0].time = dev_attr.time;
	attr_value_list[0].w_dim.dim_x = dev_attr.dim_x;
	attr_value_list[0].w_dim.dim_y = dev_attr.dim_y;
		
	if (dev_attr.LongSeq.operator->() != NULL)
		attr_value_list[0].value.long_att_value(dev_attr.LongSeq.in());
	else if (dev_attr.ext->Long64Seq.operator->() != NULL)
		attr_value_list[0].value.long64_att_value(dev_attr.ext->Long64Seq.in());
	else if (dev_attr.ShortSeq.operator->() != NULL)
		attr_value_list[0].value.short_att_value(dev_attr.ShortSeq.in());
	else if (dev_attr.DoubleSeq.operator->() != NULL)
		attr_value_list[0].value.double_att_value(dev_attr.DoubleSeq.in());
	else if (dev_attr.StringSeq.operator->() != NULL)
		attr_value_list[0].value.string_att_value(dev_attr.StringSeq.in());
	else if (dev_attr.FloatSeq.operator->() != NULL)
		attr_value_list[0].value.float_att_value(dev_attr.FloatSeq.in());
	else if (dev_attr.BooleanSeq.operator->() != NULL)
		attr_value_list[0].value.bool_att_value(dev_attr.BooleanSeq.in());
	else if (dev_attr.UShortSeq.operator->() != NULL)
		attr_value_list[0].value.ushort_att_value(dev_attr.UShortSeq.in());
	else if (dev_attr.UCharSeq.operator->() != NULL)
		attr_value_list[0].value.uchar_att_value(dev_attr.UCharSeq.in());
	else if (dev_attr.ext->ULongSeq.operator->() != NULL)
		attr_value_list[0].value.ulong_att_value(dev_attr.ext->ULongSeq.in());
	else if (dev_attr.ext->ULong64Seq.operator->() != NULL)
		attr_value_list[0].value.ulong64_att_value(dev_attr.ext->ULong64Seq.in());
	else if (dev_attr.ext->StateSeq.operator->() != NULL)
		attr_value_list[0].value.state_att_value(dev_attr.ext->StateSeq.in());
	else if (dev_attr.ext->EncodedSeq.operator->() != NULL)
		attr_value_list[0].value.encoded_att_value(dev_attr.ext->EncodedSeq.in());

	int ctr = 0;
	AttributeValueList_4_var attr_value_list_4;
	
	while (ctr < 2)
	{
		try 
		{
			if (connection_state != CONNECTION_OK) reconnect(dbase_used);
			
//
// Throw exception if caller not allowed to write_attribute
//
			 
			if (access == ACCESS_READ)
			{
				try
				{
					device->ping();
				}
				catch(...) {connection_state = CONNECTION_NOTOK;throw;}
				
				TangoSys_OMemStream desc;
				desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
			
				NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
											  	  (const char *)"DeviceProxy::write_read_attribute()");
			}

//
// Now, call the server
//
			
			ClntIdent ci;
			ApiUtil *au = ApiUtil::instance();
			ci.cpp_clnt(au->get_client_pid());
				
			attr_value_list_4 = ext->device_4->write_read_attributes_4(attr_value_list,ci);

			ctr = 2;

		}
		catch (Tango::MultiDevFailed &e)
		{

//
// Transfer this exception into a DevFailed exception
//

			Tango::DevFailed ex(e.errors[0].err_list);
			TangoSys_OMemStream desc;
			desc << "Failed to write_read_attribute on device " << device_name;
			desc << ", attribute ";
			desc << attr_value_list[0].name.in();
			desc << ends;
			Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_read_attribute()");

		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to write_read_attribute on device " << device_name;
			desc << ", attribute ";
			desc << attr_value_list[0].name.in();
			desc << ends;

			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"DeviceProxy::write_read_attribute()");
			else
                Except::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        	desc.str(), (const char*)"DeviceProxy::write_read_attribute()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","write_read_attribute()");
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","write_read_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_read_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_read_attribute()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","write_read_attribute");
			}
			else
			{
				connection_state = CONNECTION_NOTOK;
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << device_name << ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"DeviceProxy::write_read_attribute()");
			}
		}
        catch (CORBA::SystemException &ce)
        {
			connection_state = CONNECTION_NOTOK;

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes on device " << device_name << ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"DeviceProxy::write_read_attribute()");
		}
	}

//
// Init the returned DeviceAttribute instance
//

	DeviceAttribute ret_dev_attr;
	ApiUtil::attr_to_device(&(attr_value_list_4[0]),version,&ret_dev_attr);
				
//
// Add an error in the error stack in case there is one
//
		
	long nb_except = ret_dev_attr.ext->err_list.in().length();
	if (nb_except != 0)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to write_read_attribute on device " << device_name;
		desc << ", attribute " << dev_attr.name << ends;
		
		ret_dev_attr.ext->err_list.inout().length(nb_except + 1);
		ret_dev_attr.ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
		ret_dev_attr.ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::write_read_attribute()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
		string st = desc.str();
		ret_dev_attr.ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
		char *tmp_str = desc.str();
		ret_dev_attr.ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
		delete[] tmp_str;
#endif
		ret_dev_attr.ext->err_list[nb_except].severity = Tango::ERR;
	} 

	return(ret_dev_attr);
}


} // End of Tango namespace
