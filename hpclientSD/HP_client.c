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
 * @author rto
 */
#include "HP_client.h"

RequestService *request_service_head = NULL;
HP_EVENTS_MANAGER *events_manager_head = NULL;
pthread_mutex_t emh_mutex = PTHREAD_MUTEX_INITIALIZER;

void *exitEventsCallback(HP_EVENTS_MANAGER *_events_manager);

int write_str_buffer_to_file(char *filename, char *mybuffer) {
			//Open file
			FILE *file;
			file = fopen(filename, "w");
			if (!file) {
				fprintf(stderr, "Unable to open file %s", filename);
				//return -1;
			}
			//return -1;
			fwrite(mybuffer, strlen(mybuffer), 1, file);
			fclose(file);
			return 1;
		}

void service_found_callback(char *_avahi_name, char* _address, uint16_t _port_num, char* _tmp_url) {
	char *_request_xml_url = NULL;
	char *_subscribe_url = NULL;
	char _port[10];
	char *_service_xml = NULL;
	char *_hp_server_url = NULL;
	char *_url = NULL;
	char _service_name = NULL;
	RequestService *_matching_request_service = NULL;

	sprintf(_port, "%u", _port_num);

	char* pch = strrchr(_tmp_url, '"');
	_tmp_url[pch - _tmp_url] = '\0';

	printf("STRLEN : %d\n", strlen(_tmp_url));

	_url = (char*) malloc((strlen(_tmp_url) - strlen("\"URI=") + 1) * sizeof(char));

	strncpy(_url, &_tmp_url[strlen("\"URI=")], strlen(_tmp_url) - strlen("\"URI=") + 1);

	printf("URL : %s, %s\n", _tmp_url, _url);

	_hp_server_url = (char*) malloc((strlen("http://") + strlen(_address) + strlen(_port) + 2) * sizeof(char));
	strcpy(_hp_server_url, "http://");
	strcat(_hp_server_url, _address);
	strcat(_hp_server_url, ":");
	strcat(_hp_server_url, _port);
	/* Create URL */
	_request_xml_url = (char*) malloc((strlen(_hp_server_url) + strlen(_url) + strlen("?x=1") + 1) * sizeof(char));
	strcpy(_request_xml_url, _hp_server_url);
	strcat(_request_xml_url, _url);
	strcat(_request_xml_url, "?x=1");

	printf("HTTP GET on %s\n", _request_xml_url);
	_service_xml = http_get_function(_request_xml_url);
	if (_service_xml == NULL) {
		printf("http_get_function failed\n");
		free(_hp_server_url);
		free(_request_xml_url);
		return;
	}

	free(_request_xml_url);

	if (get_matching_request_service(_service_xml, request_service_head, &_matching_request_service) == 0) {
		HP_EVENTS_MANAGER *_iterator = NULL;

		free(_service_xml);

		_matching_request_service->value_url = (char*) malloc((strlen(_url) + 1) * sizeof(char));
		_matching_request_service->complete_url = (char*) malloc(
				(strlen(_hp_server_url) + strlen(_url) + 1) * sizeof(char));
		_matching_request_service->name = (char*) malloc((strlen(_avahi_name) + 1) * sizeof(char));

		strcpy(_matching_request_service->value_url, _url);
		strcpy(_matching_request_service->complete_url, _hp_server_url);
		strcat(_matching_request_service->complete_url, _url);

		strcpy(_matching_request_service->name, _avahi_name);
		////////////////////////////////////HERE THE CLIENT FOUND THE SERVICE YOU'RE LOOKING FOR////////////////////////////////////////////

		/**SEND A PUT REQUEST TO THE SERVICE, WITH THE VALUE TO TURN THE LAMP ON**/
		//		printf("USRL: %s", _matching_request_service->RSID );
		//		http_put_function ( _matching_request_service->RSID, get_xml_value("00887766") );
		//send_put(_matching_request_service->RSID, "00AACCDD");

		//***************//
//		printf("RSID: %s\n", _matching_request_service->RSID);
//
//		if (strcmp(_matching_request_service->RSID, "1") == 0) {
//			printf("handling temperature input\n");
//			char *get_data = http_get_function(_matching_request_service->complete_url);
//			if (get_data)
//				printf("\nGET :\n %s\n", get_data);
//			char *value = get_value_from_xml_value(get_data);
//			if (value)
//				printf("VALUE : %s\n", value);
//			float scale = 1000;
//			float temp = atof(value) / scale;
//			printf("Float VALUE : %f\n", temp);
//		}
//
//		if (strcmp(_matching_request_service->RSID, "2") == 0) {
//			printf("handling heat source\n");
//			//send_put("2", "00AACCDD"); // not working!!
//			http_put_function(_matching_request_service->complete_url, get_xml_value("00AABBFF00"));
//			printf("ebd of handling heat source\n");
//
//		}
		//*******************//

		printf("OUTPUT-URL: %s\n", _matching_request_service->complete_url);

		write_str_buffer_to_file("mainSD-OUTPUT.url", _matching_request_service->complete_url) ;


		sleep(1);
		avahi_browse_stop();
		 exit(0);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (_matching_request_service->on_value_change_head) {

			_subscribe_url
					= (char*) malloc((strlen(_hp_server_url) + strlen(_url) + strlen("?p=1") + 1) * sizeof(char));

			strcpy(_subscribe_url, _hp_server_url);
			strcat(_subscribe_url, _url);
			strcat(_subscribe_url, "?p=1\0");

			printf("Subscribe url : %s\n", _subscribe_url);

			SubscribedService *_new_subscribed_service = (SubscribedService*) malloc(sizeof(SubscribedService));
			_new_subscribed_service->request_service = _matching_request_service;

			LL_FOREACH( events_manager_head, _iterator ) {
				if (strcmp(_iterator->hp_url, _hp_server_url) == 0) {
					subscribe_to_service(_iterator, _subscribe_url);
					LL_APPEND( _iterator->subscribed_service_head, _new_subscribed_service );
					free(_subscribe_url);
					return;
				}
			}

			HP_EVENTS_MANAGER *_new_events_manager = create_events_manager(_hp_server_url);
			LL_APPEND( events_manager_head, _new_events_manager );
			LL_APPEND( _new_events_manager->subscribed_service_head, _new_subscribed_service );
			subscribe_to_service(_new_events_manager, _subscribe_url);

			return;
		}

		free(_hp_server_url);
		return;

	}

	free(_service_xml);
}

