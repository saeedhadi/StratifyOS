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

/*! \addtogroup UNI_SLEEP
 * @{
 */

/*! \file */

#include <unistd.h>
#include <time.h>
#include "../sched/sched_flags.h"

static void priv_sleep(void * args) MCU_PRIV_EXEC_CODE;

/*! \details This function causes the calling thread to sleep for \a seconds seconds.
 * \return 0
 */
unsigned int sleep(unsigned int seconds /*! The number of seconds to sleep */){
	struct sched_timeval interval;
	div_t d;

	if ( task_get_current() != 0 ){
		if ( seconds < SCHED_TIMEVAL_SECONDS ){
			interval.tv_sec = 0;
			interval.tv_usec = seconds * 1000000;
		} else {
			d = div(seconds, SCHED_TIMEVAL_SECONDS);
			interval.tv_sec = d.quot;
			interval.tv_usec = d.rem;
		}
		mcu_core_privcall(priv_sleep, &interval);
	}
	return 0;
}

void priv_sleep(void * args){
	struct sched_timeval * p;
	struct sched_timeval abs_time;
	p = (struct sched_timeval*)args;
	sched_priv_get_realtime(&abs_time);
	abs_time.tv_sec += p->tv_sec;
	abs_time.tv_usec += p->tv_usec;
	if ( abs_time.tv_usec > SCHED_TIMEVAL_SECONDS*1000000 ){
		abs_time.tv_sec++;
		abs_time.tv_usec -= SCHED_TIMEVAL_SECONDS*1000000;
	}
	sched_priv_timedblock(NULL, &abs_time);
}

/*! @} */
