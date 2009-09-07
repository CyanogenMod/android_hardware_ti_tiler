/*
 * dmmtest.c
 *
 * DMM test app support functions for TI OMAP processors.
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
#include <fcntl.h> /* open () */
#include <unistd.h> /* close() */
#include <sys/mman.h> /* mmap() */
#include <errno.h>
#include <string.h> /* strerror() */
#include <assert.h>
#include "memmgr.h"
#include "tilermgr.h"

/* #define BUFFERLEN 0x1fe000 */ /* 510 pages */
#define BUFFERLEN 0x3000 /* 13 pages */
#define MAPLEN 0x1000
#define PAGESIZE 0x1000
#define CONTAINER_W 256
#define CONTAINER_H 128
#define CONTAINER_LEN CONTAINER_W*CONTAINER_H*PAGE
#define MAX_NUM_BUF 4
#define TILERTEST_MEM_8BIT  0x60000000
#define TILERTEST_MEM_16BIT 0x68000000
#define TILERTEST_MEM_32BIT 0x70000000
#define TILERTEST_MEM_PAGED 0x78000000
#define TILERTEST_MEM_END   0x80000000

#define TILERTEST_COUNT 10

#define dump(x) fprintf(stdout, "%s::%s():%d: %lx\n", __FILE__, __func__, __LINE__, (unsigned long)x); fflush(stdout);

extern int errno;
static struct node *lsthd;

struct node {
    void *ptr;
    unsigned long reserved;
    struct node *nextnode;
};

static int MemAlloc_Alloc(struct MemAllocBlock *b);
static int MemAlloc_Free(void *p);

static void read_buffer_data(void *p, unsigned long len);
static void write_buffer_data(void *p, unsigned long len, unsigned long data);

static int removenode(struct node *listhead, void *p);
static int destroylist(struct node *listhead);
static int addnode(struct node *listhead, struct node *n);
static int getnode(struct node *listhead, struct node **npp, void *p);
static int createlist(struct node **listhead);

int main(int argc, const char *argv[])
{
	int error = TILERMGR_ERR_GENERIC;
	MemAllocBlock b1 = {0};
	int fd;
	SSPtr physptr = 0x0;
	SSPtr ssptr[TILERTEST_COUNT] = {0};
	int i = 0;

	assert(TilerMgr_Open() == 0);
	assert(createlist(&lsthd) == 0);
	dump(0);

#if 0
	b1.pixelFormat = PIXEL_FMT_8BIT;
	b1.dim.d2.width = 176;
	b1.dim.d2.height = 144;

	for (i = 0; i < TILERTEST_COUNT; i++) 
	{
		ssptr[i] = TILERMGR_Alloc(b1.pixelFormat, b1.dim.d2.width, b1.dim.d2.height);
		if (ssptr[i] == 0x0)
			fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		dump(ssptr[i]);
	}
#endif

#if 0
	dump(0);
	fd = open("/dev/tiler", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		TILERMGR_Free(b1.reserved);
		return error;
	}

	dump(b1.dim.d2.height*PAGESIZE);
	b1.ptr = (void *)mmap(0, b1.dim.d2.height*PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, b1.reserved);
	if ((unsigned long *)b1.ptr == MAP_FAILED) {
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		close(fd);
		TILERMGR_Free(b1.reserved);
		return error;
	}
	memset(b1.ptr, 0x0, b1.dim.d2.height*PAGESIZE);
#endif

#if 0
	dump(0);
	physptr = TILERMGR_VirtToPhys(b1.ptr);

	dump(physptr);
	write_buffer_data(b1.ptr, 176, 0xaaaaaaaa);
	dump(0);
	read_buffer_data(b1.ptr, 176);
	dump(0);

	physptr = TILERMGR_VirtToPhys(b1.ptr+0x400);
	fprintf(stdout, "%s::%s():%d: physptr -> 0x%lx\n", __FILE__, __func__, __LINE__, physptr); fflush(stdout);

	dump(0);
	write_buffer_data(b1.ptr+0x400, 176, 0xbbbbbbbb);
	dump(0);
	read_buffer_data(b1.ptr+0x400, 176);
	dump(0);
	read_buffer_data(b1.ptr, b1.dim.d2.height*PAGESIZE);
#endif

	dump(0);
	for (i = 0; i < TILERTEST_COUNT; i++) {
		error = TilerMgr_Free(ssptr[i]);
		if (error != TILERMGR_ERR_NONE)
			fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		dump(ssptr[i]);
	}

	dump(0);
	error = TilerMgr_Close();
	if (error != TILERMGR_ERR_NONE)
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);

	error = destroylist(lsthd);
	if (error != -1)
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr); return -1;

	return 0;
}

