/*
 * tilermgr.c
 *
 * TILER library support functions for TI OMAP processors.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>   /* strerror() */
#include <fcntl.h>    /* open() */
#include <stropts.h>  /* ioctl() */
#include <unistd.h>   /* close() */
#include <sys/mman.h> /* mmap() */
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <tiler.h>
#include "tilermgr.h"
#include "mem_types.h"

#define dump(x) fprintf(stdout, "%s::%s():%d: %lx\n", __FILE__, __func__, __LINE__, (unsigned long)x); fflush(stdout); 

extern int errno;
static struct tiler_block_info *block;
static int fd;

int TilerMgr_Close()
{
    dump(0);
    int ret = -1;

    ret = ioctl(fd, TILIOC_CLOSE, (unsigned long)block);
    if (ret == -1)
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);

    close(fd);

    if (block != NULL)
        free(block);

    return TILERMGR_ERR_NONE;
}

int TilerMgr_Open()
{
    int ret = -1;
    void *ptr = NULL;

    dump(0);
    fd = open(TILER_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        return TILERMGR_ERR_GENERIC;
    }

    dump(0);
    block = (struct tiler_block_info *)malloc(sizeof(struct tiler_block_info));
    ptr = memset(block, 0x0, sizeof(struct tiler_block_info));
    if ((struct tiler_block_info *)ptr != block) {
        fprintf(stderr, "memset():fail\n"); fflush(stderr);
        free(block); block = NULL; close(fd);
        return TILERMGR_ERR_GENERIC;
    }

    dump(0);
    ret = ioctl(fd, TILIOC_OPEN, (unsigned long)block);
    if (ret == -1) {
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        free(block); block = NULL; close(fd);
        return TILERMGR_ERR_GENERIC;
    }
    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_Alloc(enum pixel_fmt_t pf, pixels_t w, pixels_t h)
{
    int ret = -1;
    assert(pf >= PIXEL_FMT_8BIT && pf <= PIXEL_FMT_32BIT);
    assert(w > 0 && w <= TILER_WIDTH * 64);
    assert(h > 0 && h <= TILER_HEIGHT * 64);

    block->fmt = pf;
    block->dim.d2.width = w;
    block->dim.d2.height = h;

    dump(0);
    ret = ioctl(fd, TILIOC_GBUF, block);
    if (ret < 0) {
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        return 0x0;
    }
    return block->ssptr;
}

int TilerMgr_Free(SSPtr s)
{
    int ret = -1;

    assert(s >= TILER_MEM_8BIT && s < TILER_MEM_32BIT);
    block->ssptr = s;

    ret = ioctl(fd, TILIOC_FBUF, (unsigned long)block);
    if (ret < 0) {
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        return TILERMGR_ERR_GENERIC;
    }
    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_PageModeAlloc(bytes_t len)
{
    int ret = -1;

    assert(len > 0 && len <= TILER_LENGTH);

    block->fmt = TILFMT_PAGE;
    block->dim.d1.length = len;

    ret = ioctl(fd, TILIOC_GBUF, (unsigned long)block);
    if (ret < 0) {
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        return 0x0;
    }
    return block->ssptr;
}

int TilerMgr_PageModeFree(SSPtr s)
{
    int ret = -1;

    block->ssptr = s;

    ret = ioctl(fd, TILIOC_FBUF, block);
    if (ret < 0) {
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        return TILERMGR_ERR_GENERIC;
    }
    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_VirtToPhys(void *ptr)
{
    int ret = -1;

    assert(ptr != NULL);

    block->ptr = ptr;

    ret = ioctl(fd, TILIOC_GSSP, block);
    if (ret < 0) {
        fprintf(stderr, "ioctl():fail\n"); fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno)); fflush(stderr);
        return 0x0;
    }
    return block->ssptr;
}

