/*
 *
 *      Author: tsh,rto
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

#ifndef NET_H
#define NET_H

int net_connect(char *hostname, char *port);
int net_disconnect(int fd);
int IP_send_String(char *device, char *port, unsigned char *binary_data, unsigned char length, char *reply);
int IP_send_String_no_ack(char *device, char *port, unsigned char *binary_data, unsigned char length, char *reply);
#endif
