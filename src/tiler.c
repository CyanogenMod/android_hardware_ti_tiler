/*
 * tilermgr.c
 *
 * DMM library support functions for TI OMAP processors.
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
#include <string.h>  /* strerror() */
#include <fcntl.h>   /* open() */
#include <stropts.h> /* ioctl() */
#include <unistd.h>  /* close() */
#include <sys/mman.h> /* mmap() */
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "tilermgr.h"
/* #include "dmm.h" */

#define DMM_DRIVER_NAME "/dev/tiler"
#define PAGE        0x1000
#define CONT_W 256
#define CONT_H 128
#define CONT_LEN CONT_W*CONT_H*PAGE
#define IOCSINIT  _IOWR ('z', 100, unsigned long)
#define IOCGALLOC _IOWR ('z', 101, unsigned long)
#define IOCSFREE  _IOWR ('z', 102, unsigned long)
#define IOCGTSPTR _IOWR ('z', 103, unsigned long)
#define IOCSDEINIT _IOWR ('z', 104, unsigned long)
#define IOCGSSPTR _IOWR ('z', 105, unsigned long)

struct dmm_data {
    int pixfmt;
    unsigned short w;
    unsigned short h;
    unsigned long len;
    short seczone;
    unsigned long ssptr;
    unsigned long tsptr;
    unsigned long vsptr;
    short syx;
    unsigned long ctxptr;
};

extern int errno;
static struct dmm_data *d;
static int fd;

int
tilerDeinit()
{
    int result = -1;

    result = ioctl(fd, IOCSDEINIT, d);
    if (result == -1) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
    fflush(stderr);
    return result;
    }

    free(d);
    close(fd);
    return 0;
}

int
tilerInit()
{
    int result = -1;

    fd = open(DMM_DRIVER_NAME, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
    fflush(stderr);
    return -1;
    }

    d = (struct dmm_data*)malloc(sizeof(struct dmm_data));
    memset(d,0x0,sizeof(struct dmm_data));

    result = ioctl(fd, IOCSINIT, d);
    if (result == -1) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
    fflush(stderr);
    return result;
    }
    return 0;
}

SSPtr
tilerAlloc(enum kPixelFormat pf, pixels_t w, pixels_t h, short seczone)
{
    SSPtr ptr = NULL;
    int result = -1;
    assert(pf >= 1 && pf <= 3);
    assert(w > 0 && w <= CONT_W*64);
    assert(h > 0 && h <= CONT_H*64);

    d->pixfmt  = pf;
    d->w       = w;
    d->h       = h;
    d->seczone = seczone;

    result = ioctl(fd, IOCGALLOC, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        return ptr;
    }
    return (SSPtr)d->ssptr;
}

int
tilerFree(SSPtr p)
{
    int result = -1;
    assert(p != NULL);

    d->ssptr = (unsigned long)p;

    result = ioctl(fd, IOCSFREE, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        return result;
    }
    return 0;
}

SSPageModePtr
tilerPageModeAlloc(bytes_t len)
{
    SSPtr ptr = NULL;
    int result = -1;

    assert(len > 0 && len <= CONT_LEN);

    d->pixfmt  = 0;   /* MODE_PAGE */
    d->len     = len;
    d->seczone = 0;

    result = ioctl(fd, IOCGALLOC, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
    fflush(stderr);
        return ptr;
    }

    return (SSPtr)d->ssptr;
}

int
tilerPageModeFree(SSPageModePtr p)
{
    int result = -1;

    assert(p != NULL);

    d->ssptr = (unsigned long)p;

    result = ioctl(fd, IOCSFREE, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        return result;
    }
    return 0;
}

TSPtr
convertToTilerSpace(SSPtr p, short rotationAndMirroring)
{
    int result = -1;

    assert(p != NULL);

    d->ssptr = (unsigned long)p;
    d->syx = rotationAndMirroring;

    result = ioctl(fd, IOCGTSPTR, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
    fflush(stderr);
        return NULL;
    }

    return (TSPtr)d->tsptr;
}

TSPageModePtr
convertPageModeToTilerSpace(SSPageModePtr p)
{
    int result = -1;

    assert(p != NULL);

    d->ssptr = (unsigned long)p;
    d->syx = 0x0;

    result = ioctl(fd, IOCGTSPTR, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        return NULL;
    }

    return (TSPtr)d->tsptr;
}

enum kRefCorner
tilerGetRefCorner(TSPtr p)
{
    return kTopLeft;
}

extern unsigned long *
tilervirt2phys(unsigned long *vsptr)
{
    int result = -1;

    assert(vsptr != NULL);

    d->vsptr = (unsigned long)vsptr;

    result = ioctl(fd, IOCGSSPTR, d);
    if (result < 0) {
        fprintf(stderr, "ioctl():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
    }

    return (unsigned long *)d->ssptr;
}

/* TODO: stubbing TILER user space APIs for now... */

SSPtr
tilerRealloc(SSPtr p, pixels_t w, pixels_t h)
{
    assert(p != NULL);
    assert(w > 0 && w <= CONT_W);
    assert(h > 0 && h <= CONT_H);

    /* p = tilerAlloc(k8bit, w, h, 0); */
    p = (SSPtr)0x0000da7a;
    return p;
}

SSPageModePtr
tilerPageModeRealloc(SSPageModePtr p, bytes_t len)
{
    assert(p != NULL);
    assert(len > 0 && len <= CONT_LEN);
    /* p = tilerAlloc(0, len, 1, 0); */
    p = (SSPtr)0x0000da7a;
    return p;
}

