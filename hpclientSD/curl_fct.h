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
#ifndef CURL_FCT_H
#define CURL_FCT_H

#include <curl/curl.h>
#include <pthread.h>
#include "request_service.h"

struct CBC
{
  char *buf;
  size_t pos;
  size_t size;
};

typedef struct HP_EVENTS_MANAGER HP_EVENTS_MANAGER;
typedef struct SubscribedService SubscribedService;

struct HP_EVENTS_MANAGER
{
	pthread_t *events_thread;
	char *base_url;
	CURL *get_subscription_c;
	CURL *get_events_c;
	CURL *share_c;
	struct CBC *cbc;
	char *cbc_buf;
	int exit;
	pthread_mutex_t *exit_mutex;
	int nb_subscribed;
	int started;
	char *hp_url;
	SubscribedService *subscribed_service_head;
	HP_EVENTS_MANAGER *next;
};

struct SubscribedService
{
	RequestService *request_service;
	SubscribedService *next;
};

typedef void (*EventsFunction) (HP_EVENTS_MANAGER*, char*);
typedef void (*GetEventsExit) (HP_EVENTS_MANAGER*);

int init_curl(EventsFunction _e, GetEventsExit _exit_events);
int deinit_curl();
char * http_get_function(char *_url);
char * http_put_function(char *_url, char *_data);
void * http_get_events_function(void *_data_pointer, GetEventsExit exit_callback);

HP_EVENTS_MANAGER * create_events_manager(char *_hp_server_url);
int destroy_events_manager( HP_EVENTS_MANAGER *_events_manager );
int subscribe_to_service( HP_EVENTS_MANAGER *_events_manager, char *_url );

#endif /* CURL_FCT_H */

