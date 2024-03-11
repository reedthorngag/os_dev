
read_lba_blocks:

	mov dl,[drive_number]
	mov ah,0x42
	;mov si,disk_address_packet
	int 0x13
	jc .failed

	ret

.failed:
	mov bx,ax
	call print_hex
	call hang



; lba disk address packet
disk_address_packet:
	db 0x10
	db 0x00
.number_of_blocks:
	dw 0x0004
.transfer_buffer_offset:
	dw 0x7e00
.transfer_buffer_segment:
	dw 0x0000
.LBA_address:
	dq 1
	dq 0

disk_address_packet_2:
	db 0x10
	db 0x00
.number_of_blocks:
	dw 0x00080
.transfer_buffer_offset:
	dw 0x0000
.transfer_buffer_segment:
	dw 0x1000
.LBA_address:
	dq 0x2
	dq 0

