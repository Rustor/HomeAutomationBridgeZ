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
#include "curl_fct.h"
#include "HP_client.h"
#include <stdio.h>
#include <string.h>

#define COOKIE_FILE "temp.cookie"

#define SSL 0

CURL *get_c;
CURL *put_c;

EventsFunction events_function;
GetEventsExit exit_events_callback;

pthread_mutex_t cookie_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t share_mutex = PTHREAD_MUTEX_INITIALIZER;
#if SSL == 1
pthread_mutex_t ssl_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

pthread_mutex_t put_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t get_mutex = PTHREAD_MUTEX_INITIALIZER;

char *get_buf=NULL;
char *get_events_buf=NULL;

int quit = 0;

struct CBC *get_cbc=NULL;
struct CBC *put_cbc=NULL;
struct CBC_events *get_events_cbc=NULL;

static
void share_lock_function(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr)
{

	switch ( data ) {
		case CURL_LOCK_DATA_SHARE:
			pthread_mutex_lock(&share_mutex);
			break;
		case CURL_LOCK_DATA_DNS:
			//pthread_mutex_lock(&dns_mutex);
			break;
		case CURL_LOCK_DATA_COOKIE:
			pthread_mutex_lock(&cookie_mutex);
			break;
#if SSL == 1
		case CURL_LOCK_DATA_SSL_SESSION:
			pthread_mutex_lock(&ssl_mutex);
			break;
#endif
		default:
			fprintf(stderr, "lock: no such data: %d\n", (int)data);
			return;
	}

}

static 
void share_unlock_function(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr)
{

	switch ( data ) {
		case CURL_LOCK_DATA_SHARE:
			pthread_mutex_unlock(&share_mutex);
			break;
		case CURL_LOCK_DATA_DNS:
			//pthread_mutex_unlock(&dns_mutex);
			break;
		case CURL_LOCK_DATA_COOKIE:
			pthread_mutex_unlock(&cookie_mutex);
			break;
#if SSL == 1
		case CURL_LOCK_DATA_SSL_SESSION:
			pthread_mutex_unlock(&ssl_mutex);
			break;
#endif
		default:
			fprintf(stderr, "lock: no such data: %d\n", (int)data);
			return;
	}

}

static size_t putBuffer(void *ptr, size_t size, size_t nmemb, void *cbx)
{
	struct CBC *cbc = cbx;
	
	int length = cbc->size - cbc->pos;
	if(length > size * nmemb)
	{
		printf("Read_callback : Data to long\n");
		length = size*nmemb; 
	}
	memcpy(ptr, &cbc->buf[cbc->pos], length);
	cbc->pos += length;
	return length;
}

static size_t
copyBuffer (void *ptr, size_t size, size_t nmemb, void *ctx)
{
	struct CBC *cbc = ctx;

	if (cbc->pos + size * nmemb > cbc->size)
		return 0;                   /* overflow */
	memcpy (&cbc->buf[cbc->pos], ptr, size * nmemb);
	cbc->pos += size * nmemb;
	return size * nmemb;
}

int progress_fct(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	HP_EVENTS_MANAGER *_events_manager = (HP_EVENTS_MANAGER*) clientp;

	pthread_mutex_lock( _events_manager->exit_mutex );
	
	if( _events_manager->exit == 1 )
	{
		pthread_mutex_unlock( _events_manager->exit_mutex );
		return 1;
	}

	pthread_mutex_unlock( _events_manager->exit_mutex );
	
	return 0;
	
}

static size_t
copyBufferEvents (void *ptr, size_t size, size_t nmemb, void *ctx)
{
	printf("EVENTS %s\n", (char*)ptr);
	
	events_function((HP_EVENTS_MANAGER*)ctx, (char*)ptr);
	
	return size * nmemb;
}

