/*
 * timemanager.c
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

#include "framework.h"
#include "msghandler.h"

extern unsigned char AlarmSignal[];

// Current countdown value for alarm
extern unsigned int AlarmCountdown[];
extern struct Context Context_array[];


///// TimeManager defines
#define TIMEMANAGER_TICK_USEC 100000


void* TimeManagerTask(void* _context) {
	int i;
	// The two globally write/readable arrays AlarmCountdown and AlarmSignal
	// should probably be protected by a mutex

	while(communication_needed) {
		// Decrement all non-zero alarms
		for(i=0;i<MAX_TASKS;i++) {
			if(AlarmCountdown[i] == 0) {
				// Do nothing
				// This empty branch is here to avoid further comparisons
				// on a skippable field since this will most often be the case
			}
			else {
				if(AlarmCountdown[i] <= 1) {
					// Alarm is expiring now
#ifdef _DEBUG_VERBOSE
					printf("TimeManager: Alarm[%i] is expiring now\n", i);
#endif
					// Set the AlarmSignal
					AlarmSignal[i] = 1;

					// Clear AlarmCountdown
					AlarmCountdown[i] = 0;

					// Signal the corresponding task whether it is listening or not
					//pthread_cond_signal(Context_array[i].condition);
					//pthread_cond_signal(	msg_context.condition);
					pthread_cond_signal(	Context_array[i].condition );
				} else {
					// Alarm should be counted down
					//printf("TimeManager: Counting down Alarm[%i] from %i to %i\n", i, AlarmCountdown[i], AlarmCountdown[i]-1);
					AlarmCountdown[i]--;
				}
			}
		}
		usleep(TIMEMANAGER_TICK_USEC);
	}
	return NULL;
}

// time is in 100s of milliseconds (10 = 1 sec)
void* alarmSet(int Task_id, unsigned int time) {
	AlarmCountdown[Task_id] = time;
	return NULL;
}

char alarmTestAndReset(int Task_id) {
	if(AlarmSignal[Task_id] == 1) {
		AlarmSignal[Task_id] = 0;
		return TRUE;
	} else {
		return FALSE;
	}
}

