/*
 * parser.c
 *
 *  Created on: Feb 22, 2012
 *      Author: rto
 *
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

char d_zigbee_device_on(char *url )

{
	char line[255];
	char sysstr[255];
	sprintf(sysstr, "hpclientPUT PUT %s 0401210600FF0403116701", url);
		syscall_pipe(sysstr, line);
		//printf("%s", line);
}

char d_zigbee_device_off(char *url  )

{
	char line[255];
	char sysstr[255];
	sprintf(sysstr, "hpclientPUT PUT %s 0401210600FF0403116700" , url);
		syscall_pipe(sysstr, line);
		//printf("%s", line);
}

int ODSDL_parser( char *devicexml_path, char * deviceid, char * serviceid, char * inputparm, char *outbuffer, char * url )
{
// placeholder
	printf("inputparm = %s\n", inputparm);
	if (strncmp(devicexml_path, "7A621A210401090001080000030004000500060002070A000900010604", strlen("7A621A210401090001080000030004000500060002070A000900010604")) == 0)
	{
		if (!strncmp(inputparm, "Off", 3))
		{
			printf("turning off\n");
				d_zigbee_device_off(url);
		}
		else //if (strncmp(inputparm, "Off", 3))
				{printf("turning on\n");d_zigbee_device_on(url);
		}
	}
}


int get_desc(char * url, char * result )
{
	char sysstr[255];
	sprintf(sysstr, "hpclientPUT PUT %s GETDESC_REQ21" , url);
	syscall_pipe(sysstr, result);
	//printf("%s", result);
	//strcpy(result, "7A621A210401090001080000030004000500060002070A000900010604");
}


int hp_ODSDL_parser( char *url, char * servicetype, char * location, char * inputparm, char *outbuffer )
{ // location will be ignored
	//char deviceid[255];
	char devicetypeid[512];
	//char serviceid[255];

	get_desc( url, devicetypeid); // get device type from real device

	//get_device_id_from_xml(devicetypeid, deviceid); // using device type as file name to find driver

	//get_service_id( devicetypeid, servicetype,  serviceid); // simply get the first service that matches service type for this device

	ODSDL_parser( devicetypeid , NULL , NULL, inputparm, outbuffer, url ); // NULL is for dont care for device and service id in this simple first version
																	// the parser should just take firste device and service in the destriptor file
	return 0;
}