HP_EVENTS_MANAGER * create_events_manager(char *_hp_server_url)
{

	if( _hp_server_url == NULL)
		return NULL;
	
	HP_EVENTS_MANAGER *_events_manager = (HP_EVENTS_MANAGER*)malloc(sizeof(HP_EVENTS_MANAGER));
	if( !_events_manager )
		return NULL;

	_events_manager->exit = 0;

	_events_manager->nb_subscribed = 0;

	_events_manager->started = 0;

	_events_manager->hp_url = _hp_server_url;

	_events_manager->subscribed_service_head = NULL;

	if( !_events_manager->hp_url )
	{
		free(_events_manager);
		return NULL;
	}

	_events_manager->cbc_buf = (char*)malloc(2048*sizeof(char));
	if( !_events_manager->cbc_buf )
	{
		free(_events_manager->hp_url);
		free(_events_manager);
		return NULL;
	}
	
	_events_manager->cbc = (struct CBC*)malloc(sizeof(struct CBC));
	if( !_events_manager->cbc )
	{
		free( _events_manager->cbc_buf );
		free(_events_manager->hp_url);
		free( _events_manager );
		return NULL;
	}
	
	_events_manager->cbc->buf = _events_manager->cbc_buf;
	_events_manager->cbc->pos = 0;
	_events_manager->cbc->size = 2048;

	_events_manager->get_events_c = curl_easy_init();
	if( !_events_manager->get_events_c )
	{
		free( _events_manager->cbc );
		free( _events_manager->cbc_buf );
		free(_events_manager->hp_url);
		free( _events_manager );
		return NULL;
	}
	
	_events_manager->get_subscription_c = curl_easy_init();
	if( !_events_manager->get_subscription_c )
	{
		curl_easy_cleanup( _events_manager->get_events_c );
		free( _events_manager->cbc );
		free( _events_manager->cbc_buf );
		free(_events_manager->hp_url);
		free( _events_manager );
		return NULL;
	}
	
	_events_manager->share_c = curl_share_init();
	if( !_events_manager->share_c )
	{
		curl_easy_cleanup( _events_manager->get_subscription_c );
		curl_easy_cleanup( _events_manager->get_events_c );
		free( _events_manager->cbc );
		free( _events_manager->cbc_buf );
		free(_events_manager->hp_url);
		free( _events_manager );
		return NULL;
	}
	   
	curl_share_setopt(_events_manager->share_c, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
	curl_share_setopt(_events_manager->share_c, CURLSHOPT_LOCKFUNC, &share_lock_function);
	curl_share_setopt(_events_manager->share_c, CURLSHOPT_UNLOCKFUNC, &share_unlock_function);

	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_WRITEFUNCTION, &copyBuffer);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_WRITEDATA, _events_manager->cbc);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_FAILONERROR, 1);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_TIMEOUT, 150L);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_CONNECTTIMEOUT, 15L);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt (_events_manager->get_subscription_c, CURLOPT_SHARE, _events_manager->share_c);

	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_WRITEFUNCTION, &copyBufferEvents);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_WRITEDATA, _events_manager);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_FAILONERROR, 1);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_SHARE, _events_manager->share_c);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_PROGRESSFUNCTION, &progress_fct);
	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_PROGRESSDATA, _events_manager);

	_events_manager->exit_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if( !_events_manager->exit_mutex )
	{
		curl_easy_cleanup( _events_manager->get_subscription_c );
		curl_easy_cleanup( _events_manager->get_events_c );
		curl_share_cleanup( _events_manager->share_c );
		free( _events_manager->cbc );
		free( _events_manager->cbc_buf );
		free(_events_manager->hp_url);
		free( _events_manager );
		return NULL;
	}

	pthread_mutex_init( _events_manager->exit_mutex, NULL );

	_events_manager->events_thread = NULL;

	return _events_manager;

}

int destroy_events_manager( HP_EVENTS_MANAGER *_events_manager )
{
	SubscribedService *_iterator, *_tmp; 
	
	if( _events_manager )
	{

		if( _events_manager->events_thread )
		{
			if( pthread_equal( pthread_self(), *_events_manager->events_thread ) == 0 )
			{
				pthread_mutex_lock( _events_manager->exit_mutex );

				_events_manager->exit = 1;

				pthread_mutex_unlock( _events_manager->exit_mutex );

				pthread_join( *_events_manager->events_thread, NULL );
			}
		}

		if( _events_manager->subscribed_service_head )
		{
			LL_FOREACH_SAFE(_events_manager->subscribed_service_head, _iterator, _tmp)
			{
				LL_DELETE(_events_manager->subscribed_service_head, _iterator);
				free(_iterator);
			}
		}

		if( _events_manager->hp_url )
			free( _events_manager->hp_url );
		
		if( _events_manager->cbc_buf )
			free( _events_manager->cbc_buf );

		if( _events_manager->cbc )
			free( _events_manager->cbc );

		if( _events_manager->get_events_c )
			curl_easy_cleanup( _events_manager->get_events_c );

		if( _events_manager->get_subscription_c )
			curl_easy_cleanup( _events_manager->get_subscription_c );

		if( _events_manager->share_c )
			curl_share_cleanup( _events_manager->share_c );

		if( _events_manager->exit_mutex )
		{
			pthread_mutex_destroy( _events_manager->exit_mutex );
			free( _events_manager->exit_mutex );
		}

		if ( _events_manager->events_thread )
			free( _events_manager->events_thread );

		free( _events_manager );

	    return 0;
	}

	return -1;
}

