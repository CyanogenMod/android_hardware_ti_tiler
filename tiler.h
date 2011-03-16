/*
 * tiler.h
 *
 * TILER driver support functions for TI OMAP processors.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _TILER_H_
#define _TILER_H_

#define TILER_MEM_8BIT  0x60000000
#define TILER_MEM_16BIT 0x68000000
#define TILER_MEM_32BIT 0x70000000
#define TILER_MEM_PAGED 0x78000000
#define TILER_MEM_END   0x80000000

#define TILER_PAGE 0x1000
#define TILER_WIDTH    256
#define TILER_HEIGHT   128
#define TILER_BLOCK_WIDTH  64
#define TILER_BLOCK_HEIGHT 64
#define TILER_LENGTH (TILER_WIDTH * TILER_HEIGHT * TILER_PAGE)

#define TILER_DEVICE_PATH "/dev/tiler"
#define TILER_MAX_NUM_BLOCKS 16

enum tiler_fmt {
	TILFMT_MIN     = -1,
	TILFMT_INVALID = -1,
	TILFMT_NONE    = 0,
	TILFMT_8BIT    = 1,
	TILFMT_16BIT   = 2,
	TILFMT_32BIT   = 3,
	TILFMT_PAGE    = 4,
	TILFMT_MAX     = 4
};

struct area {
	uint16_t width;
	uint16_t height;
};

struct tiler_block_info {
	enum tiler_fmt fmt;
	union {
		struct area area;
		uint32_t len;
	} dim;
	uint32_t stride;
	void *ptr;
	uint32_t ssptr;
};

struct tiler_buf_info {
	int32_t num_blocks;
	struct tiler_block_info blocks[TILER_MAX_NUM_BLOCKS];
	int32_t offset;
};

#define TILIOC_GBUF  _IOWR('z', 100, uint32_t)
#define TILIOC_FBUF  _IOWR('z', 101, uint32_t)
#define TILIOC_GSSP  _IOWR('z', 102, uint32_t)
#define TILIOC_MBUF  _IOWR('z', 103, uint32_t)
#define TILIOC_UMBUF _IOWR('z', 104, uint32_t)
#define TILIOC_QBUF  _IOWR('z', 105, uint32_t)
#define TILIOC_RBUF  _IOWR('z', 106, uint32_t)
#define TILIOC_URBUF _IOWR('z', 107, uint32_t)
#define TILIOC_QUERY_BLK _IOWR('z', 108, uint32_t)

#endif
