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

/**
 * Tiler Memory Allocator is responsible for: 
 * <ol> 
 * <li>Getting the stride information for containers(???) or 
 * buffers 
 * <li>Converting virtual addresses to physical addresses. 
 * </ol>
 */
/** ---------------------------------------------------------------------------
 * Type definitions
 */

/**
 * System Space Pointer
 * 
 * This is handled as a unsigned long so that no dereferencing
 * is allowed by user space components.
 */
typedef unsigned long SSPtr;

/**
 * Returns the tiler stride corresponding to the virtual 
 * address.  For 1D and 2D buffers it returns the stride 
 * supplied with the allocation/mapping.  For non-tiler buffers 
 * it returns the page size. 
 * <p> 
 * NOTE: on Ducati phase 1, stride should return 16K for 8-bit 
 * 2D buffers, 32K for 16-bit and 32-bit 2D buffers, the stride 
 * used for alloc/map for 1D buffers, and the page size for 
 * non-tiler buffers. 
 *  
 * For unmapped addresses it returns 0.  However, this cannot be 
 * used to determine if an address is unmapped as 1D buffers 
 * could also have 0 stride (e.g. compressed buffers). 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param ptr    pointer to a virtual address
 * 
 * @return The stride of the block that contains the address.
 */
bytes_t TilerMem_GetStride(void *ptr);

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
