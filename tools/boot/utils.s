; Utilities for Boot Loader


; print subroutine
;
; Inputs:
;   si = message to print
;
; Modified Registers:
;   ax, bx, si

print:
	mov ax, 0x0E0D ; AH=0Eh - print character; AL=0x0Dh - carriage return
	mov bx, 0x0007 ; BH=00h - page zero; BL=0x07 - white text, black background
.pr1:
	int 10h ; bios call to print character
	ss lodsb ; load a character from si to al
	test al, al ; a test to see if al is zero so we can stop the loop
	jnz .pr1
	ret


; read sector subroutine
; assumes 16-bit; will not work on 32-bit
;
; Inputs:
;   ax = sector number
;   bx = offset
;   es = segment
;
; Modified Registers:
;   ax, bx, cx, dx
;
; Memory Locations Read:
;   sc_b4_prt, sc_p_trk, heads, drive
;
; Memory Locations Written:
;   ES:BX through ES:BX+0x0200

read_disk:
	push ax ; when int 13h is executed, ah contains the error code
	push bx ; to make computation less confining, bx is retrieved before reading
	push dx
	pop dx
	xor dx, dx ; when dividing, dx:ax is treated as one value
	; convert from LBA to CHS
	add ax, [sc_b4_prt] ; ax does not account for boot sector, so this adjusts for it
	mov bx, [sc_p_trk] ; bx = sectors per track
	div bx ; sector / tracks = [ax]CHStrack R [dx](CHSsector-1)
	inc dx ; dx = CHSsector
	mov cl, dl ; for a 1.44 floppy dx !> 18, dx !> 0x0012 so dh=00 dl=CHSsector cl=CHSsector as needed for int 13
	xor dx, dx ; in preparation for division
	div word [heads] ; [ax]ABStrack / heads = [ax]CHStrack R [dx]head
	mov ch, al ; for a floppy al !> dx for floppy 0 or 1
	mov dh, dl ; so dh=00 dl=CHShead dh=CHShead as needed for int 13
	mov ah, 02h ; set the high bit for reading as needed by int13
	mov al, 01h ; read one sector (VERY INEFFICENTLY) for int13
	mov dl, [drive] ; set drive from memory loaded from bios
	pop bx ; retrieve bx for offset; es unmodified from entry
	int 0x13 ; call interrupt; sector is now read and placed in [ES:BX]
	pop ax ; return ax's proper value
	ret


; get file FAT cluster
;
; Inputs:
;   di = fat entry + 11
;
; Modified Registers:
;   ax, si

get_disk_read_information:
	xor ax, ax
	mov si, di
	add si, 0Fh ; skip data not needed, to get to fat sector
	lodsw ; load 16bit value in AX, ax=fat cluster
	ret


; read FAT from disk into memory
;
; Modified Registers:
;   bx, cx
;
; Memory Locations Read:
;   sc_b4_fat, sc_p_fat, sector_buffer_base

read_fat_into_memory:
	push ax ; save AX from computation
	mov ax, [sc_b4_fat] ; get sector to start reading at
	mov cx, [sc_p_fat] ; set loop to number of sectors in fat
	mov bx, sector_buffer_base ; set memory segment
.rfim1:
	push cx ; save cx
	call read_disk
	pop cx ; return cx
	inc ax ; adjust ax, and read again
	add bx, 0x0200 ; adjust bx for sector
	loop .rfim1 ; loop for number of sectors
	pop ax ; get ax
	ret


; get next cluster in chain

next_cluster_in_chain:
.nsic1:
	mov si, sector_buffer_base
	mov dx, ax
	mov cx, ax
	shr cx, 1
	add cx, ax ; add ax and cx which is file_sys_cluster * 1.5 = fat bytes
	add si, cx
	lodsw
	test dx, 01h
	jz .nsic2
	shr ax, 4
	jmp .nsic3
.nsic2:
	and ax, 0xFFF
.nsic3:
	cmp ax, 0xFF8
	jae .nsic4
	push ax
	push dx
	call disk_to_mem
	pop dx
	pop ax
	jmp .nsic1
.nsic4:
	ret


; read cluster into memory

disk_to_mem:
	call cluster_to_sector
	push cx
	xor bx, bx
	call read_disk
	mov bx, es
	add bx, 0x020
	mov es, bx
	xor bx, bx
	pop cx
	ret


; convert cluster number to sector number

cluster_to_sector:
	sub ax, 03h
	add ax, [first_data_sector]
	inc ax
	ret


; reboot subroutine

reboot:
	xor	ah, ah ; clear ah in order to read a character
	int	16h ; wait for input from keyboard
	int	19h ; reboot system
