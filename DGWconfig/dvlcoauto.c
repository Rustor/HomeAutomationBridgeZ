/*
 * Author: rto
 *
 * 	This program is free software: you can redistribute it and/or modify
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
//#define HELLO_GROUP "255.255.255.255"
#define MAXBUF		1024
#define	MY_PORT		9998
#define PEER_PORT	9999
#define MAXIP		16		/* XXX.XXX.XXX.XXX */
//#define LOCALADDR "192.168.1.143:60001"
//#define LOCALADDR ":60001"
#define configmsg "010348C07EF506000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003139322E3136382E312E3500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049EE020000"

char buffer[MAXBUF];
int SetAddress(char* Composite, struct sockaddr_in *Addr) {
	int i;
	char IPAddress[MAXIP];

	bzero(Addr, sizeof(*Addr));
	Addr->sin_family = AF_INET;
	for (i = 0; Composite[i] != ':' && Composite[i] != 0 && i < MAXIP; i++)
		IPAddress[i] = Composite[i];
	IPAddress[i] = 0;
	if (Composite[i] == ':')
		Addr->sin_port = htons(atoi(Composite + i + 1));
	else
		Addr->sin_port = 0;
	if (*IPAddress == 0) {
		Addr->sin_addr.s_addr = INADDR_ANY;
		return 0;
	} else
		return (inet_aton(IPAddress, &Addr->sin_addr) == 0);
}

int UDP_broadcast(char *gwaddr, int port, char* message, int len) {
	struct sockaddr_in addr;
	int fd;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/*	if ( SetAddress( LOCALADDR, &addr) != 0 )
	 {
	 //perror(Strings[1]);
	 exit(errno);

	 }
	 */
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(atoi("60001"));
	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		perror("Bind");
		exit(errno);
	}

	/* set up destination address */
	int on = 0;
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof on) == -1) {

		perror("socket");//syslog(LOG_ERR, "setsockopt SO_BROADCAST: %m");
		exit(1);
	}

	/* set up destination address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = inet_addr(HELLO_GROUP);
	addr.sin_addr.s_addr = inet_addr(gwaddr);
	addr.sin_port = htons(port);

	if (sendto(fd, message, len, 0, (struct sockaddr *) &addr, sizeof(addr))
			< 0) {
		perror("sendto");
		exit(1);
	}
	return 0;
}

const unsigned char* binary_to_hexbinaryString(unsigned char *input,
		unsigned int buffer_length, unsigned char *printbuffer) {
	unsigned int i;
	for (i = 0; i < buffer_length; i++) {
		sprintf(&printbuffer[i * 2], "%02X", input[i]);
	}
	return printbuffer;
}
unsigned char hexbinary_to_binary(unsigned char *input, unsigned char *output) {
	unsigned char value = 0, length, j;

	length = strlen(input) / 2;
	for (j = 0; j < length; j++) {
		value = 0;
		// Convert 2 hexascii to 1 char
		if (*input >= '0' && *input <= '9')
			value = (value << 4) + (*input - '0');
		else if (*input >= 'A' && *input <= 'F')
			value = (value << 4) + (*input - 'A' + 10);
		else if (*input >= 'a' && *input <= 'f')
			value = (value << 4) + (*input - 'a' + 10);
		input++;
		if (*input >= '0' && *input <= '9')
			value = (value << 4) + (*input - '0');
		else if (*input >= 'A' && *input <= 'F')
			value = (value << 4) + (*input - 'A' + 10);
		else if (*input >= 'a' && *input <= 'f')
			value = (value << 4) + (*input - 'a' + 10);
		input++;
		output[j] = value;
	}
	output[j] = 0; // Null terminate
	return length;
}

int main(int argc, char **argv) {
	unsigned char message[1000], length;
	if (!memcmp(argv[1], "--help", 6)) {
		puts("Broadcast to   : udpsender <port> <ASCII message>");

	} else {

		length = hexbinary_to_binary(configmsg, message);
		memcpy(message + 0x47, argv[2], strlen(argv[2]));

		UDP_broadcast(argv[3], atoi(argv[1]), message, length);
	}
}
