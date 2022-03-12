/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2011  Nokia Corporation
 *  Copyright (C) 2011  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <bluetooth/bluetooth.h>

#include "uuid.h"

int bt_uuid16_create(bt_uuid_t *btuuid, uint16_t value)
{
	memset(btuuid, 0, sizeof(bt_uuid_t));
	btuuid->type = BT_UUID16;
	btuuid->value.u16 = value;

	return 0;
}

int bt_uuid32_create(bt_uuid_t *btuuid, uint32_t value)
{
	memset(btuuid, 0, sizeof(bt_uuid_t));
	btuuid->type = BT_UUID32;
	btuuid->value.u32 = value;

	return 0;
}

int bt_uuid128_create(bt_uuid_t *btuuid, uint128_t value)
{
	memset(btuuid, 0, sizeof(bt_uuid_t));
	btuuid->type = BT_UUID128;
	btuuid->value.u128 = value;

	return 0;
}

static inline int is_uuid128(const char *string)
{
	return (strlen(string) == 36 &&
			string[8] == '-' &&
			string[13] == '-' &&
			string[18] == '-' &&
			string[23] == '-');
}

static inline int is_base_uuid128(const char *string)
{
	uint16_t uuid;
	char dummy[2];

	if (!is_uuid128(string))
		return 0;

	return sscanf(string,
		"0000%04hx-0000-1000-8000-00805%1[fF]9%1[bB]34%1[fF]%1[bB]",
		&uuid, dummy, dummy, dummy, dummy) == 5;
}

static inline int is_uuid32(const char *string)
{
	return (strlen(string) == 8 || strlen(string) == 10);
}

static inline int is_uuid16(const char *string)
{
	return (strlen(string) == 4 || strlen(string) == 6);
}

static int bt_string_to_uuid16(bt_uuid_t *uuid, const char *string)
{
	uint16_t u16;
	char *endptr = NULL;

	u16 = strtol(string, &endptr, 16);
	if (endptr && (*endptr == '\0' || *endptr == '-')) {
		bt_uuid16_create(uuid, u16);
		return 0;
	}

	return -EINVAL;
}

static int bt_string_to_uuid32(bt_uuid_t *uuid, const char *string)
{
	uint32_t u32;
	char *endptr = NULL;

	u32 = strtol(string, &endptr, 16);
	if (endptr && *endptr == '\0') {
		bt_uuid32_create(uuid, u32);
		return 0;
	}

	return -EINVAL;
}

static int bt_string_to_uuid128(bt_uuid_t *uuid, const char *string)
{
	uint32_t data0, data4;
	uint16_t data1, data2, data3, data5;
	uint128_t u128;
	uint8_t *val = (uint8_t *) &u128;

	if (sscanf(string, "%08x-%04hx-%04hx-%04hx-%08x%04hx",
				&data0, &data1, &data2,
				&data3, &data4, &data5) != 6)
		return -EINVAL;

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

	bt_uuid128_create(uuid, u128);

	return 0;
}

int bt_string_to_uuid(bt_uuid_t *uuid, const char *string)
{
	if (is_base_uuid128(string))
		return bt_string_to_uuid16(uuid, string + 4);
	else if (is_uuid128(string))
		return bt_string_to_uuid128(uuid, string);
	else if (is_uuid32(string))
		return bt_string_to_uuid32(uuid, string);
	else if (is_uuid16(string))
		return bt_string_to_uuid16(uuid, string);

	return -EINVAL;
}