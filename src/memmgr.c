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

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1

#include "../../tiler-omap4/drivers/media/video/tiler/dmm.h"

typedef struct tiler_block_info tiler_block_info;

#define __DEBUG__
// #define __DEBUG_ENTRY__
#define __DEBUG_ASSERT__

#include "memmgr.h"
#include "utils.h"
#include "tilermem.h"
#include "memmgr_utils.h"

/* list of allocations */
struct _AllocData {
    void *bufPtr;
    unsigned long offset;
    struct _AllocList {
        struct _AllocList *next, *last;
        struct _AllocData *me;
    } link;
};
struct _AllocList allocs;
static int allocs_inited = 0;

typedef struct _AllocList _AllocList;
typedef struct _AllocData _AllocData;

static int refCnt = 0;
static int td = -1;
static void init()
{
    if (!allocs_inited)
    {
        DLIST_INIT(allocs);
        allocs_inited = 1;
    }
}

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
    if (!refCnt++) {
        /* initialize lists */
        init();
#ifndef __STUB_TILER__
        td = open("/dev/tiler", O_RDWR | O_SYNC);
        if (NOT_I(td,>=,0) || NOT_I(tilerInit(),==,0)) {
            refCnt--;
            return MEMMGR_ERR_GENERIC;
        }
#else
        td = 2;
        return 0;
#endif
    }
    return MEMMGR_ERR_NONE;
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
    if (!--refCnt) {
#ifndef __STUB_TILER__
        fclose(td);
        td = -1;
        return A_I(tilerDeinit(),==,0);
#else
        return 0;
#endif
    }
    return 0;
}

/**
 * Returns the size of the supplied block
 * 
 * @author a0194118 (9/4/2009)
 * 
 * @param blk    Pointer to the tiler_block_info struct
 * 
 * @return size of the block in bytes 
 */
static bytes_t def_size(tiler_block_info *blk)
{
    return (blk->fmt == PIXEL_FMT_PAGE ?
            blk->length :
            blk->height * def_stride(blk->width * def_bpp(blk->fmt)));
}

enum tiler_fmt tiler_get_fmt(SSPtr ssptr)
{
#ifndef __STUB_TILER__
    return (ssptr == 0              ? TILFMT_INVALID :
            ssptr < TILER_MEM_8BIT  ? TILFMT_NONE :
            ssptr < TILER_MEM_16BIT ? TILFMT_8BIT :
            ssptr < TILER_MEM_32BIT ? TILFMT_16BIT :
            ssptr < TILER_MEM_PAGED ? TILFMT_32BIT :
            ssptr < TILER_MEM_END   ? TILFMT_PAGE : TILFMT_NONE);
#else
    /* if emulating, we need to get through all allocated memory segments */
    init();
    _AllocData *ad;
    void *ptr = (void *) ssptr;
    if (!ptr) return TILFMT_INVALID;
    DLIST_MLOOP(allocs, ad, link) {
        int ix;
        struct tiler_buf_info *buf = (struct tiler_buf_info *) ad->offset;
        // P("buf[%d]", buf->num_blocks);
        for (ix = 0; ix < buf->num_blocks; ix++)
        {
            // P("block[%p-%p]", buf->blocks[ix].ptr, buf->blocks[ix].ptr + def_size(buf->blocks + ix));
            if (ptr >= buf->blocks[ix].ptr &&
                ptr < buf->blocks[ix].ptr + def_size(buf->blocks + ix))
                return buf->blocks[ix].fmt;
        }
    }
    return TILFMT_NONE;
#endif
}

/**
 * Checks whether the tiler_block_info is filled in correctly
 * 
 * @author a0194118 (9/4/2009)
 * 
 * @param blk   Pointer to the tiler_block_info struct
 * 
 * @return 0 on success, non-0 on failure
 */
