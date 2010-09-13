/*
 * types.h
 *
 * Type definitions for the Memory Interface for TI OMAP processors.
 *
 * Copyright (C) 2008-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _MEM_TYPES_H_
#define _MEM_TYPES_H_

/* for bool definition */
#include <stdbool.h>
#include <stdint.h>

/** ---------------------------------------------------------------------------
 * Type definitions
 */

/**
 * Buffer length in bytes
 */
typedef uint32_t bytes_t;

/**
 * Length in pixels
 */
typedef uint16_t pixels_t;


/**
 * Pixel format
 *
 * Page mode is encoded in the pixel format to handle different
 * set of buffers uniformly
 */
enum pixel_fmt_t {
    PIXEL_FMT_MIN   = 0,
    PIXEL_FMT_8BIT  = 0,
    PIXEL_FMT_16BIT = 1,
    PIXEL_FMT_32BIT = 2,
    PIXEL_FMT_PAGE  = 3,
    PIXEL_FMT_MAX   = 3
};

typedef enum pixel_fmt_t pixel_fmt_t;

/**
 * Ducati Space Virtual Address Pointer
 *
 * This is handled as a unsigned long so that no dereferencing
 * is allowed by user space components.
 */
typedef uint32_t DSPtr;

/**
 * System Space Pointer
 *
 * This is handled as a unsigned long so that no dereferencing
 * is allowed by user space components.
 */
typedef uint32_t SSPtr;

/**
 * Error values
 *
 * Page mode is encoded in the pixel format to handle different
 * set of buffers uniformly
 */
#define MEMMGR_ERR_NONE    0
#define MEMMGR_ERR_GENERIC 1

#endif

