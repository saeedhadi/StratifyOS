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

/*! \addtogroup SYSCALLS
 * @{
 */

/*! \file */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <reent.h>

#include "mcu/mcu.h"
#include "mcu/task.h"
#include "mcu/core.h"


static void priv_update_guard(void * args) MCU_PRIV_EXEC_CODE;

void * _sbrk_r(struct _reent * reent_ptr, ptrdiff_t incr){
	char * stack;
	char * base;
	ptrdiff_t size;

	if ( reent_ptr->procmem_base == NULL ){
		return NULL;
	}
	size = reent_ptr->procmem_base->size;
	base = (char*)&(reent_ptr->procmem_base->base);


#if SINGLE_TASK == 0
	stack = (void*)task_get_sbrk_stack_ptr(reent_ptr);
#else
	stack = core_get_thread_stack_ptr();
#endif


	//leave some room for the stack to grow
	if ( (stack != NULL) &&  ((base + size + incr) > (stack - MALLOC_SBRK_JUMP_SIZE*4)) ){
		return NULL;
	}


	//adjust the location of the stack guard -- always 32 bytes for processes
#if USE_MEMORY_PROTECTION > 0
	mcu_core_privcall(priv_update_guard, base + size + incr);
#endif

	reent_ptr->procmem_base->size += incr;
	return (caddr_t) (base + size);
}

#if USE_MEMORY_PROTECTION > 0
void priv_update_guard(void * args){
	int tid;
	tid = task_get_thread_zero( task_get_pid(task_get_current() ) );
	task_priv_set_stackguard(tid, args, SCHED_DEFAULT_STACKGUARD_SIZE);
}
#endif


void * _sbrk(ptrdiff_t incr) {
	return _sbrk_r(_REENT, incr);
}

/*! @} */