static int check_block(tiler_block_info *blk, bool is_page_sized)
{
    /* check pixelformat */
    if (NOT_I(blk->fmt,>=,PIXEL_FMT_MIN) ||
        NOT_I(blk->fmt,<=,PIXEL_FMT_MAX)) return MEMMGR_ERR_GENERIC;

    
    if (blk->fmt == PIXEL_FMT_PAGE)
    {   /* check 1D buffers */

        /* length must be multiple of stride if stride > 0 */
        if (NOT_I(blk->length,>,0) ||
            (blk->stride && NOT_I(blk->length % blk->stride,==,0)))
            return MEMMGR_ERR_GENERIC;
    }
    else 
    {   /* check 2D buffers */
   
        /* check width, height and stride (must be the default stride or 0) */
        bytes_t stride = def_stride(blk->width * def_bpp(blk->fmt));
        if (NOT_I(blk->width,>,0) ||
            NOT_I(blk->height,>,0) ||
            (blk->stride && NOT_I(blk->stride,==,stride)))
            return MEMMGR_ERR_GENERIC;
    }

    if (is_page_sized && NOT_I(def_size(blk) & (PAGE_SIZE - 1),==,0))
        return MEMMGR_ERR_GENERIC;

    return MEMMGR_ERR_NONE;
}

static void alloc_cache_add(void *bufPtr, unsigned long offset)
{
    _AllocData *ad = NEW(_AllocData);
    ad->bufPtr = bufPtr;
    ad->offset = offset;
    DLIST_MADD_BEFORE(allocs, ad, link);
}

static unsigned long alloc_cache_find(void *bufPtr)
{
    _AllocData *ad;
    DLIST_MLOOP(allocs, ad, link) {
        if (ad->bufPtr == bufPtr) return ad->offset;
    }
    return 0;
}

static unsigned long alloc_cache_del(void *bufPtr)
{
    _AllocData *ad;
    DLIST_MLOOP(allocs, ad, link) {
        if (ad->bufPtr == bufPtr) {
            unsigned long offset = ad->offset;
            DLIST_REMOVE(ad->link);
            FREE(ad);
            return offset;
        }
    }
    return 0;
}

static int cache_check()
{
    int num_allocs = 0;

    init();

    _AllocData *ad;
    DLIST_MLOOP(allocs, ad, link) { num_allocs++; }

    return (num_allocs == refCnt) ? MEMMGR_ERR_NONE : MEMMGR_ERR_GENERIC;
}

bytes_t MemMgr_PageSize()
{
    return PAGE_SIZE;
}

void *MemMgr_Alloc(MemAllocBlock blocks[], int num_blocks)
{
    IN;

    /* need to access ssptrs */
    struct tiler_block_info *blks = (tiler_block_info *) blocks;

    /* check arguments */
    if (NOT_I(num_blocks,>,0)) goto FAIL;
    if (NOT_I(num_blocks,<=,TILER_MAX_NUM_BLOCKS)) goto FAIL;

    /* check block allocation params */
    int ix, ok = 1;
    for (ix = 0; ix < num_blocks; ix++)
    {
        NOT_I(blks[ix].ptr,==,NULL);
        NOT_I(blks[ix].ssptr,==,0);
        if (check_block(blks + ix, ix < num_blocks - 1))
        {
            DP("for block[%d]", ix);
            ok = 0;
        }
    }
    if (!ok) goto FAIL;

    /* need tiler */
    if (NOT_I(inc_ref(),==,0)) goto FAIL;

    /* ----- begin recoverable portion ----- */

    int ret;

    /* allocate each buffer using tiler driver and initialize block info */
    bytes_t size = 0;
    for (ix = 0; ix < num_blocks; ix++)
    {
        blks[ix].ptr = NULL;
#ifndef __STUB_TILER__
        blks[ix].ssptr = tilerAlloc(blks + ix);
        if (NOT_I(blks[ix].ssptr,>,0)) goto FAIL_ALLOC;
#else
        blks[ix].ssptr = def_size(blks + ix);
#endif
        if (blks[ix].fmt != PIXEL_FMT_PAGE)
            blks[ix].stride = def_stride(blks[ix].width *
                                         def_bpp(blks[ix].fmt));
        size += def_size(blks + ix);
    }

    /* register buffer with tiler */
    struct tiler_buf_info buf;
    buf.num_blocks = num_blocks;
    memcpy(buf.blocks, blocks, sizeof(tiler_block_info) * num_blocks);
#ifndef __STUB_TILER__
    ret = ioctl(td, TILIOC_RBUF, &buf);
    if (NOT_P(buf.offset,!=,0)) goto FAIL_ALLOC;

    /* map blocks to process space */
    void *bufPtr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        td, buf.offset);
