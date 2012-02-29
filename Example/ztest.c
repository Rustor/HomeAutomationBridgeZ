#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "parser.h"


int find_service_at_location(char * servicetype, char *location, char * resulturl)
{
	char line[255];
	char sysstr[255];

	sprintf(sysstr, "./helper.sh %s %s",servicetype,  location);
	syscall_pipe(sysstr, line);

	sscanf(line, "%s", resulturl);
	//printf("%s\n", resulturl);

	 //strcpy(resulturl, "http://192.168.1.243:8889/Bridge/outside/light_source/1");
}

int main (int argc, char *argv[])
{
	char outputurl[255];
	char sysstr[255];
	char resultbuf[255];

	char location[255] = "outside";
	char requested_service[255] = "light_source";

	//********* locate light source service that is outside ***********//
	find_service_at_location(requested_service , location , outputurl);

	printf("Service url = %s\n", outputurl);

	sleep(2);

	// *********** use service ************************ //
	hp_ODSDL_parser( outputurl, requested_service , location ,  argv[1] , resultbuf );


	return 0;
}
