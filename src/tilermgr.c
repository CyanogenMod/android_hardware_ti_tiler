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
#include "utils.h"

extern int errno;

#define dump(x) fprintf(stdout, "%s::%s():%d: %lx\n", \
		__FILE__, __func__, __LINE__, (unsigned long)x); fflush(stdout); 
#define TILERMGR_ERROR(x) \
	fprintf(stderr, "%s::%s():%d:%s\n", __FILE__, __func__, __LINE__, x);  \
	fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));       \
	fflush(stderr);

static int fd;

int TilerMgr_Close()
{
    int ret = -1;
    struct tiler_block_info block = {0};
    int fd = 0;

    ret = ioctl(fd, TILIOC_CLOSE, (unsigned long)(&block));
    if (ret == -1)
        TILERMGR_ERROR("ioctl():fail");

    close(fd);

    if (ret == -1)
        return TILERMGR_ERR_GENERIC;
    else
        return TILERMGR_ERR_NONE;
}

int TilerMgr_Open()
{
    int ret = -1;
    struct tiler_block_info block = {0};

    fd = open(TILER_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        TILERMGR_ERROR("open():fail");
        return TILERMGR_ERR_GENERIC;
    }

    ret = ioctl(fd, TILIOC_OPEN, (unsigned long)(&block));
    if (ret == -1) {
        TILERMGR_ERROR("ioctl():fail");
        close(fd);
        return TILERMGR_ERR_GENERIC;
    }

    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_Alloc(enum pixel_fmt_t pf, pixels_t w, pixels_t h)
{
    int ret = -1;
    struct tiler_block_info block = {0};
    
    if (pf < PIXEL_FMT_8BIT || pf > PIXEL_FMT_32BIT)
        return 0x0;
    if (w <= 0 || w > TILER_WIDTH * 64)
        return 0x0;
    if (h <= 0 || h > TILER_HEIGHT * 64)
        return 0x0;

    block.fmt = pf;
    block.dim.area.width = w;
    block.dim.area.height = h;

    ret = ioctl(fd, TILIOC_GBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR("ioctl():fail");
        return 0x0;
    }
    return block.ssptr;
}

int TilerMgr_Free(SSPtr s)
{
    int ret = TILERMGR_ERR_GENERIC;
    struct tiler_block_info block = {0};

    if (s < TILER_MEM_8BIT || s >= TILER_MEM_PAGED)
        return TILERMGR_ERR_GENERIC;
    
    block.ssptr = s;

    ret = ioctl(fd, TILIOC_FBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR("ioctl():fail");
        return TILERMGR_ERR_GENERIC;
    }
    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_PageModeAlloc(bytes_t len)
{
    int ret = -1;
    struct tiler_block_info block = {0};

    if(len < 0 || len > TILER_LENGTH)
        return 0x0;

    block.fmt = TILFMT_PAGE;
    block.dim.len = len;

    ret = ioctl(fd, TILIOC_GBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR("ioctl():fail");
        return TILERMGR_ERR_GENERIC;
    }
    return block.ssptr;
}

int TilerMgr_PageModeFree(SSPtr s)
{
    int ret = -1;
    struct tiler_block_info block = {0};

    if (s < TILER_MEM_PAGED || s >= TILER_MEM_END)
        return TILERMGR_ERR_GENERIC;
    
    block.ssptr = s;

    ret = ioctl(fd, TILIOC_FBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR("ioctl():fail");
        return TILERMGR_ERR_GENERIC;
    }
    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_VirtToPhys(void *ptr)
{
    int ret = -1;
    struct tiler_block_info block = {0};

    if(ptr == NULL) 
        return 0x0;

    block.ptr = ptr;

    ret = ioctl(fd, TILIOC_GSSP, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR("ioctl():fail");
        return 0x0;
    }
    return block.ssptr;
}

