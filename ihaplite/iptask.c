/*
 * iptask.c
 *
 *  Created on: Jul 21, 2009
 *      Author: tsh,rto
 *
 *      This program is free software: you can redistribute it and/or modify
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

#include "tcpserver.h"
#include "framework.h"
#include "msghandler.h"
#include "auxfunctions.h"

#define PRINT_STATUS 1

// Receives from another IP server and sends reply back
void* IPTask(void* _IP_data_task) {
	struct IP_data* IP_data_task = (struct IP_data*) _IP_data_task;
	struct Context* context = (struct Context*) IP_data_task->ptr_context;
	unsigned char VPS = IP_data_task->VPS, *dataPtr, msgFlag;
	char printbuffer[DATASIZE];

	///////////////////////////////////////
	// task_id needs to be made generic! //
	//name_id[SENDERNAMESIZE] = {IPTask1_id,UNUSED,UNUSED};
	///////////////////////////////////////
	unsigned char name_id[SENDERNAMESIZE] = { context->task_id, UNUSED,UNUSED };

	int server_port = IP_data_task->port;

	printf("IPTask%i: Started\n", VPS);

	// Initialize socket and port
	int s, client;
	unsigned int tmp;
	fd_set fdset;
	struct timeval timer;
	struct sockaddr_in c_addr;
	//unsigned char *pointer;

	if ((s = server_init(server_port)) < 0)
		//return(s);

		FD_ZERO(&fdset);
	FD_SET(s, &fdset);
	timer.tv_sec = 30;
	timer.tv_usec = 0;

	pthread_mutex_lock(context->mutex);
	while (communication_needed) {
		//Wait for IP packet
		if (select(s + 1, &fdset, NULL, NULL, NULL)> 0 ) {
			//#ifdef _DEBUG_VERBOSE
				printf("IPTask%i: SELECT has continued\n",VPS);
				//#endif
				tmp = sizeof(c_addr);
				client = accept(s, (struct sockaddr *)&c_addr, &tmp);
				if (client < 0) {
					printf("FAIL1: %s\n", strerror(errno));
				}

				if (PRINT_STATUS) {
					printf("IPTask%i: client connect from %s\n",VPS,
					inet_ntoa(c_addr.sin_addr));
				}
				usleep(10000);
				if ( (dataPtr = receive_cmd(client, VPS)) != NULL )
				{
					//#ifdef _DEBUG_VERBOSE
				printf("IPTask%i: Got [%s] from receive_cmd (ASCII)\n", VPS,binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
//#endif

		dataPtr[1] = strlen((char*)dataPtr+2); // TODO remove warning
		// Convert hexbinary to binary if first char is a hexbin char else skip conv
		if( ((0x30 <= dataPtr[2]) &&  (dataPtr[2] <= 0x39)) || (('a' <= dataPtr[2] ) && (dataPtr[2]  <= 'f')) || (('A' <= dataPtr[2]) && (dataPtr[2] <= 'F')))
						{ dataPtr[1] = hexbinary_to_binary( &dataPtr[2], &dataPtr[2]);}

		printf("IPTask%i: Receiving from port %i: [",VPS, server_port);
		printf("%s]\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
		//printf("IPTask%i: Has data after strcat: [%s]\n",VPS,binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));

		// Pass message on
//#ifdef _DEBUG_VERBOSE
		printf("broadcast: IPTask%i\n",VPS);
//#endif
		broadcast_msg(name_id, Out1, dataPtr);

		// DEBUG: Must wait for receiver to pass data on
		//sleep(1.5);


		// Begin listening on Condition for reply
//#ifdef _DEBUG_VERBOSE
		printf("IPTask%i: calling receive()\n",VPS);
//#endif
		//msgFlag = receive(IPTask1_id, context->condition, context->mutex);
		msgFlag = receive(VPS, context->condition, context->mutex);

		dataPtr = context->holder;
		printf("  *** IPTask%i: Sending on port %i: [%s] ***\n", VPS, server_port, binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
//#ifdef _DEBUG_VERBOSE
		printf("IPTask%i: received from sender: [%s]\n", VPS, binary_to_hexbinaryString(context->sender,SENDERNAMESIZE,printbuffer));
//#endif


		// Send reply back thru open connection
		// dataPtr+2 to avoid sending the VPS and length

		//if(server_port==MANAGEMENTPORT) IP_send_server_String(client, dataPtr+2, dataPtr[1]);
		//else IP_send_server_hexbinaryString(client, dataPtr+2, dataPtr[1]);
		IP_send_server_hexbinaryString(client, dataPtr+2, dataPtr[1]);
		printf("IPTask%i: ip send server func: \n", VPS);
		}
		else printf("IPTask%i: no data in client \n", VPS);
		// Shutdown connection
		if ((shutdown(client, 0) < 0) || (close(client) < 0)) {
			printf("FAIL2: %s\n", strerror(errno));
		}
	 } // end select
			} // while-end

			// Shutdown listening server
			if (s> 0)
			server_end(s);
			printf("IPTask%i: unlocking mutex and stopping\n",VPS);
			pthread_mutex_unlock(context->mutex);
			return NULL;
		}
