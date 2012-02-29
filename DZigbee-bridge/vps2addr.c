/*
 * vps2qaddr.c
 *
 *  Created on: Mar 31, 2010
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vps2addr.h"

#define	CHECKSUM(a)	a[0]^a[1]^a[2]^a[3]^a[4]

typedef struct vpstable_t {
	unsigned char used;
	unsigned char addr[ADDRLEN];
	unsigned char nwkaddr[ADDRLEN2];
} vpstable_t;
struct vpstable_t *addrs[MAXVPS];

void genaddrs() {
	addrs[9]->addr[0] = 0x23;
	addrs[9]->addr[1] = 0x12;
	addrs[9]->used = 1;
	addrs[7]->addr[0] = 0x43;
}

void genvpstable(void) {
	unsigned char c;
	for (c = 0; c < MAXVPS; c++)
		addrs[c] = (struct vpstable_t *) calloc(1, sizeof(vpstable_t));
}

void clearvpstable(void) {
	unsigned char c;
	for (c = 0; c < MAXVPS; c++)
		addrs[c]->used = 0;
}

unsigned char vps_add(unsigned char addr[ADDRLEN]) {
	unsigned char i;
	for (i = 1; i != 0; i++) {
		if (addrs[i]->used == 0) {
			addrs[i]->used = 1;
			memcpy(addrs[i]->addr, addr, ADDRLEN);
			return i;
		}
	}
	return 0; // mean invalid
}

void vps_add_nwk(unsigned char vps, unsigned char addr[ADDRLEN2]) {
	memcpy(addrs[vps]->nwkaddr, addr, ADDRLEN2);
}

void vps_del(unsigned char vps) {
	addrs[vps]->used = 0;
}

unsigned char *vps_vtoa(unsigned char vps) {
	return addrs[vps]->addr;
}

unsigned char vps_atov(unsigned char *addr) {
	unsigned char i;
	for (i = 1; i < MAXVPS; i++) {
		if (addrs[i]->used != 0)
			if (memcmp(addr, addrs[i]->addr, ADDRLEN) == 0x00)
				return i;
	}

	return 0;
}

unsigned char *vps_vtoa_nwk(unsigned char vps) {
	return addrs[vps]->nwkaddr;
}

unsigned char vps_atov_nwk(unsigned char *addr) {
	unsigned char i;
	for (i = 1; i < MAXVPS; i++) {
		if (addrs[i]->used != 0)
			if (memcmp(addr, addrs[i]->nwkaddr, ADDRLEN2) == 0x00)
				return i;
	}

	return 0;
}

unsigned char check_valid_vps(unsigned char vps)

{
	if (addrs[vps]->used == 1)
		return 1;
	else
		return 0;
}


