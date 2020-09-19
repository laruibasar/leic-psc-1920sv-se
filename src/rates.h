/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */

#define API_URL "https://api.exchangeratesapi.io/history?"
#define API_URL_SIZE 256 /* url max size aprox 96 + 96 */ 

#define E_DOWNLOAD -1
#define E_EXTR_JSON -2
#define E_OOM -3

typedef struct date {
	uint16_t year;
	uint8_t month;
	uint8_t day;
} Date;

struct currency {
	char *name;
	float value;
};

struct rate {
	Date  day;
	size_t count;
	struct currency *currencies;
};

typedef struct rates {
	Date start_at;
	Date end_at;
	struct rate *rates;
	size_t count;
	char *base;
} Rates;

/* 
 * Generate HTTP GET request from the url in the parameters
 * return the pointer to the response and the data size
 */
char	*http_get_data(const char*, size_t*);

/*
 * Get exchanges rates from the api.
 */
int	 get_rates(Date, Date, unsigned int, char**, Rates*);
void	 free_rates(Rates*);
char	*set_url(Date, Date, unsigned int, char**);

/*
 * Convert strings to Date object.
 * Error when month or day = 0.
 */
Date	convert_date(const char*);

/* 
 * Extract data from the request buffer and convert to Rates
 * using jansson lib.
 * Return errors for values lesser 0
 */
int	extract_rates(const char*, size_t, Rates*);

/* 
 * Create new string from json_t string types
 * because this are free by lib as documented in
 * jansson website
 */
char	*str_from_json(const char*);
