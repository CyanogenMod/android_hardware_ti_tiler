/*
 * memmgr_test.c
 *
 * Memory Allocator Interface tests.
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

/* retrieve type definitions */
#define __DEBUG__
#define __DEBUG_ENTRY__
#define __DEBUG_ASSERT__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../src/memmgr.h"
#include "../src/memmgr_utils.h"
#include "../src/tilermem.h"
#include "../src/utils.h"

#define FALSE 0

#define TESTS\
    T(alloc_1D_test(4096, 0))\
    T(alloc_2D_test(64, 64, PIXEL_FMT_8BIT))\
    T(alloc_2D_test(64, 64, PIXEL_FMT_16BIT))\
    T(alloc_2D_test(64, 64, PIXEL_FMT_32BIT))\
    T(alloc_NV12_test(64, 64))\
    T(map_1D_test(4096, 0))\
    T(alloc_1D_test(176 * 144 * 2, 512))\
    T(alloc_2D_test(176, 144, PIXEL_FMT_8BIT))\
    T(alloc_2D_test(176, 144, PIXEL_FMT_16BIT))\
    T(alloc_2D_test(176, 144, PIXEL_FMT_32BIT))\
    T(alloc_NV12_test(176, 144))\
    T(map_1D_test(176 * 144 * 2, 2048))\
    T(alloc_1D_test(640 * 480 * 2, 0))\
    T(alloc_2D_test(640, 480, PIXEL_FMT_8BIT))\
    T(alloc_2D_test(640, 480, PIXEL_FMT_16BIT))\
    T(alloc_2D_test(640, 480, PIXEL_FMT_32BIT))\
    T(alloc_NV12_test(640, 480))\
    T(map_1D_test(640 * 480 * 2, 0))\
    T(alloc_1D_test(848 * 480 * 2, 0))\
    T(alloc_2D_test(848, 480, PIXEL_FMT_8BIT))\
    T(alloc_2D_test(848, 480, PIXEL_FMT_16BIT))\
    T(alloc_2D_test(848, 480, PIXEL_FMT_32BIT))\
    T(alloc_NV12_test(848, 480))\
    T(map_1D_test(848 * 480 * 2, 0))\
    T(alloc_1D_test(1280 * 720 * 2, 0))\
    T(alloc_2D_test(1280, 720, PIXEL_FMT_8BIT))\
    T(alloc_2D_test(1280, 720, PIXEL_FMT_16BIT))\
    T(alloc_2D_test(1280, 720, PIXEL_FMT_32BIT))\
    T(alloc_NV12_test(1280, 720))\
    T(map_1D_test(1280 * 720 * 2, 0))\
    T(alloc_1D_test(1920 * 1080 * 2, 0))\
    T(alloc_2D_test(1920, 1080, PIXEL_FMT_8BIT))\
    T(alloc_2D_test(1920, 1080, PIXEL_FMT_16BIT))\
    T(alloc_2D_test(1920, 1080, PIXEL_FMT_32BIT))\
    T(alloc_NV12_test(1920, 1080))\
    T(map_1D_test(1920 * 1080 * 2, 0))\
    T(neg_alloc_tests())\
    T(neg_free_tests())\
    T(neg_map_tests())\
    T(neg_unmap_tests())\
    T(neg_check_tests())\
    T(maxalloc_1D_test(4096))\
    T(maxalloc_2D_test(64, 64, PIXEL_FMT_8BIT))\
    T(maxalloc_2D_test(64, 64, PIXEL_FMT_16BIT))\
    T(maxalloc_2D_test(64, 64, PIXEL_FMT_32BIT))\
    T(maxalloc_NV12_test(64, 64))\
    T(maxmap_1D_test(4096))\
    T(maxalloc_1D_test(176 * 144 * 2))\
    T(maxalloc_2D_test(176, 144, PIXEL_FMT_8BIT))\
    T(maxalloc_2D_test(176, 144, PIXEL_FMT_16BIT))\
    T(maxalloc_2D_test(176, 144, PIXEL_FMT_32BIT))\
    T(maxalloc_NV12_test(176, 144))\
    T(maxmap_1D_test(176 * 144 * 2))\
    T(maxalloc_1D_test(640 * 480 * 2))\
    T(maxalloc_2D_test(640, 480, PIXEL_FMT_8BIT))\
    T(maxalloc_2D_test(640, 480, PIXEL_FMT_16BIT))\
    T(maxalloc_2D_test(640, 480, PIXEL_FMT_32BIT))\
    T(maxalloc_NV12_test(640, 480))\
    T(maxmap_1D_test(640 * 480 * 2))\
    T(maxalloc_1D_test(848 * 480 * 2))\
    T(maxalloc_2D_test(848, 480, PIXEL_FMT_8BIT))\
    T(maxalloc_2D_test(848, 480, PIXEL_FMT_16BIT))\
    T(maxalloc_2D_test(848, 480, PIXEL_FMT_32BIT))\
    T(maxalloc_NV12_test(848, 480))\
    T(maxmap_1D_test(848 * 480 * 2))\
    T(maxalloc_1D_test(1280 * 720 * 2))\
    T(maxalloc_2D_test(1280, 720, PIXEL_FMT_8BIT))\
    T(maxalloc_2D_test(1280, 720, PIXEL_FMT_16BIT))\
    T(maxalloc_2D_test(1280, 720, PIXEL_FMT_32BIT))\
    T(maxalloc_NV12_test(1280, 720))\
    T(maxmap_1D_test(1280 * 720 * 2))\
    T(maxalloc_1D_test(1920 * 1080 * 2))\
    T(maxalloc_2D_test(1920, 1080, PIXEL_FMT_8BIT))\
    T(maxalloc_2D_test(1920, 1080, PIXEL_FMT_16BIT))\
    T(maxalloc_2D_test(1920, 1080, PIXEL_FMT_32BIT))\
    T(maxalloc_NV12_test(1920, 1080))\
    T(maxmap_1D_test(1920 * 1080 * 2))\
    T(star_test(1000, 10))\

