/*Copyright 2011 Aalborg University. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Aalborg University ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Aalborg University OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed*/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#include "tcpclient.h"
#include "udpserver.h"
#include "utils.h"

#include <errno.h>
#include <string.h>
#include <netdb.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#define MAXBUFLEN 255
#define DATASIZE 8192
/** Once the library is installed, should be remplaced by <hpdaemon/homeport.h> */
#include "homeport.h"

/** Define the lamp states */
float temp0_state = 25.000;
float temp1_state = 26.000;
char heatpump_state[255] = "On";
char bridgeipaddr[100] = "127.0.0.1"; // default ip (local)


/** A GET function for a service
*	Takes a Service structure in parameter, and return a service value as a char*
*/
size_t
get_temp ( Service* service, char *buffer, size_t max_buffer_size )
{
	printf("Received GET temp on %s %s\n", service->description, service->ID);
	if( 0 == strcmp(service->ID, "0") )
		sprintf(buffer, "%6.0f", temp0_state *1000);
	else if ( 0 == strcmp(service->ID, "1") )
		sprintf(buffer, "%6.0f", temp1_state * 1000);
	else
	{
		printf("ERROR\n");
		return 0;
	}
	return strlen(buffer);
}
/** A PUT function for a service
*	Takes a Service structure in parameter, and return an updated service value as a char*
*/
size_t
put_temp ( Service* service, char *buffer, size_t max_buffer_size, char *put_value )
{
	printf("Received PUT tenp on %s id : %s value : %s \n", service->description, service->ID, put_value);

	if( 0 == strcmp(service->ID, "0") )
		temp0_state = atof(put_value)/1000;
	else if ( 0 == strcmp(service->ID, "1") )
		temp1_state = atof(put_value)/1000;

	strcpy(buffer, put_value);
	return strlen(buffer);
}

size_t
get_heat ( Service* service, char *buffer, size_t max_buffer_size )
{
	printf("Received GET heatpump on %s %s\n", service->description, service->ID);
	sprintf(buffer,"%s", heatpump_state);
	return strlen(buffer);
}

put_heat ( Service* service, char *buffer, size_t max_buffer_size, char *put_value )
{
	printf("Received PUT heat on %s id : %s value : %s \n", service->description, service->ID, put_value);

	if( 0 == strcmp(put_value, "On") )
	system("./on_helper.sh");
	else if ( 0 == strcmp(put_value, "Off") )
	system("./off_helper.sh");
	else return -1; // return if invalid input

	printf("Setting devices to: %s\n",put_value);
	strncpy(heatpump_state, put_value, 255);
	sprintf(buffer,"Heatpump is %s", heatpump_state);

	return strlen(buffer);
}
size_t
get_bridge( Service* service, char *buffer, size_t max_buffer_size )
{
	char inbuffer[512];
	printf("Received GET temp on %s %s\n", service->description, service->ID);

		if( 0 == strcmp(service->ID, "0") )
			EventListen_local(bridgeipaddr, "10001", inbuffer);

		else if ( 0 == strcmp(service->ID, "1") )
			EventListen_local(bridgeipaddr, "10002", inbuffer);

		else if ( 0 == strcmp(service->ID, "2") )
			EventListen_local(bridgeipaddr, "10003", inbuffer);

		else if ( 0 == strcmp(service->ID, "3") )
			EventListen_local(bridgeipaddr, "10004", inbuffer);

		else if ( 0 == strcmp(service->ID, "4") )
			EventListen_local(bridgeipaddr, "10005", inbuffer);

		else if ( 0 == strcmp(service->ID, "5") )
			EventListen_local(bridgeipaddr, "10006", inbuffer);

		strcpy(buffer, inbuffer);
		return strlen(buffer);
}

