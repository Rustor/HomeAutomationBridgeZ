/*
 * sessionmanager.h
 *
 *  Created on: Mar 5, 2010
 *      Author: tsh,rto
 *
 *      This program is free software: you can redistribute it and/or modify
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

//#define SessionManagerTask_id 5
void* SessionManagerTask(void*); //multiplexes several IP tasks to a single serial task
#define UDPPORTBASE 10000