/**
 * Fills up a range of memory using a start address and start 
 * value. 
 * 
 * @author a0194118 (9/6/2009)
 * 
 * @param start   start value
 * @param bi      pointer to block info strucure
 */
void fill_mem(uint16_t start, MemAllocBlock *block)
{
    uint16_t *ptr = (uint16_t *)block->ptr, delta = 1, step = 1;
    bytes_t height, width, stride, i;
    if (block->pixelFormat == PIXEL_FMT_PAGE)
    {
        height = 1;
        stride = width = block->length;      
    }
    else
    {
        height = block->height;
        width = block->width;
        stride = block->stride;
    }
    width *= def_bpp(block->pixelFormat);

    // P("(%p,0x%lx*0x%lx,s=0x%lx)", block->ptr, width, height, stride);

    A_I(width,<=,stride);
    while (height--)
    {
        for (i = 0; i < width; i += sizeof(uint16_t))
        {
            *ptr++ = start;
            start += delta;
            delta += step;
            /* increase step if overflown */
            if (delta < step) delta = ++step;
        }
        while (i < stride)
        {
            *ptr++ = 0;
            i += sizeof(uint16_t);
        }
    }
}

/**
 * Fills up a range of memory using a start address and start 
 * value. 
 * 
 * @author a0194118 (9/6/2009)
 * 
 * @param start   start value
 * @param block      pointer to block info strucure
 * 
 * @return int 
 */
int check_mem(uint16_t start, MemAllocBlock *block)
{
    uint16_t *ptr = (uint16_t *)block->ptr, delta = 1, step = 1;
    bytes_t height, width, stride, r, i;
    if (block->pixelFormat == PIXEL_FMT_PAGE)
    {
        height = 1;
        stride = width = block->length;      
    }
    else
    {
        height = block->height;
        width = block->width;
        stride = block->stride;
    }
    width *= def_bpp(block->pixelFormat);

    A_I(width,<=,stride);
    for (r = 0; r < height; r++)
    {
        for (i = 0; i < width; i += sizeof(uint16_t))
        {
            if (*ptr++ != start) {
                DP("assert: val[%lu,%lu] (=0x%x) != 0x%x", r, i, *--ptr, start);
                return 1;
            }
            start += delta;
            delta += step;
            /* increase step if overflown */
            if (delta < step) delta = ++step;
        }
        while (i < stride)
        {
            if (*ptr++) {
                DP("assert: val[%lu,%lu] (=0x%x) != 0", r, i, *--ptr);
                return 1;
            }
            i += sizeof(uint16_t);
        }
    }
    return 0;
}

void *alloc_1D(bytes_t length, bytes_t stride, uint16_t val)
{
    MemAllocBlock block;
    memset(&block, 0, sizeof(block));

    block.pixelFormat = PIXEL_FMT_PAGE;
    block.length = length;
    block.stride = stride;

    void *bufPtr = MemMgr_Alloc(&block, 1);
    CHK_P(bufPtr,==,block.ptr);
    if (bufPtr) {
        if (NOT_I(MemMgr_IsMapped(bufPtr),!=,0) ||
            NOT_I(MemMgr_Is1DBlock(bufPtr),!=,0) ||
            NOT_I(MemMgr_Is2DBlock(bufPtr),==,0) ||
            NOT_I(MemMgr_GetStride(bufPtr),==,block.stride) ||
            NOT_P(TilerMem_VirtToPhys(bufPtr),==,block.reserved) ||
            NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(bufPtr)),==,PAGE_SIZE))
        {
            MemMgr_Free(bufPtr);
            return NULL;
        }
        fill_mem(val, &block);
    }
    return bufPtr;
}

