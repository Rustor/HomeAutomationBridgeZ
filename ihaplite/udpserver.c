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

#include "udpserver.h"


#define MAXBUFLEN 255
#define DATASIZE 255
#define MAX_VPS 2


int UDP_init(int * ports, fd_set *fdset)
{
	int sockfd;
	// struct addrinfo hints, *servinfo;
	// struct addrinfo *p;
	int fd_max, i;
	// int j, udpport;

	struct sockaddr_in s_addr;

	int option = 1;

	FD_ZERO(fdset);
	bzero(&s_addr,sizeof(s_addr));
	s_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	s_addr.sin_family = AF_INET;

	for(i = 0; i < MAX_VPS; i++) {
		s_addr.sin_port = htons(ports[i]);

		if ( (sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
			perror("UDP_listen: socket error\n");
			return -1;
		}

		setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, (void *)&option, sizeof(option));

		 if (bind(sockfd,(struct sockaddr *)&s_addr,sizeof(s_addr)) < 0) {
			perror("UDP_listen: bind\n");
			return -1;
		 }
		 FD_SET(sockfd, fdset);
		 fd_max = sockfd;
	}
	return fd_max;
}


int	UDP_listen(fd_set *fdset, int fd_max, unsigned char* buffer) {
	fd_set fd_currentset;
	int i, k, numbytes = 0, local_port;
	// int j;
	unsigned char length = 0, remote_ip[INET6_ADDRSTRLEN];
	// unsigned char VPS, printbuffer[DATASIZE];
	//struct sockaddr_storage their_addr;

	struct sockaddr_in remote_addr;
	struct sockaddr_in local_addr;
	size_t remote_addr_len;
	size_t local_addr_len;

	remote_addr_len = sizeof remote_addr;
	local_addr_len = sizeof local_addr;

	FD_ZERO(&fd_currentset);
	memcpy(&fd_currentset, fdset, sizeof(fd_currentset));
	//FD_COPY(fdset, &fd_currentset);

	// Find which port is involved
	if ( select(fd_max + 1, &fd_currentset, NULL, NULL, NULL) == -1) {
		perror("UDP_listen: select error\n");
		exit(1);
	}


	for(i = 0; i < fd_max+1; i++) {
		if(FD_ISSET(i, &fd_currentset)) {
			if ((numbytes = recvfrom(i, buffer, MAXBUFLEN-1 , 0,
				(struct sockaddr *)&remote_addr, (socklen_t *)&remote_addr_len)) == -1) {
				perror("UDP_listen: receive error\n");
				exit(1);
			}
			// Null terminate
			buffer[numbytes] = '\0';
			numbytes++;
			for(k=0;length+1>k;k++) {
				buffer[length-k] = buffer[length-k];
			}

			//buffer[1] = length;
			// Get remote address:
			inet_ntop(remote_addr.sin_family, get_in_addr((struct sockaddr *)&remote_addr),(char *)remote_ip, sizeof remote_ip);

			// Get local port:
			getsockname(i,(struct sockaddr *)&local_addr,(socklen_t *)&local_addr_len);
			local_port = get_in_port((struct sockaddr *)&local_addr);


			printf("UDP_listen: Data received from %s on port %i: ", remote_ip, local_port);


			//getsockname(i,&remote_addr,&remote_addr_len);
			//getsockname(i,&their_addr,&addr_len);
			//remote_port = get_in_port((struct sockaddr *)&their_addr);
			//remote_port = get_in_port(&remote_addr);
			//printf("%s\"\n", buffer);

		}
	}
	return numbytes;
}


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

uint16_t get_in_port(struct sockaddr *sa)
{
	//uint16_t *ptr;
	//int port;
	//ptr = &(((struct sockaddr_in*)sa)->sin_port);
	//port = ntohs(*ptr);
	return ntohs((((struct sockaddr_in*)sa)->sin_port));
}

