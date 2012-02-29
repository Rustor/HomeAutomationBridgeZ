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

#ifndef XMLAPI_H
#define XMLAPI_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mxml.h>
#include "request_service.h"
#include "utlist.h"

#define	ENCODING "UTF-8"
#define XML_FILE_NAME "requested_services.xml"

/**How do we retrieve this ID ?*/
#define DEVICE_LIST_ID "12345"

char * get_xml_value(char* value);
char* get_value_from_xml_value(char* _xml_value); //DONE
int parse_xml( char *_xml_path, RequestService **requested_service );
int get_matching_request_service( char *_service_xml, RequestService *_request_service_head, 
                                 RequestService **_request_service_match_out );

int creaters(char *servicetypeinput, char * servicelocationinput, RequestService ** request_service_head);


#endif
