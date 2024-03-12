
read_lba_blocks:
	mov dl,[drive_number]
	mov ah,0x42
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
	dw 0x80
.transfer_buffer_offset:
	dw 0x0000
.transfer_buffer_segment:
	dw 0x07e0
.LBA_address:
	dq 1

	dq 0



