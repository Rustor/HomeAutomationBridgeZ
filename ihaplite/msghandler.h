/*
 * messagehandler.h
 *
 *  Created on: Mar 10, 2010
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

#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

void* msgHandler(void*);

void* broadcast_msg(unsigned char[], const unsigned char, unsigned char*);
char receive(int, pthread_cond_t*, pthread_mutex_t*);
char msgTestAndReset(int);
void* msgSignalAndRelease(int);

void insert_msg_table_entry(unsigned char Sender_taskid, unsigned char Sender_output, unsigned char Receiver_taskid, unsigned char Receiver_input);

#endif /* MESSAGEHANDLER_H_ */
