
/*
 *
 *      Author: tsh, rto
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

#include "framework.h"
#include "msghandler.h"
#include "auxfunctions.h"

// Experimental move of data structures only related to message handler
extern unsigned char msg_table[][MAX_OUTPUT][2];
extern unsigned char MessageSignal[];
extern struct Context Context_array[];


// Queue structs
unsigned char queue[QUEUESIZE];
struct Context queue_struct[QUEUESIZE];

// Global queue for the message handler
int currentmsg;

char queue_nextmsg(void);
int queue_add(void);
void queue_remove(void);

// Message handler extensions:
// Must take a message and rewrite the address field to be the receiver,
// otherwise the receiver cannot tell what input it was sent to

// Routing table example:
// IPtask1-M1 -> SecTask1-M3
// SecTask1-M1 -> SerialTask1-M1
// etc.

// Message handler reads the first part from address field, writes the second part
// and sends it to the second part.
// The receiver can tell which M-port it has been addressed to when it awakens.

/*
Message handler can go into deadlock if trying to deliver a message to a task
that is trying to deliver a message to the msghandler.
The solution is that msghandler must never sleep/block except when ready to receive.
A queue can be implemented which the msghandler puts every message into.
Everytime it runs, it tries non-blocking to deliver every message in the queue, and
then returns to a pthread_cond_timedwait, then tries to deliver the messages again.
If it is not attempted to empty the entire queue every time, the queue will never shrink.
*/