#else
    void *bufPtr = malloc(size);
    // P("<= [0x%lx]", size);
    /* save */
    struct tiler_buf_info *buf_c = NEW(struct tiler_buf_info);
    buf.offset = (unsigned long) buf_c;
    /* fill out pointers */
    for (size = ix = 0; ix < num_blocks; ix++)
    {
        buf.blocks[ix].ptr = bufPtr + size;
        size += def_size(blks + ix);
    }

    memcpy(buf_c, &buf, sizeof(struct tiler_buf_info));
#endif
    if (A_P(bufPtr,!=,0)) {
        /* cache offset for buffer */
        alloc_cache_add(bufPtr, buf.offset);

        /* fill out pointers */
        for (size = ix = 0; ix < num_blocks; ix++)
        {
            blks[ix].ptr = bufPtr + size;
#ifdef __STUB_TILER__
            blks[ix].ssptr = (unsigned long) blks[ix].ptr;
#endif
            size += def_size(blks + ix);
        }

        close(td);
        CHK_I(cache_check(),==,0);
        return R_P(bufPtr);
    }
    
    /* ------ error handling ------ */

#ifndef __STUB_TILER__
    /* failed to map: unregister */
    ret = ioctl(td, TILIOC_URBUF, &buf);

FAIL_ALLOC:
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
#endif

    A_I(dec_ref(),==,0);
FAIL:
    CHK_I(cache_check(),==,0);
    return R_P(NULL);
}

int MemMgr_Free(void *bufPtr)
{
    IN;

    int ret = MEMMGR_ERR_NONE, ok = 1;
    bytes_t size = 0;
    struct tiler_buf_info buf;

    /* retrieve registered buffers from vsptr */
    /* if this succeeds, Memory Allocator stops tracking this buffer */
    buf.offset = alloc_cache_del(bufPtr);
    if (NOT_L(buf.offset,!=,0)) ret = MEMMGR_ERR_GENERIC;

    /* get block information for the buffer */
    if (!ret)
    {
#ifndef __STUB_TILER__
        ret = A_I(ioctl(td, TILIOC_QBUF, &buf),==,0);

        /* unregister buffer */
        if (!ret)
        {
            ret = A_I(ioctl(td, TILIOC_URBUF, &buf),==,0);
    
            /* free each block */
            int ix;
            if (!ret)
            {
                for (ix = 0; ix < buf.num_blocks; ix++)
                {
                    if (tilerFree(buf.blocks[ix].ssptr))
                        ret = MEMMGR_ERR_GENERIC;
                    size += def_size(buf.blocks + ix);
                }
            }

            /* unmap buffer */
            int ret2 = A_I(munmap(bufPtr, size),==,0);
            ret = ret ? ret : ret2;
        }
#else
        void *ptr = (void *) buf.offset;
        FREE(ptr);
#endif
        A_I(dec_ref(),==,0);
    }

    CHK_I(cache_check(),==,0);
    return R_I(ret);
}

