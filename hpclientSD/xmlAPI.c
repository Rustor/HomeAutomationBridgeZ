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
#include "xmlAPI.h"

void parse_error_cleanup(RequestService *request_service_head) {
	destroy_request_service_list(request_service_head);
}

int parse_xml(char *_xml_path, RequestService **request_service_head) {
	FILE *fp;
	mxml_node_t *tree;
	mxml_node_t *rs_list_node;
	mxml_node_t *rs_node;
	mxml_node_t *ovc_node;
	mxml_node_t *action_node;

	fp = fopen(_xml_path, "r");

	if (fp == NULL)
		return -1;

	tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	if (tree == NULL) {
		fclose(fp);
		return -1;
	}

	rs_list_node = mxmlFindElement(tree, tree, "RequestServiceList", NULL, NULL, MXML_DESCEND);
	if (rs_list_node == NULL) {
		mxmlDelete(tree);
		fclose(fp);
		return -1;
	}

	for (rs_node = mxmlFindElement(rs_list_node, rs_list_node, "RequestService", NULL, NULL, MXML_DESCEND); rs_node
			!= NULL; rs_node = mxmlFindElement(rs_node, rs_list_node, "RequestService", NULL, NULL, MXML_DESCEND)) {
		RequestService *rs_tmp = (RequestService*) malloc(sizeof(RequestService));
		if (rs_tmp == NULL) {
			parse_error_cleanup(*request_service_head);
			mxmlDelete(tree);
			fclose(fp);
			return -1;
		}

		const char *attr = NULL;

		init_request_service_struct(rs_tmp);

		attr = mxmlElementGetAttr(rs_node, "DeviceType");
		if (attr != NULL) {
			rs_tmp->device_type = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (rs_tmp->device_type == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}

			strcpy(rs_tmp->device_type, attr);
		}

		attr = mxmlElementGetAttr(rs_node, "DeviceID");
		if (attr != NULL) {
			rs_tmp->device_ID = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (rs_tmp->device_ID == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}

			strcpy(rs_tmp->device_ID, attr);
		}

		attr = mxmlElementGetAttr(rs_node, "ServiceType");
		if (attr != NULL) {
			rs_tmp->type = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (rs_tmp->type == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}
			strcpy(rs_tmp->type, attr);
		}

		attr = mxmlElementGetAttr(rs_node, "ServiceID");
		if (attr != NULL) {
			rs_tmp->ID = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (rs_tmp->ID == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}
			strcpy(rs_tmp->ID, attr);
		}

		attr = mxmlElementGetAttr(rs_node, "Location");
		if (attr != NULL) {
			rs_tmp->location = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (rs_tmp->location == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}
			strcpy(rs_tmp->location, attr);
		}

		attr = mxmlElementGetAttr(rs_node, "RSID");
		if (attr == NULL) {
			parse_error_cleanup(*request_service_head);
			destroy_request_service_struct(rs_tmp);
			mxmlDelete(tree);
			fclose(fp);
			return -1;
		}

		rs_tmp->RSID = (char*) malloc(strlen(attr) * sizeof(char) + 1);
		if (rs_tmp->RSID == NULL) {
			parse_error_cleanup(*request_service_head);
			destroy_request_service_struct(rs_tmp);
			mxmlDelete(tree);
			fclose(fp);
			return -1;
		}
		strcpy(rs_tmp->RSID, attr);

		for (ovc_node = mxmlFindElement(rs_node, rs_node, "OnValueChange", NULL, NULL, MXML_DESCEND); ovc_node != NULL; ovc_node
				= mxmlFindElement(ovc_node, rs_node, "OnValueChange", NULL, NULL, MXML_DESCEND)) {
			OnValueChange *onValueChange = (OnValueChange*) malloc(sizeof(OnValueChange));
			if (onValueChange == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}

			init_on_value_change_struct(onValueChange);

			attr = mxmlElementGetAttr(ovc_node, "value");
			if (attr == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_on_value_change_struct(onValueChange);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}

			onValueChange->value = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (onValueChange->value == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_on_value_change_struct(onValueChange);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}
			strcpy(onValueChange->value, attr);

			for (action_node = mxmlFindElement(ovc_node, ovc_node, "Action", NULL, NULL, MXML_DESCEND); action_node
					!= NULL; action_node = mxmlFindElement(action_node, ovc_node, "Action", NULL, NULL, MXML_DESCEND)) {
				Action *action = (Action*) malloc(sizeof(Action));
				if (action == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				init_action_struct(action);

				attr = mxmlElementGetAttr(action_node, "Type");
				if (attr == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_action_struct(action);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				action->type = (char*) malloc(strlen(attr) * sizeof(char) + 1);
				if (action->type == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_action_struct(action);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				strcpy(action->type, attr);

				attr = mxmlElementGetAttr(action_node, "RSID");
				if (attr == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_action_struct(action);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				action->RSID = (char*) malloc(strlen(attr) * sizeof(char));
				if (action->RSID == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_action_struct(action);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				strcpy(action->RSID, attr);

				attr = mxmlElementGetAttr(action_node, "value");
				if (attr == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_action_struct(action);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				action->value = (char*) malloc(strlen(attr) * sizeof(char));
				if (action->value == NULL) {
					parse_error_cleanup(*request_service_head);
					destroy_action_struct(action);
					destroy_on_value_change_struct(onValueChange);
					destroy_request_service_struct(rs_tmp);
					mxmlDelete(tree);
					fclose(fp);
					return -1;
				}
				strcpy(action->value, attr);

				LL_APPEND( onValueChange->action_head, action );

			}
			if (strcmp(onValueChange->value, "default") == 0)
				rs_tmp->default_value_change = onValueChange;
			else
				LL_APPEND( rs_tmp->on_value_change_head, onValueChange );

		}

		LL_APPEND( *request_service_head, rs_tmp );

	}

	mxmlDelete(tree);
	fclose(fp);

	return 0;

}

int creaters(char *servicetypeinput, char * servicelocationinput, RequestService **request_service_head) {
	FILE *fp;
	mxml_node_t *tree;
	mxml_node_t *rs_list_node;
	mxml_node_t *rs_node;
	mxml_node_t *ovc_node;
	mxml_node_t *action_node;
	RequestService *rs_tmp = (RequestService*) malloc(sizeof(RequestService));

	if (rs_tmp == NULL) {
		parse_error_cleanup(*request_service_head);
		mxmlDelete(tree);
		fclose(fp);
		return -1;
	}

	const char *attr = NULL;

	init_request_service_struct(rs_tmp);

	attr = servicetypeinput;

	//attr = mxmlElementGetAttr(rs_node, "ServiceType");
	if (attr != NULL) {
		rs_tmp->type = (char*) malloc(strlen(attr) * sizeof(char) + 1);
		if (rs_tmp->type == NULL) {
			parse_error_cleanup(*request_service_head);
			destroy_request_service_struct(rs_tmp);
			mxmlDelete(tree);
			fclose(fp);
			return -1;
		}
		strcpy(rs_tmp->type, attr);
	}

	attr = servicelocationinput;
	//attr = mxmlElementGetAttr(rs_node, "Location");
	if (attr != NULL) {
		rs_tmp->location = (char*) malloc(strlen(attr) * sizeof(char) + 1);
		if (rs_tmp->location == NULL) {
			parse_error_cleanup(*request_service_head);
			destroy_request_service_struct(rs_tmp);
			mxmlDelete(tree);
			fclose(fp);
			return -1;
		}
		strcpy(rs_tmp->location, attr);
	}

	//attr = mxmlElementGetAttr(action_node, "RSID");
	//attr = mxmlElementGetAttr(rs_node, "RSID");
			if (attr == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}

			rs_tmp->RSID = (char*) malloc(strlen(attr) * sizeof(char) + 1);
			if (rs_tmp->RSID == NULL) {
				parse_error_cleanup(*request_service_head);
				destroy_request_service_struct(rs_tmp);
				mxmlDelete(tree);
				fclose(fp);
				return -1;
			}
			strcpy(rs_tmp->RSID, "1");



	LL_APPEND( *request_service_head, rs_tmp );

	return 0;

}

int get_matching_request_service(char *_service_xml, RequestService *_request_service_head,
		RequestService **_request_service_match_out) {
	mxml_node_t *tree;
	mxml_node_t *service_node;
	mxml_node_t *device_node;
	RequestService *iterator;
	const char *attr = NULL;
	int found = 1;

	if (_service_xml == NULL)
		return -1;

	tree = mxmlLoadString(NULL, _service_xml, MXML_TEXT_CALLBACK);
	if (tree == NULL) {
		return -1;
	}

	service_node = mxmlFindElement(tree, tree, "service", NULL, NULL, MXML_DESCEND);
	if (service_node == NULL) {
		mxmlDelete(tree);
		return -1;
	}

	LL_FOREACH( _request_service_head, iterator ) {
		found = 1;

		if (iterator-> ID) {
			attr = mxmlElementGetAttr(service_node, "id");
			if (attr == NULL) {
				mxmlDelete(tree);
				return -1;
			}
			if (strcmp(iterator->ID, attr) != 0) {
				found = 0;
			}
		}
		if (found && iterator->type) {
			attr = mxmlElementGetAttr(service_node, "type");
			if (attr == NULL) {
				mxmlDelete(tree);
				return -1;
			}

			if (strcmp(iterator->type, attr) != 0)
				found = 0;
		}
		if (found && (iterator->device_ID || iterator->device_type || iterator->location)) {
			device_node = mxmlFindElement(service_node, service_node, "device", NULL, NULL, MXML_DESCEND);
			if (device_node == NULL) {
				mxmlDelete(tree);
				return -1;
			}

			if (iterator->device_ID) {
				attr = mxmlElementGetAttr(device_node, "id");
				if (attr == NULL) {
					mxmlDelete(tree);
					return -1;
				}

				if (strcmp(iterator->device_ID, attr) != 0)
					found = 0;
			}

			if (found && iterator->device_type) {
				attr = mxmlElementGetAttr(device_node, "type");
				if (attr == NULL) {
					mxmlDelete(tree);
					return -1;
				}

				if (strcmp(iterator->device_type, attr) != 0)
					found = 0;
			}

			if (found && iterator->location) {
				attr = mxmlElementGetAttr(device_node, "location");
				if (attr == NULL) {
					mxmlDelete(tree);
					return -1;
				}

				if (strcmp(iterator->location, attr) != 0)
					found = 0;
			}

		}

		if (found) {
			*_request_service_match_out = (RequestService*) malloc(sizeof(RequestService));
			copy_request_service(iterator, *_request_service_match_out);
			return 0;
		}

	}

	return -1;
}

char * get_xml_value(char* value) {
	mxml_node_t *xml;
	mxml_node_t *xml_value;

	xml = mxmlNewXML("1.0");
	xml_value = mxmlNewElement(xml, "value");
	mxmlNewText(xml_value, 0, value);

	char* return_value = mxmlSaveAllocString(xml, MXML_NO_CALLBACK);

	mxmlDelete(xml);

	return return_value;
}

char*
get_value_from_xml_value(char* _xml_value) {
	mxml_node_t *xml;
	mxml_node_t *node;

	xml = mxmlLoadString(NULL, _xml_value, MXML_TEXT_CALLBACK);
	if (xml == NULL) {
		printf("XML value format uncompatible with HomePort\n");
		return NULL;
	}

	node = mxmlFindElement(xml, xml, "value", NULL, NULL, MXML_DESCEND);
	if (node == NULL || node->child == NULL) {
		mxmlDelete(xml);
		printf("No \"value\" in the XML file\n");
		return NULL;
	}

	char *return_value = malloc(sizeof(char) * (strlen(node->child->value.text.string) + 1));
	strcpy(return_value, node->child->value.text.string);

	mxmlDelete(xml);

	return return_value;
}