static void write_buffer_data(void *ptr, unsigned long len, unsigned long data) 
{
	unsigned long *ptmp = NULL;
	int i = -1;
    
	fprintf(stdout, "%s::%s():%d\n", __FILE__, __func__, __LINE__); fflush(stdout);
	ptmp = (unsigned long *)ptr;
    
	if (ptmp != NULL) {
		for (i = 0; i < len; i+=4) {
			*ptmp = data;
			ptmp++;
		}
	}
	else {
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
	}
}

static void read_buffer_data(void *ptr, unsigned long len)
{
	unsigned long *ptmp = NULL;
	int i = -1;

	fprintf(stdout, "%s::%s():%d: ptr -> %p, len = %ld\n", __FILE__, __func__, __LINE__, ptr, len); fflush(stdout);
	ptmp = (unsigned long *)ptr;
    
	if (ptmp != NULL) {
		for (i = 0; i < len; i+=4) {
			fprintf(stdout, "r[%p] -> 0x%lx\n", ptmp, *ptmp); fflush(stdout);
			ptmp++;
		}
	}
	else {
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
	}
}
#if 0
int
main(int argc, const char *argv[])
{
    struct MemAllocBlock p1 = {0};
    struct MemAllocBlock p2 = {0};
    int error = -1;
    SSPtr ssptr = 0xffffffff;

    assert(TILERMGR_Open() == 0);
    assert(createlist(&lsthd) == 0);

#if 0
    p1.pixelFormat = 0;
    p1.width = 0;
    p1.height = 0;
    p1.length = BUFFERLEN;
    p1.securityZone = 0;
    p1.buf_type = BUFFER_1D;
    p1.stride = 0x1000;
    p1.reserve = NULL;
    p1.vsptr = 0xffffffff;
    p1.ssptr = NULL;
    p1.fd = -1;

    error = MemAlloc_Alloc(&p1);
    /* read_buffer_data((unsigned long *)p1.vsptr, BUFFERLEN); */
    /* write_buffer_data((unsigned long *)p1.vsptr, MAPLEN, 0xeeeeeeee); */
    /* read_buffer_data((unsigned long *)p1.vsptr, MAPLEN); */
#endif

#if 0
    p2.pixelFormat = 0;
    p2.width = 0;
    p2.height = 0;
    p2.length = BUFFERLEN;
    p2.securityZone = 0;
    p2.buf_type = BUFFER_1D;
    p2.stride = 0x1000;
    p2.reserve = NULL;
    p2.vsptr = 0xffffffff;
    p2.ssptr = NULL;
    p2.fd = -1;

    error = MemAlloc_Alloc(&p2);
    /* read_buffer_data((unsigned long *)p2.vsptr, 256); */
    write_buffer_data((unsigned long *)p2.vsptr, MAPLEN, 0xaaaaaaaa);
    read_buffer_data((unsigned long *)p2.vsptr, MAPLEN);
#else
    p2.pixelFormat = PIXEL_FMT_8BIT;
    p2.dim.d2.width= 176;
    p2.dim.d2.height = 144;
    p2.stride = 0x0;
    p2.ptr = NULL;
    p2.reserved = NULL;

    error = MemAlloc_Alloc(&p2);

    ssptr = TILERMGR_VirtToPhys(p2.ptr);
    fprintf(stdout, "%s::%s():%d: ssptr -> 0x%lx\n", __FILE__, __FUNCTION__, __LINE__, ssptr);
    fflush(stdout);

    /* read_buffer_data((unsigned long *)p2.vsptr, MAPLEN); */ /* p2.height*PAGE); */
    write_buffer_data((unsigned long *)p2.ptr, 176, 0xaaaaaaaa);

    fprintf(stdout, "%s::%s():%d: write_buffer_data(%p, 176, 0xaaaaaaaa)\n",
            __FILE__, __FUNCTION__, __LINE__, (unsigned long *)p2.ptr);
    fflush(stdout);
    read_buffer_data((unsigned long *)p2.ptr, 176);

    ssptr = TILERMGR_VirtToPhys(p2.ptr+0x400);
    fprintf(stdout, "%s::%s():%d: ssptr -> 0x%lx\n", __FILE__, __FUNCTION__, __LINE__, ssptr);
    fflush(stdout);

    write_buffer_data((unsigned long *)p2.ptr+0x400, 176, 0xbbbbbbbb);
    fprintf(stdout, "%s::%s():%d: write_buffer_data(%p, 176, 0xbbbbbbbb)\n",
            __FILE__, __FUNCTION__, __LINE__, (unsigned long *)p2.ptr+0x400);
    fflush(stdout);
    read_buffer_data((unsigned long *)p2.ptr+0x400, 176);
#endif
    /* read the 1st buffer again */
    /* read_buffer_data((unsigned long *)p1.vsptr, 176); */

    /* read the 2nd buffer again */    
    /* read_buffer_data((unsigned long *)p2.vsptr+0x1000, 176); */

    fprintf(stdout, "%s::%s():%d: read_buffer_data(%p, 0x2000)\n",
            __FILE__, __FUNCTION__, __LINE__, (unsigned long *)p2.ptr);
    fflush(stdout);
    read_buffer_data((unsigned long *)p2.ptr, 0x2000);

    error = MemAlloc_Free(p2.ptr);
    if (error != 0) {
        fprintf(stderr, "%s::%s():%d:MemAlloc_Free() fail\n",
            __FILE__, __FUNCTION__, __LINE__);
        fflush(stderr);
    }

#if 0
    error = MemAlloc_Free((unsigned long *)p1.vsptr);
    if (error != 0) {
        fprintf(stderr, "%s::%s():%d:MemAlloc_Free() fail\n",
            __FILE__, __FUNCTION__, __LINE__);
        fflush(stderr);
    }
#endif

    error = TILERMGR_Close();
    if (error != 0) {
        fprintf(stderr, "%s::%s():%d:TILERMGR_Close() fail\n",
            __FILE__, __FUNCTION__, __LINE__);
        fflush(stderr);
    }

    error = destroylist(lsthd);
    if (error != 0) {
        fprintf(stderr, "%s::%s():%d:destroylist() fail\n",
            __FILE__, __FUNCTION__, __LINE__);
        fflush(stderr);
    }
    return error;
}



