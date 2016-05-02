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

#include "config.h"
#include <errno.h>
#include "mcu/eeprom.h"
#include "mcu/debug.h"
#include "mcu/core.h"

#if MCU_EEPROM_PORTS > 0

#define EEPROM_PAGES (MCU_EEPROM_SIZE/MCU_EEPROM_PAGE_SIZE)

typedef struct MCU_PACK {
	uint8_t * buf;
	mcu_event_handler_t handler;
	uint16_t len;
	uint8_t isread;
	uint8_t offset;
	uint8_t page;
	uint8_t ref_count;
} eeprom_local_t;

static eeprom_local_t eeprom_local;
static void exec_callback(void * data);

static int calc_offset(int loc){
	return loc % MCU_EEPROM_PAGE_SIZE;
}

static int calc_page(int loc){
	return loc / MCU_EEPROM_PAGE_SIZE;
}


void _mcu_eeprom_dev_power_on(int port){
	uint8_t phase[3];
	int cpu_mhz;

	if( eeprom_local.ref_count == 0 ){
		LPC_EEPROM->PWRDWN = 0;

		//enable the interrupt
		_mcu_core_priv_enable_irq((void*)(EEPROM_IRQn));

		//initialize the EEPROM clock
		LPC_EEPROM->CLKDIV = (mcu_board_config.core_cpu_freq / 375000) - 1;

		//initialize the STATE register
		cpu_mhz = mcu_board_config.core_cpu_freq / 1000000;
		phase[0] = (((cpu_mhz*15) + 500) / 1000) - 1;
		phase[1] = (((cpu_mhz*55) + 500) / 1000) - 1;
		phase[2] = (((cpu_mhz*35) + 500) / 1000) - 1;
		LPC_EEPROM->WSTATE = phase[0] | (phase[1]<<8) | (phase[2]<<16);

	}

	eeprom_local.ref_count++;
}

void _mcu_eeprom_dev_power_off(int port){

	if ( eeprom_local.ref_count > 0 ){
		if ( eeprom_local.ref_count == 1 ){
			//disable the interrupt
			_mcu_core_priv_disable_irq((void*)(EEPROM_IRQn));

			//power down
			LPC_EEPROM->PWRDWN = 1;
		}
		eeprom_local.ref_count--;
	}

}

int _mcu_eeprom_dev_powered_on(int port){
	return ( LPC_EEPROM->PWRDWN & (1<<0) ) == 0;
}


int mcu_eeprom_getattr(int port, void * ctl){
	eeprom_attr_t * attr = ctl;
	attr->size = 4032;
	return 0;
}
int mcu_eeprom_setattr(int port, void * ctl){
	return 0;
}

int mcu_eeprom_setaction(int port, void * ctl){
	mcu_action_t * action = (mcu_action_t *)ctl;
	if( action->callback == 0 ){
		if( eeprom_local.buf != 0 ){
			exec_callback(DEVICE_OP_CANCELLED);
		}
	}

	if( _mcu_core_priv_validate_callback(action->callback) < 0 ){
		return -1;
	}

	eeprom_local.handler.callback = action->callback;
	eeprom_local.handler.context = action->context;
	return -1;
}


int _mcu_eeprom_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	if ( wop->nbyte == 0 ){
		return 0;
	}

	//Check to see if the port is busy
	if ( eeprom_local.buf != 0 ){
		errno = EAGAIN;
		return -1;
	}

	//check for a valid wop->loc value
	if( ((wop->loc + wop->nbyte) > MCU_EEPROM_SIZE) || (wop->loc < 0) ){
		errno = EINVAL;
		return -1;
	}

	//Initialize variables
	eeprom_local.buf = wop->buf;
	eeprom_local.len = wop->nbyte;
	eeprom_local.isread = 0;

	eeprom_local.page = calc_page(wop->loc);
	eeprom_local.offset = calc_offset(wop->loc);

	//Check the local buffer for bytes that are immediately available
	if( _mcu_core_priv_validate_callback(wop->callback) < 0 ){
		return -1;
	}

	eeprom_local.handler.callback = wop->callback;
	eeprom_local.handler.context = wop->context;

	//fill the page
	LPC_EEPROM->ADDR = eeprom_local.offset;
	LPC_EEPROM->CMD = 3;
	do {
		LPC_EEPROM->WDATA = *eeprom_local.buf;
		//wait until the previous data has written
		while( (LPC_EEPROM->INT_STATUS & (1<<26)) == 0 ){
			;
		}
		LPC_EEPROM->INT_CLR_STATUS = (1<<26);
		eeprom_local.len--;
		eeprom_local.offset++;
		eeprom_local.buf++;
	} while( (eeprom_local.offset < MCU_EEPROM_PAGE_SIZE) && (eeprom_local.len > 0) );



	LPC_EEPROM->ADDR = eeprom_local.page << 6;
	eeprom_local.page++;
	eeprom_local.offset = 0;
	LPC_EEPROM->INT_SET_ENABLE = (1<<28);

	LPC_EEPROM->CMD = 6; //erase/program page


	return 0;

}

