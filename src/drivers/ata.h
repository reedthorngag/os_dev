#include <typedefs.h>

u16 DATA_PORT = 0x1F0;
u16 ERROR_PORT = 0x1F1;

u16 SECTOR_COUNT_PORT = 0x1F2;
u16 LBA_LOW_PORT = 0x1F3;
u16 LBA_MID_PORT = 0x1F4;
u16 LBA_HIGH_PORT = 0x1F5;

u16 DEVICE_CONTROL_PORT = 0x3F6;
u16 DRIVE_ADDRESS_REGISTER = 0x3F7;

u16 DRIVE_SELECT_PORT = 0x1F6;
u16 COMMAND_IO = 0x1F7;

u8 MASTER_DRIVE = 0xE0;
u8 SLAVE_DRIVE = 0xF0;

enum Error {
    AMNF = 1<<0,	// Address mark not found.
    TKZNF= 1<<1,	// Track zero not found.
    ABRT = 1<<2,	// Aborted command.
    MCR  = 1<<3,	// Media change request.
    IDNF = 1<<4,	// ID not found.
    MC   = 1<<5,	    // Media changed.
    UNC  = 1<<6,	// Uncorrectable data error.
    BBK  = 1<<7,    // Bad Block detected.
};

enum Status {
    ERR  = 1<<0,	// Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).
    IDX  = 1<<1,	// Index. Always set to zero.
    CORR = 1<<2,	// Corrected data. Always set to zero.
    DRQ  = 1<<3,	// Set when the drive has PIO data to transfer, or is ready to accept PIO data.
    SRV  = 1<<4,	// Overlapped Mode Service Request.
    DF   = 1<<5,	// Drive Fault Error (does not set ERR).
    RDY  = 1<<6,	// Bit is clear when drive is spun down, or after an error. Set otherwise.
    BSY  = 1<<7,	// Indicates the drive is preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset.
};

enum Command {
    IDENTIFY = 0xEC
};

i32 init_ata();

u8* read(u32 position, u32 blocks);

i32 write(u8* data, u32 size);