int UDP_broadcast(int port, unsigned char* message, int len) {
    struct sockaddr_in addr;
    int fd;
    char printbuffer[DATASIZE];

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    /* set up destination address */
	int	on = 1;
   	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof on)
         == -1) {
      perror("socket");//syslog(LOG_ERR, "setsockopt SO_BROADCAST: %m");
      exit(1);
   }

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(HELLO_GROUP);
    addr.sin_port=htons(port);

    printf("UDP Broadcast on port %i: [%s]\n", port, (unsigned char*) message);

    if (sendto(fd, message, len , 0,(struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
    	perror("sendto");
    	exit(1);
	}
	close(fd);
	return 0;
}

int	get_ip_UDP_listen(fd_set *fdset, int fd_max, unsigned char* buffer, char * remote_ip) {
	fd_set fd_currentset;
	int i, numbytes = 0, local_port;
	// int j;
	// unsigned char VPS, printbuffer[DATASIZE];
	//unsigned char length = 0;
	//struct sockaddr_storage their_addr;

	struct sockaddr_in remote_addr;
	struct sockaddr_in local_addr;
	size_t remote_addr_len;
	size_t local_addr_len;

	remote_addr_len = sizeof remote_addr;
	local_addr_len = sizeof local_addr;

	FD_ZERO(&fd_currentset);
	memcpy(&fd_currentset, fdset, sizeof(fd_currentset));
	//FD_COPY(fdset, &fd_currentset);

	// Find which port is involved
	if ( select(fd_max + 1, &fd_currentset, NULL, NULL, NULL) == -1) {
		perror("UDP_listen: select error\n");
		exit(1);
	}


	for(i = 0; i < fd_max+1; i++) {
		if(FD_ISSET(i, &fd_currentset)) {
			if ((numbytes = recvfrom(i, buffer, MAXBUFLEN-1 , 0,
				(struct sockaddr *)&remote_addr, (socklen_t *)&remote_addr_len)) == -1) {
				perror("UDP_listen: receive error\n");
				exit(1);
			}
			// Null terminate
			buffer[numbytes] = '\0';
			numbytes++;

			//for(k=0;length+1>k;k++) {
			//	buffer[length-k] = buffer[length-k];
			//}

			//buffer[1] = length;
			// Get remote address:
			inet_ntop(remote_addr.sin_family, get_in_addr((struct sockaddr *)&remote_addr),remote_ip, 20 );//sizeof remote_ip);

			// Get local port:
			getsockname(i,(struct sockaddr *)&local_addr,(socklen_t *)&local_addr_len);
			local_port = get_in_port((struct sockaddr *)(&local_addr));


			printf("UDP_listen: Data received from %s on port %i: ", remote_ip, local_port);


			//getsockname(i,&remote_addr,&remote_addr_len);
			//getsockname(i,&their_addr,&addr_len);
			//remote_port = get_in_port((struct sockaddr *)&their_addr);
			//remote_port = get_in_port(&remote_addr);
			//printf("%s\"\n", buffer);

		}
	}
	return numbytes;
}

int EventListen_local(char * bridgeaddr, char * bridgeport, char *resultbuffer) {
	//unsigned char bridgeaddr[20], gatewayaddr[20], bridgeport[20];
	// function of the code is not cut in stone.
	//unsigned char *dataPtr;
	int ports[MAX_VPS], fd_max, running = 1, len;
	fd_set fdset;
	//ports[1] = 10000;
	//ports[2] = 9009;
	//char * ipaddr_s, *ipport_s;
	char remote_ip[INET6_ADDRSTRLEN];
	//gvps_id_to_ip(device_id, gatewayaddr, bridgeaddr, bridgeport);
	//ipaddr_s = getIPADRformVPS(device_id);
	//ipport_s = getIPPortformVPS(device_id);

	ports[1] = atoi(bridgeport);

	printf("Starting udplisten on %d\n", ports[1]);
	printf("Expecting something from IP: %s\n", bridgeaddr);

	fd_max = UDP_init( ports, &fdset);

	while (running) {
		len = get_ip_UDP_listen(&fdset, fd_max, (unsigned char *) resultbuffer, remote_ip);

		if (!(unsigned char) strcmp(remote_ip, bridgeaddr)) {
			printf("we got %s from right IP: %s %s\n", resultbuffer, remote_ip,
					bridgeport);

			return len;
		} else {
			printf("we got %s from wrong IP: %s %s\n", resultbuffer, remote_ip,
					bridgeport);
		}
	}
	return 0;
}
