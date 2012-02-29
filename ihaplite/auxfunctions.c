/*
 * auxfunctions.c
 *
 *  Created on: Mar 5, 2010
 *      Author: tsh,rto
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

#include "auxfunctions.h"

unsigned char hexbinary_to_binary( unsigned char *input, unsigned char *output) {
	unsigned char value = 0, length, j;

	length = strlen( (char *) input)/2;
	for(j = 0; j < length; j++) {
		value = 0;
		// Convert 2 hexascii to 1 char
		if (*input >= '0' && *input <= '9')
			value = (value << 4) + (*input - '0');
		else if (*input >= 'A' && *input <= 'F')
			value = (value << 4) + (*input - 'A' + 10);
		else if (*input >= 'a' && *input <= 'f')
			value = (value << 4) + (*input - 'a' + 10);
		input++;
		if (*input >= '0' && *input <= '9')
			value = (value << 4) + (*input - '0');
		else if (*input >= 'A' && *input <= 'F')
			value = (value << 4) + (*input - 'A' + 10);
		else if (*input >= 'a' && *input <= 'f')
			value = (value << 4) + (*input - 'a' + 10);
		input++;
		output[j] = value;
	}
	output[j] = 0;	// Null terminate
	return length;
}

unsigned char binary_to_hexbinary(unsigned char *input, unsigned char *output, unsigned int input_length) {
	unsigned char value = 0, j;

	for(j = 0; j < input_length; j++) {
		value = *input >> 4;
		if(value < 10) {
			*output = value + '0';
		} else {
			*output = value + 55;
		}
		output++;
		value = *input & 0x0F;

		if(value < 10) {
			*output = value + '0';
		} else {
			*output = value + 55;
		}
		output++;
		input++;
	}
	output[j] = '\0';
	return (input_length*2);
}

// ONLY FOR DEBUG PURPOSE - PRINT TO console
const char * binary_to_hexbinaryString( unsigned char *input, unsigned int buffer_length, char *printbuffer) {
	unsigned int i;
	for(i=0;i<buffer_length;i++) {
		sprintf(&printbuffer[i*3], "%02X ", input[i]);
	}
	return printbuffer;
}
