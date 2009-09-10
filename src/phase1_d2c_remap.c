/*
 * phase1_d2c_remap.c
 *
 * Ducati to Chiron Tiler block remap functions for TI OMAP processors.
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

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tiler.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define __DEBUG__
#define __DEBUG_ASSERT__

#include "mem_types.h"
#include "utils.h"
#include "tilermem_utils.h"
#include "phase1_d2c_remap.h"

#include "memmgr_common.c"

struct _ReMapData {
    void     *bufPtr;
    uint32_t  tiler_id;
    struct _ReMapList {
        struct _ReMapList *next, *last;
        struct _ReMapData *me;
    } link;
};
struct _ReMapList bufs;
static int bufs_inited = 0;
static pthread_mutex_t che_mutex = PTHREAD_MUTEX_INITIALIZER;

static void dump_block(struct tiler_block_info *blk, char *prefix, char *suffix)
{
    switch (blk->fmt)
    {
    case PIXEL_FMT_PAGE:
        P("%s [p=%p(0x%lx),l=0x%lx,s=%ld]%s", prefix, blk->ptr, blk->ssptr,
          blk->dim.len, blk->stride, suffix);
        break;
    case PIXEL_FMT_8BIT:
    case PIXEL_FMT_16BIT:
    case PIXEL_FMT_32BIT:
        P("%s [p=%p(0x%lx),%d*%d*%d,s=%ld]%s", prefix, blk->ptr, blk->ssptr,
          blk->dim.area.width, blk->dim.area.height, def_bpp(blk->fmt) * 8,
          blk->stride, suffix);
        break;
    default:
        P("%s*[p=%p(0x%lx),l=0x%lx,s=%ld,fmt=0x%x]%s", prefix, blk->ptr,
          blk->ssptr, blk->dim.len, blk->stride, blk->fmt, suffix);
    }
}

static void dump_buf(struct tiler_buf_info* buf, char* prefix)
{
    P("%sbuf={n=%d,id=0x%x,", prefix, buf->num_blocks, buf->offset);
    int ix = 0;
    for (ix = 0; ix < buf->num_blocks; ix++)
    {
        dump_block(buf->blocks + ix, "", ix + 1 == buf->num_blocks ? "}" : "");
    }
}

/**
 * Initializes the static structures
 * 
 * @author a0194118 (9/8/2009)
 */
static void init()
{
    if (!bufs_inited)
    {
        DLIST_INIT(bufs);
        bufs_inited = 1;
    }
}

/**
 * Records a buffer-pointer -- tiler-ID mapping. 
 * 
 * @author a0194118 (9/7/2009)
 * 
 * @param bufPtr    Buffer pointer
 * @param tiler_id  Tiler ID
 */
static void remap_cache_add(void *bufPtr, uint32_t tiler_id)
{
    pthread_mutex_lock(&che_mutex);
    init();
    struct _ReMapData *ad = NEW(struct _ReMapData);
    ad->bufPtr = bufPtr;
    ad->tiler_id = tiler_id;
    DLIST_MADD_BEFORE(bufs, ad, link);
    pthread_mutex_unlock(&che_mutex);
}

/**
 * Retrieves the tiler ID for given buffer pointer from the 
 * records. If the tiler ID is found, it is removed from the 
 * records as well. 
 * 
 * @author a0194118 (9/7/2009)
 * 
 * @param bufPtr    Buffer pointer
 * 
 * @return Tiler ID on success, 0 on failure.
 */