size_t
put_bridge ( Service* service, char *buffer, size_t max_buffer_size, char *put_value )
{
	printf("Received PUT tenp on %s id : %s value : %s \n", service->description, service->ID, put_value);
//	char idbuffer1[100];
//	char idbuffer2[100];
//
char inbuffer[512];

char line[255];
char sysstr[255];

	if( 0 == strcmp(service->ID, "0") )
			IP_send_String(bridgeipaddr , "10001" , put_value, strlen(put_value) , inbuffer);

	else if ( 0 == strcmp(service->ID, "1") )

			IP_send_String(bridgeipaddr , "10002" , put_value, strlen(put_value) , inbuffer);

	else if ( 0 == strcmp(service->ID, "2") )
			IP_send_String(bridgeipaddr , "10003" , put_value, strlen(put_value) , inbuffer);

	else if ( 0 == strcmp(service->ID, "3") )
			IP_send_String(bridgeipaddr , "10004" , put_value, strlen(put_value) , inbuffer);

	else if ( 0 == strcmp(service->ID, "4") )
			IP_send_String(bridgeipaddr , "10005" , put_value, strlen(put_value) , inbuffer);

	else if ( 0 == strcmp(service->ID, "5") )
			IP_send_String(bridgeipaddr , "10006" , put_value, strlen(put_value) , inbuffer);
	sleep(3);
	strcpy(buffer, inbuffer);
	return strlen(buffer);
}



