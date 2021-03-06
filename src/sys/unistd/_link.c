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

/*! \addtogroup UNI_FS
 * @{
 */

/*! \file */

#include "unistd_fs.h"
#include "unistd_flags.h"

/*! \details This function creates a hard link between \a old and \a new.
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - ENOTSUP:  operation not supported
 *
 */
int link(const char *old, const char *new);

int _link(const char *old, const char *new){
	const sysfs_t * fs_old;
	const sysfs_t * fs_new;

	if ( sysfs_ispathinvalid(old) == true ){
		return -1;
	}

	if ( sysfs_ispathinvalid(new) == true ){
		return -1;
	}

	fs_old = sysfs_find(old, true);
	fs_new = sysfs_find(new, true);

	if ( (fs_old == fs_new) && (fs_new != NULL) ){
		return fs_new->link(&fs_new->cfg,
				sysfs_stripmountpath(fs_old, old),
				sysfs_stripmountpath(fs_new, new)
				);
	}

	if ( (fs_new == NULL) || (fs_old == NULL) ){
		errno = ENOENT;
	} else {
		errno = ENOTSUP;
	}
	return -1;
}

/*! @} */