int free_1D(bytes_t length, bytes_t stride, uint16_t val, void *bufPtr)
{
    MemAllocBlock block;
    memset(&block, 0, sizeof(block));

    block.pixelFormat = PIXEL_FMT_PAGE;
    block.length = length;
    block.stride = stride;
    block.ptr = bufPtr;

    int ret = A_I(check_mem(val, &block),==,0);
    ERR_ADD(ret, MemMgr_Free(bufPtr));
    return ret;
}

void *alloc_2D(pixels_t width, pixels_t height, pixel_fmt_t fmt, bytes_t stride,
               uint16_t val)
{
    MemAllocBlock block;
    memset(&block, 0, sizeof(block));

    block.pixelFormat = fmt;
    block.width  = width;
    block.height = height;
    block.stride = stride;

    void *bufPtr = MemMgr_Alloc(&block, 1);
    CHK_P(bufPtr,==,block.ptr);
    if (bufPtr) {
        bytes_t cstride = (fmt == PIXEL_FMT_8BIT  ? TILER_STRIDE_8BIT :
                           fmt == PIXEL_FMT_16BIT ? TILER_STRIDE_16BIT :
                           TILER_STRIDE_32BIT);

        if (NOT_I(MemMgr_IsMapped(bufPtr),!=,0) ||
            NOT_I(MemMgr_Is1DBlock(bufPtr),==,0) ||
            NOT_I(MemMgr_Is2DBlock(bufPtr),!=,0) ||
            NOT_I(block.stride,!=,0) ||
            NOT_I(MemMgr_GetStride(bufPtr),==,block.stride) ||
            NOT_P(TilerMem_VirtToPhys(bufPtr),==,block.reserved) ||
            NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(bufPtr)),==,cstride))
        {
            MemMgr_Free(bufPtr);
            return NULL;
        }
        fill_mem(val, &block);
    }
    return bufPtr;
}

int free_2D(pixels_t width, pixels_t height, pixel_fmt_t fmt, bytes_t stride,
            uint16_t val, void *bufPtr)
{
    MemAllocBlock block;
    memset(&block, 0, sizeof(block));

    block.pixelFormat = fmt;
    block.width  = width;
    block.height = height;
    block.stride = def_stride(width * def_bpp(fmt));
    block.ptr = bufPtr;

    int ret = A_I(check_mem(val, &block),==,0);
    ERR_ADD(ret, MemMgr_Free(bufPtr));
    return ret;
}

void *alloc_NV12(pixels_t width, pixels_t height, uint16_t val)
{
    MemAllocBlock blocks[2];
    memset(blocks, 0, sizeof(blocks));

    blocks[0].pixelFormat = PIXEL_FMT_8BIT;
    blocks[0].width  = width;
    blocks[0].height = height;
    blocks[1].pixelFormat = PIXEL_FMT_16BIT;
    blocks[1].width  = width >> 1;
    blocks[1].height = height >> 1;

    void *bufPtr = MemMgr_Alloc(blocks, 2);
    CHK_P(blocks[0].ptr,==,bufPtr);
    if (bufPtr) {
        void *buf2 = bufPtr + blocks[0].stride * height;
        if (NOT_P(blocks[1].ptr,==,buf2) ||
            NOT_I(MemMgr_IsMapped(bufPtr),!=,0) ||
            NOT_I(MemMgr_IsMapped(buf2),!=,0) ||
            NOT_I(MemMgr_Is1DBlock(bufPtr),==,0) ||
            NOT_I(MemMgr_Is1DBlock(buf2),==,0) ||
            NOT_I(MemMgr_Is2DBlock(bufPtr),!=,0) ||
            NOT_I(MemMgr_Is2DBlock(buf2),!=,0) ||
            NOT_I(blocks[0].stride,!=,0) ||
            NOT_I(blocks[1].stride,!=,0) ||
            NOT_I(MemMgr_GetStride(bufPtr),==,blocks[0].stride) ||
            NOT_I(MemMgr_GetStride(buf2),==,blocks[1].stride) ||
            NOT_P(TilerMem_VirtToPhys(bufPtr),==,blocks[0].reserved) ||
            NOT_P(TilerMem_VirtToPhys(buf2),==,blocks[1].reserved) ||
            NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(bufPtr)),==,TILER_STRIDE_8BIT) ||
            NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(buf2)),==,TILER_STRIDE_16BIT))
        {
            MemMgr_Free(bufPtr);
            return NULL;            
        }

        fill_mem(val, blocks);
        fill_mem(val, blocks + 1);
    } else {
        A_I(blocks[1].ptr,==,NULL);
    }

    return bufPtr;
}

