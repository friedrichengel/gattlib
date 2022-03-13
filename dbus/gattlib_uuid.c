/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, Friedrich Engel
 * 
 * based on work from:
 * BlueZ - Bluetooth protocol stack for Linux
 *  Copyright (C) 2011  Nokia Corporation
 *  Copyright (C) 2011  Marcel Holtmann <marcel@holtmann.org>
 * 
 * Variable / Function Names taken from BlueZ for interoperatibility with gattlib
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <bluetooth/bluetooth.h>

#include "gattlib_uuid.h"

int bt_string_to_uuid(bt_uuid_t *uuid, const char *string){
	unsigned char uuid_type = 0;
	uint32_t data0, data4;
	uint16_t data1, data2, data3, data5, u16;
	uint128_t u128;
	uint8_t *val = (uint8_t *) &u128;
	char *endptr = NULL;
	char dummy[2];

	if (strlen(string) == 36 && string[8] == '-' && string[13] == '-' && string[18] == '-' && string[23] == '-'){
		if (sscanf(string, "0000%04hx-0000-1000-8000-00805%1[fF]9%1[bB]34%1[fF]%1[bB]", &u16, dummy, dummy, dummy, dummy) == 5){
			uuid_type = 255;
		}else if (sscanf(string, "%08x-%04hx-%04hx-%04hx-%08x%04hx", &data0, &data1, &data2, &data3, &data4, &data5) == 6){
			uuid_type = 128;
		}
	}
	if (strlen(string) == 8 || strlen(string) == 10){
		uuid_type = 32;
	}
	if (strlen(string) == 4 || strlen(string) == 6){
		uuid_type = 16;
	}

	memset(uuid, 0, sizeof(bt_uuid_t));
	switch (uuid_type){
		case 16:
			if (endptr && (*endptr == '\0' || *endptr == '-')) {
				uuid->type = BT_UUID16;
				uuid->value.u16 = strtol(string, &endptr, 16);
				return 0;
			}
			break;

		case 32:
			if (endptr && *endptr == '\0') {
				uuid->type = BT_UUID32;
				uuid->value.u32 = strtol(string, &endptr, 16);				
				return 0;
			}
			break;

		case 128:
			data0 = htonl(data0);
			data1 = htons(data1);
			data2 = htons(data2);
			data3 = htons(data3);
			data4 = htonl(data4);
			data5 = htons(data5);

			memcpy(&val[0], &data0, 4);
			memcpy(&val[4], &data1, 2);
			memcpy(&val[6], &data2, 2);
			memcpy(&val[8], &data3, 2);
			memcpy(&val[10], &data4, 4);
			memcpy(&val[14], &data5, 2);

			uuid->type = BT_UUID128;
			uuid->value.u128 = u128;
			return 0;
			break;

		case 255:
			if (endptr && (*endptr == '\0' || *endptr == '-')) {
				uuid->type = BT_UUID16;
				uuid->value.u16 = strtol(string + 4, &endptr, 16);
				return 0;
			}
			break;
	}
	return -EINVAL;
}