void *MemMgr_Map(MemAllocBlock blocks[], int num_blocks)
{
    IN;

    /* need to access ssptrs */
    struct tiler_block_info *blks = (tiler_block_info *) blocks;

    /* check arguments */
    if (NOT_I(num_blocks,>,0)) goto FAIL;
    if (NOT_I(num_blocks,<=,TILER_MAX_NUM_BLOCKS)) goto FAIL;

    /* need tiler */
    if (NOT_I(inc_ref(),==,0)) goto FAIL;

    /* check block allocation params */
    int ix, ok = 1;
    for (ix = 0; ix < num_blocks; ix++)
    {
        NOT_I(blks[ix].ptr,!=,NULL);
        NOT_I(blks[ix].ssptr,==,0);
        if (check_block(blks + ix, TRUE))
        {
            DP("for block[%d]", ix);
            ok = 0;
        }
    }
    if (!ok) goto FAIL;

    /* we only map 1 1D buffer for now */
    if (NOT_I(num_blocks,==,1) ||
        NOT_I(blocks[0].pixelFormat,==,PIXEL_FMT_PAGE))
        goto FAIL;

    /* ----- begin recoverable portion ----- */

    /* initialize buffer specs */
    int ret, td = -1;

    /* allocate each buffer using tiler driver */
    bytes_t size = 0;
    for (ix = 0; ix < num_blocks; ix++)
    {
#ifndef __STUB_TILER__
        blks[ix].ssptr = tilerMap(blks + ix);
#endif
        if (NOT_I(blks[ix].ssptr,>,0)) goto FAIL_MAP;
        size += def_size(blks + ix);
    }

    /* map bufer into tiler space and register with tiler manager */
    struct tiler_buf_info buf;
    buf.num_blocks = num_blocks;
    memcpy(buf.blocks, blocks, sizeof(tiler_block_info) * num_blocks);
    ret = ioctl(td, TILIOC_MBUF, &buf);
    if (NOT_P(buf.offset,!=,0)) goto FAIL_MAP;

    /* map blocks to process space */
    void *bufPtr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        td, buf.offset);
    if (NOT_P(bufPtr,!=,0)) {
        /* cache offset for buffer */
        alloc_cache_add(bufPtr, buf.offset);

        /* fill out pointers */
        close(td);
        CHK_I(cache_check(),==,0);
        return R_P(bufPtr);
    }

    /* ------ error handling ------ */

#ifndef __STUB_TILER__
    /* failed to map: unregister */
    ret = ioctl(td, TILIOC_URBUF, &buf);
#endif

FAIL_MAP:
#ifndef __STUB_TILER__
    while (ix)
    {
        ix--;
        tilerUnMap(blks[ix].ssptr);
    }
#endif

    /* clear ssptr and ptr fields for all blocks */
    for (ix = 0; ix < num_blocks; ix++)
    {
        blks[ix].ssptr = 0;
        blks[ix].ptr = NULL;
    }

    A_I(dec_ref(),==,0);
FAIL:
    CHK_I(cache_check(),==,0);
    return R_P(NULL);
}

int MemMgr_UnMap(void *bufPtr)
{
    IN;

    int ret = MEMMGR_ERR_NONE, ok = 1, td = -1;
    bytes_t size = 0;
    struct tiler_buf_info buf;

    /* retrieve registered buffers from vsptr */
    /* if this succeeds, Memory Allocator stops tracking this buffer */
    buf.offset = alloc_cache_del(bufPtr);
    if (NOT_L(buf.offset,!=,0)) ret = MEMMGR_ERR_GENERIC;

    /* get block information for the buffer */
    if (!ret)
    {
#ifndef __STUB_TILER__
        ret = A_I(ioctl(td, TILIOC_QBUF, &buf),==,0);

        /* unregister buffer */
        if (!ret)
        {
            ret = A_I(ioctl(td, TILIOC_URBUF, &buf),==,0);
    
            /* free each block */
            int ix;
            if (!ret)
            {
                for (ix = 0; ix < buf.num_blocks; ix++)
                {
                    if (tilerUnMap(buf.blocks[ix].ssptr))
                        ret = MEMMGR_ERR_GENERIC;
                    size += def_size(buf.blocks + ix);
                }
            }

            /* unmap buffer */
            int ret2 = A_I(munmap(bufPtr, size),==,0);
            ret = ret ? ret : ret2;
        }

        A_I(dec_ref(),==,0);
#endif
    }

    CHK_I(cache_check(),==,0);
    return R_I(ret);
}

bool MemMgr_Is1DBlock(void *ptr)
{
    IN;

    SSPtr ssptr = TilerMem_VirtToPhys(ptr);
    enum tiler_fmt fmt = tiler_get_fmt(ssptr);
    return R_I(fmt == TILFMT_PAGE);
}

