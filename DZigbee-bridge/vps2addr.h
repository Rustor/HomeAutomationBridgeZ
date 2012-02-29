/*
 * vps2qaddr.h
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

#ifndef VPS2ADDR_H_
#define VPS2ADDR_H_


#define ADDRLEN	8
#define ADDRLEN2 2
// MAXVPS set to 11 means 10 vps since vps start from 1
#define MAXVPS 11

void genvpstable(void);
void clearvpstable(void);

unsigned char check_valid_vps(unsigned char vps);

// add addr into VPS message exchange table
unsigned char vps_add(unsigned char addr[ADDRLEN]);

// remove addr in VPS message exchange table
void vps_del(unsigned char vps);

// get addr for VPS# in  VPS message exchange table
unsigned char *vps_vtoa(unsigned char vps);

// get vps for a addr in  VPS message exchange table;
unsigned char vps_atov(unsigned char *addr);

//add zigbee network addr vps table for known entry
void vps_add_nwk(unsigned char vps, unsigned char addr[ADDRLEN2]);

// get vps from nwk addr
unsigned char *vps_vtoa_nwk(unsigned char vps);

//get nwk addr from vps
unsigned char vps_atov_nwk(unsigned char *addr);


#endif /* VPS2ADDR_H_ */
