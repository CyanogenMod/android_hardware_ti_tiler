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

	b1.pixelFormat = PIXEL_FMT_8BIT;
	b1.dim.area.width = 176;
	b1.dim.area.height = 144;

	for (i = 0; i < TILERTEST_COUNT; i++) 
	{
		ssptr[i] = TilerMgr_Alloc(b1.pixelFormat, b1.dim.area.width, b1.dim.area.height);
		if (ssptr[i] == 0x0)
			fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		dump(ssptr[i]);
	}

#if 0
	dump(0);
	fd = open("/dev/tiler", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		TilerMgr_Free(b1.reserved);
		return error;
	}

	dump(b1.dim.area.height*PAGESIZE);
	b1.ptr = (void *)mmap(0, b1.dim.area.height*PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, ssptr[TILERTEST_COUNT]);
	if ((unsigned long *)b1.ptr == MAP_FAILED) {
		fprintf(stderr, "%s::%s():%d:F!\n", __FILE__, __func__, __LINE__); fflush(stderr);
		close(fd);
		TilerMgr_Free(ssptr[TILERTEST_COUNT]);
		return error;
	}
	memset(b1.ptr, 0x0, b1.dim.area.height*PAGESIZE);

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
	read_buffer_data(b1.ptr, b1.dim.area.height*PAGESIZE);
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

