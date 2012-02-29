/*
 * utils.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *copystr(char *str)
{
	char *string;
	if (!str)
		return(0);
	string= (char *)malloc(strlen(str)+1);
	if (string)
		strcpy(string, str);
	return(string);
}

char *getarg(char *arg, int argc, char **args) {
	int arglen;
	char *retstr = NULL;
	int totalargs = argc;

	if ((!arg) || (!*arg) || (argc < 2))
		return (NULL);

	arglen = strlen(arg);
	do {
		if (strncmp(arg, args[argc - 1], arglen) == 0) {
			if (strlen(args[argc - 1]) != arglen) {
				retstr = copystr(args[argc - 1] + arglen);
			} else {
				if (totalargs > argc) {
					retstr = copystr(args[argc]);
				} else
					retstr = copystr(args[argc - 1]);
			}
			return (retstr);
		}
	} while (--argc > 1);

	return (NULL);
}


syscall_pipe(char *sys_str, char *line) {
	FILE *fpipe;
	if (!(fpipe = (FILE*) popen(sys_str, "r"))) {
		// If fpipe is NULL
		printf("Problems with pipe");
	}
	fgets(line, 255, fpipe);
	pclose(fpipe);
}

int load_file_into_buffer(char *buffer, char *filename) {
	//  just dummy function to get at device file
	//int i, j, vport, length, currenttablelenth = 2;
	//unsigned char *dataPtr;
	FILE *file;
	unsigned long fileLen;
	//Open file
	file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Unable to open file %s", filename);
		return -1;
	}
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);
	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);
	buffer[fileLen] = 0x00;
	return fileLen;
}
