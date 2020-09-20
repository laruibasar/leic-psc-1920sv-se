/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */
#include <stdio.h>
#include <string.h>

#include <jansson.h>

#include "rates.h"

#define URI_START_AT "start_at=%d-%d-%d&"
#define URI_END_AT "end_at=%d-%d-%d&"
#define URI_BASE "base=%s"
#define URI_SYMBOLS "&symbols="

#define STR_DATE_SIZE 20
#define STR_BASE_SIZE 10
#define STR_SYMBOLS_SIZE 8 + 4 * 32 /* 32 currencies max */
#define STR_CURR_SIZE 3

int
get_rates(Date start_at, Date end_at,
	    unsigned int n_currencies, char *currencies[], Rates *result)
{
	size_t size;
	char *get, 
	     *url = set_url(start_at, end_at, n_currencies, currencies);

	if ((get = http_get_data(url, &size)) == NULL) {
		fprintf(stderr, "Failed to download\n");
		free(get);
		free(url);
		return E_DOWNLOAD;
	}
	free(url);

	int rates = extract_rates(get, size, result);
	free(get);
	
	if (rates < 0)
		return E_EXTR_JSON;

	result->start_at = start_at;
	result->end_at = end_at;
	return 0;
}

void
free_rates(Rates *rates)
{
	/* clear base string */
	free(rates->base);

	/* clear currency names */
	for (int i = 0; i < rates->count; i++) {
		struct rate *r = &rates->rates[i];
		for (int j = 0; j < r->count; j++) {
			struct currency *c = &r->currencies[j];
			free(c->name);
		}
	}
}

int
extract_rates(const char *data, size_t size, Rates *extract_rates)
{
	json_t *root;
	json_error_t error;

	root = json_loads(data, 0, &error);
	if (!root) {
		fprintf(stderr, "JSON error: on line %d: %s\n",
			error.line, error.text);
		return -1;
	}

	if (!json_is_object(root)) {
		fprintf(stderr, "JSON error: not an object\n");
		json_decref(root);
		return -1;
	}
	
	json_t *base;
	base = json_object_get(root, "base");
	char *base_curr = str_from_json(json_string_value(base));

	json_t *rates = json_object_get(root, "rates");
	if (!json_is_object(rates)) {
		fprintf(stderr, "JSON error: rates not an object\n");
		json_decref(root);
		return -1;
	}

	const char *key;
	json_t *value;
	size_t number_rates = json_object_size(rates);
	struct rate *result_rates = malloc(number_rates * sizeof(struct rate));
	if (result_rates == NULL) {
		fprintf(stderr, "Out of memory to store rates");
		json_decref(root);
		return -1;
	}

	int count_rate = 0;
	json_object_foreach(rates, key, value) {
		struct rate *r;
		if ((r = malloc(sizeof(struct rate))) == NULL) {
			fprintf(stderr, "Out of memory for rate");
			json_decref(root);
			return -1;
		}
		
		r->day = convert_date(key);

		size_t size = json_object_size(value);
		r->count = size;
		
		struct currency *currencies;
		if ((currencies = malloc(size * sizeof(struct currency))) == NULL) {
			fprintf(stderr, "Out of memory for rate");
			json_decref(root);
			return -1;
		}
		
		int count_curr = 0;
		const char *sub_key;
		json_t *sub_value;
		void *iter = json_object_iter(value);
		while (iter != NULL) {
			struct currency *cur = malloc(sizeof(struct currency));
			if (cur == NULL) {
				fprintf(stderr, "Out of memory for currency");
				json_decref(root);
				return -1;
			}
			sub_key = json_object_iter_key(iter);
			sub_value = json_object_iter_value(iter);
			char *name = str_from_json(sub_key);

			cur->name = name;
			cur->value = json_real_value(json_object_iter_value(iter));
		
			currencies[count_curr++] = *cur;
			iter = json_object_iter_next(value, iter);
		}
		r->currencies = currencies;
		result_rates[count_rate++] = *r;
	}

	/* cleaning json_t */
	json_decref(root);

	extract_rates->rates = result_rates;
	extract_rates->count = number_rates;
	extract_rates->base = base_curr;

	return 0;
}

Date
convert_date(const char *src_date)
{
	Date date;
	date.year = 0;
	date.month = 0;
	date.day = 0;
	
	const char *c = src_date;
	uint16_t year = 0;
	uint8_t month = 0,
		day = 0;
	
	while (*c != 0) {
		if (*c == '-')
			break;
		year = year * 10 + (*c++ - '0');
	}
	c++;

	while (*c != 0) {
		if (*c == '-')
			break;
		month = month * 10 + (*c++ - '0');
	}
	c++;

	while (*c != 0)
		day = day * 10 + (*c++ - '0');
	
	/* TODO: check month and day, for 28 or 29, 30 and 31 days */
	/* for simplicity, we only check for 31 days  */
	date.year = year;
	date.month = (month > 12) ? 0 : month;
	date.day = (day > 31) ? 0 : day;

	return date;
}

char *
set_url(Date start_at, Date end_at,
	    unsigned int n_currencies, char *currencies[])
{
	char *url;
	if ((url = malloc(API_URL_SIZE)) == NULL) {
		fprintf(stderr, "Out of memory");
		exit(1);
	}

	char start[STR_DATE_SIZE], end[STR_DATE_SIZE],
	     base[STR_BASE_SIZE], symbols[STR_SYMBOLS_SIZE]; 

	strncpy(url, API_URL, API_URL_SIZE - 1);
	
	snprintf(start, STR_DATE_SIZE, URI_START_AT, start_at.year, start_at.month, start_at.day);
	strncat(url, start, API_URL_SIZE - 1);

	snprintf(end, STR_DATE_SIZE, URI_END_AT, end_at.year, end_at.month, end_at.day);
	strncat(url, end, API_URL_SIZE - 1);
	
	snprintf(base, STR_BASE_SIZE, URI_BASE, *currencies);
	strncat(url, base, API_URL_SIZE - 1);

	if (n_currencies > 1) {
		int i = n_currencies - 1;
		char sym[STR_CURR_SIZE + 1]; /* allow for \0 */
		strncpy(symbols, URI_SYMBOLS, STR_SYMBOLS_SIZE - 1);
		
		while (i > 0) {
			int ret = snprintf(sym, sizeof(sym), "%s", currencies[i--]);
			if (ret < 0 || ret > sizeof(sym)) {
				fprintf(stderr, "Currency not well formated: %s",
					currencies[i + 1]);
				exit(1);
			}
			strncat(symbols, sym, STR_SYMBOLS_SIZE - 1);
			if (i != 0)
				strncat(symbols, ",", STR_SYMBOLS_SIZE - 1);
		}
		strncat(url, symbols, API_URL_SIZE - 1);
	}
	url[API_URL_SIZE - 1] = '\0'; /* sure termination of string */

	return url;
}

char *
str_from_json(const char *src)
{
	size_t size = (size_t)strlen(src);
	char *dst;
	if ((dst = malloc(size + 1)) == NULL) {
		fprintf(stderr, "Out of memory to copy string");
		exit(1);
	}

	memcpy(dst, src, size);
	dst[size] = '\0';

	return dst;
}
