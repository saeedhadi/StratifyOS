/* Copyright 2011-2016 Tyler Gilbert; 
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 */


#ifndef CAFS_BLOCK_H_
#define CAFS_BLOCK_H_

#include "../sffs/sffs_local.h"

/*! \details This function finds a free data block.  The data block will
 * be marked as "allocated" when this function returns.  The function will try to allocate
 * the data in an erasable block that is held by other blocks with the same file serial number.
 */
block_t sffs_block_alloc(const void * cfg, serial_t serialno, block_t hint, uint8_t type);
int sffs_block_save(const void * cfg, block_t sffs_block_num, sffs_block_data_t * data);
int sffs_block_saveraw(const void * cfg, block_t sffs_block_num, sffs_block_data_t * data);
int sffs_block_load(const void * cfg, block_t sffs_block_num, sffs_block_data_t * data);
int sffs_block_loadhdr(const void * cfg, sffs_block_hdr_t * dest, block_t src);
int sffs_block_setstatus(const void * cfg, block_t sffs_block_num, uint8_t status);

int sffs_block_discardopen(const void * cfg);

serial_t sffs_block_get_serialno(const void * cfg, block_t block);

block_t sffs_block_geteraseable(const void * cfg);
int sffs_block_gettotal(const void * cfg);
block_t sffs_block_getfirst(const void * cfg);

static inline int sffs_block_discard(const void * cfg, block_t sffs_block_num) MCU_ALWAYS_INLINE;
int sffs_block_discard(const void * cfg, block_t sffs_block_num){
	return sffs_block_setstatus(cfg, sffs_block_num, BLOCK_STATUS_DIRTY);
}

static inline int sffs_block_close(const void * cfg, block_t sffs_block_num) MCU_ALWAYS_INLINE;
int sffs_block_close(const void * cfg, block_t sffs_block_num){
	return sffs_block_setstatus(cfg, sffs_block_num, BLOCK_STATUS_CLOSED);
}

#endif /* CAFS_BLOCK_H_ */
