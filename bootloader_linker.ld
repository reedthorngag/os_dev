OUTPUT_FORMAT(binary)
SECTIONS {
    . = 0x7C00;
    
    .boot :
    {
        *(.boot)
    }

    . = 0xa000;

    .data :
    {
        *(.kernel_data)
        second_stage.o
    }

}