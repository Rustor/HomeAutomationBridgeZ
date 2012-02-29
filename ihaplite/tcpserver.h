/*
 *
  *      Author: tsh,rto
 *
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
 */


#define virtualport     '3'
#define PRINT_STATUS 1


unsigned char *receive_cmd(int client, unsigned char VPS) ;
int server_init(int server_port);
int server_end(int s);
void IP_send_server_hexbinaryString(int client, unsigned char *binary_data, unsigned char length);
// send raw bin data -- should be ASCII
void IP_send_server_String(int client, unsigned char *binary_data, unsigned char length);
void respond(int client, unsigned char *message, unsigned char length);
