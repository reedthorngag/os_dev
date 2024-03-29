#include <pci.h>

// I stole a lot of this code from https://github.com/foliagecanine/tritium-os/blob/master/kernel/arch/i386/sysmgmt/pci.c

typedef struct {
	uint8_t class;
	uint8_t subclass;
	const char *name;
} pci_classname;

pci_classname classnames[] = {
	{0x00, 0x00, "Non-VGA-Compatible Unclassified Device"},
	{0x00, 0x01, "VGA-Compatible Unclassified Device"},
	{0x01, 0x00, "SCSI Bus Controller"},
	{0x01, 0x01, "IDE Controller"},
	{0x01, 0x02, "Floppy Disk Controller"},
	{0x01, 0x03, "IPI Bus Controller"},
	{0x01, 0x04, "RAID Controller"},
	{0x01, 0x05, "ATA Controller"},
	{0x01, 0x06, "Serial ATA Controller"},
	{0x01, 0x07, "Serial Attached SCSI Controller"},
	{0x01, 0x08, "Non-Volatile Memory Controller"},
	{0x01, 0x80, "Other Mass Storage Controller"},
	{0x02, 0x00, "Ethernet Controller"},
	{0x02, 0x01, "Token Ring Controller"},
	{0x02, 0x02, "FDDI Controller"},
	{0x02, 0x03, "ATM Controller"},
	{0x02, 0x04, "ISDN Controller"},
	{0x02, 0x05, "WorldFip Controller"},
	{0x02, 0x06, "PICMG 2.14 Multi Computing Controller"},
	{0x02, 0x07, "Infiniband Controller"},
	{0x02, 0x08, "Fabric Controller"},
	{0x02, 0x80, "Other Network Controller"},
	{0x03, 0x00, "VGA Compatible Controller"},
	{0x03, 0x01, "XGA Controller"},
	{0x03, 0x02, "3D Controller (Not VGA-Compatible)"},
	{0x03, 0x80, "Other Display Controller"},
	{0x04, 0x00, "Multimedia Video Controller"},
	{0x04, 0x01, "Multimedia Audio Controller"},
	{0x04, 0x02, "Computer Telephony Device"},
	{0x04, 0x03, "Audio Device"},
	{0x04, 0x80, "Other Multimedia Controller"},
	{0x05, 0x00, "RAM Controller"},
	{0x05, 0x01, "Flash Controller"},
	{0x05, 0x80, "Other Memory Controller"},
	{0x06, 0x00, "Host Bridge"},
	{0x06, 0x01, "ISA Bridge"},
	{0x06, 0x02, "EISA Bridge"},
	{0x06, 0x03, "MCA Bridge"},
	{0x06, 0x04, "PCI-to-PCI Brige"},
	{0x06, 0x05, "PCMCIA Bridge"},
	{0x06, 0x06, "NuBus Bridge"},
	{0x06, 0x07, "CardBus Bridge"},
	{0x06, 0x08, "RACEway Bridge"},
	{0x06, 0x09, "PCI-to-PCI Bridge"},
	{0x06, 0x0A, "Infiniband-to-PCI Host Bridge"},
	{0x06, 0x80, "Other Bridge"},
	{0x07, 0x00, "Serial Controller"},
	{0x07, 0x01, "Parallel Controller"},
	{0x07, 0x02, "Multiport Serial Controller"},
	{0x07, 0x03, "Modem"},
	{0x07, 0x04, "IEEE 488.1/2 (GPIB) Controller"},
	{0x07, 0x05, "Smart Card Controller"},
	{0x07, 0x80, "Other Simple Communication Controller"},
	{0x08, 0x00, "PIC"},
	{0x08, 0x01, "DMA Controller"},
	{0x08, 0x02, "Timer"},
	{0x08, 0x03, "RTC Controller"},
	{0x08, 0x04, "PCI Hot-Plug Controller"},
	{0x08, 0x05, "SD Host Controller"},
	{0x08, 0x07, "IOMMU"},
	{0x08, 0x80, "Other Base System Peripheral"},
	{0x09, 0x00, "Keyboard Controller"},
	{0x09, 0x01, "Digitizer Pen"},
	{0x09, 0x02, "Mouse Controller"},
	{0x09, 0x03, "Scanner Controller"},
	{0x09, 0x04, "Gameport Controller"},
	{0x09, 0x80, "Other Input Device Controller"},
	{0x0A, 0x00, "Generic Docking Station"},
	{0x0A, 0x80, "Other Docking Station"},
	{0x0B, 0x00, "386 Processor"},
	{0x0B, 0x01, "486 Processor"},
	{0x0B, 0x02, "Pentium Processor"},
	{0x0B, 0x03, "Pentioum Pro Processor"},
	{0x0B, 0x10, "Alpha Processor"},
	{0x0B, 0x20, "PowerPC Processor"},
	{0x0B, 0x30, "MIPS Processor"},
	{0x0B, 0x40, "Co-Processor"},
	{0x0B, 0x80, "Other Processor"},
	{0x0C, 0x00, "FireWire (IEEE 1394) Controller"},
	{0x0C, 0x01, "ACCESS Bus Controller"},
	{0x0C, 0x02, "SSA"},
	{0x0C, 0x03, "USB Controller"},
	{0x0C, 0x04, "Fibre Channel"},
	{0x0C, 0x05, "SMBus Controller"},
	{0x0C, 0x06, "InfiniBand Controller"},
	{0x0C, 0x07, "IPMI Interface"},
	{0x0C, 0x08, "SERCOS Interface (IEC 61491)"},
	{0x0C, 0x09, "CANbus Controller"},
	{0x0C, 0x80, "Other Serial Bus Controller"},
	{0x0D, 0x00, "iRDA Compatible Controller"},
	{0x0D, 0x00, "Consumer IR Controller"},
	{0x0D, 0x00, "RF Controller"},
	{0x0D, 0x00, "Bluetooth Controller"},
	{0x0D, 0x00, "Broadband Controller"},
	{0x0D, 0x00, "Ethernet Controller (802.1a)"},
	{0x0D, 0x00, "Ethernet Controller (802.1b)"},
	{0x0D, 0x00, "Other Wireless Controller"},
	{0x0E, 0x00, "I20"},
	{0x0F, 0x01, "Satellite TV Controller"},
	{0x0F, 0x02, "Satellite Audio Controller"},
	{0x0F, 0x03, "Satellite Voice Controller"},
	{0x0F, 0x04, "Satellite Data Controller"},
	{0x10, 0x00, "Network and Computing Encryption/Decryption"},
	{0x10, 0x10, "Entertainment Encryption/Decryption"},
	{0x10, 0x80, "Other Encryption Controller"},
	{0x11, 0x00, "DPIO Modules"},
	{0x11, 0x01, "Performance Counters"},
	{0x11, 0x10, "Communication Synchronizer"},
	{0x11, 0x20, "Signal Processing Management"},
	{0x11, 0x80, "Other Signal Processing Controller"},
};

const char* find_pci_name(u8 class, u8 subclass) {
    switch(class) {
		case 0x12:
			return "Processing Accelerator";
		case 0x13:
			return "Non-Essential Instrumentation";
		case 0x40:
			return "Co-Processor";
		case 0xFF:
			return "Vendor Specific";
		default:
			for (u32 i = 0; i < sizeof(classnames)/sizeof(pci_classname); i++) {
				if (classnames[i].class == class && classnames[i].subclass == subclass) {
					return classnames[i].name;
				}
			}
			return "Unknown PCI Device";
    }
}

void scan_pci() {
    for (u8 i = 0; i < 16; i++) {
        for (u8 j = 0; j < 16; j++) {

        }
    }
}