
/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>

#include "rates.h"

void	usage(void);

int
main(int argc, char *argv[])
{
	if (argc < 4) {
		usage();
		exit(-1);
	}

	Rates rates;
	rates.base = NULL;
	rates.count = 0;
	rates.rates = NULL;

	Date start_at, end_at;
	unsigned int n_curr = argc - 3; /* remove first args, keep base curr */	
	int status;
	char *start, *end, *currencies[n_curr];

	start = argv[1];
	end = argv[2];
	
	start_at = convert_date(start);
	end_at = convert_date(end);

	for (int i = 0; i < n_curr; i++) 
		currencies[i] = argv[3 + i];

	if ((status = get_rates(start_at, end_at, n_curr, currencies, &rates)) < 0) {
		fprintf(stderr, "Failed to get rates: %d", status);
		exit(1);
	}
	printf("Call get_rates: %d\n", status);

	printf("Base: %s\n", rates.base);
	for (int i = 0; i < rates.count; i++) {
		struct rate r = rates.rates[i];
		for (int j = 0; j < r.count; j++) {
			struct currency c = r.currencies[j];
			printf("%s: %.4f ", c.name, c.value);
		}
		printf("\n");
	}


	exit(0);
}

void
usage()
{
	printf("usage: ./test2 start_date end_date base [currency1 currency2 ...]\n");
	printf("\t start_date: yyyy-mm-dd\n");
	printf("\t end_date: yyyy-mm-dd\n");
}
