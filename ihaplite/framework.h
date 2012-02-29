/*
 * master.h
 *
 *  Created on: Dec 8, 2008
 *      Author: tsh,rto
 *      NOTE: this file is the messagehandler.h
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
 *
 *
 */


#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_ 1
#include <pthread.h>

////////////////////// Defines /////////////////////////
#define TRUE 1
#define FALSE 0

// Beware increasing this value!
// Most likely you will need to redefine a lot of unsigned chars
// to unsigned ints in functions
#define DATASIZE 255
#define DATASIZE_HEXBINARY DATASIZE*2+1

///// MessageHandler
#define SENDERNAMESIZE 3
#define QUEUESIZE 4
#define QUEUE_OCCUPIED 1
#define QUEUE_FREE 0

///// SessionManager defines
#define SESSION_TIMEOUT 15
#define SERVER_PORT     10000
#define MANAGEMENTPORT 10000
//#define VPS_SIZE 10

///// Sender ID structure: [TaskID,InputID,OutputID]
#define TaskID 0
#define InputID 1
#define OutputID 2

///// Task Input and Output port ID symbols
#define UNUSED 0
#define In1 1
#define In2 2
#define In3 3
#define In4 4
#define In5 5
#define In6 6
#define In7 7
#define In8 8
#define In9 9
#define In10 10
#define In11 11

#define Out1 1
#define Out2 2
#define Out3 3
#define Out4 4
#define Out5 5
#define Out6 6
#define Out7 7
#define Out8 8
#define Out9 9
#define Out10 10
#define Out11 11

///// Number of I/O ports on a component
#define MAX_OUTPUT 11


// Pre-defined IPTask IDs for softwiring, up to 10 available

#define IPTask0_id 0+USER_TASKS
#define IPTask1_id 1+USER_TASKS
#define IPTask2_id 2+USER_TASKS
#define IPTask3_id 3+USER_TASKS
#define IPTask4_id 4+USER_TASKS
#define IPTask5_id 5+USER_TASKS
#define IPTask6_id 6+USER_TASKS
#define IPTask7_id 7+USER_TASKS
#define IPTask8_id 8+USER_TASKS
#define IPTask9_id 9+USER_TASKS
#define IPTask10_id 10+USER_TASKS


/////////////////// End of defines /////////////////////

extern unsigned char max_vps;
extern unsigned char max_tasks;

#define MAX_TASKS max_tasks
#define MAX_VPS max_vps

///// TimeManager
#define NON_TASK_THREADS 1

// Data structure prototyping
struct Context msg_context;

// Used to terminate the infinite loop in message handler
char communication_needed;

/*This data structure contains information that needs to be shared between
threads. It is passed to the producer and consumer threads' starter functions.
The alternative to this is to use global variables.*/
struct Context{
        pthread_cond_t* condition;
        pthread_mutex_t* mutex;
        /*This is used to hold a character while it is passed from one thread to
        another. Only the thread which owns the mutex lock should access it.*/
        unsigned char *holder;
        unsigned char sender[SENDERNAMESIZE];

        unsigned char occupied;
        unsigned char task_id;
};



/* Data for IP-related tasks */
struct IP_data{
	struct Context* ptr_context;
	int port;
	unsigned char VPS;
};


#endif /* FRAMEWORK_H_ */