int free_NV12(pixels_t width, pixels_t height, uint16_t val, void *bufPtr)
{
    MemAllocBlock blocks[2];
    memset(blocks, 0, sizeof(blocks));

    blocks[0].pixelFormat = PIXEL_FMT_8BIT;
    blocks[0].width  = width;
    blocks[0].height = height;
    blocks[0].stride = def_stride(width);
    blocks[0].ptr = bufPtr;
    blocks[1].pixelFormat = PIXEL_FMT_16BIT;
    blocks[1].width  = width >> 1;
    blocks[1].height = height >> 1;
    blocks[1].stride = def_stride(width * 2);
    blocks[1].ptr = bufPtr + blocks[0].stride * height;

    int ret = A_I(check_mem(val, blocks),==,0);
    ERR_ADD(ret, check_mem(val, blocks + 1));
    ERR_ADD(ret, MemMgr_Free(bufPtr));
    return ret;
}

void *map_1D(void *dataPtr, bytes_t length, bytes_t stride, uint16_t val)
{
    MemAllocBlock block;
    memset(&block, 0, sizeof(block));

    block.pixelFormat = PIXEL_FMT_PAGE;
    block.length = length;
    block.stride = stride;
    block.ptr    = dataPtr;

    void *bufPtr = MemMgr_Map(&block, 1);
    CHK_P(bufPtr,==,block.ptr);
    if (bufPtr) {
        if (NOT_I(MemMgr_IsMapped(bufPtr),!=,0) ||
            NOT_I(MemMgr_Is1DBlock(bufPtr),!=,0) ||
            NOT_I(MemMgr_Is2DBlock(bufPtr),==,0) ||
            NOT_I(MemMgr_GetStride(bufPtr),==,block.stride) ||
            NOT_P(TilerMem_VirtToPhys(bufPtr),==,block.reserved) ||
            NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(bufPtr)),==,PAGE_SIZE))
        {
            MemMgr_UnMap(bufPtr);
            return NULL;
        }
        block.ptr = dataPtr;
        fill_mem(val, &block);
    }
    return bufPtr;
}

int unmap_1D(void *dataPtr, bytes_t length, bytes_t stride, uint16_t val, void *bufPtr)
{
    MemAllocBlock block;
    memset(&block, 0, sizeof(block));

    block.pixelFormat = PIXEL_FMT_PAGE;
    block.length = length;
    block.stride = stride;
    block.ptr = dataPtr;
    int ret = A_I(check_mem(val, &block),==,0);
    ERR_ADD(ret, MemMgr_UnMap(bufPtr));
    return ret;
}

int alloc_1D_test(bytes_t length, bytes_t stride)
{
    printf("Allocate & Free %lub 1D buffer\n", length);

    uint16_t val = (uint16_t) rand();
    void *ptr = alloc_1D(length, stride, val);
    if (!ptr) return 1;
    int res = free_1D(length, stride, val, ptr);
    return res;
}

int alloc_2D_test(pixels_t width, pixels_t height, pixel_fmt_t fmt)
{
    printf("Allocate & Free %ux%ux%lub 1D buffer\n", width, height, def_bpp(fmt));

    uint16_t val = (uint16_t) rand();
    void *ptr = alloc_2D(width, height, fmt, 0, val);
    if (!ptr) return 1;
    int res = free_2D(width, height, fmt, 0, val, ptr);
    return res;
}

int alloc_NV12_test(pixels_t width, pixels_t height)
{
    printf("Allocate & Free %ux%u NV12 buffer\n", width, height);

    uint16_t val = (uint16_t) rand();
    void *ptr = alloc_NV12(width, height, val);
    if (!ptr) return 1;
    int res = free_NV12(width, height, val, ptr);
    return res;
}

int map_1D_test(bytes_t length, bytes_t stride)
{
    length = (length + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1);
    printf("Mapping and UnMapping 0x%lxb 1D buffer\n", length);

    /* allocate aligned buffer */
    void *buffer = malloc(length + PAGE_SIZE - 1);
    void *dataPtr = (void *)(((uint32_t)buffer + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));
    uint16_t val = (uint16_t) rand();
    void *ptr = map_1D(dataPtr, length, stride, val);
    if (!ptr) return 1;
    int res = unmap_1D(dataPtr, length, stride, val, ptr);
    FREE(buffer);
    return res;
}

#define MAX_ALLOCS 10

int maxalloc_1D_test(bytes_t length)
{
    printf("Allocate & Free max # of %lub 1D buffers\n", length);

    struct data {
        uint16_t val;
        void    *bufPtr;
        struct data *next, *last, *me;
    } head, *dp, *dp2;

    /* allocate as many buffers as we can */
    DLIST_INIT(head);
    void *ptr;
    int num_bufs = 0;
    do
    {
        uint16_t val = (uint16_t) rand();
        ptr = alloc_1D(length, 0, val);
        dp = NEW(struct data);
        if (ptr && dp)
        {
            dp->val = val;
            dp->bufPtr = ptr;
            DLIST_ADD_BEFORE(head, dp, *dp);
            num_bufs++;
        }
        else
        {
            FREE(dp);
            if (ptr) free_1D(length, 0, val, ptr);
        }
        
    } while (ptr && num_bufs < MAX_ALLOCS);

    P(":: Allocated %d buffers", num_bufs);

    int res = 0;
    DLIST_SAFE_LOOP(head, dp, dp2) {
        ERR_ADD(res, free_1D(length, 0, dp->val, dp->bufPtr));
        DLIST_REMOVE(*dp);
    }
    return res;
}