static uint32_t remap_cache_del(void *bufPtr)
{
    struct _ReMapData *ad;
    pthread_mutex_lock(&che_mutex);
    init();
    DLIST_MLOOP(bufs, ad, link) {
        if (ad->bufPtr == bufPtr) {
            uint32_t tiler_id = ad->tiler_id;
            DLIST_REMOVE(ad->link);
            FREE(ad);
            pthread_mutex_unlock(&che_mutex);
            return tiler_id;
        }
    }
    pthread_mutex_unlock(&che_mutex);
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
static bytes_t def_size(struct tiler_block_info *blk)
{
    return (blk->fmt == PIXEL_FMT_PAGE ?
            blk->dim.len :
            blk->dim.area.height * def_stride(blk->dim.area.width * def_bpp(blk->fmt)));
}

/**
 * During Phase 1, tiler info is needed to remap tiler blocks 
 * from Ducati to Chiron.  However, this is no longer needed in 
 * phase 2, as the virtual stride will be the same on Ducati and 
 * Chiron.  This API is provided here temporarily for phase 1.
 * 
 * @author a0194118 (9/9/2009)
 * 
 * @param num_blocks 
 * @param dsptrs 
 * @param lengths 
 * 
 * @return void* 
 */
void *tiler_assisted_phase1_D2CReMap(int num_blocks, DSPtr dsptrs[],
                                     bytes_t lengths[])
{
    IN;

    /* we can only remap up to the TILER supported number of blocks */
    if (NOT_I(num_blocks,>,0) || NOT_I(num_blocks,<=,TILER_MAX_NUM_BLOCKS))
        return R_P(NULL);

    struct tiler_buf_info buf;
    ZERO(buf);
    buf.num_blocks = num_blocks;
    int ix, res;
    bytes_t size = 0;

    /* need tiler driver */
    int td = open("/dev/tiler", O_RDWR | O_SYNC);
    if (NOT_I(td,>=,0)) return R_P(NULL);

    void *bufPtr = NULL;

    /* for each block */
    for (ix = 0; ix < num_blocks; ix++)
    {
        /* convert DSPtrs to SSPtrs using SysLink */       
        SSPtr ssptr = buf.blocks[ix].ssptr = SysLink_DucatiToPhys(dsptrs[ix]);
        if (NOT_P(buf.blocks[ix].ssptr,!=,0)) {
            P("for dsptrs[%d]=0x%x", ix, dsptrs[ix]);
            goto FAIL;
        }

        /* query tiler driver for details on these blocks, such as
           width/height/len/fmt */
        dump_block(buf.blocks + ix, "=(qb)=>", "");
        res = ioctl(td, TILIOC_QBLOCK, buf.blocks + ix);
        dump_block(buf.blocks + ix, "<=(qb)=", "");

        if (NOT_I(res,==,0) || NOT_I(buf.blocks[ix].ssptr,!=,0))
        {
            P("tiler did not allocate dsptr[%d]=0x%x ssptr=0x%x", ix, dsptrs[ix], ssptr);
            goto FAIL;
        }

        /* add up size of buffer after remap */
        size += def_size(buf.blocks + ix);
    }

    /* register this buffer and/or query last registration */
    dump_buf(&buf, "==(RBUF)=>");
    res = ioctl(td, TILIOC_RBUF, &buf);
    dump_buf(&buf, "<=(RBUF)==");
    if (NOT_I(res,==,0) || NOT_P(buf.offset,!=,0)) goto FAIL;

    /* map blocks to process space */
    bufPtr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                        td, buf.offset);
    DP("ptr=%p", bufPtr);

    /* if failed to map: unregister buffer */
    if (NOT_P(bufPtr,!=,NULL))
    {
        A_I(ioctl(td, TILIOC_URBUF, &buf),==,0);
    }
    /* otherwise, fill out pointers */
    else
    {
        /* cache tiler ID for buffer */
        remap_cache_add(bufPtr, buf.offset);

        /* fill out pointers */
        for (size = ix = 0; ix < num_blocks; ix++)
        {
            buf.blocks[ix].ptr = bufPtr + size;
            /* P("   [0x%p]", blks[ix].ptr); */
            size += def_size(buf.blocks + ix);
        }
    }

FAIL:
    close(td);

    return R_P(bufPtr);
}


int tiler_assisted_phase1_DeMap(void *bufPtr)
{
    IN;

    int ret = REMAP_ERR_GENERIC, ix;
    struct tiler_buf_info buf;
    ZERO(buf);

    /* need tiler driver */
    int td = open("/dev/tiler", O_RDWR | O_SYNC);
    if (NOT_I(td,>=,0)) return R_I(ret);

    /* retrieve registered buffers from vsptr */
    /* :NOTE: if this succeeds, Memory Allocator stops tracking this buffer */
    buf.offset = remap_cache_del(bufPtr);

    if (A_L(buf.offset,!=,0))
    {
        /* get block information for the buffer */
        dump_buf(&buf, "==(QBUF)=>");
        ret = A_I(ioctl(td, TILIOC_QBUF, &buf),==,0);
        dump_buf(&buf, "<=(QBUF)==");

        /* unregister buffer, and free tiler chunks even if there is an
           error */
        if (!ret)
        {
            dump_buf(&buf, "==(URBUF)=>");
            ret = A_I(ioctl(td, TILIOC_URBUF, &buf),==,0);
            dump_buf(&buf, "<=(URBUF)==");
    
            /* unmap buffer */
            bytes_t size = 0;
            for (ix = 0; ix < buf.num_blocks; ix++)
            {
                size += def_size(buf.blocks + ix);
            }
            ERR_ADD(ret, munmap(bufPtr, size));
        }
    }

    close(td);

    return R_I(ret);
}