int 
MemAlloc_Free(void *p) 
{
    int error = -1;
    unsigned long *physptr = NULL;
    struct node *n = NULL;

    error = getnode(lsthd, &n, p);
    if (error != 0) {
        fprintf(stderr, "getnode():fail\n");
        fflush(stderr);
        return error;
    }

    fprintf(stdout, "n->ptr --> %p\n", n->ptr);
    fprintf(stdout, "n->reserved = 0x%lx\n", n->reserved);
    fprintf(stdout, "ptr = %p\n", p);
    fflush(stdout);
    
    /* TODO: see if there is a user space virt_to_phys API */
    physptr = (unsigned long *)TilerMem_VirtToPhys((unsigned long *)p);
    fprintf(stdout, "physptr --> %p\n", physptr);
    
    if (physptr == NULL) {
        fprintf(stderr, "TilerMem_VirtToPhys():returned NULL\n");
        fflush(stderr); 
    }   
    else if (physptr >= START_1D && physptr < END_1D) {
        error = TILERMGR_PageModeFree((void *)n->reserved); 
        if (error != 0) {
            fprintf(stderr, "TILERMGR_PageModeFree():fail\n");
            fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
            fflush(stderr);
        }
    }
    else if (physptr >= START_2D && physptr < END_2D) {         
        error = TILERMGR_Free((void *)n->reserved); 
        if (error != 0) {
            fprintf(stderr, "TILERMGR_Free():fail\n");
            fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
            fflush(stderr);
        }
    }
    else {
        fprintf(stderr, "%s::%s():%d:physptr out of range\n",
                __FILE__, __FUNCTION__, __LINE__);
    }
    fprintf(stdout, "physptr = %p\n", physptr);
    fflush(stdout);

    error = removenode(lsthd, p);
    if (error != 0) {
        fprintf(stderr, "removenode():fail\n");
        fflush(stderr);
    }
    
    error = munmap(p, MAPLEN); 
    if (error != 0) {
        fprintf(stderr, "munmap():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        /* we should try to return the first error */
        return error;
    }
    return 0;
}

int 
MemAlloc_Alloc(struct MemAllocBlock *b)
{
    int error = -1;
    struct node n = {0};
    unsigned long mapsize = -1;
    int fd = -1;

    if ((*b).pixelFormat == kPage) {
        assert((*b).length > 0 && (*b).length <= CONTAINER_LEN);
        (*b).reserved = (unsigned long)TILERMGR_PageModeAlloc((*b).length);

        fprintf(stdout, "%s::%s():%d: (*b).reserved = 0x%lx\n",
                __FILE__, __FUNCTION__, __LINE__, (*b).reserved);
        fprintf(stdout, "%s::%s():%d: (*b).length = %ld\n",
                __FILE__, __FUNCTION__, __LINE__, (*b).length);
        fflush(stdout);
    }
    else if ((*b).pixelFormat >= k8bit && (*b).pixelFormat <= k32bit) {
        assert((*b).width > 0 && (*b).width <= CONTAINER_W*64);
        assert((*b).height > 0 && (*b).height <= CONTAINER_H*64);
        (*b).reserved = (unsigned long)TILERMGR_Alloc((*b).pixelFormat,
                                                      (*b).width,
                                                      (*b).height);
	
	fprintf(stdout, "%s::%s():%d: (*b).reserved = 0x%lx\n",
                __FILE__, __FUNCTION__, __LINE__, (*b).reserved);
        fprintf(stdout, "%s::%s():%d: (*b).width = %d\n",
                __FILE__, __FUNCTION__, __LINE__, (*b).width);
        fprintf(stdout, "%s::%s():%d: (*b).height = %d\n",
                __FILE__, __FUNCTION__, __LINE__, (*b).height);
        fflush(stdout);
    }
    else {
        fprintf(stderr, "unsupported buffer type.\n");
        fflush(stderr);
        return error;
    }

    if ((*b).reserved == 0x0)
            return error;

    fd = open("/dev/tiler", O_RDWR | O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "open():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        if ((*b).pixelFormat == kPage)
            TILERMGR_PageModeFree((void *)(*b).reserved);
        else
            TILERMGR_Free((void *)(*b).reserved);
        return error;
    }

    mapsize = (*b).height*PAGE;

    (*b).ptr = (void *)mmap(0, mapsize,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED,
                            fd,
                            (*b).reserved));

    fprintf(stdout, "%s::%s():%d: (*b).ptr -> %p\n",
            __FILE__, __FUNCTION__, __LINE__, (*b).ptr);
    fflush(stdout);

    if ((unsigned long *)((*b).ptr) == MAP_FAILED) {
        fprintf(stderr, "mmap():fail\n");
        fprintf(stderr, "errno(%d) - \"%s\"\n", errno, strerror(errno));
        fflush(stderr);
        close(fd);
        if ((*b).pixelFormat == kPage)
            TILERMGR_PageModeFree((void *)(*b).reserved);
        else
            TILERMGR_Free((void *)(*b).reserved);
        return error;
    }

    n.ptr = (*b).ptr;
    n.reserved = (*b).reserved;
    n.nextnode = NULL;
    error = addnode(lsthd, &n);

    if (error != 0) {
        fprintf(stderr, "addnode():fail\n");
        fflush(stderr);
        munmap((*b).ptr, MAPLEN);
        close(fd);
        if ((*b).pixelFormat == kPage)
            TILERMGR_PageModeFree((void *)(*b).reserved);
        else
            TILERMGR_Free((void *)(*b).reserved);
        return error;
    }
    return 0;
}
#endif

