/*
 * tilermem.h
 *
 * Tiler Memory Interface functions for TI OMAP processors.
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

#ifndef _TILERMEM_H_
#define _TILERMEM_H_

/* retrieve type definitions */
#include "mem_types.h"

/**
 * Tiler Memory Allocator is responsible for: 
 * <ol> 
 * <li>Getting the stride information for containers(???) or 
 * buffers 
 * <li>Converting virtual addresses to physical addresses. 
 * </ol>
 */

/**
 * Returns the tiler stride corresponding to the system space 
 * address.  For 2D buffers it returns the container stride. For
 * 1D buffers it returns the page size.  For non-tiler buffers 
 * it returns 0. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param ptr    pointer to a virtual address
 * 
 * @return The stride of the block that contains the address.
 */
bytes_t TilerMem_GetStride(SSPtr ssptr);

/**
 * Retrieves the physical system-space address that corresponds 
 * to the virtual address. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param ptr    pointer to a virtual address
 * 
 * @return The physical system-space address that the virtual 
 *         address refers to.  If the virtual address is invalid
 *         or unmapped, it returns 0.
 */
SSPtr TilerMem_VirtToPhys(void *ptr);

#endif
