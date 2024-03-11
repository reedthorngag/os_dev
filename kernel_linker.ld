OUTPUT_FORMAT(binary)
SECTIONS {

    . = 0x10000;

    .data :
    {
        kernel.o
        *(.kernel)
        *(.data)
        *(.kernel_data)
    }

    .text :
    {
        *(.text)
    }

    .resources :
    {
        *(.rodata)
        *(.resources)
        font.o
    }

    .bss :
    {
        *(.bss)
    }
}