void* msgHandler(void* _context) {
	struct Context* context = (struct Context*)_context;
	unsigned char sender_taskid, sender_outputid, receiver_taskid;// j;
	int i;
	//printf("msgHandler: context[0]->holder: %s\n", context->holder);

	//printf("MsgHandler: communication_needed %i\n", communication_needed);

	// Initialize queue
		for(i=0; i < QUEUESIZE; i++) {
			queue[i] = QUEUE_FREE;
		}


	pthread_mutex_lock(context->mutex);
	// Go into infinite receive message loop
	while(communication_needed){
		// Wait for condition when a new message is ready
#ifdef _DEBUG_VERBOSE
		puts("msgHandler: waiting for new message...");
#endif
		pthread_cond_wait(context->condition, context->mutex);
#ifdef _DEBUG_VERBOSE
		puts("msgHandler: msgCondition signal received, msgMutex locked, setting occupied");
#endif


		/* Multicast/(Broadcast)
		 * In case more tasks need to be released on a single message, a multicast is used.
		 * Several receivers need to be defined in the message (perhaps in a new field in the context struct?)
		 * When such a message reaches this point, it is simply duplicated and put into the queue
		 * several times each with a different recipient. The messages can then be treated as normal
		 * separate messages and delivered when the recipient is ready.
		 */
		if(alarmTestAndReset(max_tasks-1)) { // if alarm occ. then skip queue add
			puts("msgHandler: Alarm signal received skipping queue add");
		}
		else {
			if(queue_add()  ) {
			puts("msgHandler: PANIC! Queue full");
			}
		}
		while( queue_nextmsg() ) {
#ifdef _DEBUG_VERBOSE
			char printbufferMH[DATASIZE];
			printf("msgHandler: Sender is: [%s]\n", binary_to_hexbinaryString(context->sender,SENDERNAMESIZE,printbufferMH));
			printf("msgHandler: Data is: [%s]\n", binary_to_hexbinaryString(context->holder,context->holder[1]+2,printbufferMH));
#endif

			// Convert sender info to int
			sender_taskid = context->sender[TaskID];
			sender_outputid = context->sender[OutputID];

			// Test of Context array
			//strcpy(SerialReceiver_context->sender,"973");
			//printf("msgHandler: Context-Sender is: [%s]\n", SerialReceiver_context->sender);
			//printf("msgHandler: Context-Sender is: [%s]\n", Context_table[SerialReceiverTask_id]->sender);


			/*
			printf("msgHandler: msg_table output[%i][%i][]: %s\n",
								sender_taskid,
								sender_outputid,
								&msg_table[sender_taskid] [sender_outputid-1] [0]);
			*/

			/*
			printf("msgHandler: msg_table output[%i][%i][0]: %s\n",
					context->sender[TaskID],
					context->sender[OutputID],
					&msg_table[context->sender[TaskID]] [context->sender[TaskID]] [0]);
					*/


					//printf("msgHandler: msg_table output 2: [%s]", &msg_table[context->sender[TaskID]] [context->sender[InputID]][0]);


			/*for(i = 0;i<6;i++) {
				for(j = 0;j<4;j++) {
					///for(j = 0;j<3;j++) {
					///	printf("msgTable: %i\n", msg_table[i][k][j]);
					//}
					printf("msgTable: %i %i %i\n", msg_table[i][j][0], msg_table[i][j][1], msg_table[i][j][2]);
				}
			}*/

			/*for(i = 0;i<6;i++) {
				for(j = 0;j<4;j++) {
					printf("msgTable: %s\n", &msg_table[i][j][0]);
				}
			}*/


			//printf("msgHandler DEBUG: msg_table [%i]\n", msg_table[sender_taskid] [sender_outputid-1] [TaskID]);
			//printf("msgHandler DEBUG: msg_table [%i]\n", msg_table[sender_taskid] [sender_outputid-1] [InputID]);
			receiver_taskid = msg_table[sender_taskid] [sender_outputid-1] [TaskID];
			//printf("msgHandler: receiver_taskid %i\n", receiver_taskid);
			context->sender[TaskID]  = msg_table[sender_taskid] [sender_outputid-1] [TaskID];
			context->sender[InputID] = msg_table[sender_taskid] [sender_outputid-1] [InputID];
			context->sender[OutputID] = UNUSED;
#ifdef _DEBUG_VERBOSE
			printf("msgHandler: Receiver is [%s]\n", binary_to_hexbinaryString(context->sender,SENDERNAMESIZE,printbufferMH));
#endif
			//printf("msgHandler: Receiver is int:[%i]\n", receiver_taskid);

			if(!pthread_mutex_trylock(Context_array[receiver_taskid].mutex)) {
			//printf("msgHandler: context[0]->holder: %s\n", Context_array[0].holder);
			//printf("msgHandler: trylock error %s\n", strerror(pthread_mutex_trylock(Context_array[0].mutex)));
#ifdef _DEBUG_VERBOSE
				printf("msgHandler: locked Context_table[%i]->mutex\n", receiver_taskid);
#endif
				Context_array[receiver_taskid].holder = context->holder;
				//strcpy(Context_array[receiver_taskid].sender,context->sender);
				memcpy(Context_array[receiver_taskid].sender,context->sender, SENDERNAMESIZE);

				// Set message signal
				msgSignalAndRelease(context->sender[TaskID]);
				// print MessageSignal
#ifdef _DEBUG_VERBOSE
				unsigned char j;
				printf("MessageSignal: ");
				for(j=0;j < max_tasks;j++){
					printf("%i ", MessageSignal[j]);
				}
				printf("\n");
#endif

				queue_remove();
#ifdef _DEBUG_VERBOSE
				printf("msgHandler: sending Context_table[%i]->signal\n", receiver_taskid);
#endif
				pthread_cond_signal(Context_array[receiver_taskid].condition);
#ifdef _DEBUG_VERBOSE
				printf("msgHandler: unlocking Context_table[%i]->-mutex\n", receiver_taskid);
#endif
				pthread_mutex_unlock(Context_array[receiver_taskid].mutex);
			}else {
				printf("msgHandler: Could not acquire lock! Leaving message in queue[%i]       ** \n", currentmsg);
				printf("msgHandler: Requesting alarm in 200 ms, alarm id: %d \n", max_tasks-1);
				alarmSet(max_tasks-1, 2);
			}
#ifdef _DEBUG_VERBOSE
			printf("msgHandler: end of msg while\n");
#endif
		} // while-end queue_nextmsg
		//printf("msgHandler: setting Occupied to 0\n");
		context->occupied = 0;
	} // while-end communication_needed
	pthread_mutex_unlock(context->mutex);
	puts("msgHandler: Stopped");
	return NULL;
}

char queue_nextmsg(void) {
	currentmsg++;
	//printf("queue_nextmsg: currentmsg is %i\n", currentmsg);
	while(currentmsg < QUEUESIZE) {
		if(queue[currentmsg] == QUEUE_FREE) {
#ifdef _DEBUG_VERBOSE
			printf("queue_nextmsg: queue[%i] free\n", currentmsg);
#endif
			currentmsg++;
		}else {
			//copy queue[currentmsg] to context
			memcpy(&msg_context,&queue_struct[currentmsg],sizeof(msg_context));
#ifdef _DEBUG_VERBOSE
			printf("queue_nextmsg: copying queue[%i] to context\n", currentmsg);
#endif
			return TRUE;
		}
		//currentmsg++;
	}
	// Set to -1, otherwise the function skips msg 0 on the first run
	currentmsg = -1;
#ifdef _DEBUG_VERBOSE
	printf("queue_nextmsg: setting currentmsg to %i\n", currentmsg);
#endif
	return FALSE;
}

