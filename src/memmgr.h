/*
 * memmgr.h
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

#ifndef _MEMMGR_H_
#define _MEMMGR_H_

/* retrieve type definitions */
#include "mem_types.h"

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

typedef enum kPixelFormat kPixelFormat;

/**
 * Processors supported
 */
enum kMemMgr_Proc {
    kMemMgr_ProcMin = 1,
    kChiron = 1,
    kDucati = 2,
    kMemMgr_ProcMax = 2,
};

typedef enum kMemMgr_Proc kMemMgr_Proc;

/**
 * Memory Allocator block specification
 *  
 * Size of a 2D buffer is calculated as height * stride.  stride
 * is the smallest multiple of page size that is at least 
 * the width, and is set by MemMgr_Alloc.
 *  
 * Size of a 1D buffer is calculated as length.  stride is not 
 * set by MemMgr_Alloc, but it can be set by the user.  length 
 * must be a multiple of stride unless stride is 0. 
 * 
 * @author a0194118 (9/1/2009)
 */
struct MemAllocBlock {
    kPixelFormat pixelFormat;  /* pixel format */
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
    kMemMgr_Proc processor;
    union {
        DSPtr  dsptr;
        void  *ptr;
    };
    bytes_t length;
};

typedef struct MemMapBlock MemMapBlock;

/**
 * Returns the page size.  This is required for allocating 1D 
 * blocks that stack under any other blocks. 
 * 
 * @author a0194118 (9/3/2009)
 * 
 * @return Page size.
 */
bytes_t MemMgr_PageSize();

/**
 * Allocates a buffer as a list of blocks (1D or 2D), and maps 
 * them so that they are packaged consecutively. Returns the 
 * pointer to the first block, or NULL on failure. 
 * <p> 
 * The size of each block other than the last must be a multiple
 * of the page size.  This ensures that the blocks stack 
 * correctly.  Set stride to 0 to avoid stride/length alignment 
 * constraints.  Stride of 2D blocks will be updated by this 
 * method. 
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
 * the individual blocks.  The stride information is set for 2D 
 * blocks. 
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
void *MemMgr_Alloc(MemAllocBlock blocks[], int num_blocks);

/**
 * Frees a buffer allocated by MemMgr_Alloc(). It fails for 
 * any buffer not allocated by MemMgr_Alloc() or one that has 
 * been already freed. 
 * <p> 
 * It also unregisters the buffer with the memory allocator. 
 * <p> 
 * This function unmaps the processor's virtual address to the 
 * tiler address for all blocks allocated, unregisters the 
 * buffer, and frees all of its tiler blocks. 
 * <p> 
 * In phase 2, memalloc free will not free any preallocated 
 * buffers passed into MemMgr_Alloc(). 
 *  
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr     Pointer to the buffer allocated (returned) 
 *                   by MemMgr_Alloc()
 * 
 * @return TRUE on success, FALSE on failure
 */
bool MemMgr_Free(void *bufPtr);

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
bool MemMgr_Is2DBuffer(void *ptr);

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
bool MemMgr_Is1DBuffer(void *ptr);

/**
 * Checks if a given virtual address is mapped by tiler manager
 * to tiler space. 
 * <p>
 * This function is equivalent to MemMgr_Is1DBuffer(ptr) || 
 * MemMgr_Is2DBuffer(ptr).  It retrieves the system space
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
 *         MemMgr_MapIn1DMode() or MemMgr_MapIn2DMode()
 */
bool MemMgr_IsMapped(void *ptr);

/* ------------------ generic choice ------------------ */

/**
 * Remaps portion(s) of buffer(s) already mapped to tiler (e.g. 
 * using MemMgr_Alloc, or MemMgr_MapIn1DMode) into another 
 * processor's MMU.  Only certain configurations are supported: 
 *  
 * In phase 1, the supported configurations are:
 * <ol> 
 * <li> Mapping exactly one whole block from Chiron to Ducati. 
 * <li> Mapping any number of whole blocks from Ducati to 
 * Chiron. 
 * </ol>
 *  
 * In phase 2, the supported configurations are: 
 * <ol>
 * <li> Mapping exactly one whole buffer from Chiron to Ducati. 
 * <li> Mapping exactly one whole buffer from Ducati to Chiron. 
 * </ol> 
 *  
 * A buffer is any set-of-blocks either allocated by 
 * MemMgr_Alloc() or mapped by MemMgr_Map() functions. 
 *  
 * @author a0194118 (9/1/2009)
 *  
 * @param blocks      Address, source and length of each block 
 *                    to be mapped
 * @param num_blocks  Number of blocks to be mapped.
 * @param target      Pointer to the MemMapBlock structure that 
 *                    will contain the address and size of the
 *                    mapped buffer.  The processor field is the
 *                    target processor to map to.  If the
 *                    mapping fails, the address and length will
 *                    be set to 0.
 * 
 * @return TRUE on success.  FALSE on failure.
 */
