/*
 * memalloc.h
 *
 * Memory Allocator Interface functions for TI OMAP processors.
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

#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

/**
 * Memory Allocator is responsible for: 
 * <ol>
 * <li>Allocate 1D and 2D blocks and pack them into a buffer. 
 * <li>Free such allocated blocks 
 * <li>Map 1D buffers into tiler space 
 * <li>Unmap 1D buffers from tiler space 
 * <li>Verify if an address lies in 1D or 2D space, whether it 
 * is mapped (to tiler space) 
 * <li>Mapping Ducati memory blocks to host processor and vice 
 * versa. 
 * </ol> 
 *  
 * The allocator distinguishes between: 
 * <ul> 
 * <li>1D and 2D blocks 
 * <li>2D blocks allocated by MemAlloc are non-cacheable.  All 
 * other blocks are cacheable (e.g. 1D blocks).  Preallocated
 * may or may not be cacheable, depending on how they've been 
 * allocated, but are assumed to be cacheable. 
 * <li>buffers (an ordered collection of one or more blocks 
 * mapped consecutively) 
 * </ul> 
 *  
 * The allocator tracks each buffer based on (addr, size). 
 *  
 * Also, via the tiler manager, it tracks each block.  The tiler 
 * manager itself also tracks each buffer. 
 *  
 */
 
/** ---------------------------------------------------------------------------
 * Type definitions
 */

/**
 * Ducati Space Virtual Address Pointer
 * 
 * This is handled as a unsigned long so that no dereferencing
 * is allowed by user space components.
 */
typedef unsigned long DSPtr;

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
 * Pixel format 
 *  
 * Page mode is encoded in the pixel format to handle different 
 * set of buffers uniformly 
 */
enum kPixelFormat {
    kPixelFormatMin = 1,
	k8bit  = 1,
	k16bit = 2,
	k32bit = 3,
    kPage  = 4,
    kPixelFormatMax = 4
};

/**
 * Memory Allocator block specification
 *  
 * Size of a 2D buffer is calculated as height * stride.  stride
 * must be the smallest multiple of page size that is at least 
 * the width. 
 *  
 * Size of a 1D buffer is calculated as length.  stride can be 
 * any multiple of page size.  length must be a multiple of 
 * stride, unless stride is 0. 
 *  
 * @author a0194118 (9/1/2009)
 */
struct MemAllocBlock {
    enum kPixelFormat pixelFormat;  /* pixel format */
    union {
        struct {
            pixels_t width;  /* width of 2D buffer */
            pixels_t height; /* height of 2D buffer */
        };
        struct {
            bytes_t length;  /* length of 1D buffer.  Must be multiple of
                                stride if stride is not 0. */
        };
    };
    unsigned long stride;    /* must be multiple of page size.  Can be 0 only
                                if pixelFormat is kPage. */
    void *ptr;               /* pointer to beginning of buffer */
    unsigned long reserved;  /* system space address (used internally) */
};

typedef struct MemAllocBlock MemAllocBlock;

/**
 * Memory mapping block specification
 * 
 * @author a0194118 (9/1/2009)
 */
struct MemMapBlock {
    DSPtr   dsptr;
    bytes_t length;
};

typedef struct MemMapBlock MemMapBlock;

/**
 * Allocates a buffer as a list of blocks (1D or 2D), and maps 
 * them so that they are packaged consecutively. Returns the 
 * pointer to the first block, or NULL on failure. 
 * <p> 
 * The size of each block ther than the last must be a multiple 
 * of the page size.  This ensures that the blocks stack 
 * correctly. 
 * <p> 
 * 2D blocks will be non-cacheable, while 1D blocks will be 
 * cacheable.
 * <p> 
 * On success, the buffer is registered with the memory 
 * allocator. 
 * <p> 
 * As a side effect, if the operation was successful, the ssptr 
 * fields of the block specification will be filled with the 
 * system-space addresses, while the ptr fields will be set to 
 * the individual blocks. 
 * <p> 
 * Phase 2 may allow passing preallocated data pointers in the 
 * ptr field of the MemAllocBlock structure.  These are assumed 
 * to be cacheable, although they may not be. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param blocks     Block specification information.  This 
 *                   should be an array of at least num_blocks
 *                   elements.
 * @param num_blocks Number of blocks to be included in the 
 *                   allocated memory segment
 * 
 * @return Pointer to the buffer, which is also the pointer to 
 *         the first allocated block. NULL if allocation failed.
 */
