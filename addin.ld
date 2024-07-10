OUTPUT_ARCH(sh3)
ENTRY(initialize)
MEMORY
{
        rom  : o = 0x00300200, l = 512k
        ram  : o = 0x08100000, l = 64k  /* pretty safe guess */
}
SECTIONS
{
        .text : {
                *(.pretext)     /* init stuff */
                *(.text)
        } > rom
        .rodata : {
                *(.rodata)
                *(.rodata.str1.4)
		. = ALIGN(4);
                _romdata = . ;  /* symbol for initialization data */
        } > rom
        .bss : {
                _bbss = . ;
                _bssdatasize = . ;
                LONG(0);        /* bssdatasize */
                *(.bss) *(COMMON);
                _ebss = . ;
        } > ram
        .data : {
                _bdata = . ;
                *(.data);
                _edata = . ;
        } > ram AT> rom
}