int maxalloc_2D_test(pixels_t width, pixels_t height, pixel_fmt_t fmt)
{
    printf("Allocate & Free max # of %ux%ux%lub 1D buffers\n", width, height, def_bpp(fmt));

    struct data {
        uint16_t val;
        void    *bufPtr;
        struct data *next, *last, *me;
    } head, *dp, *dp2;

    /* allocate as many buffers as we can */
    DLIST_INIT(head);
    void *ptr;
    int num_bufs = 0;
    do
    {
        uint16_t val = (uint16_t) rand();
        ptr = alloc_2D(width, height, fmt, 0, val);
        dp = NEW(struct data);
        if (ptr && dp)
        {
            dp->val = val;
            dp->bufPtr = ptr;
            DLIST_ADD_BEFORE(head, dp, *dp);
            num_bufs++;
        }
        else
        {
            FREE(dp);
            if (ptr) free_2D(width, height, fmt, 0, val, ptr);
        }

    } while (ptr && num_bufs < MAX_ALLOCS);

    P(":: Allocated %d buffers", num_bufs);

    int res = 0;
    DLIST_SAFE_LOOP(head, dp, dp2) {
        ERR_ADD(res, free_2D(width, height, fmt, 0, dp->val, dp->bufPtr));
        DLIST_REMOVE(*dp);
    }
    return res;
}

int maxalloc_NV12_test(pixels_t width, pixels_t height)
{
    printf("Allocate & Free max # of %ux%u NV12 buffers\n", width, height);

    struct data {
        uint16_t val;
        void    *bufPtr;
        struct data *next, *last, *me;
    } head, *dp, *dp2;

    /* allocate as many buffers as we can */
    DLIST_INIT(head);
    void *ptr;
    int num_bufs = 0;
    do
    {
        uint16_t val = (uint16_t) rand();
        ptr = alloc_NV12(width, height, val);
        dp = NEW(struct data);
        if (ptr && dp)
        {
            dp->val = val;
            dp->bufPtr = ptr;
            DLIST_ADD_BEFORE(head, dp, *dp);
            num_bufs++;
        }
        else
        {
            FREE(dp);
            if (ptr) free_NV12(width, height, val, ptr);
        }

    } while (ptr && num_bufs < MAX_ALLOCS);

    P(":: Allocated %d buffers", num_bufs);

    int res = 0;
    DLIST_SAFE_LOOP(head, dp, dp2) {
        ERR_ADD(res, free_NV12(width, height, dp->val, dp->bufPtr));
        DLIST_REMOVE(*dp);
    }
    return res;
}

int maxmap_1D_test(bytes_t length)
{
    length = (length + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1);
    printf("Map & UnMap max # of %lxb 1D buffers\n", length);

    struct data {
        uint16_t val;
        void    *bufPtr;
        void    *buffer;
        void    *dataPtr;
        struct data *next, *last, *me;
    } head, *dp, *dp2;

    /* allocate as many buffers as we can */
    DLIST_INIT(head);
    void *ptr;
    int num_bufs = 0;
    do
    {
        /* allocate aligned buffer */
        void *ptr = malloc(length + PAGE_SIZE - 1);
        if (ptr)
        {
            void *buffer = ptr;
            void *dataPtr = (void *)(((uint32_t)buffer + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));
            uint16_t val = (uint16_t) rand();
            ptr = map_1D(dataPtr, length, 0, val);
            dp = NEW(struct data);
            if (ptr && dp)
            {
                dp->val = val;
                dp->bufPtr = ptr;
                dp->buffer = buffer;
                dp->dataPtr = dataPtr;
                DLIST_ADD_BEFORE(head, dp, *dp);
                num_bufs++;
            }
            else
            {
                FREE(dp);
                FREE(buffer);
                if (ptr) unmap_1D(dataPtr, length, 0, val, ptr);
            }
        }
    } while (ptr && num_bufs < MAX_ALLOCS);

    P(":: Mapped %d buffers", num_bufs);

    int res = 0;
    DLIST_SAFE_LOOP(head, dp, dp2) {
        ERR_ADD(res, unmap_1D(dp->dataPtr, length, 0, dp->val, dp->bufPtr));
        FREE(dp->buffer);
        DLIST_REMOVE(*dp);
    }
    return res;
}

