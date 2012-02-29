/*
 *
 *       Author: tsh,rto
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


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
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
#include "tcpclient.h"

#define MAXBUFLEN 255
#define DATASIZE 8192

//#define MAX_VPS 2
#define byte	unsigned char

int receive_data(int client, char *cmd, int maxlen) // read one line ie. until cr/lf
{
	int connected = 1, cmd_len = 0;
	fd_set fdset;
	struct timeval timer;
	//char *cmd;
	// cmd = (char *)calloc(1, 1000);

	while (connected) {
		FD_ZERO(&fdset);
		FD_SET(client, &fdset);
		timer.tv_sec = 0;
		timer.tv_usec = 200;
		if (select(client + 1, &fdset, NULL, NULL, &timer)> 0 ) {
			read(client, &cmd[cmd_len], 1);
			cmd_len += 1;
			if (cmd_len == maxlen) {
				printf("receive buffer exceeded\n");
				connected = 0;
			}
			if ( (cmd_len> 0) && 0
			&&
			((cmd[cmd_len-1] == 0x0d)
					|| (cmd[cmd_len-1] == 0x0a))
			)
			{
				connected = 0;
			}
		}
	}
	//free(cmd);
				return(cmd_len);
			}

int IP_send_String(char *device, char *port,
		unsigned char *binary_data, unsigned char length, char *reply) {
	int server, len, offset;

	// unsigned int alloclen = DATASIZE;
	// unsigned int i;
	// unsigned char printbuffer[DATASIZE];
	// unsigned char *dataPtr;


	// send value to remote server
	if ((server = net_connect(device, port)) <= 0) {
		printf("IP_send_string: Could not connect to %s on port %s\n", device,
				port);
		sprintf(reply, "CONN_FAILED"); ///
		return -1;
	}

	if (length > 0)
		write(server, binary_data, length);
	write(server, "\r\n", 2); // line feed / CR telnet emulation

	// Wait for reply
	//len = recv( server, reply, DATASIZE , 0);
	//len = receive_data(server, reply, DATASIZE); // until buffer is out
	offset = 0;
	while ((len = read(server, reply + offset, DATASIZE - 1 - offset))) {
		offset = offset + len;
	}
	len = offset;
	// printf("len = %d", len);
	reply[len+1] = '\0';
	len++;
	//reply[length-1] = 'X';
	net_disconnect(server);

	return (len);
}

int IP_send_String_no_ack(char *device, char *port,
		unsigned char *binary_data, unsigned char length, char *reply) {
	int server;
	unsigned int len = 0;
	// unsigned int alloclen = DATASIZE;
	// unsigned int i;
	// unsigned char printbuffer[DATASIZE];
	// unsigned char *dataPtr;

	// send value to remote server
	if ((server = net_connect(device, port)) <= 0) {
		printf("IPsendcommand: Could not connect to %s on port %s\n", device,
				port);
		sprintf(reply, "CONN_FAILED"); ///
		return -1;
	}

	if (length > 0)
		write(server, binary_data, length);
	write(server, "\r\n", 2);

	// Wait for reply
	//len = read(server, reply, DATASIZE);
	// printf("len = %d", len);
	//reply[len] = '\0';
	//reply[length-1] = 'X';
	net_disconnect(server);

	return (len);
}

int net_connect(char *hostname, char *port) {
	int server, prt;
	// int hostaddr;
	struct hostent *name;
	struct sockaddr_in addr;

	if ((server = socket(AF_INET,SOCK_STREAM, 0)) <= 0)
		return (server);

	if (!(name = gethostbyname(hostname)))
		return (0x00);

	addr.sin_family = AF_INET;
	if (!(prt = strtol(port, NULL, 10)))
		return (prt);
	addr.sin_port = htons(prt);
	//hostaddr= (byte)name->h_addr[3]<<24;
	//hostaddr|= (byte)name->h_addr[2]<<16;
	//hostaddr|= (byte)name->h_addr[1]<<8;
	//hostaddr|= (byte)name->h_addr[0];

	addr.sin_addr.s_addr = inet_addr(hostname);

	//addr.sin_addr.s_addr= hostaddr;

	if (connect(server, (struct sockaddr *) &addr, sizeof(addr)) != 0x00)
		return (-1);

	return (server);
}

int net_disconnect(int fd) {
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return (0);
}
