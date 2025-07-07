MEMORY
{
  FLASH (rx)            : ORIGIN = 0x08000000, LENGTH = 3072K
  TCMSRAM (rwx)          : ORIGIN = 0x10000000, LENGTH = 64K
  SRAM (rwx)             : ORIGIN = 0x20000000, LENGTH = 192K
}

/* Entry Point */
ENTRY(__start)
PROVIDE(_reset = SystemInit);

/* Highest address of the user mode stack */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x400 ;      /* required amount of heap  */
_Min_Stack_Size = 0x400 ; /* required amount of stack */

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);    
    _vector_start = .;
    KEEP(*(.vectors))
    KEEP(*(.gnu.linkonce.irq_vector_table*))
    _vector_end = .;
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    _stext = .;
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP(*(.gnu.linkonce.syscall_tables))
    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  /* used by the startup to initialize data */
  __data_region_start = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    __data_start = _sdata;
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
    __data_end = _edata;
  } >SRAM AT> FLASH

  __data_region_end = .;

  .bss :
  {
    . = ALIGN(4);
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end = _ebss;
  } >SRAM

  /* User_heap section, used to check that there is enough RAM left */
  ._user_heap :
  {
    . = ALIGN(8);
    __heap_start = .;
    . = . + _Min_Heap_Size;
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    __heap_end = .;
  } >SRAM


  /* No-initialization section for variables that should retain their value across resets */
  .noinit (NOLOAD) :
  {
    . = ALIGN(4);
    _sstack = .;
    *(.noinit)         /* .noinit sections */    
    . = ALIGN(4);
    _estack = .;
  } >TCMSRAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}