int queue_add(void) {
	//find next NULL in queue
	int counter;
	for(counter = 0; counter < QUEUESIZE; counter++) {
		if(queue[counter] == QUEUE_FREE) {
			// copy context into queue
#ifdef _DEBUG_VERBOSE
			puts("queue_add: found a free space");
#endif
			memcpy(&queue_struct[counter],&msg_context,sizeof(msg_context));
#ifdef _DEBUG_VERBOSE
			printf("queue_add: copying context into queue[%i]\n", counter);
#endif
			queue[counter] = QUEUE_OCCUPIED;
			return 0;
		}
	}
	puts("Queue_add ERROR: Could not add new element into queue.");
	return EOVERFLOW;
}

void queue_remove(void) {
	queue[currentmsg] = QUEUE_FREE;
#ifdef _DEBUG_VERBOSE
	printf("queue_remove: removing queue[%i]\n", currentmsg);
#endif
}


// Sends a message from thread to the message handler
void* broadcast_msg(unsigned char _sender[], const unsigned char outport, unsigned char* _dataPtr){
//Broadcast pseudo-code:
//	lock
//	while(msg_context->occupied){
//		unlock
//		allow msgHandler to take mutex
//		lock
//	}
//	broadcast
//	unlock
	char printbuffer[DATASIZE];

	_sender[InputID] = UNUSED;
	_sender[OutputID] = outport;
#ifdef _DEBUG_VERBOSE
	printf("broadcast [%s]: locking msg_context.mutex\n", binary_to_hexbinaryString(_sender,SENDERNAMESIZE,printbuffer));
#endif
	pthread_mutex_lock(msg_context.mutex);

	while(msg_context.occupied){
		printf("*** broadcast [%s]: context is occupied!\n", binary_to_hexbinaryString(_sender,SENDERNAMESIZE,printbuffer));
		pthread_mutex_unlock(msg_context.mutex);
		//sleep(0.5);
		pthread_yield();
		printf("broadcast [%s]: relocking msg_context.mutex\n", binary_to_hexbinaryString(_sender,SENDERNAMESIZE,printbuffer));
		pthread_mutex_lock(msg_context.mutex);
	}
#ifdef _DEBUG_VERBOSE
	printf("broadcast [%s]: writing data to msg_context\n", binary_to_hexbinaryString(_sender,SENDERNAMESIZE,printbuffer));
#endif
	msg_context.holder = _dataPtr;
	//strcpy(msg_context.sender, _sender);

	memcpy(msg_context.sender, _sender, SENDERNAMESIZE);
	msg_context.occupied = 1;

	// Signal msgHandler that new data is ready
	pthread_cond_signal(msg_context.condition);
#ifdef _DEBUG_VERBOSE
	printf("broadcast [%s]: signal msgHandler\n", binary_to_hexbinaryString(_sender,SENDERNAMESIZE,printbuffer));
#endif

	// unlock mutex
#ifdef _DEBUG_VERBOSE
	printf("broadcast [%s]: unlocking msgMutex and stopping\n", binary_to_hexbinaryString(_sender,SENDERNAMESIZE,printbuffer));
#endif
	pthread_mutex_unlock(msg_context.mutex);

	return NULL;
}

// Block and prepare to receive next message
char receive(int Task_id, pthread_cond_t* _condition, pthread_mutex_t* _mutex){
#ifdef _DEBUG_VERBOSE
	printf("Receive: locking Task[%i]'s context->mutex\n", Task_id);
#endif
	pthread_cond_wait(_condition, _mutex);
#ifdef _DEBUG_VERBOSE
	printf("Receive: Task[%i] has been signalled, returning\n", Task_id);
#endif
	return msgTestAndReset(Task_id);
}

char msgTestAndReset(int Task_id) {
	if(MessageSignal[Task_id] == 1) {
		MessageSignal[Task_id] = 0;
		//puts("msgTestAndReset has returned TRUE");
		return TRUE;
	} else {
		//puts("msgTestAndReset has returned FALSE");
		return FALSE;
	}
}

void* msgSignalAndRelease(int Task_id) {
	MessageSignal[Task_id] = 1;
	//printf("msgSignalAndRelease: setting Task[%i]\n", Task_id);
	return NULL;
}


void insert_msg_table_entry(unsigned char Sender_taskid, unsigned char Sender_output,
		unsigned char Receiver_taskid, unsigned char Receiver_input) {
	msg_table[Sender_taskid][Sender_output-1][0] = Receiver_taskid;
	msg_table[Sender_taskid][Sender_output-1][1] = Receiver_input;
}

