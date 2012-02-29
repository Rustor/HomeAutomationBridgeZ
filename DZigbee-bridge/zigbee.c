/*
 * zigbee.c
 *
 *  Created on: Mar 5, 2010
 *      Author: rto
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
#include "zigbee.h"
#include "auxfunctions.h"
#include "vps2addr.h"

#define TASKNAME_STRING		"ZigBeeTask"

#define TOGGLELAMPIEEE "000300C015D60003xxxx000D00BC15000401210600FF04031167xx"
#define TOGGLELAMPNWK "000300C00FCC0002dc540401210600FF0403116702"
#define EXCLUDEIEEE "000300C00DCE02FCFF009502000D00BC150000"
#define GETEPDESCIEEE "000300C00DCE02FCFF009502000D00BC150000"
#define CONFIGHEADER  "000300C028EB0003"
#define CONFIGCSTRING "000000210000001600"

#define VPSLEN dataPtr[1]
#define VPSPORT dataPtr[0]

#define	CHECKSUM(a)	a[0]^a[1]^a[2]^a[3]^a[4]

unsigned int htoi(const char *ptr);

void* ZigBeeTask(void* _context) {
	struct Context* context = (struct Context*) _context;
	unsigned char name_id[SENDERNAMESIZE] = { context->task_id, UNUSED, UNUSED };
	unsigned char * zcmd, *mcmd, *dataPtr, *buffer, msgFlag;
	unsigned char vpsport, vpslen, serviceUpperLayers = 0, messageFromRadio = 0;

	unsigned char do_broadcast_to_serial = 0, do_broadcast_to_upperlayers = 0;
	char printbuffer[DATASIZE];
	unsigned char myvps = 0, newvps = 0, getdecs_vps = 0;
	int includemode = 0, excludemode = 0, len, netupdatemode = 0, getdescmode = 0;
	unsigned int len2;

	unsigned char validvps = 0, endpoint, gwieeeaddr[8];

	puts(TASKNAME_STRING ": Started");
	pthread_mutex_lock(context->mutex);
	genvpstable();
	clearvpstable();

	while (communication_needed) {

		puts(TASKNAME_STRING ": calling receive()");
		msgFlag = receive(context->task_id, context->condition, context->mutex);
		dataPtr = context->holder;
		printf(TASKNAME_STRING ": Receiving data: [%s]\n",
				binary_to_hexbinaryString(dataPtr, dataPtr[1] + 2, printbuffer));
		printf(TASKNAME_STRING ": received from sender: [%s]\n",
				binary_to_hexbinaryString(name_id, SENDERNAMESIZE, printbuffer));
		do_broadcast_to_serial = 0;
		do_broadcast_to_upperlayers = 0;
		serviceUpperLayers = 0;
		messageFromRadio = 0;

		switch (context->sender[InputID]) {
		case In1:
			serviceUpperLayers = 0;
			messageFromRadio = 1;
			puts("broadcast Out1: " TASKNAME_STRING);
			dataPtr[0] = 0x00;
			if (dataPtr[1] == 0x00)
				break;
			serviceUpperLayers = 0;
			messageFromRadio = 1;
			break;
		case In2:
			serviceUpperLayers = 1;
			messageFromRadio = 0;
			puts("broadcast Out2: " TASKNAME_STRING);
			break;
		default:
			break;
		}

		if (serviceUpperLayers) {
			serviceUpperLayers = 0;
			vpsport = dataPtr[0];
			vpslen = dataPtr[1];
			if (vpsport == 0) {
				mcmd = dataPtr + 2;
				do_broadcast_to_serial = 1;

				if (!(unsigned char) memcmp("NETLIST_REQ", mcmd, 11)) {

					memcpy(mcmd, "\x00\x03\x00\xc0\x04\xcd\x03\x05\x00\x00", 10);
					dataPtr[1] = 10;
					mcmd[5] = CHECKSUM(mcmd);
					do_broadcast_to_serial = 1;
					netupdatemode = 1;
				}

				if (!(unsigned char) memcmp("INCLUDE_REQ", mcmd, 11))
				{
					memcpy(mcmd, "\x00\x03\x00\xc0\x06\xc5\x01\x36\xFC\xFF\xFE\x01", 12);
					includemode = 1;
					excludemode = 0;
					dataPtr[1] = 12;
					do_broadcast_to_serial = 1;
				}

				if (!(unsigned char) memcmp("XADDSTOP_REQ", mcmd, 11)) {

					memcpy(mcmd, "\x00\x03\x00\xc0\x06\xc5\x01\x36\xFC\xFF\x00\x01", 12);
					includemode = 0;
					excludemode = 0;
					dataPtr[1] = 12;
					do_broadcast_to_serial = 0;
				}

				if (!(unsigned char) memcmp("GETDESC_REQ", dataPtr + 2, 11)) {

					memcpy(mcmd, "\x00\x03\x00\xc0\x07\xc4\x01\x04\xdc\x54\xdc\x54\x21", 13);
					includemode = 0;
					excludemode = 0;
					dataPtr[1] = 13;

					do_broadcast_to_serial = 0;
				}

				if (!(unsigned char) memcmp("XCLUDE_REQ", mcmd, 10)) {

					excludemode = 1;
					includemode = 0;
					free(dataPtr);
					do_broadcast_to_serial = 0;
				}
			}

			else
				validvps = check_valid_vps(vpsport);

			if (vpsport > 0 && vpsport < 11 && validvps) {
				if (!(unsigned char) memcmp("XCLUDE_REQ", dataPtr + 2, 10)) {

					hexbinary_to_binary(EXCLUDEIEEE, dataPtr + 2);

					memcpy(dataPtr + 2 + 10, vps_vtoa(vpsport), 8);

					dataPtr[20] = 0x00;
					dataPtr[1] = 19;

					excludemode = 1;
					includemode = 0;
					do_broadcast_to_serial = 1;
				} else if (!(unsigned char) memcmp("GETDESC_REQ", dataPtr + 2, 11)) {
					getdescmode = 1;
					endpoint = (unsigned char) htoi(dataPtr + 13);
					printf("endpoint was: %02X \n", endpoint);

					buffer = (unsigned char *) calloc(1, 200);
					buffer[0] = 0x00;
					buffer[1] = 0x03;
					buffer[2] = 0x00;
					buffer[3] = 0xC0;
					buffer[4] = 0x07;
					buffer[5] = CHECKSUM(buffer);
					buffer[6] = 0x01;
					buffer[7] = 0x04;

					getdecs_vps = dataPtr[0];
					memcpy(buffer + 8, vps_vtoa_nwk(dataPtr[0]), 2);
					memcpy(buffer + 10, vps_vtoa_nwk(dataPtr[0]), 2);

					buffer[12] = endpoint;
					len = 13;
					memcpy(dataPtr + 2, buffer, len);
					dataPtr[1] = len;
					free(buffer);
					do_broadcast_to_serial = 1;

				} else //
				if (!(unsigned char) memcmp("GETUID_REQ", dataPtr + 2, 10)) {

					memcpy(dataPtr + 2, vps_vtoa(vpsport), 8);
					dataPtr[1] = 8;
					do_broadcast_to_serial = 0;
					do_broadcast_to_upperlayers = 1;
				} else if (!(unsigned char) memcmp("XONFIG_REQ", dataPtr + 2, 10)) {

					buffer = (unsigned char *) calloc(1, 200);
					hexbinary_to_binary(CONFIGHEADER, buffer);
					len = 8;
					memcpy(buffer + len, vps_vtoa(vpsport), 8);
					len = len + 8;
					hexbinary_to_binary(CONFIGCSTRING, buffer + len);
					len = len + strlen(CONFIGCSTRING) / 2;
					memcpy(buffer + len, vps_vtoa(vpsport), 8);
					len = len + 8;
					hexbinary_to_binary(dataPtr + 2 + 10, buffer + len);
					len = len + (VPSLEN - 10) / 2;
					memcpy(buffer + len, gwieeeaddr, 8);
					len = len + 8;
					buffer[len] = 0xc0;
					len++;
					memcpy(dataPtr + 2, buffer, len);
					dataPtr[1] = len;
					free(buffer);
					do_broadcast_to_serial = 1;

				} else {

					mcmd = dataPtr + 2;
					buffer = (unsigned char *) calloc(1, 200);
					buffer[0] = 0x00;
					buffer[1] = 0x03;
					buffer[2] = 0x00;
					buffer[3] = 0xC0;
					buffer[4] = VPSLEN + 10;
					buffer[5] = CHECKSUM(buffer);
					buffer[6] = 0x00;
					buffer[7] = 0x03;

					memcpy(buffer + 8, vps_vtoa(dataPtr[0]), 8);
					memcpy(buffer + 16, &dataPtr[2], dataPtr[1]);
					len = dataPtr[1] + 16;
					memcpy(dataPtr + 2, buffer, len);
					dataPtr[1] = len;
					free(buffer);
					do_broadcast_to_serial = 1;
				}

			} else if (validvps == 0 && vpsport > 0) {

				memcpy(dataPtr + 2, "invalid vps : ", 14);
				dataPtr[0] = vpsport;
				dataPtr[1] = 14;
				do_broadcast_to_upperlayers = 1;
				do_broadcast_to_serial = 0;
			}

		}
		if (messageFromRadio) {
			messageFromRadio = 0;
			do_broadcast_to_upperlayers = 1;
			dataPtr[0] = 0x00;

			zcmd = dataPtr + 2;

			if (includemode && dataPtr[8] == 0xc1 && dataPtr[9] == 0x20 && dataPtr[10] == 0xf1) {
				includemode = 0;

				newvps = vps_add(dataPtr + 10 + 2);
				memcpy(zcmd, "\x00\x03\x00\xC0\x0E\xCD\x02\xFC\xFF\xFE", 10);
				memcpy(zcmd + 10, vps_vtoa(newvps), 8);
				memcpy(zcmd + 18, "\x01\x00", 2);

				do_broadcast_to_serial = 1;
				do_broadcast_to_upperlayers = 0;
			}

			if (dataPtr[3] == 0x53 && dataPtr[4] == 0xc4 && dataPtr[5] == 0xc4 && dataPtr[6] == 0x08) {
				memcpy(gwieeeaddr, zcmd + 6, 8);
				do_broadcast_to_serial = 0;
				do_broadcast_to_upperlayers = 0;
			}

			if (!includemode && dataPtr[8] == 0x42) {

				myvps = vps_atov(zcmd + 15);
				vps_add_nwk(myvps, zcmd + 23);
				do_broadcast_to_serial = 0;
				do_broadcast_to_upperlayers = 0;
			}

			if (includemode && dataPtr[8] == 0x42) {
				includemode = 0;

				newvps = vps_add(zcmd + 15);
				memcpy(zcmd, "\x00\x03\x00\xC0\x0E\xCD\x02\xFC\xFF\xFE", 10);
				memcpy(zcmd + 10, vps_vtoa(newvps), 8);
				memcpy(zcmd + 18, "\x01\x00", 2);
				do_broadcast_to_serial = 1;
				do_broadcast_to_upperlayers = 0;
			}

			if (excludemode && zcmd[6] == 0x40 && zcmd[7] == 0x02 && zcmd[8] == 0x00) {
				excludemode = 0;
				myvps = vps_atov(&dataPtr[12 + 2]);

				hexbinary_to_binary(EXCLUDEIEEE, dataPtr + 2);
				memcpy(dataPtr + 2 + 10, vps_vtoa(myvps), 8);
				dataPtr[20] = 0x00;
				dataPtr[1] = 19;

				do_broadcast_to_serial = 1;
				do_broadcast_to_upperlayers = 0;

			}

			if (netupdatemode && dataPtr[8] == 0x83) {
				netupdatemode = 0;

				clearvpstable();
				buffer = (unsigned char *) calloc(1, 200);

				if (*(zcmd + 12) != 0xFF) {
					myvps = vps_add(zcmd + 12);
					vps_add_nwk(myvps, zcmd + 20);
					strcat(buffer, " 10001 ");
				}
				if (*(zcmd + 22) != 0xFF) {
					myvps = vps_add(zcmd + 22);
					vps_add_nwk(myvps, zcmd + 30);
					strcat(buffer, " 10002 ");
				}
				if (*(zcmd + 32) != 0xFF) {
					myvps = vps_add(zcmd + 32);
					vps_add_nwk(myvps, zcmd + 40);
					strcat(buffer, " 10003 ");
				}
				if (*(zcmd + 42) != 0xFF) {
					myvps = vps_add(zcmd + 42);
					vps_add_nwk(myvps, zcmd + 50);
					strcat(buffer, " 10004 ");
				}
				if (*(zcmd + 52) != 0xFF) {
					myvps = vps_add(zcmd + 52);
					vps_add_nwk(myvps, zcmd + 60);
					strcat(buffer, " 10005 ");
				}

				do_broadcast_to_serial = 0;

				memcpy(dataPtr + 2, "netlist_done: ", 14);
				strcat(dataPtr + 2, buffer);
				free(buffer);
				dataPtr[0] = 0;
				dataPtr[1] = strlen(dataPtr + 2);
				do_broadcast_to_upperlayers = 1;
			}

			if (dataPtr[8] == 0x80 && dataPtr[20] != 0x00) {

				myvps = vps_atov(&dataPtr[10]);
				dataPtr[0] = myvps;
				dataPtr[1] = 11;
				strcpy(dataPtr + 2, "DATA_FAILED");
			}

			if (dataPtr[8] == 0x80 && dataPtr[20] == 0x00) {
				myvps = vps_atov(&dataPtr[10]);

			}

			if (dataPtr[8] == 0x40 && !getdescmode) {

				myvps = vps_atov(&dataPtr[14]);

				len2 = dataPtr[1] - 6 - 6 - 8;
				dataPtr = dataPtr + 6 + 6 + 8;
				dataPtr[0] = myvps;
				dataPtr[1] = len2;
			}

			if (dataPtr[8] == 0x81) {
				getdescmode = 0;

				myvps = vps_atov_nwk(&dataPtr[11]);
				len2 = dataPtr[1] - 11 + 2;
				dataPtr = dataPtr + 2 + 13 - 2 - 4;
				dataPtr[2] = 'z';
				dataPtr[3] = 'b';
				dataPtr[0] = myvps;
				dataPtr[1] = len2;
			}

		}
		if (do_broadcast_to_serial) {
			dataPtr[0] = 0xff;
			broadcast_msg(name_id, Out2, dataPtr);
		};
		if (do_broadcast_to_upperlayers) {
			broadcast_msg(name_id, Out1, dataPtr);
		};

	} // while

	puts(TASKNAME_STRING ": unlocking mutex and stopping");
	pthread_mutex_unlock(context->mutex);
	return NULL;
}
unsigned int htoi(const char *ptr) {
	unsigned int value = 0;
	char ch = *ptr;

	/*--------------------------------------------------------------------------*/

	while (ch == ' ' || ch == '\t')
		ch = *(++ptr);

	for (;;) {

		if (ch >= '0' && ch <= '9')
			value = (value << 4) + (ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			value = (value << 4) + (ch - 'A' + 10);
		else if (ch >= 'a' && ch <= 'f')
			value = (value << 4) + (ch - 'a' + 10);
		else
			return value;
		ch = *(++ptr);
	}
}