static int createlist(struct node **listhead)
{
    int error = -1;
    void *ret = NULL;

    *listhead = (struct node *)malloc(sizeof(struct node));
    if (*listhead == NULL)
    {
        fprintf(stderr, "createlist error\n");
        fflush(stderr);
        return error;
    }
    ret = memset(*listhead, 0x0, sizeof(struct node));
    if (ret != *listhead) {
        fprintf(stderr, "memset():fail\n");
        fflush(stderr);
        return error;
    }
    else {    
        fprintf(stdout, "%s::%s():%d: createlist success!\n", 
        __FILE__, __FUNCTION__, __LINE__);
        fflush(stdout);
    }
    return 0;
}

static int addnode(struct node *listhead, struct node *node)
{
    int error = -1;
    struct node *tmpnode = NULL;
    struct node *newnode = NULL;
    void *ret = NULL;

    assert(listhead != NULL);
    newnode = (struct node *)malloc(sizeof(struct node));
    if (newnode == NULL) {
        fprintf(stderr, "addnode error\n");
        fflush(stderr);
        return error;
    }
    ret = memset(newnode, 0x0, sizeof(struct node));
    if (ret != newnode) {
        fprintf(stderr, "memset():fail\n");
        fflush(stderr);
        return error;
    }
    ret = memcpy(newnode, node, sizeof(struct node));
    if (ret != newnode) {
        fprintf(stderr, "memcpy():fail\n");
        fflush(stderr);
        return error;
    }   
    tmpnode = listhead;

    while (tmpnode->nextnode != NULL)
    {
        tmpnode = tmpnode->nextnode;
    }
    tmpnode->nextnode = newnode;

    return 0;
}

