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
/*
 * xmlhttputils.c - this is an small exert from the hpclientSD code
 */
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <pthread.h>
#include <mxml.h>

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

char * get_xml_value(char* return_value) {
	mxml_node_t *xml;
	mxml_node_t *xml_value;

	xml = mxmlNewXML("1.0");
	xml_value = mxmlNewElement(xml, "value");
	mxmlNewText(xml_value, 0, return_value);

	return_value = mxmlSaveAllocString(xml, MXML_NO_CALLBACK);

	mxmlDelete(xml);

	return return_value;

}

struct CBC {
	char *buf;
	size_t pos;
	size_t size;
};

CURL *get_c;
CURL *put_c;

char *get_buf = NULL;
char *get_events_buf = NULL;

struct CBC *get_cbc = NULL;
struct CBC *put_cbc = NULL;

static size_t putBuffer(void *ptr, size_t size, size_t nmemb, void *cbx) {
	struct CBC *cbc = cbx;

	int length = cbc->size - cbc->pos;
	if (length > size * nmemb) {
		printf("Read_callback : Data to long\n");
		length = size * nmemb;
	}
	memcpy(ptr, &cbc->buf[cbc->pos], length);
	cbc->pos += length;
	return length;
}

static size_t copyBuffer(void *ptr, size_t size, size_t nmemb, void *ctx) {
	struct CBC *cbc = ctx;

	if (cbc->pos + size * nmemb > cbc->size)
		return 0; /* overflow */
	memcpy(&cbc->buf[cbc->pos], ptr, size * nmemb);
	cbc->pos += size * nmemb;
	return size * nmemb;
}

int init_curl(void) {

	get_c = curl_easy_init();
	put_c = curl_easy_init();

	if (!get_c || !put_c)
		return -1;

	get_buf = (char*) malloc(2048 * sizeof(char));

	get_cbc = (struct CBC*) malloc(sizeof(struct CBC));
	put_cbc = (struct CBC*) malloc(sizeof(struct CBC));

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

pthread_mutex_t get_mutex = PTHREAD_MUTEX_INITIALIZER;

char * http_get_function(char *_url) {
	pthread_mutex_lock(&get_mutex);

	char *_get_data = NULL;
	CURLcode errornum;

	curl_easy_setopt (get_c, CURLOPT_URL, _url);

	get_cbc->buf = get_buf;
	get_cbc->pos = 0;

	if (CURLE_OK != (errornum = curl_easy_perform(get_c))) {
		fprintf(stderr, "GET curl_easy_perform failed: `%s'\n", curl_easy_strerror(errornum));
		long http_code;
		curl_easy_getinfo(get_c, CURLINFO_RESPONSE_CODE, &http_code);
		printf("HTTP error : %ld\n", http_code);
		pthread_mutex_unlock(&get_mutex);
		return NULL;
	}

	_get_data = (char*) malloc((get_cbc->pos + 1) * sizeof(char));

	strncpy(_get_data, get_cbc->buf, get_cbc->pos);
	_get_data[get_cbc->pos] = '\0';

	pthread_mutex_unlock(&get_mutex);

	return _get_data;
}
