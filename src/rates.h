/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2020 Luis Bandarra <luis@bandarra.pt>
 * All rights reserved.
 */

/* Generate HTTP GET request from the url in the parameters
 * return the pointer to the response and the data size
 */
char *	http_get_data(const char*, size_t*);