static int removenode(struct node *listhead, void *ptr)
{
    struct node *node = NULL;
    struct node *tmpnode = NULL;

    assert(listhead != NULL);
    node = listhead;

    while (node->nextnode != NULL)
    {
        if (node->nextnode->ptr == ptr)
        {
            tmpnode = node->nextnode;
            node->nextnode = tmpnode->nextnode;
            free(tmpnode);
            tmpnode = NULL;
        
        fprintf(stdout, "%s::%s():%d: removenode found!\n", 
        __FILE__, __FUNCTION__, __LINE__);
        fflush(stdout);
        
            return 0;
        }
        node = node->nextnode;
    }
    return -1;
}

static int destroylist(struct node *listhead)
{
    struct node *tmpnode = NULL;
    struct node *node = NULL;

    assert(listhead != NULL);
    
    node = listhead;

    while (node->nextnode != NULL)
    {
        tmpnode = node->nextnode;
        node->nextnode=tmpnode->nextnode;
        free(tmpnode);
        tmpnode = NULL;
    }

    free(listhead);
    return 0;
}

static int getnode(struct node *listhead, struct node **nodepp, void *ptr)
{
    struct node *node = NULL;

    assert(listhead != NULL);
    node = listhead;

    while (node->nextnode != NULL)
    {
        if (node->nextnode->ptr == ptr)
        {
        fprintf(stdout, "%s::%s():%d: getnode found!\n", 
        __FILE__, __FUNCTION__, __LINE__);
        fflush(stdout);
        *nodepp = node->nextnode;
            return 0;
        }
        node = node->nextnode;
    }
    return -1;
}

