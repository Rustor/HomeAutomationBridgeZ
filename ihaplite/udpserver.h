/*
 *
 *  Created on: Mar 31, 2010
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
#define HELLO_GROUP "192.168.1.255"

int UDP_broadcast(int port, unsigned char* message, int len); //function here broadcast no matter bin/hexbin
int UDP_init(int ports[], fd_set *fdset);
int	UDP_listen(fd_set *fdset, int fd_max, unsigned char* buffer);
void *get_in_addr(struct sockaddr *sa);
uint16_t get_in_port(struct sockaddr *sa);
int	get_ip_UDP_listen(fd_set *fdset, int fd_max, unsigned char* buffer, char * remote_ip);
int EventListen_local(char * bridgeaddr, char * bridgeport, char *resultbuffer);