bool MemMgr_ReMap(MemMapBlock blocks[], int num_blocks, MemMapBlock *target);

/**
 * Unmaps a buffer from the MMU of a processor, but not from 
 * tiler space. This method only succeeds for buffers returned 
 * by MemMgr_ReMap, and fails on successive unmaps attempted on
 * the same buffer address. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param buffer   Address and processor of the buffer, as 
 *                 returned by MemMgr_ReMap()
 * 
 * @return TRUE on success.  FALSE on failure.
 */
bool MemMgr_DeMap(MemMapBlock buffer);

/**
 * This function maps the user provided data buffer to the tiler
 * space as blocks, and maps that area into the process space 
 * consecutively. You can map a data buffer multiple times, 
 * resulting in multiple mapping for the same buffer. However, 
 * you cannot map a buffer that is already mapped to tiler, e.g. 
 * a buffer pointer returned by this method. 
 *  
 * In phase 1, the supported configurations are:
 * <ol> 
 * <li> Mapping exactly one 1D block to tiler space (e.g. 
 * MapIn1DMode). 
 * </ol>
 *  
 * In phase 2, these configurations are also supported: 
 * <ol>
 * <li> Mapping exactly one 2D block to tiler space (e.g. 
 * MapIn2DMode). 
 * </ol> 
 *  
 * @author a0194118 (9/3/2009)
 * 
 * @param blocks     Block specification information.  This 
 *                   should be an array of at least num_blocks
 *                   elements.  The ptr fields must contain the
 *                   user allocated buffers for the block.
 *                   These will be updated with the mapped
 *                   addresses of these blocks on success.
 * @param num_blocks Number of blocks to be included in the 
 *                   mapped memory segment
 * 
 * @return Pointer to the buffer, which is also the pointer to 
 *         the first mapped block. NULL if allocation failed.
 */
void *MemMgr_Map(MemAllocBlock blocks[], int num_blocks);

/* ------------------ original specific choices ------------------ */

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
 * @param stride  The stride of the buffer. size must be a 
 *                multiple of stride unless it is 0.
 * 
 * @return On success it returns a new virtual address that
 *         points to the tiler-mapped 1D buffer.  Else, it
 *         returns NULL.
 */
void *MemMgr_MapIn1DMode(void *dataPtr, bytes_t size, bytes_t stride);

/**
 * This function unmaps the user provided data buffer from tiler
 * space that was mapped to the tiler space in paged mode using 
 * MemMgr_MapIn1DMode().  It also unmaps the buffer itself 
 * from the process space.  Trying to unmap a previously 
 * unmapped buffer will fail. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr    Pointer to the buffer as returned by a 
 *                  previous call to MemMgr_MapIn1DMode()
 *  
 * @return On success it returns TRUE.  Else, it returns FALSE. 
 */
bool MemMgr_UnMap(void *bufPtr);

/**
 * Remaps a portion of a buffer already mapped to tiler (e.g.
 * using MemMgr_Alloc, or MemMgr_MapIn1DMode) into the 
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
DSPtr MemMgr_ReMapToDucati(void *bufPtr, bytes_t length);

/* You can implement tiler-mapped-agnostic MapToDucati in the following fashion. 
   Note, however, that you will need to keep track of the stride and the new
   mapped buffer pointer.
 
    DSPtr MemMgr_MapToDucati(void *bufPtr, bytes_t length, bytes_t stride)
    {
        if (!MemMgr_IsMapped(bufPtr))
        {
            void *newPtr = MemMgr_MapIn1DMode(bufPtr, length, stride);
            return MemMgr_ReMapToDucati(newPtr, length);
        }
        else
        {
            return MemMgr_ReMapToDucati(bufPtr, length);
        }
    }
*/

/**
 * Unmaps a buffer from the Ducati's MMU, but not from tiler. 
 * This method only succeeds for addresses returned by 
 * MemMgr_ReMapToDucati, and fails on successive unmaps 
 * attempted on the same ducati address. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param bufDPtr   Ducati-space virtual address of the buffer, 
 *                  as returned by Memmgr_ReMapToDucati
 * 
 * @return TRUE on success.  FALSE on failure.
 */
bool MemMgr_DeMapFromDucati(DSPtr bufDPtr);

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
void *MemMgr_ReMapFromDucati(MemMapBlock blocks[], int num_blocks);

/**
 * Unmaps a buffer from the MMU, but not from tiler space.  This 
 * method only succeeds for buffers returned by 
 * MemMgr_ReMapFromDucati, and fails on successive unmaps 
 * attempted on the same buffer address. 
 * 
 * @author a0194118 (9/1/2009)
 * 
 * @param bufPtr   Address of the buffer, as returned by 
 *                 MemMgr_MapFromDucati()
 * 
 * @return TRUE on success.  FALSE on failure.
 */
bool MemMgr_DeMap(void *bufPtr);

#endif
