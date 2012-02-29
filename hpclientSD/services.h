/*Copyright 2011 Aalborg University. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Aalborg University ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Aalborg University OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed*/

/*
 * @author Thibaut Le Guilly
 */
#ifndef SERVICES_H
#define SERVICES_H

#include "parameter_list.h" 

typedef struct Device Device;
struct Device
{
    char *description;
    char *ID;
	char *UID;
	char *IP;
	char *port;
	char *location;
	char *type;
};

typedef struct Service Service;
struct Service
{
    char *description;
    char *ID;
    char *value_url;
    char *type;
	char *unit;
    Device *device;
	ParameterList parameter_list;
};

Service* create_service_struct(
                               char *_description,
                               char *_ID,
                               char *_value_url,
                               char *_type,
                               char *_unit,
                               Device *_device,
                               Parameter *_parameter);

void free_service_struct(Service *_service);

Device* create_device_struct(
                             char *_description,
                             char *_ID,
                             char *_UID,
                             char *_IP,
                             char *_port,
                             char *_location,
                             char *_type);
void free_device_struct(Device *_device);

int add_parameter_to_service(Parameter *_parameter, Service *_service);

#endif
