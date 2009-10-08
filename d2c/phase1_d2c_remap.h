/*
 * phase1_d2c_remap.h
 *
 * Ducati to Chiron Tiler block remap Interface for TI OMAP processors.
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

#ifndef _PHASE1_D2C_REMAP_H_
#define _PHASE1_D2C_REMAP_H_

#define REMAP_ERR_NONE    0
#define REMAP_ERR_GENERIC 1

/**
 * During Phase 1, tiler info is needed to remap tiler blocks 
 * from Ducati to Chiron.  However, this is no longer needed in 
 * phase 2, as the virtual stride will be the same on Ducati and 
 * Chiron.  This API is provided here temporarily for phase 1. 
 *  
 * This method remaps a list of tiler blocks (specified by the 
 * Ducati virtual address and the block size) in a consecutive 
 * fashion into one virtual buffer on Chiron. 
 *  
 * :NOTE: this function does not work correctly for 2D buffers
 * of the following geometries:
 * 
 *  16-bit, width > 73.148 * height
 *  32-bit, width > 36.574 * height
 * 
 * This is due to limitations of the API definition, as tiler 
 * driver does not keep track of exact widths and heights of its
 * allocated blocks, and the remapping algorithm is trying to
 * determine the page-width of each block.  For buffers
 * mentioned above there are multiple solutions, and the
 * 
 * This limitation will not apply in phase 2.
 *  
 * @author a0194118 (9/9/2009)
 * 
 * @param num_blocks   Number of blocks to remap
 * @param dsptrs       Array of Ducati addresses (one for each 
 *                     block).  Each address must point to the
 *                     beginning of a tiler allocated 1D or 2D
 *                     block.
 * @param lengths      Array of block lenghts (one for each 
 *                     block).  This is the desired length of
 *                     the blocks on the Chiron, and must be
 *                     multiples of the page size. These are
 *                     also used to infer the height of 2D
 *                     buffers, and the length of 1D buffers, as
 *                     the tiler driver does not keep track of
 *                     exact block sizes, only allocated block
 *                     sizes.
 * 
 * @return Pointer to the remapped combined buffer.  NULL on 
 *         failure.
 */
void *tiler_assisted_phase1_D2CReMap(int num_blocks, DSPtr dsptrs[],
                                     bytes_t lengths[]);

/**
 * During Phase 1, tiler info is needed to remap tiler blocks 
 * from Ducati to Chiron.  However, this is no longer needed in 
 * phase 2, as the virtual stride will be the same on Ducati and 
 * Chiron.  This API is provided here temporarily for phase 1.
 *  
 * This method unmaps a previously remapped buffer from the 
 * Chiron side. 
 *  
 * @author a0194118 (9/9/2009)
 * 
 * @param bufPtr   Pointer to the buffer as returned by a 
 *                 tiler_assisted_phase1_D2CReMap call.
 * 
 * @return 0 on success, non-0 error value on failure.
 */
int tiler_assisted_phase1_DeMap(void *bufPtr);

#endif