void *MemAlloc_Alloc(MemAllocBlock blocks[], int num_blocks);

/**
 * Frees a buffer allocated by MemAlloc_Alloc(). It fails for 
 * any buffer not allocated by MemAlloc_Alloc() or one that has 
 * been already freed. 
 * <p> 
 * It also unregisters the buffer with the memory allocator. 
 * <p> 
 * This function unmaps the processor's virtual address to the 
 * tiler address for all blocks allocated, unregisters the 
 * buffer, and frees all of its tiler blocks. 
 * <p> 
 * In phase 2, memalloc free will not free any preallocated 
 * buffers passed into MemAlloc_Alloc(). 
 *  
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr     Pointer to the buffer allocated (returned) 
 *                   by MemAlloc_Alloc()
 * 
 * @return TRUE on success, FALSE on failure
 */
bool MemAlloc_Free(void *bufPtr);

/**
 * This function maps the user provided data buffer to the tiler
 * space in paged mode, and maps that area into the process 
 * space. You can map a data buffer multiple times, resulting in
 * multiple mapping for the same buffer. However, you cannot map 
 * a buffer that is already mapped to tiler, e.g. a buffer 
 * pointer returned by this method. 
 *  
 * @author a0194118 (9/1/2009)
 * @param dataPtr Pointer to the buffer allocated by the
 *                user.
 * @param size    Size of the buffer allocated by the user. Must 
 *                be a multiple of stride unless stride is 0.
 * @param stride  The stride of the buffer. Must be multiple of 
 *                the physical page size or 0.
 * 
 * @return On success it returns a new virtual address that
 *         points to the tiler-mapped 1D buffer.  Else, it
 *         returns NULL.
 */
void *MemAlloc_MapIn1DMode(void *dataPtr, bytes_t size, bytes_t stride);

/**
 * This function unmaps the user provided data buffer from tiler
 * space that was mapped to the tiler space in paged mode using 
 * MemAlloc_MapIn1DMode().  It also unmaps the buffer itself 
 * from the process space.  Trying to unmap a previously 
 * unmapped buffer will fail. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr    Pointer to the buffer as returned by a 
 *                  previous call to MemAlloc_MapIn1DMode()
 *  
 * @return On success it returns TRUE.  Else, it returns FALSE. 
 */
bool MemAlloc_UnMap(void *bufPtr);

/**
 * Checks if a given virtual address lies in a 2D buffer. 
 * <p> 
 * This function retrieves the system space address that the 
 * virtual address maps to.  If this system space address is 
 * within the 2D tiler area, it is considered lying within a 2D 
 * buffer. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param ptr   Pointer to a virtual address
 * 
 * @return TRUE if the virtual address is within a mapped 2D 
 *         buffer.  FALSE if the virtual address is not mapped,
 *         invalid, or is mapped to an area other than a 2D
 *         buffer.  In phase 1, however, it is TRUE it
 *         the virtual address is mapped to any area of the
 *         tiler space other than page mode.
 */
bool MemAlloc_Is2DBuffer(void *ptr);

