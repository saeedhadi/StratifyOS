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



#ifndef CAFS_LITE_FILE_H_
#define CAFS_LITE_FILE_H_

#include <stdbool.h>

#include "../../../include/stratify/sffs.h"
#include "../sffs/sffs_block.h"
#include "../sffs/sffs_dir.h"
#include "../sffs/sffs_local.h"

typedef struct {
	int segment;
	int offset;
} sffs_file_segment_t;


int sffs_file_new(const void * cfg, cl_handle_t * handle, const char * name, int mode, sffs_dir_lookup_t * entry, uint8_t type, int amode);
int sffs_file_open(const void * cfg, cl_handle_t * handle, serial_t serialno, int amode, bool trunc);
int sffs_file_close(const void * cfg, cl_handle_t * handle);
int sffs_file_write(const void * cfg, cl_handle_t * handle, int start_segment, int nsegments);

int sffs_file_remove(const void * cfg, serial_t serialno);

int sffs_file_startread(const void * cfg, cl_handle_t * handle);
int sffs_file_finishread(const void * cfg, cl_handle_t * handle);

int sffs_file_startwrite(const void * cfg, cl_handle_t * handle);
int sffs_file_finishwrite(const void * cfg, cl_handle_t * handle);

int sffs_file_read(const void * cfg, cl_handle_t * handle, int start_segment,  int nsegments);
void sffs_file_getsegment(const void * cfg, sffs_file_segment_t * segment, int loc);

int sffs_file_swapsegment(const void * cfg, cl_handle_t * handle, int new_segment);

int sffs_file_loadsegment(const void * cfg, cl_handle_t * handle, int segment);
int sffs_file_savesegment(const void * cfg, cl_handle_t * handle);

int sffs_file_clean(const void * cfg, serial_t serialno, block_t hdr_block, uint8_t status);




#endif /* CAFS_LITE_FILE_H_ */
