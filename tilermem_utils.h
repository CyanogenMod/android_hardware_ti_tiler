/*
 * tilermem_utils.h
 *
 * Memory Allocator Interface internal definitions and functions needed for
 * unit testing.
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

#ifndef _TILERMEM_UTILS_H_
#define _TILERMEM_UTILS_H_

#include <tiler.h>

#define TILER_PAGE_WIDTH   64
#define TILER_PAGE_HEIGHT  64

#define TILER_STRIDE_8BIT  (TILER_WIDTH * TILER_PAGE_WIDTH)
#define TILER_STRIDE_16BIT (TILER_WIDTH * TILER_PAGE_WIDTH * 2)
#define TILER_STRIDE_32BIT (TILER_WIDTH * TILER_PAGE_WIDTH * 2)

#define PAGE_SIZE           TILER_PAGE

#endif
