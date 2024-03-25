OUTPUT_FORMAT(binary)
SECTIONS {
    . = 0x7C00;
    
    .boot :
    {
        *(.boot)
    }

    .data :
    {
        *(.kernel_data)
        second_stage.o
    }

    bootloader_end = .;

}