int star_test(uint32_t num_ops, uint16_t num_slots)
{
    printf("Random set of %d Allocs/Maps and Frees/UnMaps for %d slots\n", num_ops, num_slots);
    srand(0xFB72316A);
    struct data {
        int      op;
        uint16_t val;
        pixels_t width, height;
        bytes_t  length;
        void    *bufPtr;
        void    *buffer;
        void    *dataPtr;
    } *mem;

    /* allocate memory state */
    mem = NEWN(struct data, num_slots);
    if (!mem) return NOT_P(mem,!=,NULL);

    /* perform alloc/free/unmaps */
    int res = 0, ix;
    while (!res && num_ops--)
    {
        ix = rand() % num_slots;
        /* see if we need to free/unmap data */
        if (mem[ix].bufPtr)
        {
            /* check memory fill */
            switch (mem[ix].op)
            {
            case 0: res = unmap_1D(mem[ix].dataPtr, mem[ix].length, 0, mem[ix].val, mem[ix].bufPtr);
                FREE(mem[ix].buffer);
                break;
            case 1: res = free_1D(mem[ix].length, 0, mem[ix].val, mem[ix].bufPtr); break;
            case 2: res = free_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_8BIT, 0, mem[ix].val, mem[ix].bufPtr); break;
            case 3: res = free_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_16BIT, 0, mem[ix].val, mem[ix].bufPtr); break;
            case 4: res = free_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_32BIT, 0, mem[ix].val, mem[ix].bufPtr); break;
            case 5: res = free_NV12(mem[ix].width, mem[ix].height, mem[ix].val, mem[ix].bufPtr); break;
            }
            ZERO(mem[ix]);
        }
        /* we need to allocate/map data */
        else
        {
            int op = rand();
            /* set width */
            pixels_t width, height;          
            switch (op & 15) {
            case 0:                         width = 1920; height = 1080; break;
            case 1: case 2:                  width = 1280; height = 720; break;
            case 3: case 4:                   width = 640; height = 480; break;
            case 5: case 6: case 7:           width = 848; height = 480; break;
            case 8: case 9: case 10: case 11: width = 176; height = 144; break;
            default:                                width = height = 64; break;
            }
            mem[ix].length = (bytes_t)width * height;
            mem[ix].width = width;
            mem[ix].height = height;
            mem[ix].val = ((uint16_t)rand());

            /* perform operation */
            switch ((op >> 4) & 15)
            {
            case 0: case 1: case 2: case 3: /* map 1D buffer */
                mem[ix].op = 0;
                /* allocate aligned buffer */
                mem[ix].length = (mem[ix].length + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1);
                mem[ix].buffer = malloc(mem[ix].length + PAGE_SIZE - 1);
                if (mem[ix].buffer)
                {
                    mem[ix].dataPtr = (void *)(((uint32_t)mem[ix].buffer + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));
                    mem[ix].bufPtr = map_1D(mem[ix].dataPtr, mem[ix].length, 0, mem[ix].val);
                    if (!mem[ix].bufPtr) FREE(mem[ix].buffer);
                }                
                break;
            case 4: case 5: case 6:
                mem[ix].op = 1;
                mem[ix].bufPtr = alloc_1D(mem[ix].length, 0, mem[ix].val);
                break;
            case 7: case 8:
                mem[ix].op = 2;
                mem[ix].bufPtr = alloc_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_8BIT, 0, mem[ix].val);
                break;
            case 9: case 10:
                mem[ix].op = 3;
                mem[ix].bufPtr = alloc_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_16BIT, 0, mem[ix].val);
                break;
            case 11:
                mem[ix].op = 4;
                mem[ix].bufPtr = alloc_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_32BIT, 0, mem[ix].val);
                break;
            case 12: case 13: case 14: case 15:
                mem[ix].op = 5;
                mem[ix].bufPtr = alloc_NV12(mem[ix].width, mem[ix].height, mem[ix].val);
                break;               
            }
        }
    }

    /* unmap and free everything */
    for (ix = 0; ix < num_slots; ix++)
    {
        if (mem[ix].bufPtr)
        {
            /* check memory fill */
            switch (mem[ix].op)
            {
            case 0: ERR_ADD(res, unmap_1D(mem[ix].dataPtr, mem[ix].length, 0, mem[ix].val, mem[ix].bufPtr));
                FREE(mem[ix].buffer);
                break;
            case 1: ERR_ADD(res, free_1D(mem[ix].length, 0, mem[ix].val, mem[ix].bufPtr)); break;
            case 2: ERR_ADD(res, free_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_8BIT, 0, mem[ix].val, mem[ix].bufPtr)); break;
            case 3: ERR_ADD(res, free_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_16BIT, 0, mem[ix].val, mem[ix].bufPtr)); break;
            case 4: ERR_ADD(res, free_2D(mem[ix].width, mem[ix].height, PIXEL_FMT_32BIT, 0, mem[ix].val, mem[ix].bufPtr)); break;
            case 5: ERR_ADD(res, free_NV12(mem[ix].width, mem[ix].height, mem[ix].val, mem[ix].bufPtr)); break;
            }
        }
    }
    FREE(mem);

    return res;
}