void service_removed_callback(char* _name) {
	RequestService *_iterator;
	HP_EVENTS_MANAGER *_events_iterator;
	SubscribedService *_sub_service_iterator, *_tmp;

	LL_FOREACH( request_service_head, _iterator ) {
		if (_iterator->name) {
			if (strcmp(_iterator->name, _name) == 0) {
				LL_FOREACH( events_manager_head, _events_iterator ) {
					LL_FOREACH_SAFE( _events_iterator->subscribed_service_head, _sub_service_iterator, _tmp );
{						if ( _sub_service_iterator->request_service == _iterator )
						{
							LL_DELETE( _events_iterator->subscribed_service_head, _sub_service_iterator );
							free(_sub_service_iterator);
							break;
						}
					}
				}

				if( _iterator->value_url )
				free(_iterator->value_url);
				if( _iterator->complete_url )
				free(_iterator->complete_url);

				free(_iterator->name);
			}

		}
	}

}

int send_put(char *_RSID, char *_value) {
	RequestService *_rs_iterator = NULL;
	printf("PUT start\n");

	LL_FOREACH( request_service_head, _rs_iterator ) {
		if (strcmp(_rs_iterator->RSID, _RSID) == 0) {
			if (_rs_iterator->complete_url != NULL) {
				printf("Send put to : %s, %s\n", _rs_iterator->complete_url, _value);
				http_put_function(_rs_iterator->complete_url, get_xml_value(_value));
				printf("PUT sent\n");
			}
			return 0;
		}
	}

	return -1;
}

int parse_event(char *_data_in, char** _data_out, char** _id_out) {
	char *pch;

	pch = strtok(_data_in, "<");

	*_data_out = (char*) malloc((strlen(pch) - strlen("data: ") + 1) * sizeof(char));
	strcpy(*_data_out, &pch[strlen("data: ")]);

	pch = strtok(NULL, "<");

	*_id_out = (char*) malloc((strlen(pch) - strlen("id: ") + 1) * sizeof(char));
	strcpy(*_id_out, &pch[strlen("br>id: ")]);

	return 0;
}

void events_callback(HP_EVENTS_MANAGER *_events_manager, char *_data) {
	char *_data_out = NULL, *_id_out = NULL;
	RequestService *_rs_iterator;

	parse_event(_data, &_data_out, &_id_out);

	LL_FOREACH(request_service_head, _rs_iterator) {
		if (_rs_iterator->value_url && _rs_iterator->complete_url) {
			if (strcmp(_id_out, _rs_iterator->value_url) == 0) {
				OnValueChange *_ovc_iterator;
				LL_FOREACH( _rs_iterator->on_value_change_head, _ovc_iterator ) {
					if (strcmp(_ovc_iterator->value, _data_out) == 0) {
						Action *_action_iterator;

						LL_FOREACH( _ovc_iterator->action_head, _action_iterator ) {
							if (strcmp(_action_iterator->type, "PUT") == 0) {
								printf("Send PUT\n");
								send_put(_action_iterator->RSID, _action_iterator->value);
							}
						}

						return;
					}
				}
				if (_rs_iterator->default_value_change) {
					Action *_action_iterator;

					LL_FOREACH( _rs_iterator->default_value_change->action_head, _action_iterator ) {
						if (strcmp(_action_iterator->type, "PUT") == 0) {
							printf("Send default PUT\n");
							send_put(_action_iterator->RSID, _action_iterator->value);
							return;
						}
					}
				}
			}
		}
	}
}

int HP_client_init(char *servicetypeinput , char * servicelocationinput ) {

//	if (parse_xml(XML_PATH, &request_service_head)) {
	//	printf("Parse xml error\n");
	//	return -1;
//	}

	creaters( servicetypeinput, servicelocationinput , &request_service_head);

	init_curl(events_callback, exitEventsCallback);
//printf("hello2 - after curl \n");
	if (avahi_browse_start(service_found_callback)) {
		printf("avahi_browse_start error\n");
		destroy_request_service_list(request_service_head);
		return -1;
	}
//printf("hello3 - after a browse starrt\n");
	return 0;

}

int HP_client_deinit() {
	HP_EVENTS_MANAGER *_tmp, *_iterator;

	avahi_browse_stop();

	destroy_request_service_list(request_service_head);

	LL_FOREACH_SAFE(events_manager_head, _iterator, _tmp) {
		LL_DELETE(events_manager_head, _iterator);
		destroy_events_manager(_iterator);
	}

	return 0;
}

void *exitEventsCallback(HP_EVENTS_MANAGER *_events_manager) {

	LL_DELETE( events_manager_head, _events_manager );
	destroy_events_manager(_events_manager);

}