/**
 * Checks if a given virtual address lies in a tiler 1D buffer. 
 * <p> 
 * This function retrieves the system space address that the 
 * virtual address maps to.  If this system space address is 
 * within the 1D tiler area, it is considered lying within a 1D 
 * buffer. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param ptr   Pointer to a virtual address
 * 
 * @return TRUE if the virtual address is within a mapped 1D 
 *         tiler buffer.  FALSE if the virtual address is not
 *         mapped, invalid, or is mapped to an area other than a
 *         1D tiler buffer.  In phase 1, however, it is TRUE it
 *         the virtual address is mapped to the page-mode area
 *         of the tiler space.
 */
bool MemAlloc_Is1DBuffer(void *ptr);

/**
 * Checks if a given virtual address is mapped by tiler manager
 * to tiler space. 
 * <p>
 * This function is equivalent to MemAlloc_Is1DBuffer(ptr) || 
 * MemAlloc_Is2DBuffer(ptr).  It retrieves the system space
 * address that the virtual address maps to. If this system 
 * space address lies within the tiler area, the function 
 * returns TRUE.
 *  
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param ptr   Pointer to a virtual address
 * 
 * @return TRUE if the virtual address is within a buffer that 
 *         was mapped into tiler space, e.g. by calling
 *         MemAlloc_MapIn1DMode() or MemAlloc_MapIn2DMode()
 */
bool MemAlloc_IsMapped(void *ptr);

/**
 * Remaps a portion of a buffer already mapped to tiler (e.g.
 * using MemAlloc_Alloc, or MemAlloc_MapIn1DMode) into the 
 * Ducati's MMU. In phase 2, this portion may contain a list of 
 * separate whole blocks, but in phase 1, the address range 
 * specified by bufPtr and length must be a single whole block. 
 *  
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr    Pointer to a buffer 
 * @param length    Length of the buffer 
 * 
 * @return A Ducati virtual address that points to the same 
 *         buffer.
 */
DSPtr MemAlloc_ReMapToDucati(void *bufPtr, bytes_t length);

/* You can implement tiler-mapped-agnostic MapToDucati in the following fashion. 
   Note, however, that you will need to keep track of the stride and the new
   mapped buffer pointer.
 
    DSPtr MemAlloc_MapToDucati(void *bufPtr, bytes_t length, bytes_t stride)
    {
        if (!MemAlloc_IsMapped(bufPtr))
        {
            void *newPtr = MemAlloc_MapIn1DMode(bufPtr, length, stride);
            return MemAlloc_ReMapToDucati(newPtr, length);
        }
        else
        {
            return MemAlloc_ReMapToDucati(bufPtr, length);
        }
    }
*/

int a;

/**
 * Unmaps a buffer from the Ducati's MMU, but not from tiler. 
 * This method only succeeds for addresses returned by 
 * MemAlloc_ReMapToDucati, and fails on successive unmaps 
 * attempted on the same ducati address. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param bufDPtr   Ducati-space virtual address of the buffer, 
 *                  as returned by Memalloc_ReMapToDucati
 * 
 * @return TRUE on success.  FALSE on failure.
 */
bool MemAlloc_DeMapFromDucati(DSPtr bufDPtr);

/**
 * Maps a set of blocks already mapped to tiler from the Ducati 
 * side into the host MMU. 
 *  
 * @author a0194118 (9/1/2009)
 * 
 * @param blocks      Address and length of each block to be 
 *                    mapped 
 * @param num_blocks  Number of blocks to be mapped.
 * 
 * @return pointer to the buffer on the host, or NULL on 
 *         failure.
 */
void *MemAlloc_ReMapFromDucati(MemMapBlock blocks[], int num_blocks);

/**
 * Unmaps a buffer from the MMU, but not from tiler space.  This 
 * method only succeeds for buffers returned by 
 * MemAlloc_ReMapFromDucati, and fails on successive unmaps 
 * attempted on the same buffer address. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr   Address of the buffer, as returned by 
 *                 MemAlloc_MapFromDucati()
 * 
 * @return TRUE on success.  FALSE on failure.
 */
bool MemAlloc_DeMap(void *bufPtr);

#endif
