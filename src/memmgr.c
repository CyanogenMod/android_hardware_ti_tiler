/*
 * memmgr.c
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

#include "memmgr.h"
#include "utils.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "tilermem.h"

#define TILIOC_MBUF 0  /* map buffer allocated by user - this could be combined 
                          with the ioctl used in tilerAlloc, as the ptr field in
                          tiler_block_info could be NULL if allocation is needed,
                          and the address if buffer is preallocated.  This would
                          allow mix/match of preallocated/to-be-allocated blocks.*/
#define TILIOC_QBUF 0  /* query buffer based on offset */
#define TILIOC_RBUF 0  /* register buffer to get offset */
#define TILIOC_URBUF 0 /* unregister buffer based on offset */

#define TILER_MEM_8BIT  0x60000000
#define TILER_MEM_16BIT 0x68000000
#define TILER_MEM_32BIT 0x70000000
#define TILER_MEM_PAGED 0x78000000
#define TILER_MEM_END   0x80000000

#define TILER_MAX_NUM_BLOCKS 4

struct tiler_block_info {
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
                                if pixelFormat is TILFMT_PAGE. */
    void *ptr;               /* pointer to beginning of buffer */
    unsigned long ssptr;     /* system space address */
};

typedef struct tiler_block_info tiler_block_info;

/**
 * Memory Allocator parameters
 * 
 * @author a0194118 (9/2/2009)
 */
struct tiler_buf_info {
    int num_blocks;
    tiler_block_info blocks[TILER_MAX_NUM_BLOCKS];
    int offset;
};

typedef struct tiler_buf_info tiler_buf_info;

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

typedef enum tiler_fmt tiler_fmt;

tiler_fmt tiler_get_fmt(SSPtr ssptr)
{
    return (ssptr == 0              ? TILFMT_INVALID :
            ssptr < TILER_MEM_8BIT  ? TILFMT_NONE :
            ssptr < TILER_MEM_16BIT ? TILFMT_8BIT :
            ssptr < TILER_MEM_32BIT ? TILFMT_16BIT :
            ssptr < TILER_MEM_PAGED ? TILFMT_32BIT :
            ssptr < TILER_MEM_END   ? TILFMT_PAGE : TILFMT_NONE);
}

static int refCnt = 0;

/**
 * Increases the reference count.  Initialized tiler if this was 
 * the first reference 
 * 
 * @author a0194118 (9/2/2009)
 * 
 * @return 0 on success, non-0 on failure.
 */
static int inc_ref()
{
    /* initialize tiler on first call */
    /* :TODO: concurrency */
    if (!refCnt++) return A_I(tilerInit(),==,0);
    return 0;
}

/**
 * Decreases the reference count.  Deinitialized tiler if this 
 * was the last reference 
 * 
 * @author a0194118 (9/2/2009)
 * 
 * @return 0 on success, non-0 on failure.
 */
static int dec_ref()
{
    if (refCnt <= 0) return 1;
    if (!--refCnt) return A_I(tilerDeinit(),==,0);
    return 0;
}

void *MemMgr_Alloc(MemAllocBlock blocks[], int num_blocks)
{
    IN;

    tiler_block_info *blks = (tiler_block_info *) blocks;

    /* check arguments */
    if (NOT_I(num_blocks,>,0)) goto FAIL;
    if (NOT_I(num_blocks,<=,TILER_MAX_NUM_BLOCKS)) goto FAIL;

    /* need tiler */
    if (NOT_I(inc_ref(),==,0)) goto FAIL;

    /* check block allocation params */
    int ix, ok = 1;
    for (ix = 0; ix < num_blocks; ix++)
    {
        if (blocks[ix].pixelFormat < TILFMT_PAGE ||
            blocks[ix].pixelFormat > TILFMT_32BIT)
        {
            DP("blocks[%d] (=%d) is invalid", ix, blocks[ix].pixelFormat);
            ok = 0;
        }
    }

    /* ----- begin recoverable portion ----- */

    /* initialize buffer specs */
    int ret, td = -1;

    /* allocate each buffer using tiler driver */
    bytes_t size = 0;
    for (ix = 0; ok && ix < num_blocks; ix++)
    {
        tiler_block_info *blk = (tiler_block_info *) (blocks + ix);
        blk->ptr = NULL;
        blk->ssptr = tilerAlloc(blk);
        size += (blk->pixelFormat == TILFMT_PAGE ?
                 blk->length : blk->stride * blk->height);
        /* what happens if we fail? */
    }

    /* open tiler */
    td = open("/dev/tiler", O_RDWR | O_SYNC);
    if (NOT_I(td,>=,0)) goto FAIL_ALLOC;

    /* register buffer with tiler */
    tiler_buf_info ap;
    memcpy(ap.blocks, blocks, sizeof(tiler_block_info) * num_blocks);
    ret = ioctl(td, TILIOC_RBUF, &ap);

    if (NOT_P(ap.offset,!=,0)) goto FAIL_ALLOC;

    /* map blocks to process space */
    void *bufPtr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        td, ap.offset);

    if (NOT_P(bufPtr,!=,0)) {
        /* cache offset for buffer */
        cache_buf_add(bufPtr, ap.offset);

        /* fill out pointers */
        close(td);
        return R_P(bufPtr);
    }
    
    /* ------ error handling ------ */

    /* failed to map: unregister */
    ret = ioctl(td, TILIOC_URBUF, &ap);

