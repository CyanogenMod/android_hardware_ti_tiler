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
#include <sys/ioctl.h>
/* #include "memmgr.h" */
#include "tiler.h"
#include "tilermgr.h"

#define PAGESIZE 0x1000
#define CONTAINER_W 256
#define CONTAINER_H 128
#define CONTAINER_LEN CONTAINER_W*CONTAINER_H*PAGE
#define TILERTEST_MEM_8BIT  0x60000000
#define TILERTEST_MEM_16BIT 0x68000000
#define TILERTEST_MEM_32BIT 0x70000000
#define TILERTEST_MEM_PAGED 0x78000000
#define TILERTEST_MEM_END   0x80000000

#define dump(x) fprintf(stdout, "%s::%s():%d: %lx\n", \
		__FILE__, __func__, __LINE__, (unsigned long)x); fflush(stdout);
#define TILERTEST_EPRINT \
	fprintf(stderr, "%s::%s():%d: error\n", __FILE__, __func__, __LINE__); \
	fflush(stderr);

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
	struct tiler_block_info block[TILER_MAX_NUM_BLOCKS];
	struct tiler_buf_info bufinfo;
	int fd = 0;
	int fd2 = 0;
	SSPtr physptr = 0x0;
	SSPtr ssptr[TILER_MAX_NUM_BLOCKS] = {0};
	void *vsptr[TILER_MAX_NUM_BLOCKS] = {NULL};
	int i = 0;
	int ret = -1;
	int offset = 0;
	unsigned long size = 0;

	assert(TilerMgr_Open() == 0);
	assert(createlist(&lsthd) == 0);
	dump(0);

	for (i = 0; i < TILER_MAX_NUM_BLOCKS; i++) 
	{
		block[i].fmt = TILFMT_8BIT;
		block[i].dim.area.width = 176;
		block[i].dim.area.height = 144;
		block[i].ssptr = TilerMgr_Alloc(block[i].fmt, block[i].dim.area.width, block[i].dim.area.height);
		if (block[i].ssptr == 0x0)
			TILERTEST_EPRINT;
		dump(block[i].ssptr);
		bufinfo.num_blocks = i+1;
		bufinfo.blocks[i].fmt = block[i].fmt;
		bufinfo.blocks[i].dim.area.width = block[i].dim.area.width;
		bufinfo.blocks[i].dim.area.height = block[i].dim.area.height;
		bufinfo.blocks[i].ssptr = block[i].ssptr;
	}

	fd = open("/dev/tiler", O_RDWR | O_SYNC);
	if (fd < 0) {
		TILERTEST_EPRINT;
		for(i=0;i<TILER_MAX_NUM_BLOCKS;i++)
			TilerMgr_Free(block[i].ssptr);
		return error;
	}

	ret = ioctl(fd, TILIOC_RBUF, (unsigned long)(&bufinfo));
	if (ret == -1) {
		TILERTEST_EPRINT;
		for(i=0;i<TILER_MAX_NUM_BLOCKS;i++)
			TilerMgr_Free(block[i].ssptr);
		close(fd);
		return error;
	}

	dump(bufinfo.offset);
	offset = bufinfo.offset;

	memset(&bufinfo, 0x0, sizeof(struct tiler_buf_info));
	bufinfo.offset = offset;
	
	ret = ioctl(fd, TILIOC_QBUF, (unsigned long)(&bufinfo));
	if (ret == -1) {
		TILERTEST_EPRINT;
		for(i=0;i<TILER_MAX_NUM_BLOCKS;i++)
			TilerMgr_Free(block[i].ssptr);
		close(fd);
		return error;
	}

	dump(bufinfo.num_blocks);
	dump(bufinfo.offset);
	for (i = 0; i < TILER_MAX_NUM_BLOCKS; i++) {
		dump(bufinfo.blocks[i].ssptr);
		size += bufinfo.blocks[i].dim.area.height;
	}

	fd2 = open("/dev/tiler", O_RDWR | O_SYNC);
	if (fd2 < 0) {
		TILERTEST_EPRINT;
		return error;
	}
	vsptr[0] = (void *)mmap(0, size*PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bufinfo.offset);
	/* vsptr[0] = (void *)mmap(0, bufinfo.blocks[0].dim.area.height*PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, bufinfo.blocks[0].ssptr); */
	if ((unsigned long *)vsptr[0] == MAP_FAILED) {
		TILERTEST_EPRINT;
		close(fd2);
	}
	memset(vsptr[0], 0x0, size*PAGESIZE);

	dump(0);
	physptr = TilerMgr_VirtToPhys(vsptr[0]);

	dump(physptr);
	write_buffer_data(vsptr[0], 176, 0xaaaaaaaa);
	dump(0);
	read_buffer_data(vsptr[0], 176);
	dump(0);

	physptr = TilerMgr_VirtToPhys(vsptr[0]+0x400);
	fprintf(stdout, "%s::%s():%d: physptr -> 0x%lx\n", __FILE__, __func__, __LINE__, (unsigned long)physptr); fflush(stdout);

	dump(0);
	write_buffer_data(vsptr[0]+0x400, 176, 0xbbbbbbbb);
	dump(0);
	read_buffer_data(vsptr[0]+0x400, 176);
	dump(0);
	read_buffer_data(vsptr[0], 6*PAGESIZE);

	dump(0);
	for (i = 0; i < TILER_MAX_NUM_BLOCKS; i++) {
		error = TilerMgr_Free(block[i].ssptr);
		if (error != TILERMGR_ERR_NONE)
			TILERTEST_EPRINT;
		dump(block[i].ssptr);
	}

	dump(0);
	error = TilerMgr_Close();
	if (error != TILERMGR_ERR_NONE)
		TILERTEST_EPRINT;

	error = destroylist(lsthd);
	if (error != 0)
		TILERTEST_EPRINT; return -1;

	return 0;
}

static void write_buffer_data(void *ptr, unsigned long len, unsigned long data) 
{
	unsigned long *ptmp = NULL;
	int i = -1;
    
	dump(0);
	ptmp = (unsigned long *)ptr;
    
	if (ptmp != NULL) {
		for (i = 0; i < len; i+=4) {
			*ptmp = data;
			ptmp++;
		}
	}
	else {
		TILERTEST_EPRINT;
	}
}

static void read_buffer_data(void *ptr, unsigned long len)
{
	unsigned long *ptmp = NULL;
	int i = -1;

	dump(ptr);
	dump(len);
	ptmp = (unsigned long *)ptr;
    
	if (ptmp != NULL) {
		for (i = 0; i < len; i+=4) {
			fprintf(stdout, "r[%p] -> 0x%lx\n", ptmp, *ptmp); fflush(stdout);
			ptmp++;
		}
	}
	else {
		TILERTEST_EPRINT;
	}
}

static int createlist(struct node **listhead)
{
    int error = -1;
    void *ret = NULL;

    *listhead = (struct node *)malloc(sizeof(struct node));
    if (*listhead == NULL) {
        TILERTEST_EPRINT;
        return error;
    }
    ret = memset(*listhead, 0x0, sizeof(struct node));
    if (ret != *listhead) {
        TILERTEST_EPRINT;
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
        TILERTEST_EPRINT;
        return error;
    }
    ret = memset(newnode, 0x0, sizeof(struct node));
    if (ret != newnode) {
        TILERTEST_EPRINT;
        return error;
    }
    ret = memcpy(newnode, node, sizeof(struct node));
    if (ret != newnode) {
        TILERTEST_EPRINT;
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

