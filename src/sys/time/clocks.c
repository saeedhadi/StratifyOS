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


/*! \addtogroup TIME
 * @{
 */

/*! \file */


#include "config.h"
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

#include "../sched/sched_flags.h"

#define CLOCK_PROCESS_FLAG (1<<31)
static i32 convert_clocks_to_nanoseconds(i32 clocks);
static void task_timer_to_timespec(struct timespec * tp, uint64_t task_timer);


/*! \details This function is not supported.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENOTSUP:  not supported
 */
int clock_getcpuclockid(pid_t pid, clockid_t *clock_id){
	errno = ENOTSUP;
	return -1;
}

/*! \details This function gets the time of the \a id clock where \a id is one of:
 * - CLOCK_MONOTONIC
 * - CLOCK_REALTIME
 * - CLOCK_PROCESS_CPUTIME
 * - CLOCK_THREAD_CPUTIME
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL:  \a id is not one of the above clocks or tp is NULL
 *
 */
int clock_gettime(clockid_t id, struct timespec * tp){
	uint64_t task_timer;
	div_t d;
	struct sched_timeval sched_time;
	int pid;
	int i;

	if ( tp == NULL ){
		errno = EINVAL;
		return -1;
	}

	switch(id){
	case CLOCK_MONOTONIC:
	case CLOCK_REALTIME:
		mcu_core_privcall((core_privcall_t)sched_priv_get_realtime, &sched_time);
		d = div(sched_time.tv_usec, 1000000);
		tp->tv_sec = sched_time.tv_sec * SCHED_TIMEVAL_SECONDS + d.quot;
		tp->tv_nsec = d.rem * 1000;
		break;

	case CLOCK_PROCESS_CPUTIME_ID:
		//Sum the task timers for the calling process
		pid = task_get_pid( task_get_current() );
		task_timer = 0;
		for(i=0; i < task_get_total(); i++){
			if ( task_get_pid(i) == pid ){
				task_timer += task_gettime(i);
			}
		}
		task_timer_to_timespec(tp, task_timer);
		break;

	case CLOCK_THREAD_CPUTIME_ID:
		//The current task timer value
		//! \todo This should be an uninterruptable read
		task_timer = task_gettime( task_get_current() );
		task_timer_to_timespec(tp, task_timer);
		break;

	default:
		errno = EINVAL;
		return -1;
	}
	return 0;
}

int32_t convert_clocks_to_nanoseconds(int32_t clocks){
	uint64_t tmp;
	tmp = (uint64_t)clocks * stratify_board_config.clk_nsec_div + 512;
	return (uint32_t)(tmp / 1024);
}

void task_timer_to_timespec(struct timespec * tp, uint64_t task_timer){
	uint64_t nanosec;
	ldiv_t divide;
	divide = ldiv(task_timer, _mcu_core_getclock());
	nanosec = divide.rem * stratify_board_config.clk_nsec_div;
	tp->tv_sec = divide.quot;
	tp->tv_nsec = nanosec;
}

/*! \details This function gets the resolution of the \a id clock where \a id is one of:
 * - CLOCK_MONOTONIC
 * - CLOCK_REALTIME
 * - CLOCK_PROCESS_CPUTIME
 * - CLOCK_THREAD_CPUTIME
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL:  \a id is not one of the above clocks or res is NULL
 *
 */
int clock_getres(clockid_t id, struct timespec * res){

	if ( res == NULL ){
		errno = EINVAL;
		return -1;
	}

	switch(id){
	case CLOCK_MONOTONIC:
	case CLOCK_REALTIME:
		//1 second
		res->tv_sec = 0;
		res->tv_nsec = 1000;
		break;

	case CLOCK_PROCESS_CPUTIME_ID:
	case CLOCK_THREAD_CPUTIME_ID:
		//One clock tick
		res->tv_sec = 0;
		res->tv_nsec = convert_clocks_to_nanoseconds(1);
		break;


	default:
		errno = EINVAL;
		return -1;
	}
	return 0;
}

/*! \details This function is not supported.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENOTSUP:  not supported
 *
 */
int clock_settime(clockid_t id, const struct timespec * tp){
	errno = ENOTSUP;
	return -1;
}

/*! @} */
