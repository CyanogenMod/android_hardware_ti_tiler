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

/** ---------------------------------------------------------------------------
 * Type definitions
 */

/** 
 * Buffer length in bytes
 */
typedef unsigned long bytes_t;

/**
 * Length in pixels
 */ 
typedef unsigned short pixels_t;

/**
 * Boolean
 */
typedef int bool;

/**
 * Ducati Space Virtual Address Pointer
 * 
 * This is handled as a unsigned long so that no dereferencing
 * is allowed by user space components.
 */
typedef unsigned long DSPtr;

/**
 * System Space Pointer
 * 
 * This is handled as a unsigned long so that no dereferencing
 * is allowed by user space components.
 */
typedef unsigned long SSPtr;

#endif

