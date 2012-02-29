/*
 * parser.h
 *
 *  Created on: Feb 22, 2012
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

#ifndef PARSER_H_
#define PARSER_H_

int ODSDL_parser( char *devicexml_path, char * deviceid, char * serviceid, char * inputparm, char *outbuffer, char * url );
int hp_ODSDL_parser( char *url, char * servicetype, char * location, char * inputparm, char *outbuffer );

#endif /* PARSER_H_ */
