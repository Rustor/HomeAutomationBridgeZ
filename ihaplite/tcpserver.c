/*
 *
 *      Author: tsh,rto
 *
 * This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "tcpserver.h"
#include "framework.h"
#include "auxfunctions.h"
//
// Executes a command received from client
//
unsigned char *receive_cmd(int client, unsigned char VPS)
{
	/*
	 * Message format: [VPS][Payload Length][Payload]
	 */
	int connected = 1,  select_temp, cmd_len = 2;// j;
	fd_set fdset;
	struct timeval timer;
	unsigned char *cmd;
	cmd = (unsigned char *)calloc(1, DATASIZE);
	cmd[0] = VPS;

	while (connected) {
		FD_ZERO(&fdset);
		FD_SET(client, &fdset);
		timer.tv_sec= 3;
		timer.tv_usec= 200;
	 	printf(",");
		select_temp = select(client+1, &fdset, NULL, NULL, &timer);
		//select_temp = select(client+1, &fdset, NULL, NULL, NULL);
	 	printf(" %d ", select_temp);
		if ( select_temp  > 0 ) {
		 	printf(".");
			read(client, &cmd[cmd_len], 1);
			cmd_len += 1;
			if (cmd_len == DATASIZE-1) {
				printf("receive_cmd: receive buffer exceeded\n");
				connected = 0;
			}
			// End message on Carriage Return (0d) or Line Feed (0a)
			if ( (cmd_len > 1)
					&& ( (cmd[cmd_len-1] == 0x0d) || (cmd[cmd_len-1] == 0x0a) )
			   ) {
				printf("\nreceive_cmd:  End message on Carriage Return \n");
				// Null terminate the array by removing the CR or LF
				cmd[cmd_len-1] = 0x00;
				// Make Length 3 less to compensate for VPS, Length, and removal of CR or LF
				cmd[1] = (unsigned char)cmd_len-3;
				//execute_cmd(client, cmd);
				connected = 0;

//#ifdef _DEBUG_VERBOSE
				int j;
				if (PRINT_STATUS) {
					//printf("%02X", cmd);
					printf("TCP receive_cmd: [");
					for(j=0;cmd_len>j;j++) {
						printf("%02X ", cmd[j]);
					}
					printf("] (ASCII)\n");
				}
//#endif
			} //end if-check
		} else {free(cmd); return NULL;} //end if-select
	}  //end while
	//free(cmd);
	//
	return(cmd);
}


int server_init(int server_port)
{
	int s, opt = 1;
	struct sockaddr_in s_addr;

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(server_port);
	s_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		printf("FAIL3: %s\n", strerror(errno));
		return(-1);
	}

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)); // RTO june2010 // jan2011
	//fcntl(s, F_SETFL, O_NONBLOCK);
	if ( bind(s, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0 ) {
		printf("FAIL4: %s\n", strerror(errno));
		return(-1);
	}

	if ( listen(s, 1) < 0 ) {
		printf("FAIL5: %s\n", strerror(errno));
		return(-1);
	}

	if (PRINT_STATUS)
		printf("server listening on port %i\n", server_port);

	return(s);
}

int server_end(int s)
{
	if ( (shutdown(s, 0) < 0) || (close(s) < 0) ) {
		printf("FAIL6: %s\n", strerror(errno));
		return(-1);
	}

	if (PRINT_STATUS)
		printf("server shutdown\n");

	return(0);
}


void IP_send_server_hexbinaryString(int client, unsigned char *binary_data, unsigned char length) {
	unsigned char len;
	unsigned char output[DATASIZE_HEXBINARY];
	//int i;
	// Convert to hexbinary string
	/*for(i=0;i<length;i++) {
		sprintf(&printbuffer[i*2], "%02X", binary_data[i]);
	}
	length += length;
	printbuffer[length] = 0x0A;  // End with new-line*/

	len = binary_to_hexbinary(binary_data, output, length);
	// TODO should be more flexible, we can only send 128 bytes due to hexbin format
	if ( length > 0 )
		write(client, output, len);

}

void IP_send_server_String(int client, unsigned char *binary_data, unsigned char length) {
	//unsigned char len;
	//unsigned char output[DATASIZE_HEXBINARY];
	//int i;
	// Convert to hexbinary string
	/*for(i=0;i<length;i++) {
		sprintf(&printbuffer[i*2], "%02X", binary_data[i]);
	}
	length += length;
	printbuffer[length] = 0x0A;  // End with new-line*/

	//len = binary_to_hexbinary(binary_data, output, length);
	// TODO should be more flexible, we can only send 128 bytes due to hexbin format
	if ( length > 0 )
		write(client, binary_data, length);
}

void respond(int client, unsigned char *message, unsigned char length)
{
	//int len = strlen(message);
	if ( length > 0 )
			write(client, message, length);
}


