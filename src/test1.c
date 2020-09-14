/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */
#include <stdlib.h>
#include <stdio.h>

#include "rates.h"

void	usage(void);

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		usage();
		exit(-1);
	}

	char *url = argv[1];
	printf("Downloading url: %s\n", url);

	char *file = argv[2];
	char *get;
	size_t size;

	if ((get = http_get_data(url, &size)) == NULL) {
		printf("Failed to save\n");
		exit(1);
	}
	FILE *fp;

	if ((fp = fopen(file, "a")) != NULL) {
		fwrite(get, sizeof(char), size, fp);
		fclose(fp);
	}

	printf("Save with success\n");
	free(get);

	exit(0);
}

void
usage()
{
	printf("usage: ./test1 url file\n");
}