int neg_alloc_tests()
{
    printf("Negative Alloc tests\n");

    MemAllocBlock block[2], *blk;
    memset(&block, 0, sizeof(block));

    int ret = 0, num_blocks;

    for (num_blocks = 1; num_blocks < 3; num_blocks++)
    {
        blk = block + num_blocks - 1;
        
        P("/* bad pixel format */");
        blk->pixelFormat = PIXEL_FMT_MIN - 1;
        blk->length = PAGE_SIZE;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);
        blk->pixelFormat = PIXEL_FMT_MAX + 1;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);
    
        P("/* bad 1D stride */");
        blk->pixelFormat = PIXEL_FMT_PAGE;
        blk->stride = PAGE_SIZE - 1;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);
    
        P("/* 0 1D length */");
        blk->length = blk->stride = 0;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);
    
        P("/* bad 2D stride */");
        blk->pixelFormat = PIXEL_FMT_8BIT;
        blk->width = PAGE_SIZE - 1;
        blk->stride = PAGE_SIZE - 1;
        blk->height = 16;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);
        
        P("/* bad 2D width */");    
        blk->stride = blk->width = 0;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);

        P("/* bad 2D height */");    
        blk->height = 0;
        blk->width = 16;
        ret |= NOT_I(MemMgr_Alloc(block, num_blocks),==,NULL);
        
        /* good 2D block */
        blk->height = 16;
    }

    return ret;
}

int neg_free_tests()
{
    printf("Negative Free tests\n");

    void *ptr = alloc_1D(PAGE_SIZE, 0, 0);
    int ret = 0;

    MemMgr_Free(ptr);

    P("/* free something twice */");
    ret |= NOT_I(MemMgr_Free(ptr),!=,0);

    P("/* free NULL */");
    ret |= NOT_I(MemMgr_Free(NULL),!=,0);

    P("/* free arbitrary value */");
    ret |= NOT_I(MemMgr_Free((void *)0x12345678),!=,0);

    P("/* free mapped buffer */");
    void *buffer = malloc(PAGE_SIZE * 2);
    void *dataPtr = (void *)(((uint32_t)buffer + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));
    ptr = map_1D(dataPtr, PAGE_SIZE, 0, 0);
    ret |= NOT_I(MemMgr_Free(ptr),!=,0);

    MemMgr_UnMap(ptr);
    
    return ret;
}

int neg_map_tests()
{
    printf("Negative Map tests\n");

    MemAllocBlock block[2], *blk;
    memset(&block, 0, sizeof(block));

    int ret = 0, num_blocks;

    for (num_blocks = 1; num_blocks < 3; num_blocks++)
    {
        blk = block + num_blocks - 1;
        
        P("/* bad pixel format */");
        blk->pixelFormat = PIXEL_FMT_MIN - 1;
        blk->length = PAGE_SIZE;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);
        blk->pixelFormat = PIXEL_FMT_MAX + 1;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);
    
        P("/* bad 1D stride */");
        blk->pixelFormat = PIXEL_FMT_PAGE;
        blk->stride = PAGE_SIZE - 1;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);
    
        P("/* 0 1D length */");
        blk->length = blk->stride = 0;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);
    
        P("/* bad 2D stride */");
        blk->pixelFormat = PIXEL_FMT_8BIT;
        blk->width = PAGE_SIZE - 1;
        blk->stride = PAGE_SIZE - 1;
        blk->height = 16;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);
        
        P("/* bad 2D width */");    
        blk->stride = blk->width = 0;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);

        P("/* bad 2D height */");    
        blk->height = 0;
        blk->width = 16;
        ret |= NOT_I(MemMgr_Map(block, num_blocks),==,NULL);
        
        /* good 2D block */
        blk->height = 16;
    }

    P("/* 2 buffers */");
    ret |= NOT_I(MemMgr_Map(block, 2),==,NULL);

    P("/* 1 2D buffer */");
    ret |= NOT_I(MemMgr_Map(block, 1),==,NULL);

    P("/* 1 1D buffer with no address */");
    block[0].pixelFormat = PIXEL_FMT_PAGE;
    block[0].length = 2 * PAGE_SIZE;
    block[0].ptr = NULL;
    ret |= NOT_I(MemMgr_Map(block, 1),==,NULL);

    P("/* 1 1D buffer with not aligned start address */");
    void *buffer = malloc(3 * PAGE_SIZE);
    void *dataPtr = (void *)(((uint32_t)buffer + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));
    block[0].ptr = dataPtr + 3;
    ret |= NOT_I(MemMgr_Map(block, 1),==,NULL);

    P("/* 1 1D buffer with not aligned length */");
    block[0].ptr = dataPtr;
    block[0].length -= 5;
    ret |= NOT_I(MemMgr_Map(block, 1),==,NULL);

    return ret;
}

