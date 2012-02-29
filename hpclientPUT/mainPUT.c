/*
 *	This program is free software: you can redistribute it and/or modify
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

#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
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
//#include "HP_client.h"
//#include "curl_fct.h"
//#include "xmlAPI.h"


#include "tcpclient.h"
#include "utils.h"
#include "xmlhttputils.h"

#define MAXBUFLEN 255
#define DATASIZE 8192



int httpput(char * local_ip_addr, char * ip_port, char * httprequesturl, char * httpvalue, char * buffer) {
	char * abuf[100];
	int len, run = 1, count = 0;
	//int len;// run = 1;
	char *bufferbak;
	bufferbak = buffer;
	strcat(buffer, "PUT ");
	strcat(buffer, httprequesturl);
	strcat(buffer, " HTTP/1.1 \nAccept: text/xml \nContent-Type: text/xml  \nContent-Length: ");
	sprintf(abuf, "%d", strlen(httpvalue));
	strcat(buffer, abuf);
	strcat(buffer, "\r\n\r");

	strcat(buffer, httpvalue);
	strcat(buffer, "\n");

	len = IP_send_String(local_ip_addr, ip_port, buffer, strlen(buffer), buffer); // pir input

	while (run && (count < 200)) {
		if ((*buffer == '<') && (*(buffer + 1) == '?'))
			run = 0;
		buffer++;
		count++;
	}
	strcpy(bufferbak, buffer - 1);

	return len;

}

int spliturl(char * text, char * ip, char * port, char *page) {
	sscanf(text, "http://%99[^:]:%99[^/]%99[^\n]", ip, port, page);
	return 0;
}

int main(int argc, char *argv[]) {
	char data[1000] = "";
	char moredata[1000] = "";
	char url[255] = "";

	init_curl();

	//init_curl(events_callback, exitEventsCallback);

	if (strncmp(argv[1], "XML", 3) == 0) {
		if (argc != 3) {
			printf("wrong number of args for get xml\n");
			exit - 1;
		}
		//printf("%s\n", url);
		strcat(url, argv[2]);
		strcat(url, "?x=1");
		char *get_data = http_get_function(url);
		//char *value = get_value_from_xml_value(get_data);
		//printf("%s\n", value);
		printf("%s\n", get_data);

	} else if (strncmp(argv[1], "GET", 3) == 0) {
		if (argc != 3) {
			printf("wrong number of args for get\n");
			exit - 1;
		}

		if (strncmp(argv[2], "FILE", 4) == 0) {
			load_file_into_buffer(url, "mainSD-OUTPUT.url");
		} else
			strcpy(url, argv[2]);

		//printf("%s\n", url);
		char *get_data = http_get_function(url);
		char *value = get_value_from_xml_value(get_data);
		//		printf("%s\n", value);
		//printf("%s\n", get_data);

	} else if (strncmp(argv[1], "PUT", 5) == 0) {

		if (argc != 4) {
			printf("wrong number of args for put\n");
			exit - 1;
		}

		if (strncmp(argv[2], "FILE", 4) == 0) {
			load_file_into_buffer(url, "mainSD-OUTPUT.url");
		} else
			strcpy(url, argv[2]);

		strcpy(data, argv[3]);

		char * xml = get_xml_value(data); //

		//xml = http_put_function(url, xml);// "<?xml ?><value> 29000</value>"
		//strcpy(data, xml); "   //  /Bridge/Livingroom/switch/0"
		char ipaddr[20] = "";
		char port[20] = "";
		char page[250] = "";

		spliturl(url, ipaddr, port, page);

		//	    printf("ip = \"%s\"\n", ipaddr);
		//	    printf("port = \"%s\"\n", port);
		//	    printf("page = \"%s\"\n", page);

		httpput(ipaddr, port, page, xml, moredata);
		//free(xml);

		char *value = get_value_from_xml_value(moredata);

		printf("%s\n", value);

	} else
		printf("Command not supported\n");

	return 0;
}

