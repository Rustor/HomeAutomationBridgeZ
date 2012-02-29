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

/***************************************************************************
 *            request_service.h
 *
 *  Tue May 17 16:40:28 2011
 *  Copyright  2011  tibo
 *  <tibo@<host>>
 ****************************************************************************/

#ifndef REQUEST_SERVICE_H
#define REQUEST_SERVICE_H

#include <stdio.h>
#include "utlist.h"

typedef struct RequestService RequestService;
typedef struct OnValueChange OnValueChange;
typedef struct Action Action;


struct RequestService
{
    char *ID;
	char *name;
    char *value_url;
	char *complete_url;
    char *type;
	char *RSID;
	char *device_type;
	char *device_ID;
	char *location;
	OnValueChange *on_value_change_head;
	OnValueChange *default_value_change;
	RequestService *next;
};

struct OnValueChange
{
	char *value;
	Action *action_head;
	OnValueChange *next;
};


struct Action
{
	char *RSID;
	char *type;
	char *value;
	Action *next;
};


RequestService * create_request_service(char *_description,
                                        char *_ID,
                                        char *_type,
                                        char *_unit,
                                        char *_RSID,
                                        char *device_type,
                                        char *device_ID);

int destroy_request_service_list( RequestService *request_service_head );
int destroy_request_service_struct(RequestService *_request_service);
int destroy_on_value_change_struct( OnValueChange *ovc_tmp );
int destroy_action_struct( Action *action_tmp );
int copy_request_service( RequestService *to_copy, RequestService *to_create );
int init_request_service_struct( RequestService *rs_tmp );
int init_on_value_change_struct( OnValueChange *ovc_tmp );
int init_action_struct( Action *action_tmp );



#endif /* REQUEST_SERVICE_H */

