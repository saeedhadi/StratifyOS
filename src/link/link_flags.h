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


#ifndef LINK_FLAGS_H_
#define LINK_FLAGS_H_

#include "mcu/mcu.h"
#include "iface/link.h"

#ifdef __cplusplus
extern "C" {
#endif



#define LINK_PHY_NAME_MAX 1024

#define LINK_MAX_SN_SIZE 512


int link_handle_err(link_transport_mdriver_t * driver, int err);

int link_ioctl_delay(link_transport_mdriver_t * driver, int fildes, int request, void * argp, int arg, int delay);


//Functions
int link_rd_err(link_transport_phy_t dev);

#define SET_FD_PORT(fd,port) (fd|(port<<8))
#define GET_FD(fd_port) (fd_port & 0xFF)
#define GET_PORT(fd_port) (fd_port >> 8)

#ifdef __link
extern int link_debug_level;
link_debug_context_t link_debug_context;
void link_write_debug_message(link_debug_context_t * context);
#define link_debug(x, ...) do { link_debug_context.type = x; sprintf(link_debug_context.function, "%s()", __FUNCTION__ ); sprintf(link_debug_context.file, "%s():", __FILE__ ); link_debug_context.line = __LINE__; sprintf(link_debug_context.msg, __VA_ARGS__); link_write_debug_message(&link_debug_context);  } while(0)
#define link_error(...) link_debug(LINK_DEBUG_CRITICAL, __VA_ARGS__)
#else
#define link_debug(x, ...)
#define link_error(...)
#endif


//default driver
int link_phy_getname(char * dest, const char * last, int len);
link_transport_phy_t link_phy_open(const char * name, int baudrate);
int link_phy_status(link_transport_phy_t handle);
int link_phy_write(link_transport_phy_t handle, const void * buf, int nbyte);
int link_phy_read(link_transport_phy_t handle, void * buf, int nbyte);
int link_phy_close(link_transport_phy_t handle);
void link_phy_flush(link_transport_phy_t handle);
int link_phy_lock(link_transport_phy_t phy);
int link_phy_unlock(link_transport_phy_t phy);
void link_phy_wait(int msec);


#ifdef __cplusplus
}
#endif


#endif /* LINK_FLAGS_H_ */
