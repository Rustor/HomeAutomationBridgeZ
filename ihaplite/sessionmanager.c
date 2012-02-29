/*
 * sessionmanager.c
 *
 *  Created on: Mar 5, 2010
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
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
//#include "defs.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "sessionmanager.h"
#include "timemanager.h"
#include "framework.h"
#include "msghandler.h"
#include "udpserver.h"
#include "auxfunctions.h"


void* SessionManagerTask(void* _context){
// Session Manager
// Has its own global Mutex and signal.
// Receive messages from IP tasks or Serial task, decision based on address field in message
// IP task messages are forwarded to serial
// Read VPS on Serial messages, forward to the correct IP task
	struct Context* context = (struct Context*)_context;
	int VPS, i, udp_port = UDPPORTBASE;
	unsigned char output_port, msgFlag;
	unsigned char name_id[SENDERNAMESIZE] = {context->task_id,UNUSED,UNUSED};
	unsigned int * session_table;
	//unsigned int session_table[6]; // runes test
	unsigned char* dataPtr, broadcastbuffer[256], len;
	char printbuffer[DATASIZE];

	session_table = (unsigned int *)calloc(MAX_VPS, sizeof(session_table) );

//sleep(1);

/*	// DEBUG
	if(pthread_mutex_trylock(Context_array[0].mutex)) {
		puts("SM: Could not lock SM mutex");
	} else {
		puts("SM: Locked SM mutex");
		printf("SM: %s\n", strerror(pthread_mutex_unlock(Context_array[0].mutex)));
	}
	if(pthread_mutex_trylock(context->mutex)) {
			puts("SM: Could not lock SM mutex");
		} else {
			puts("SM: Locked SM mutex");
			printf("SM: %s\n", strerror(pthread_mutex_unlock(context->mutex)));
		}
	// END DEBUG
*/
	memset(session_table, 0, MAX_VPS);

	pthread_mutex_lock(context->mutex);

	// Go into infinite receive message loop
	while(communication_needed){
		// Wait for condition when a new message is ready
#ifdef _DEBUG_VERBOSE
		puts("SessionManager: calling receive()");
#endif

		msgFlag = receive(context->task_id, context->condition, context->mutex);
		dataPtr = context->holder;
		//pthread_cond_wait(context->condition, context->mutex);
//sleep(1); // DEBUG

		//printf("SessionManager: msgTestAndReset returns: %i\n", msgTestAndReset(SessionManagerTask_id));
		//testvar = msgTestAndReset(SessionManagerTask_id)

		// Check if we have received a message
		if( msgFlag == TRUE ) {
			// We have received a message
#ifdef _DEBUG_VERBOSE
			printf("SessionManager: Receiving data: [");
			printf("%s]\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
			printf("SessionManager: received from sender: [%s]\n", binary_to_hexbinaryString(context->sender,SENDERNAMESIZE,printbuffer));
#endif

			// Perform session stuff and send message
			// Extract VPS from data string
			VPS = dataPtr[0];
			// Output is VPS+2 (because Out1 is occupied and Out2 is for VPS0)
			output_port = VPS+2;
#ifdef _DEBUG_VERBOSE
			printf("SessionManager: VPS found [%i]\n", VPS);
#endif
			// Switch depending on the Input port of the task
			switch(context->sender[InputID]){
				case In1:		// In1->Out1
					// Message came FROM one of the many components connected

					// Sanity check for invalid VPS
					if(VPS > MAX_VPS) {
						printf("SessionManager ERROR: VPS in input exceeds defined VPS\n");
						free(dataPtr);
						break;
					}
#ifdef _DEBUG_VERBOSE
					puts("SessionManager: Input1->Output1 case");
#endif

					// Read VPS and create entry in Session_table with timeout
					session_table[VPS] = SESSION_TIMEOUT;
					// Set alarm so sessions can be decremented each second
					printf("%s\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
					if ( strncmp(dataPtr+2, "0TIME", 5) != 0 )	{alarmSet(context->task_id, 10);

					}
					else {alarmSet(context->task_id, 3000);
						printf("detected 0TIME keyword\n");len=dataPtr[1]-5;dataPtr=dataPtr+5;dataPtr[0]=VPS;dataPtr[1]=len; printf("%s\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));}

					// Broadcast message
#ifdef _DEBUG_VERBOSE
					puts("broadcast: SessionManager");
#endif
					broadcast_msg(name_id, Out1, dataPtr);
					break;

				case In2:		// In2->Out2
					// Message is going TO one of the many components connected
					// Multiplex according to the message's VPS

					// Sanity check for invalid VPS
					if(VPS > MAX_VPS) {
						printf("SessionManager ERROR: VPS in input exceeds defined VPS\n");
						free(dataPtr);
						break;
					}
					// Read VPS entry in Session_table
					if(session_table[VPS]) {
						// Entry exists, send msg via appropriate Output

						// DEBUG: Perform some operation on data
						//strcat(dataPtr, "SM");
						//dataPtr[1] = dataPtr[1]+2;
						//printf("SessionManager: after strcat: [%s]\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
						// END DEBUG
#ifdef _DEBUG_VERBOSE
						printf("SessionManager: ->Output%i case\n", output_port);
#endif

						// Clear session entry
						session_table[VPS] = 0;
#ifdef _DEBUG_VERBOSE
						puts("broadcast: SessionManager");
#endif
						broadcast_msg(name_id, output_port, dataPtr);
					} else {
						// Entry does not exist, broadcast message on UDP
						printf("SessionManager: No session entry found, broadcasting on UDP port %i\n", udp_port+VPS);
						// dataPtr is +1 to avoid printing the VPS to IP
						// TODO convert to hexbin before broadcasting
						len = binary_to_hexbinary( dataPtr+2, broadcastbuffer ,  dataPtr[1]);
						UDP_broadcast(udp_port+VPS, broadcastbuffer, len );
					}
					break;
				default:
					puts("SessionManager switchcase FAIL!");
			} // end switch
		} // end if-msgTestAndReset

		if(alarmTestAndReset(context->task_id) == 1) {
			// An alarm has occurred
#ifdef _DEBUG_VERBOSE
			printf("SessionManager Alarm: has been signalled\n");
#endif
			// Decrement all positive-value timeouts
			// Decrement all non-zero timeouts. All zero-value entries are closed sessions

#ifdef _DEBUG_VERBOSE
			// DEBUG session_table
			int j;
			printf("SessionManager Alarm: session_table[ ");
			for(j=0;j<MAX_VPS;j++) {
				printf("%i ", session_table[j]);
			}
			printf("]\n");
			// END DEBUG session_table
#endif

			for(i=0;i<MAX_VPS;i++) {
				// Any entry that is 0 has no running session
				if(session_table[i] == 0) {
					// Do nothing, entry has no active session
				} else {
					if(session_table[i] <= 1) {
						// If the entry is about to be counted down to 0 the session has expired
#ifdef _DEBUG_VERBOSE
						printf("SessionManager: Session[%i] is expiring now\n", i);
#endif
						// Notify the waiting task about the timeout
						// Notify the appropriate VPS/IP-task

						printf("SessionManager: Notify VPS[%i] of timeout\n", i);
						//sprintf(dataPtr, "%iSessionManager: Your session has timed out!\n", i);

						// Set output port
						output_port = i+2;

						// This is probably a memory leak, unless it gets freed in the other end
						dataPtr = (unsigned char *)calloc(1, DATASIZE);
						// Return timeout message to receiver
						sprintf((char*)&dataPtr[2], "TIMEOUT_SEND");
						dataPtr[1] = strlen((char*)&dataPtr[2]);
						dataPtr[0] = i;
#ifdef _DEBUG_VERBOSE

						printf("SessionManager: Data is: [%s]\n", binary_to_hexbinaryString(dataPtr,dataPtr[1]+2,printbuffer));
#endif
						//dataPtr[0] = i;
						broadcast_msg(name_id, output_port, dataPtr);

						// Clear table entry
						session_table[i] = 0;
					} else {
						//printf("SessionManager: Counting down Session[%i] to value %i\n", i, session_table[i]);
						// Timeout should be counted down
						session_table[i]--;
						// Set the alarm so that we are notified again in 1 sec
						alarmSet(context->task_id, 10);
					}
				}
			}
		} // end if-alarmTestAndReset
	} // end while-communication_needed
	pthread_mutex_unlock(context->mutex);
	puts("SessionManager: Stopped");
	return NULL;
}