bool MemMgr_Is2DBlock(void *ptr)
{
    IN;

    SSPtr ssptr = TilerMem_VirtToPhys(ptr);
    enum tiler_fmt fmt = tiler_get_fmt(ssptr);
    return R_I(fmt == TILFMT_8BIT || fmt == TILFMT_16BIT ||
               fmt == TILFMT_32BIT);
}

bool MemMgr_IsMapped(void *ptr)
{
    IN;

    SSPtr ssptr = TilerMem_VirtToPhys(ptr);
    enum tiler_fmt fmt = tiler_get_fmt(ssptr);
    return R_I(fmt == TILFMT_8BIT || fmt == TILFMT_16BIT ||
               fmt == TILFMT_32BIT || fmt == TILFMT_PAGE);
}

bytes_t MemMgr_GetStride(void *ptr)
{
    IN;
#ifndef __STUB_TILER__
    /* :TODO: */
#else
    /* if emulating, we need to get through all allocated memory segments */
    init();

    _AllocData *ad;
    if (!ptr) return R_UP(0);
    DLIST_MLOOP(allocs, ad, link) {
        int ix;
        struct tiler_buf_info *buf = (struct tiler_buf_info *) ad->offset;
        for (ix = 0; ix < buf->num_blocks; ix++)
        {
            if (ptr >= buf->blocks[ix].ptr &&
                ptr < buf->blocks[ix].ptr + def_size(buf->blocks + ix))
                return R_UP(buf->blocks[ix].stride);
        }
    }
    return R_UP(PAGE_SIZE);
#endif
}

bytes_t TilerMem_GetStride(SSPtr ssptr)
{
    IN;
    switch(tiler_get_fmt(ssptr))
    {
    case TILFMT_8BIT:  return R_UP(TILER_STRIDE_8BIT);
    case TILFMT_16BIT: return R_UP(TILER_STRIDE_16BIT);
    case TILFMT_32BIT: return R_UP(TILER_STRIDE_32BIT);
    case TILFMT_PAGE:  return R_UP(PAGE_SIZE);
    default:           return R_UP(0);
    }
}

SSPtr TilerMem_VirtToPhys(void *ptr)
{
#ifndef __STUB_TILER__
    return (SSPtr)tilervirt2phys((unsigned long *) ptr);
#else
    return (SSPtr)ptr;
#endif
}
/**
 * Internal Unit Test.  Tests the static methods of this library
 * 
 * @author a0194118 (9/4/2009)
 */
void memmgr_internal_unit_test()
{
    A_I(refCnt,==,0);
    A_I(inc_ref(),==,0);
    A_I(refCnt,==,1);
    A_I(dec_ref(),==,0);
    A_I(refCnt,==,0);

    /* void * arithmetic */
    void *a = (void *)1000, *b = a + 2000, *c = (void *)3000;
    A_P(b,==,c);

    /* def_stride */
    A_I(def_stride(0),==,0);
    A_I(def_stride(1),==,PAGE_SIZE);
    A_I(def_stride(PAGE_SIZE),==,PAGE_SIZE);
    A_I(def_stride(PAGE_SIZE + 1),==,2 * PAGE_SIZE);

    /* def_bpp */
    A_I(def_bpp(PIXEL_FMT_32BIT),==,4);
    A_I(def_bpp(PIXEL_FMT_16BIT),==,2);
    A_I(def_bpp(PIXEL_FMT_8BIT),==,1);

    /* def_size */
    tiler_block_info blk;
    blk.fmt = TILFMT_8BIT;
    blk.width = PAGE_SIZE * 8 / 10;
    blk.height = 10;
    A_I(def_size(&blk),==,10 * PAGE_SIZE);

    blk.fmt = TILFMT_16BIT;
    blk.width = PAGE_SIZE * 7 / 10;
    A_I(def_size(&blk),==,20 * PAGE_SIZE);
    blk.width = PAGE_SIZE * 4 / 10;
    A_I(def_size(&blk),==,10 * PAGE_SIZE);

    blk.fmt = TILFMT_32BIT;
    A_I(def_size(&blk),==,20 * PAGE_SIZE);
    blk.width = PAGE_SIZE * 6 / 10;
    A_I(def_size(&blk),==,30 * PAGE_SIZE);
}


