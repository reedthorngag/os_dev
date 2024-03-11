hex_characters: db '0123456789abcdef'

; number to print in bx
; preserves all registers
print_hex:
	push ax
	push bx
	push dx
	mov ax,bx
	mov bx,0x1000
	xor dx,dx	; this is necessery for some reason (div instruction dies without it)

.hex_print_loop:
	div bx		; divide ax by bx, quotent in ax, remainder in dx
	push bx
	mov bx,ax
	mov al,[hex_characters+bx]
	mov bh,0
	mov ah,0x0e
	int 0x10

	pop ax
	push dx
	xor dx,dx
	mov bx,0x10
	div bx
	mov bx,ax
	pop ax
	cmp bx,0x00
	jne .hex_print_loop

.end:
	mov bh,0
	mov ax,0x0e20
	int 0x10		; add a space at the end for nice output

	pop dx
	pop bx
	pop ax
	ret

hang:
    cli
    hlt


print_str:
    mov ah,0x0e
.loop:
    lodsb
    cmp al,0
    je .end
    int 0x10
    jmp .loop
.end:
    ret


; pauses until a key is pressed
; preserves all registers
global pause
pause:
    push ax
.wait_for_key_loop:
    hlt

    mov ah,0x01
    int 0x16
    jz .wait_for_key_loop

    pop ax
    ret

