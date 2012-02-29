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


#ifndef IPSTREAMTASKS_H_
#define IPSTREAMTASKS_H_

#define SERVER_PORT_ZIGBEE 61001

#define PRINT_STATUS 1

int send_stream_data(int client, unsigned char * message, int len);
int receive_stream_data(int client, unsigned char * message, int len);
int use_ipstream_server(char *direction, unsigned char *message, int len);

void* IpStreamReceiverTask(void*);	//receives from remote ip client
void* IpStreamSenderTask(void*);	//sends to open stream to client
#endif
