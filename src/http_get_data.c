/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <curl/curl.h>

#include "rates.h"

/* organize data from response */
struct data {
	char *response;
	size_t size;
};

/* callback function to pass to curl to save data received */
size_t	write_to_buffer(char*, size_t, size_t, void*);

char *
http_get_data(const char* url, size_t *data_size)
{
	CURL *curl;
	CURLcode response;
	curl_global_init(CURL_GLOBAL_ALL);
	
	char *result;
	struct data data; /* store the incoming data from request */
	data.response = malloc(1);	/* grow as needed */
	data.size = 0;

	curl = curl_easy_init();
	if (curl == NULL) {
		fprintf(stderr, "Failed to create curl request: %serror: %s\n",
				url, curl_easy_strerror(errno));
		curl_easy_cleanup(curl);
		return NULL;
	}

	/* set up curl */
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);

	response = curl_easy_perform(curl);
	if (response != CURLE_OK) {
		fprintf(stderr, "Failed to connect: %s\terror: %s\n",
				url, curl_easy_strerror(errno));
		result = NULL;
	} else {
		*data_size = data.size;
		result = data.response;
	}

	curl_easy_cleanup(curl);

	result[data.size] = '\0'; /* \0 terminate data buffer  */

	return result;
}

size_t
write_to_buffer(char *data, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	struct data *d = (struct data *) userdata;

	char *ptr = realloc(d->response, d->size + realsize + 1);
	if (ptr == NULL)
		return -1; /* out of memory */

	d->response = ptr;
	memcpy(&(d->response[d->size]), data, realsize);
	d->size += realsize;
	d->response[d->size] = 0;

	return realsize;
}
