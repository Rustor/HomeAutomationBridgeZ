/*
 * zigbee-bridge.c
 *
 *  Created on: Mar 5, 2010
 *      Author: tsh,rto
 *
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
#include <pthread.h>
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

//#include "BDremote_application.h"
//#include "ipclient.h"

#include "ipstreamtasks.h"
#include "sessionmanager.h"
#include "zigbee.h"
#include "timemanager.h"
#include "iptask.h"

#include "zigbee.h"


#define IpStreamReceiverTask_id 0
#define IpStreamSenderTask_id 1
#define SessionManagerTask_id 2
#define SecurityTask_id 3
#define ApplicationSupportLayerTask_id 4
#define IPClientTask_id 5
#define ApplicationTask_id 6

#define USER_TASKS 9
#define TOTAL_TASKS (USER_TASKS + TOTAL_VPS) +1


#define TOTAL_VPS 6


unsigned char max_tasks;
unsigned char max_vps;


struct Context Context_array[TOTAL_TASKS+1];

struct IP_data  IP_data_array[TOTAL_VPS];

unsigned char AlarmSignal[TOTAL_TASKS];

unsigned int AlarmCountdown[TOTAL_TASKS];

unsigned char MessageSignal[TOTAL_TASKS];

unsigned char msg_table[TOTAL_TASKS][MAX_OUTPUT][2];

pthread_t thread_array[TOTAL_TASKS];


unsigned char binarytest[10] = {1,2,3,4,5,6,7,8,9,0};
extern int currentmsg;
int main(){
	int j;

	communication_needed = 1;
	currentmsg = -1;

	setlinebuf(stdout);

	max_tasks = TOTAL_TASKS;
	max_vps = TOTAL_VPS;

	msg_context.condition = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	msg_context.mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(msg_context.mutex, NULL);
	pthread_cond_init(msg_context.condition, NULL);
	pthread_t msg_thread;

	pthread_t time_thread;
	pthread_create(&time_thread, NULL, &TimeManagerTask, NULL);

	puts("creating msg_thread");
	msg_context.occupied = 0;
	pthread_create(&msg_thread, NULL, msgHandler, (void*)&msg_context);

	sleep(0.2);

	for(j=0;j<TOTAL_TASKS;j++) {
		Context_array[j].condition = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
		Context_array[j].mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(Context_array[j].mutex, NULL);		// Create mutex for the shared structs
		pthread_cond_init(Context_array[j].condition, NULL);	// Create conditions for the shared structs
		Context_array[j].task_id = j;


	}
	Context_array[TOTAL_TASKS-1].condition = msg_context.condition ; // store the msg cond. in the contekst array

	//////////////////////////////
	// Populate IP_data structs //
	//////////////////////////////
	for(j=0;j < TOTAL_VPS;j++) {
		IP_data_array[j].ptr_context = &Context_array[j+USER_TASKS];
		IP_data_array[j].port = SERVER_PORT+j;
		IP_data_array[j].VPS = j;
		printf("creating IPTask_thread %i\n",j);
		pthread_create(&thread_array[j+USER_TASKS], NULL, IPTask, (void*)&IP_data_array[j]);
	}

	/////////////////////////////////////////////////////////////////
	// Populate msg table with user-defined softwiring connections //
	insert_msg_table_entry(IPTask0_id,Out1,SessionManagerTask_id,In1);
		insert_msg_table_entry(IPTask1_id,Out1,SessionManagerTask_id,In1);
		insert_msg_table_entry(IPTask2_id,Out1,SessionManagerTask_id,In1);
		insert_msg_table_entry(IPTask3_id,Out1,SessionManagerTask_id,In1);
		insert_msg_table_entry(IPTask4_id,Out1,SessionManagerTask_id,In1);
		insert_msg_table_entry(IPTask5_id,Out1,SessionManagerTask_id,In1);
		insert_msg_table_entry(SessionManagerTask_id,Out1,USERTASKNAME_ID,In2);
		insert_msg_table_entry(SessionManagerTask_id,Out2,IPTask0_id,In1);
		insert_msg_table_entry(SessionManagerTask_id,Out3,IPTask1_id,In1);
		insert_msg_table_entry(SessionManagerTask_id,Out4,IPTask2_id,In1);
		insert_msg_table_entry(SessionManagerTask_id,Out5,IPTask3_id,In1);
		insert_msg_table_entry(SessionManagerTask_id,Out6,IPTask4_id,In1);
		insert_msg_table_entry(SessionManagerTask_id,Out7,IPTask5_id,In1);
	insert_msg_table_entry(IpStreamReceiverTask_id,Out1,ZigBeeTask_id,In1);
	insert_msg_table_entry(ZigBeeTask_id,Out1,SessionManagerTask_id,In2);
	insert_msg_table_entry(ZigBeeTask_id,Out2,IpStreamSenderTask_id,In1);
	//insert_msg_table_entry(ApplicationTask_id,Out1,ApplicationTask_id,In1);

	// Clear vectors
	memset(AlarmCountdown, 0, TOTAL_TASKS);
	memset(MessageSignal, 0, TOTAL_TASKS);


	//////////////////////////////////
	// Create all listening threads //
	//////////////////////////////////

	puts("creating SessionManager_thread");
	pthread_create(&thread_array[SessionManagerTask_id], NULL, SessionManagerTask, (void*)&Context_array[SessionManagerTask_id]);

	puts("creating SerialSenderTask_thread");
	pthread_create(&thread_array[IpStreamSenderTask_id], NULL, IpStreamSenderTask, (void*)&Context_array[IpStreamSenderTask_id]);

	puts("creating SerialReceiverTask_thread");
	pthread_create(&thread_array[IpStreamReceiverTask_id], NULL, IpStreamReceiverTask, (void*)&Context_array[IpStreamReceiverTask_id]);


	puts("creating ZigBeeTask_thread");

	pthread_create(&thread_array[ZigBeeTask_id], NULL, ZigBeeTask, (void*)&Context_array[ZigBeeTask_id]);

	// Delay so all listening threads are ready
	sleep(1);

	puts("*** All Listening Threads Created ***");

	pthread_join(msg_thread, NULL);
	puts("msg_thread join finished");


	puts("The End");

	return(EXIT_SUCCESS);
}