int init_curl(EventsFunction _e, GetEventsExit _exit_events)
{
	events_function = _e;
	exit_events_callback = _exit_events;
	
	get_c = curl_easy_init();
	put_c = curl_easy_init();

	if( !get_c || !put_c )
		return -1;

	get_buf=(char*)malloc(2048*sizeof(char));

	get_cbc = (struct CBC*)malloc(sizeof(struct CBC));
	put_cbc = (struct CBC*)malloc(sizeof(struct CBC));
	
	get_cbc->size = 2048;
	get_cbc->pos = 0;

	put_cbc->buf = NULL;
	put_cbc->size = 0;
	put_cbc->pos = 0;

	curl_easy_setopt (get_c, CURLOPT_WRITEFUNCTION, &copyBuffer);
	curl_easy_setopt (get_c, CURLOPT_WRITEDATA, get_cbc);
	curl_easy_setopt (get_c, CURLOPT_FAILONERROR, 1);
	//curl_easy_setopt (get_c, CURLOPT_TIMEOUT, 150L);
	//curl_easy_setopt (get_c, CURLOPT_CONNECTTIMEOUT, 150L);
	curl_easy_setopt (get_c, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	//curl_easy_setopt (get_c, CURLOPT_NOSIGNAL, 1);

	curl_easy_setopt (put_c, CURLOPT_WRITEFUNCTION, &copyBuffer);
	curl_easy_setopt (put_c, CURLOPT_WRITEDATA, get_cbc);
	curl_easy_setopt (put_c, CURLOPT_READFUNCTION, &putBuffer);
	curl_easy_setopt (put_c, CURLOPT_READDATA, put_cbc);
	curl_easy_setopt (put_c, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt (put_c, CURLOPT_PUT, 1L);
	curl_easy_setopt (put_c, CURLOPT_FAILONERROR, 1);
	curl_easy_setopt (put_c, CURLOPT_TIMEOUT, 150L);
	curl_easy_setopt (put_c, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	//curl_easy_setopt (put_c, CURLOPT_CONNECTTIMEOUT, 15L);
	curl_easy_setopt (put_c, CURLOPT_NOSIGNAL, 1);

	return 0;
}

int deinit_curl()
{
	curl_easy_cleanup(get_c);
	curl_easy_cleanup(put_c);

	free(get_buf);
	free(get_cbc);
	free(put_cbc);

	return 0;
}

char * http_get_function(char *_url)
{
	pthread_mutex_lock( &get_mutex );
	
	char *_get_data=NULL;
	CURLcode errornum;

	curl_easy_setopt (get_c, CURLOPT_URL, _url);

	get_cbc->buf=get_buf;
	get_cbc->pos = 0;

	if (CURLE_OK != (errornum = curl_easy_perform (get_c)))
	{
		fprintf (stderr,
		         "GET curl_easy_perform failed: `%s'\n",
		         curl_easy_strerror (errornum));
		long http_code;
		curl_easy_getinfo(get_c, CURLINFO_RESPONSE_CODE, &http_code);
		printf("HTTP error : %d\n", http_code);
		pthread_mutex_unlock( &get_mutex );
		return NULL;
	}	

	_get_data = (char*)malloc( ( get_cbc->pos +1 ) * sizeof(char));

	strncpy(_get_data, get_cbc->buf, get_cbc->pos);
	_get_data[get_cbc->pos] = '\0';

	pthread_mutex_unlock( &get_mutex );

	return _get_data;
}

char * http_put_function(char *_url, char *_data)
{
	pthread_mutex_lock( &put_mutex );
	
	char *_received_data=NULL;
	CURLcode errornum;
	
	get_cbc->pos=0;
	
	put_cbc->buf = _data;
    put_cbc->size = strlen(_data);
    put_cbc->pos = 0;

	if(put_c) 
	{
		curl_easy_setopt (put_c, CURLOPT_URL, _url);

		if (CURLE_OK != (errornum = curl_easy_perform (put_c)))
		{
			fprintf (stderr,
			         "PUT curl_easy_perform failed: `%s'\n",
			         curl_easy_strerror (errornum));
			long http_code;
			curl_easy_getinfo(put_c, CURLINFO_RESPONSE_CODE, &http_code);
			printf("HTTP error : %d\n", http_code);
			pthread_mutex_unlock( &put_mutex );
			return NULL;
		};

		_received_data = (char*)malloc((put_cbc->pos + 1) * sizeof(char));

		strncpy(_received_data, put_cbc->buf, put_cbc->pos);

		put_cbc->pos = 0;

		pthread_mutex_unlock( &put_mutex );

		return _received_data;
		//return put_cbc->buf;
		//return NULL;
	}

	pthread_mutex_unlock( &put_mutex );

	return NULL;
}

static
int start_events_thread( HP_EVENTS_MANAGER *_events_manager )
{	
	if( _events_manager->nb_subscribed  == 0 )
		return -1;
	
	_events_manager->events_thread = (pthread_t*)realloc(_events_manager->events_thread, sizeof(pthread_t));

	pthread_create( _events_manager->events_thread, NULL, http_get_events_function, _events_manager );

	return 0;

}

int subscribe_to_service( HP_EVENTS_MANAGER *_events_manager, char *_url )
{
	CURLcode errornum;
	
	if( !_events_manager || !_url )
		return -1;

	curl_easy_setopt( _events_manager->get_subscription_c, CURLOPT_URL, _url );

	if( CURLE_OK != (errornum = curl_easy_perform( _events_manager->get_subscription_c ) ) )
	{
		fprintf (stderr,
		         "subscribe_to_service curl_easy_perform failed: `%s'\n",
		         curl_easy_strerror (errornum));
		long http_code;
		curl_easy_getinfo(_events_manager->get_events_c, CURLINFO_RESPONSE_CODE, &http_code);
		printf("HTTP error : %d\n", http_code);
		return -1;		
	}

	_events_manager->nb_subscribed++;

	if( _events_manager->events_thread == NULL )
	{
		printf("Start thread\n");
		start_events_thread ( _events_manager );
	}
	return 0;

}

void * http_get_events_function(void *_data_pointer, GetEventsExit exit_callback)
{
	HP_EVENTS_MANAGER *_events_manager = (HP_EVENTS_MANAGER*) _data_pointer;
	CURLcode errornum;
	char *_events_url = NULL;
	long http_code;

	_events_url = (char*)malloc( ( strlen(_events_manager->hp_url) + strlen("/events") + 1 ) * sizeof(char) );
	if( !_events_url )
		return;

	
	strcpy( _events_url, _events_manager->hp_url );
	strcat( _events_url, "/events\0" );

	curl_easy_setopt (_events_manager->get_events_c, CURLOPT_URL,(char*) _events_url);

	printf("START of events\n");

	if (CURLE_OK != (errornum = curl_easy_perform(_events_manager->get_events_c)))
	{
		fprintf (stderr,
		         "GET events curl_easy_perform failed: %d `%s'\n",
		         errornum, curl_easy_strerror (errornum));
		switch( errornum )
		{
			case CURLE_PARTIAL_FILE :
				exit_events_callback(_events_manager);
				printf("EVENTSMANAGERDESTROYED");
				break;
			case CURLE_HTTP_RETURNED_ERROR :	
				curl_easy_getinfo(_events_manager->get_events_c, CURLINFO_RESPONSE_CODE, &http_code);
				printf("HTTP error : %d\n", http_code);
				break;
			case CURLE_ABORTED_BY_CALLBACK :
				break;
			default :
				printf("UNKNOWN\n");
		}

		return;
	}

	printf("END of events\n");

	return;
}
