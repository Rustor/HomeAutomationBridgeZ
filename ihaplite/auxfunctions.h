/*
 * auxfunctions.h
 *
 *  Created on: Mar 7, 2010
 *      Author: rro
 *
 *This program is free software: you can redistribute it and/or modify
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
 *
 */

#ifndef AUXFUNCTIONS_H_
#define AUXFUNCTIONS_H_



unsigned char binary_to_hexbinary(unsigned char*, unsigned char*, unsigned int);

unsigned char  hexbinary_to_binary(unsigned char *input, unsigned char *output);

// debug only on linux
const char* binary_to_hexbinaryString(unsigned char*, unsigned int, char*);


#endif /* AUXFUNCTIONS_H_ */
