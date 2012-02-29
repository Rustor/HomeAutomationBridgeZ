/*
 * ipstreamtasks.h
 *
 *  Created on: Mar 5, 2010
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

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "framework.h"
#include "msghandler.h"
#include "ipstreamtasks.h"
//#include "serialserver.h"
#include "tcpserver.h"
#include "auxfunctions.h"


int sslegacymode=1;


void* IpStreamReceiverTask(void* _context){
	struct Context* context = (struct Context*)_context;
	unsigned char *dataPtr, name_id[SENDERNAMESIZE] = {context->task_id,UNUSED,UNUSED};
	unsigned char *buffer;
	char printbuffer[DATASIZE];
	int len;


	puts("SReceiverTask: Started");

	while(communication_needed) {
		buffer = (unsigned char *)calloc(1, DATASIZE);
		dataPtr = buffer;

		buffer[0]=0xFF;
		if(sslegacymode) {len  = use_ipstream_server( "RESV" , buffer+2 , 1);
						buffer[1] = len;
						if (len < 0) break;
						}
		else 				 len = use_ipstream_server( "RESV" , buffer+2 , 1);



		printf("SReceiverTask: data from serial port [%s]\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));

		printf("SReceiverTask: Sender: [%s]\n", binary_to_hexbinaryString(name_id,SENDERNAMESIZE,printbuffer));


		puts("broadcast: SReceiverTask");
		broadcast_msg(name_id, Out1, dataPtr);

	}

    return NULL;
}


void* IpStreamSenderTask(void* _context){

	struct Context* context = (struct Context*)_context;
	unsigned char *dataPtr;
	unsigned char msgFlag;
	char printbuffer[DATASIZE];
	int len;
	puts("SSenderTask: Started");
	pthread_mutex_lock(context->mutex);

	while(communication_needed) {
		puts("SSenderTask: calling receive()");
		msgFlag = receive(context->task_id, context->condition, context->mutex);

		printf("  *** SerialSenderTask: Receiving data: [");
		dataPtr = context->holder;
		printf("%s] ***\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
		printf("SSenderTask: received from sender: [%s]\n", binary_to_hexbinaryString(context->sender,SENDERNAMESIZE,printbuffer));

		if( dataPtr[0] == 0xff)  {sslegacymode=1;  len = use_ipstream_server( "SEND" , dataPtr+2 , dataPtr[1]);}
		if( dataPtr[0] != 0xff) {sslegacymode=0;  len = use_ipstream_server( "SEND" , dataPtr , dataPtr[1]+2);}
		free(dataPtr);
	}
	puts("SSenderTask: unlocking mutex and stopping");
	pthread_mutex_unlock(context->mutex);
	return NULL;
}

static int ss;
static int serverready=0;
static int zconnected=0;
static int  client;

int use_ipstream_server(char *direction, unsigned char *message, int len)
{
		fd_set fdset;
        struct timeval timer;
        int opt = 1;
        struct sockaddr_in c_addr;
        int tmp, senddata, resvdata;

        if ( ! (unsigned char) memcmp( "SEND", direction, 4) )
        			{
					senddata=1;
					resvdata=0;
        			}
        if ( ! (unsigned char) memcmp( "RESV", direction, 4) )
                			{
        					senddata=0;
        					resvdata=1;
                			}

        if(serverready == 0)
			{
        	if ( (ss = server_init(SERVER_PORT_ZIGBEE)) < 0) return(ss);
        	setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
        	serverready=1;
			}

        if (zconnected)
			{
        	if(senddata) len = send_stream_data(client, message, len);
			if(resvdata) len = receive_stream_data(client, message, len);
			if(len==0) {
					zconnected = 0;
					if ((shutdown(client, 0) < 0) || (close(client) < 0)) {
			        printf("%s\n", strerror(errno));

					}
				}
			return len;
			}


        FD_ZERO(&fdset);
			FD_SET(ss, &fdset);
			timer.tv_sec= 3;
			timer.tv_usec= 200000;
			if ( select(ss+1, &fdset, NULL, NULL, &timer) > 0 || serverready)
				{

        				tmp = sizeof(c_addr);
        				client = accept(ss, (struct sockaddr *) &c_addr, &tmp);
        				if (client < 0)
							{
        					printf("%s\n", strerror(errno));
        					return(-1);
							}

        				if (PRINT_STATUS) printf("stream connect from %s\n", inet_ntoa(c_addr.sin_addr));

        				zconnected=1;

        				if(senddata) len = send_stream_data(client, message, len);
        				if(resvdata) len = receive_stream_data(client, message, len);

        			}

return len;
}

int receive_stream_data(int client, unsigned char * message, int len)
{
	int connected = 1, cmd_len = 0, message_detected=0, result;
	fd_set fdset;
	struct timeval timer;
	char *cmd;
	cmd = (char *)calloc(1, 1000);

	while (connected) {
		FD_ZERO(&fdset);
		FD_SET(client, &fdset);
		timer.tv_sec= 0;
		timer.tv_usec= 250;
		if ( select(client+1, &fdset, NULL, NULL, &timer) > 0 ) {
			result = read(client, &cmd[cmd_len], 1);
			cmd_len += 1;
			if (cmd_len == 254) {
				printf("receive buffer exceeded\n");
				connected = 0;
			}
			if (result==0) return 0;
			message_detected=1;

			} else  {


			if (message_detected) connected =0;
			usleep(10000);

			}
		}

	memcpy(message, cmd, cmd_len);
	free(cmd);
	return(cmd_len);
}

int send_stream_data(int client, unsigned char * message, int len)
{

	if ( len > 0 )
				write(client, message, len);

return 1;
}

