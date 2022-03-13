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

#ifndef __GATTLIB_UUID_H
#define __GATTLIB_UUID_H

#include <stdint.h>

typedef struct {
	enum {
		BT_UUID_UNSPEC = 0,
		BT_UUID16 = 16,
		BT_UUID32 = 32,
		BT_UUID128 = 128,
	} type;
	union {
		uint16_t  u16;
		uint32_t  u32;
		uint128_t u128;
	} value;
} bt_uuid_t;

int bt_string_to_uuid(bt_uuid_t *uuid, const char *string);

#endif /* __BLUETOOTH_UUID_H */