int main (int argc, char *argv[])
{
	char * d1,* d2, *d3, *d4, *d5, *d6;
	char * bl,* ipadr;
	char buffer[200]="";
	char bridgelocation[100] = "outside";
	char device1type[100] = "1";
	char device2type[100] = "2";
	char device3type[100] = "3";
	char device4type[100] = "4";
	char device5type[100] = "5";
	char device6type[100] = "6";

	bl = getarg("-l", argc, argv);
	ipadr = getarg("-a", argc, argv);

	d1 = getarg("-d1", argc, argv);
	d2 = getarg("-d2", argc, argv);
	d3 = getarg("-d3", argc, argv);
	d4 = getarg("-d4", argc, argv);
	d5 = getarg("-d5", argc, argv);
	d6 = getarg("-d6", argc, argv);

	if (bl != NULL) strcpy(bridgelocation, bl );
	if (ipadr != NULL) strcpy(bridgeipaddr, ipadr );

	if (d1 != NULL) strcpy(device1type, d1 );
	if (d2 != NULL) strcpy(device2type, d2 );
	if (d3 != NULL) strcpy(device3type, d3 );
	if (d4 != NULL) strcpy(device4type, d4 );
	if (d5 != NULL) strcpy(device5type, d5 );
	if (d6 != NULL) strcpy(device6type, d6 );

	system("zigbee-bridge &");
	//system("zwave-bridge");
	sleep(10);
	IP_send_String(bridgeipaddr , "10000" , "NETLIST_REQ", strlen("NETLIST_REQ") , buffer);
	//printf("BUFFER ** : %s\n", buffer);

	if ( 0 != strncmp(buffer, "6E65746C6973745F646F6E653", strlen("6E65746C6973745F646F6E653")) )  {printf ("bridge not responding correct ERROR" ); return -1;}
	else printf ("bridge ok\n ");

	int rc;

	/** Starts the hpdaemon. If using avahi-core pass a host name for the server, otherwise pass NULL */
	if( rc = HPD_start( HPD_USE_CFG_FILE, "Homeport", HPD_OPTION_CFG_PATH, "./hpd.cfg" ) )
	{
		printf("Failed to start HPD %d\n", rc);
		return 1;
	}

#if HPD_HTTP
	/** Creation and registration of non secure services */
	/** Create a device that will contain the services
	* 1st  parameter : A description of the device (optional)
	* 2nd  parameter : The ID of the device
	* 3rd  parameter : The device's vendor ID (optional)
	* 4th  parameter : The device's product ID (optional)
	* 5th  parameter : The device's version (optional)
	* 6th  parameter : The device's IP address (optional)
	* 7th  parameter : The device's port (optional)
	* 8th  parameter : The device's location (optional)
	* 9th  parameter : The device's type
	* 10th parameter : A flag indicating if the communication with the device should be secure
	* 				   HPD_SECURE_DEVICE or HPD_NON_SECURE_DEVICE
	*/
	Device *device1 = create_device_struct("WeatherStation",
						"1",
						"0x01",
						"0x01",
						"V1",
						NULL,
						NULL,
						"outside",
						"Weatherstation",
						HPD_NON_SECURE_DEVICE);

	Device *device2 = create_device_struct("HeatPump",
						"2",
						"0x02",
						"0x02",
						"V1",
						NULL,
						NULL,
						"inside",
						"heatpump",
						HPD_NON_SECURE_DEVICE);
	Device *device3 = create_device_struct("Bridge",
							bridgelocation,
							"0x03",
							"0x03",
							"V1",
							NULL,
							NULL,
							bridgelocation,
							"Bridge",
							HPD_NON_SECURE_DEVICE);

	/** Create a service
	* 1st parameter : The service's description (optional)
	* 2nd parameter : The service's ID
	* 4th parameter : The service's type
	* 5th parameter : The service's unit (optional)
	* 6th parameter : The device owning the service
	* 7th parameter : The service's GET function
	* 8th parameter : The service's PUT function (optional)
	* 9th parameter : The service's parameter structure
	*/
	Service *service_temp0 = create_service_struct ("tempDesc", "0", "temperature", "mili Celcius", device1,
	                                                 get_temp, put_temp,
	                                                 create_parameter_struct ("1", "50000", "0",
	                                                                          "mili", "1", "temperature",
	                                                                          "Celcius", NULL)
							,NULL);

	Service *service_heat0 = create_service_struct ("heatDesc", "0", "heat_source", "On,Off", device2,
		                                                 get_heat, put_heat,
		                                                 create_parameter_struct ("2", "5000", "0",
		                                                                          "mega", "1", "watt",
		                                                                          "watt", NULL)
								,NULL);


	Service *service_br0 = create_service_struct (device1type, "0", device1type, NULL, device3,
			                                                 get_bridge, put_bridge,
			                                                  NULL
									,NULL);

	Service *service_br1 = create_service_struct (device2type, "1", device2type, NULL, device3,
				                                                 get_bridge, put_bridge,
				                                                 NULL
										,NULL);

	Service *service_br2 = create_service_struct (device3type, "2", device3type, NULL, device3,
					                                                 get_bridge, put_bridge,
					                                                 NULL
											,NULL);

	Service *service_br3 = create_service_struct (device4type, "3", device4type, NULL, device3,
					                                                 get_bridge, put_bridge,
					                                                NULL
											,NULL);


	Service *service_br4 = create_service_struct (device5type, "4",device5type , NULL, device3,
					                                                 get_bridge, put_bridge,
					                                                NULL
											,NULL);

	Service *service_br5 = create_service_struct (device6type, "5",device6type , NULL, device3,
					                                                 get_bridge, put_bridge,
					                                                NULL
											,NULL);


	/*
	Service *service_lamp1 = create_service_struct ("Lamp1", "1", "Lamp", "0/1", device,
	                                                 get_lamp, put_lamp,
	                                                 create_parameter_struct ("0", NULL, NULL,
	                                                                          NULL, NULL, NULL,
	                                                                          NULL, NULL),NULL);
	Service *service_switch0 = create_service_struct ("Switch0", "0", "Switch", "ON/OFF", device,
	                                                   get_switch, NULL,
	                                                   create_parameter_struct ("0", NULL, NULL,
	                                                                            NULL, NULL, NULL,
	                                                                            NULL, NULL),NULL);

	Service *service_switch1 = create_service_struct ("Switch1", "1", "Switch", "ON/OFF", device,
	                                                   get_switch, NULL,
	                                                   create_parameter_struct ("0", NULL, NULL,
	                                                                            NULL, NULL, NULL,
	                                                                            NULL, NULL),NULL);
*/
	printf("Registering services temperature sensor and heat pump. Access details via URL : hostIP:port/devices\n");
	/** Register a service into the HPD web server */
	HPD_register_service (service_temp0);
	HPD_register_service (service_heat0);
	HPD_register_service (service_heat0);
	HPD_register_service (service_br0);
	HPD_register_service (service_br1);

	HPD_register_service (service_br2);
	HPD_register_service (service_br3);
	HPD_register_service (service_br4);
	HPD_register_service (service_br5);

	/* printf("Registering service lamp1. Can be accessed at URL : Example/1/Lamp/1\n");
	HPD_register_service (service_lamp1);
	printf("Registering service switch0. Can be accessed at URL : Example/1/Switch/0\n");
	HPD_register_service (service_switch0);
	printf("Registering service switch1. Can be accessed at URL : Example/1/Switch/1\n");
	HPD_register_service (service_switch1);
	 */
#endif

#if HPD_HTTPS /** Creation and registration of secure services */
	/** Create a secured device that will contain the secured services */
	Device *secure_device = create_device_struct("SecureExample",
						"1",
						"0x01",
						"0x01",
						"V1",
						NULL,
						NULL,
						"LivingRoom",
						"SecureExample",
						HPD_SECURE_DEVICE);
	/** Create a service
	* 1st parameter : The service's description (optional)
	* 2nd parameter : The service's ID
	* 4th parameter : The service's type
	* 5th parameter : The service's unit (optional)
	* 6th parameter : The device owning the service
	* 7th parameter : The service's GET function
	* 8th parameter : The service's PUT function (optional)
	* 9th parameter : The service's parameter structure
	*/
	Service *secure_service_lamp0 = create_service_struct ("Lamp0", "0", "Lamp", "ON/OFF", secure_device,
	                                                 get_lamp, put_lamp,
	                                                 create_parameter_struct ("0", NULL, NULL,
	                                                                          NULL, NULL, NULL,
	                                                                          NULL, NULL), NULL);
	Service *secure_service_lamp1 = create_service_struct ("Lamp1", "1", "Lamp", "ON/OFF", secure_device,
	                                                 get_lamp, put_lamp,
	                                                 create_parameter_struct ("0", NULL, NULL,
	                                                                          NULL, NULL, NULL,
	                                                                          NULL, NULL), NULL);
	Service *secure_service_switch0 = create_service_struct ("Switch0", "0", "Switch", "ON/OFF", secure_device,
	                                                   get_switch, NULL,
	                                                   create_parameter_struct ("0", NULL, NULL,
	                                                                            NULL, NULL, NULL,
	                                                                            NULL, NULL), NULL);

	Service *secure_service_switch1 = create_service_struct ("Switch1", "1", "Switch", "ON/OFF", secure_device,
	                                                   get_switch, NULL,
	                                                   create_parameter_struct ("0", NULL, NULL,
	                                                                            NULL, NULL, NULL,
	                                                                            NULL, NULL), NULL);
	/** Register all the device's services into the HPD web server */
	HPD_register_device_services(secure_device);


#endif

	getchar();

	system("killall zigbee-bridge");
	//system("killall zwave-bridge");


	/** Unregistering services is not necessary, calling HPD_stop unregisters and
	    deallocates the services and devices that are still register in HPD		   */
	/** However when unregistering a service note that the memory is not deallocated   */
	/** Also note that attempting to free a service that is still registered will fail */

#if HPD_HTTP
	/** Unregister a service from the HPD web server */
	HPD_unregister_service (service_temp0);
//	HPD_unregister_service (service_lamp1);
//	HPD_unregister_service (service_switch0);
//	HPD_unregister_service (service_switch1);
	/** Deallocate the memory of the services. When deallocating the last service of a device,
	    the device is deallocated too, so there is no need to call destroy_device_struct       */
	destroy_service_struct(service_temp0);
//	destroy_service_struct(service_lamp1);
//	destroy_service_struct(service_switch0);
//	destroy_service_struct(service_switch1);
#endif

#if HPD_HTTPS
	/** Unregister all the services of a device */
	HPD_unregister_device_services(secure_device);
	/** Deallocate the memory of the device, and all the services linked to this device */
	destroy_device_struct(secure_device);
#endif

	/** Stops the HPD daemon */
	HPD_stop ();

	return (0);
}