int neg_unmap_tests()
{
    printf("Negative Unmap tests\n");

    void *ptr = alloc_1D(PAGE_SIZE, 0, 0);
    int ret = 0;

    P("/* unmap alloced buffer */");
    ret |= NOT_I(MemMgr_UnMap(ptr),!=,0);

    MemMgr_Free(ptr);

    void *buffer = malloc(PAGE_SIZE * 2);
    void *dataPtr = (void *)(((uint32_t)buffer + PAGE_SIZE - 1) &~ (PAGE_SIZE - 1));
    ptr = map_1D(dataPtr, PAGE_SIZE, 0, 0);
    MemMgr_UnMap(ptr);

    P("/* unmap something twice */");
    ret |= NOT_I(MemMgr_UnMap(ptr),!=,0);

    P("/* unmap NULL */");
    ret |= NOT_I(MemMgr_UnMap(NULL),!=,0);

    P("/* unmap arbitrary value */");
    ret |= NOT_I(MemMgr_UnMap((void *)0x12345678),!=,0);

    return ret;
}

int neg_check_tests()
{
    printf("Negative Is... tests\n");
    void *ptr = malloc(32);

    int ret = 0;

    ret |= NOT_I(MemMgr_Is1DBlock(NULL),==,FALSE);
    ret |= NOT_I(MemMgr_Is1DBlock((void *)0x12345678),==,FALSE);
    ret |= NOT_I(MemMgr_Is1DBlock(ptr),==,FALSE);
    ret |= NOT_I(MemMgr_Is2DBlock(NULL),==,FALSE);
    ret |= NOT_I(MemMgr_Is2DBlock((void *)0x12345678),==,FALSE);
    ret |= NOT_I(MemMgr_Is2DBlock(ptr),==,FALSE);
    ret |= NOT_I(MemMgr_IsMapped(NULL),==,FALSE);
    ret |= NOT_I(MemMgr_IsMapped((void *)0x12345678),==,FALSE);
    ret |= NOT_I(MemMgr_IsMapped(ptr),==,FALSE);

    ret |= NOT_I(MemMgr_GetStride(NULL),==,0);
    ret |= NOT_I(MemMgr_GetStride((void *)0x12345678),==,0);
    ret |= NOT_I(MemMgr_GetStride(ptr),==,PAGE_SIZE);

    ret |= NOT_P(TilerMem_VirtToPhys(NULL),==,0);
    ret |= NOT_P(TilerMem_VirtToPhys((void *)0x12345678),==,0);
    ret |= NOT_P(TilerMem_VirtToPhys(ptr),!=,0);

    ret |= NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(NULL)),==,0);
    ret |= NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys((void *)0x12345678)),==,0);
    ret |= NOT_I(TilerMem_GetStride(TilerMem_VirtToPhys(ptr)),==,0);

    FREE(ptr);

    return ret;
}

int result(int res)
{
    if (res)
    {
        printf("==> FAIL(%d)\n", res);
        return 1;
    }
    else
    {
        printf("==> OK\n");
        return 0;
    }
}

int test(int id)
{
    int i = id;
    #define T(test) if (!--i) { printf("TEST %d - ", id); return result(test); }
    TESTS
    return -1;
}

int main(int argc, char **argv)
{
    int start = 1, end = -1, res, failed = 0, succeeded = 0;
    char c;

    /* all tests */
    if (argc == 1)
    {
    }
    /* test list */
    else if (argc == 2 && !strcmp(argv[1], "list"))
    {
    #define T(test) printf("% 3d - %s\n", ++i, #test);
        int i = 0;
        TESTS
        return 0;
    }
    /* single test */
    else if (argc == 2 && sscanf(argv[1], "%u%c", &res, &c) == 1)
    {
        start = end = atoi(argv[1]);
    }
    /* open range .. b */
    else if (argc == 3 && !strcmp(argv[1], "..") && sscanf(argv[2], "%u%c", &res, &c) == 1)
    {
        end = atoi(argv[2]);
    }
    /* open range a .. */
    else if (argc == 3 && !strcmp(argv[2], "..") && sscanf(argv[1], "%u%c", &res, &c) == 1)
    {
        start = atoi(argv[1]);
    }
    else if (argc == 4 && !strcmp(argv[2], "..") && sscanf(argv[1], "%u%c", &res, &c) == 1 && sscanf(argv[3], "%u%c", &res, &c) == 1)
    {
        start = atoi(argv[1]);
        end = atoi(argv[3]);
    }
    else
    {
        fprintf(stderr, "Usage: %s [<range>], where <range> is\n"
                "   empty:   run all tests\n"
                "   ix:      run test #ix\n"
                "   a ..:    run tests #a, #a+1, ...\n"
                "   .. b:    run tests #1, #2, .. #b\n"
                "   a .. b:  run tests #a, #a+1, .. #b\n", argv[0]);
        return -1;
    }

    do
    {
        res = test(start++);
        if (res == 1) failed++; else if (!res) succeeded++;
    } while (res != -1 && (end < 0 || start <= end));

    printf("FAILED: %d, SUCCEEDED: %d\n", failed, succeeded);

    /* also execute internal unit tests - this also verifies that we did not
       keep any references */
    memmgr_internal_unit_test();

    return failed;
}

