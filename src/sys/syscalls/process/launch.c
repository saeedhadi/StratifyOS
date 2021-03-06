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
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>. */

#include "config.h"

#include "iface/device_config.h"
#include "mcu/task.h"
#include "mcu/debug.h"

#include <fcntl.h>
#include <errno.h>

#include "stratify/sysfs.h"
#include "stratify/stratify.h"
#include "process_start.h"
#include "../../sched/sched_flags.h"
#include "../../sysfs/appfs.h"

#define PATH_ARG_MAX 256

int launch(const char * path,
		char * exec_dest,
		const char * args,
		int options,
		int ram_size,
		int (*update_progress)(int, int),
		char *const envp[]){

	char exec_path[PATH_ARG_MAX];

	//check if path exists
	if( access(path, R_OK) < 0 ){
		return -1;
	}


	if( args != 0 ){
		if( strlen(args) > PATH_ARG_MAX - PATH_MAX ){
			errno=ENAMETOOLONG;
			return -1;
		}
	}

	if( options & LINK_APPFS_EXEC_OPTIONS_REPLACE ){
		//kill the process if it is running (or abort)

		//delete the image and re-install

	}

	//if path is already in app then there is no need to install -- just execute
	if( strncmp(path, "/app/", 5) != 0 ){

		if( install(path, exec_path, options, ram_size, update_progress) < 0 ){
			return -1;
		}


	} else {
		strcpy(exec_path, path);
	}

	if( exec_dest != 0 ){
		strcpy(exec_dest, exec_path);
	}

	if( args ){
		strcat(exec_path, " ");
		strcat(exec_path, args);
	}

	return process_start(exec_path, envp);
}