int _mcu_eeprom_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	if ( rop->nbyte == 0 ){
		return 0;
	}

	//Check to see if the port is busy
	if ( eeprom_local.buf != 0 ){
		errno = EAGAIN;
		return -1;
	}

	//check for a valid rop->loc value
	if( ((rop->loc + rop->nbyte) > MCU_EEPROM_SIZE) || (rop->loc < 0) ){
		errno = EINVAL;
		return -1;
	}


	//Initialize variables
	eeprom_local.buf = rop->buf;
	eeprom_local.len = rop->nbyte;
	eeprom_local.page = calc_page(rop->loc);
	eeprom_local.offset = calc_offset(rop->loc);

	LPC_EEPROM->INT_CLR_STATUS = (1<<26) | (1<<28);


	LPC_EEPROM->ADDR = eeprom_local.offset | (eeprom_local.page << 6);
	LPC_EEPROM->CMD = 0 | (1<<3);
	do {
		*eeprom_local.buf = LPC_EEPROM->RDATA;
		//wait until the previous data has been read
		while( (LPC_EEPROM->INT_STATUS & (1<<26)) == 0 ){
			;
		}
		LPC_EEPROM->INT_CLR_STATUS = (1<<26);
		eeprom_local.len--;
		eeprom_local.offset++;
		eeprom_local.buf++;

		if( eeprom_local.offset == MCU_EEPROM_PAGE_SIZE ){
			eeprom_local.offset = 0;
			eeprom_local.page++;
			LPC_EEPROM->ADDR = (eeprom_local.page << 6);
			LPC_EEPROM->CMD = 0 | (1<<3);
		}

	} while( eeprom_local.len > 0 );
	eeprom_local.buf = 0;
	return rop->nbyte;
}

void exec_callback(void * data){
	eeprom_local.buf = 0;
	LPC_EEPROM->INT_CLR_ENABLE = (1<<26)|(1<<28); //disable the interrupts
	_mcu_core_exec_event_handler(&(eeprom_local.handler), 0);
}

void _mcu_core_eeprom_isr(void){
	uint32_t status = LPC_EEPROM->INT_STATUS;
	LPC_EEPROM->INT_CLR_STATUS = status;
	if( status & (1<<28) ){
		//this was a program/erase action
		if( eeprom_local.len > 0 ){
			LPC_EEPROM->ADDR = eeprom_local.offset;
			LPC_EEPROM->CMD = 3;
			do {
				LPC_EEPROM->WDATA = *eeprom_local.buf;
				//wait until the previous data has written
				while( (LPC_EEPROM->INT_STATUS & (1<<26)) == 0 ){
					;
				}
				LPC_EEPROM->INT_CLR_STATUS = (1<<26);
				eeprom_local.len--;
				eeprom_local.offset++;
				eeprom_local.buf++;
			} while( (eeprom_local.offset < MCU_EEPROM_PAGE_SIZE) && (eeprom_local.len > 0) );

			LPC_EEPROM->ADDR = eeprom_local.page << 6;
			LPC_EEPROM->CMD = 6; //erase/program page
			eeprom_local.page++;
			eeprom_local.offset = 0;
			return;
		}
	}

	if( eeprom_local.len == 0 ){
		exec_callback(0);
	}
}

#endif
