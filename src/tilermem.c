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

/*
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
*/
#include <stdio.h>
/*
#include <string.h>
#include <stdlib.h>
*/
#include <stdint.h>

#include <tiler.h>

#define __DEBUG__
#undef  __DEBUG_ENTRY__
#define __DEBUG_ASSERT__

#include "utils.h"
#include "tilermem.h"
#include "tilermem_utils.h"
#include "tilermgr.h"

enum tiler_fmt tiler_get_fmt(SSPtr ssptr);

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
    return R_P(TilerMgr_VirtToPhys(ptr));
#else
    return (SSPtr)ptr;
#endif
}