FAIL_ALLOC:
    if (td >= 0) close(td);

    while (ix)
    {
        ix--;
        tilerFree(blks[ix].ssptr);
    }

    /* clear ssptr and ptr fields for all blocks */
    for (ix = 0; ix < num_blocks; ix++)
    {
        blks[ix].ssptr = 0;
        blks[ix].ptr = NULL;
    }

    A_I(dec_ref(),==,0);
FAIL:
    return R_P(NULL);
}

bool MemMgr_Free(void *bufPtr)
{
    IN;

    int ret, ok = 1, td = -1;
    bytes_t size = 0;
    tiler_buf_info ap;

    /* open tiler */
    td = open("/dev/tiler", O_RDWR | O_SYNC);
    if (NOT_I(td,>=,0)) ok = 0;

    /* retrieve registered buffers from vsptr */
    if (ok)
    {
        ap.offset = cache_buf_get(bufPtr);
        if (NOT_L(ap.offset,!=,0)) ok = 0;
    }

    /* get block information for the buffer */
    if (ok)
    {
        /* at this point Memory Allocator stops tracking this buffer */
        cache_buf_del(bufPtr);

        ret = ioctl(td, TILIOC_QBUF, &ap);

        /* unregister buffer */
        if (ok)
        {
            ret = ioctl(td, TILIOC_URBUF, &ap);
    
            /* free each block */
            int ix;
            for (ix = 0; ix < ap.num_blocks; ix++)
            {
                if (tilerFree(ap.blocks[ix].ssptr)) ok = 0;
                size += ap.blocks[ix].length;
            }
        }

        /* unmap buffer */
        munmap(bufPtr, size);

        A_I(dec_ref(),==,0);
    }

    return R_I(!ok);
}

void *MemMgr_MapIn1DMode(void *dataPtr, bytes_t size, bytes_t stride)
{
    IN;

    int td;
    tiler_buf_info ap;
    ap.num_blocks = 1;
    ap.blocks[0].pixelFormat = TILFMT_PAGE;
    ap.blocks[0].length = size;
    ap.blocks[0].stride = stride;
    ap.blocks[0].ptr = dataPtr;

    /* map buffer into tiler space and register with tiler manager */
    ioctl(td, TILIOC_MBUF, &ap);

    /* map into process space */
    void *bufPtr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        td, ap.offset);

    /* cache offset for buffer */
    cache_buf_add(bufPtr, ap.offset);

    return R_P(bufPtr);
}

bool MemMgr_UnMap(void *bufPtr)
{
    IN;

    int ret, ok, td;
    tiler_buf_info ap;
    bytes_t size;

    /* at this point Memory Allocator stops tracking this buffer */
    cache_buf_del(bufPtr);

    ret = ioctl(td, TILIOC_QBUF, &ap);

    /* unregister buffer */
    if (ok)
    {
        ret = ioctl(td, TILIOC_URBUF, &ap);
        int ix;
        for (ix = 0; ix < ap.num_blocks; ix++)
        {
            if (tilerUnMap(ap.blocks[ix].ssptr)) ok = 0;
            size += ap.blocks[ix].length;
        }
    }

    /* unmap buffer */
    munmap(bufPtr, size);

    return R_I(!ok);
}

bool MemMgr_Is2DBuffer(void *ptr)
{
    IN;

    tiler_fmt fmt = get_block_format(ptr);
    return R_I(fmt == TILFMT_8BIT || fmt == TILFMT_16BIT || fmt == TILFMT_32BIT);
}

bool MemMgr_Is1DBuffer(void *ptr)
{
    IN;

    tiler_fmt fmt = get_block_format(ptr);
    return R_I(fmt == TILFMT_PAGE);
}

bool MemMgr_IsAlreadyMapped(void *ptr)
{
    IN;

    tiler_fmt fmt = get_block_format(ptr);
    return R_I(fmt == TILFMT_8BIT || fmt == TILFMT_16BIT || fmt == TILFMT_32BIT || fmt == TILFMT_PAGE);
}


DSPtr MemMgr_ReMapToDucati(void *bufPtr, bytes_t length)
{
    /* assumptions: buffer is already registered, and mapped to tiler */

    /* in phase 1, this could be a sub-block of a buffer */
    /* in phase 2, this must be the beginning of a buffer */

    /* use cache to figure out start of the buffer */

    /* get list of blocks from tiler */

    /* narrow down actual block to map */

    /* register this block with the tiler as a separate buffer */

    /* cache offset */

    /* map block to ducati space */
}

bool MemMgr_DeMapFromDucati(DSPtr bufDPtr)
{
    /* in phase 1, this could be a sub-block of a buffer */
    /* in phase 2, this must be the beginning of a buffer */

    /* get ssptr from ducati address */
    SSPtr ssptr = SysLink_Ducati_VirtToPhys(bufDPtr);

    /* find block that this ssptr belongs to */

    /* unregister this block */

    /* unmap block from ducati space */
}

void *MemMgr_ReMapFromDucati(MemMapBlock blocks[], int num_blocks)
{
    /* query tiler driver for each block's information */

    /* register buffer to be tracked by tiler */

    /* map buffer */
}

bool MemMgr_DeMap(void *bufPtr)
{
    /* find lengh of buffer in cache */
    bytes_t length = 0;

    /* unmap from process space */
    munmap(bufPtr, length);
}

