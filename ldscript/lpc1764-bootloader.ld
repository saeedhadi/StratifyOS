
EXTERN ( core_vector_table )

MEMORY {
   flash : ORIGIN = 0, LENGTH = 128K
   ram : ORIGIN = 0x10000000, LENGTH = 16K
   sysmem : ORIGIN = 0x2007F000, LENGTH = 4K
}

__ram_size__ = 32K;

SECTIONS {

   .sysmem : { 
		_sys = . ;
		*(.sysmem)
		. = ALIGN(1024);
	} >sysmem
	_esys = .;

  . = 0;
  .text : {
         _text = .;
  		 *(.startup)
  		 *(.reset_vector)
         *(.text)
         *(.init)
         *(.fini)
         *(.eh_frame)
         *(.ctors)
         *(.dtors)
         *(.jcr)
         *(.rodata)
         *(.rodata*)
         *(.glue_7)
         *(.glue_7t)
         *(.text.*)
         } >flash
  .ARM.extab : 
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > flash
	
  __exidx_start = .;
  .ARM.exidx   : { 
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
  } >flash
  __exidx_end = .;
  _etext = .;
  .data : {
         _data = .;
         *(.data)
         *(.data.*)
         } >ram AT>flash
  _edata = .;
  .bss : {
    	_bss = .;
         *(.bss)
         *(.bss.*)
         } >ram
  . = ALIGN(4);
  _ebss = .;
}

PROVIDE ( _end = . );
PROVIDE ( _iap = 0x1FFF1FF1 );
PROVIDE ( _top_of_stack = (0x20080000 - 32) );
PROVIDE ( _flash_pages = 18 );
PROVIDE ( _flash_size = 128*1024 );
PROVIDE ( _sram_size = 16*1024 );
PROVIDE ( _ahb_sram_size = 16*1024 );




