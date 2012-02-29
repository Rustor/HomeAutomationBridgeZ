/*
 * timemanager.h
 *
 *  Created on: Mar 7, 2010
 *      Author: rto,tsh
 *
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
 */

#ifndef TIMEMANAGER_H_
#define TIMEMANAGER_H_


void* TimeManagerTask(void*);
void* alarmSet(int, unsigned int);
char alarmTestAndReset(int);

#endif /* TIMEMANAGER_H_ */
