/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rates.h"

void	usage(void);
void	csv_print_header(FILE*, unsigned int, char**);
void	csv_print_line(FILE*, unsigned int, char**, struct rate*);

int
main(int argc, char *argv[])
{
	if (argc < 4) {
		usage();
		exit(-1);
	}

	FILE *fp;
	if ((fp = fopen("download.csv", "w")) == NULL) {
		fprintf(stderr, "Failed to open file to write");
		fclose(fp);
		exit(1);
	}

	Rates rates;
	rates.base = NULL;
	rates.count = 0;
	rates.rates = NULL;

	Date start_at = convert_date(argv[1]);
	Date end_at = convert_date(argv[2]);

	unsigned int n_curr = argc - 3; /* remove first args, keep base curr */	
	char *currencies[n_curr];

	for (int i = 0; i < n_curr; i++)  
		currencies[i] = argv[3 + i];

	if (get_rates(start_at, end_at, n_curr, currencies, &rates) < 0) {
		fprintf(stderr, "Failed to get rates.\n");
		exit(1);
	}

	/* write to buffer */
	if (n_curr > 1) {
		csv_print_header(fp, n_curr, currencies);
		for (int i = 0; i < rates.count; i++) {
			struct rate r = rates.rates[i];
			csv_print_line(fp, n_curr, currencies, &r);
		}
	} else {
		n_curr = rates.rates[0].count + 1;

		char *total_currencies[n_curr];
		total_currencies[0] = currencies[0]; 

		for (int i = 1; i < n_curr; i++)
			total_currencies[i] = rates
				.rates[0].currencies[i - 1].name;

		csv_print_header(fp, n_curr, total_currencies); 
		
		for (int i = 0; i < rates.count; i++) {
			struct rate r = rates.rates[i];
			csv_print_line(fp, n_curr, total_currencies, &r);
		}
	}

	fclose(fp);
	free_rates(&rates);

	exit(0);
}

void
usage()
{
	printf("usage: rates start_date end_date base [currency1 currency2 ...]\n");
	printf("\t start_date: yyyy-mm-dd\n");
	printf("\t end_date: yyyy-mm-dd\n");
}

void
csv_print_header(FILE *fp, unsigned int count, char *currencies[])
{
	fprintf(fp, "%s", currencies[0]);
	for (int i = 1; i < count; i++) {
		fprintf(fp, ",");
		fprintf(fp, "%s", currencies[i]);
	}
	fprintf(fp, "\n");
}

void
csv_print_line(FILE *fp, unsigned int n_curr, char *currencies[], 
		struct rate *rate)
{
	fprintf(fp, "%d-%.2d-%.2d",
		rate->day.year, rate->day.month, rate->day.day);

	int curr = 1;
	while (curr < n_curr) {
		fprintf(fp, ",");
		for (int i = 0; i < rate->count; i++) {
			if (strcmp(currencies[curr], 
				rate->currencies[i].name) == 0) {
				struct currency c = rate->currencies[i];
				fprintf(fp, "%.4f", c.value);
			}
		}
		curr++;
	}
	fprintf(fp, "\n");
}

