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
#include <errno.h>
#include <sys/ioctl.h>
#include <tiler.h>
#include "tilermgr.h"
#include "mem_types.h"

extern int errno;

#define TILERMGR_ERROR() \
	fprintf(stderr, "%s()::%d: errno(%d) - \"%s\"\n", \
			__FUNCTION__, __LINE__, errno, strerror(errno)); \
	fflush(stderr);

static int fd;

int TilerMgr_Close()
{
    close(fd);
    return TILERMGR_ERR_NONE;
}

int TilerMgr_Open()
{
    fd = open(TILER_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        TILERMGR_ERROR();
        return TILERMGR_ERR_GENERIC;
    }

    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_Alloc(enum pixel_fmt_t pixfmt, pixels_t width, pixels_t height)
{
    int ret = -1;
    struct tiler_block_info block = {0};
    
    if (pixfmt < PIXEL_FMT_8BIT || pixfmt > PIXEL_FMT_32BIT)
        return 0x0;
    if (width <= 0 || width > TILER_WIDTH * 64)
        return 0x0;
    if (height <= 0 || height > TILER_HEIGHT * 64)
        return 0x0;

    block.fmt = pixfmt;
    block.dim.area.width = width;
    block.dim.area.height = height;

    ret = ioctl(fd, TILIOC_GBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR();
        return 0x0;
    }
    return block.ssptr;
}

int TilerMgr_Free(SSPtr addr)
{
    int ret = -1;
    struct tiler_block_info block = {0};

    if (addr < TILER_MEM_8BIT || addr >= TILER_MEM_PAGED)
        return TILERMGR_ERR_GENERIC;

    block.ssptr = addr;

    ret = ioctl(fd, TILIOC_FBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR();
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
        TILERMGR_ERROR();
        return 0x0;
    }
    return block.ssptr;
}

int TilerMgr_PageModeFree(SSPtr addr)
{
    int ret = -1;
    struct tiler_block_info block = {0};

    if (addr < TILER_MEM_PAGED || addr >= TILER_MEM_END)
        return TILERMGR_ERR_GENERIC;

    block.ssptr = addr;

    ret = ioctl(fd, TILIOC_FBUF, (unsigned long)(&block));
    if (ret < 0) {
        TILERMGR_ERROR();
        return TILERMGR_ERR_GENERIC;
    }
    return TILERMGR_ERR_NONE;
}

SSPtr TilerMgr_VirtToPhys(void *ptr)
{
    int ret = -1;
    unsigned long tmp = 0x0;

    if(ptr == NULL) 
        return 0x0;

    tmp = (unsigned long)ptr;
    ret = ioctl(fd, TILIOC_GSSP, tmp);

    return (SSPtr)ret;
}

