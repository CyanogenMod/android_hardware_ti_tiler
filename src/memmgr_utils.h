/*
 * memmgr_utils.h
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

#ifndef _MEMMGR_UTILS_H_
#define _MEMMGR_UTILS_H_

#define TILER_MEM_8BIT  0x60000000
#define TILER_MEM_16BIT 0x68000000
#define TILER_MEM_32BIT 0x70000000
#define TILER_MEM_PAGED 0x78000000
#define TILER_MEM_END   0x80000000
#define TILER_STRIDE_8BIT  16384
#define TILER_STRIDE_16BIT 32768
#define TILER_STRIDE_32BIT 32768

#define PAGE_SIZE 4096

/**
 * Returns the default page stride for this block
 * 
 * @author a0194118 (9/4/2009)
 * 
 * @param width  Width of 2D container
 * 
 * @return Stride
 */
static bytes_t def_stride(pixels_t width)
{
    return (PAGE_SIZE - 1 + (bytes_t)width) & ~(PAGE_SIZE - 1);
}

/**
 * Returns the bytes per pixel for the pixel format.
 * 
 * @author a0194118 (9/4/2009)
 * 
 * @param pixelFormat   Pixelformat
 * 
 * @return Bytes per pixel
 */
static bytes_t def_bpp(pixel_fmt_t pixelFormat)
{
    return (pixelFormat == PIXEL_FMT_32BIT ? 4 :
            pixelFormat == PIXEL_FMT_16BIT ? 2 : 1);
}

#endif

