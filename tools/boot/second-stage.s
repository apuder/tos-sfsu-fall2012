[BITS 16]

; This is the location in memory where the second-stage boot loader will be loaded
%define SECOND_STAGE_BASE 0x1000
%define SECOND_STAGE_SEG 0x0100

; This is the location in memory where further sectors will be read
%define sector_buffer_base 1800h
%define sector_buffer_seg 0180h

; This is the location in memory where the TOS kernel will be read
%define KERNEL_BASE 0x4000
%define KERNEL_SEG 0x0400

; These are the final destinations of various fields in the boot sector
%define first_data_sector 0800h ; temporary variable
%define MaxRootEntries 0811h ; maximum number of root directory entries
%define sc_p_clu 080Dh ; sectors per cluster (byte)
%define sc_b4_fat 080Eh ; reserved sectors (word)
%define fats 0810h ; number of FATs (byte)
%define dir_ent 0811h ; number of root directory entries (word)
%define sc_p_fat 0816h ; sectors per FAT (word)
%define sc_p_trk 0818h ; sectors per track (word)
%define heads 081Ah ; number of heads (word)
%define sc_b4_prt 081Ch ; sectors before partition (dword)
%define drive 0824h ; drive number (byte)

org SECOND_STAGE_BASE

begin:
	; Clear registers
	xor ax, ax ; to clear the ax register (the accepted way to clear the register)
	mov ds, ax ; to clear ds register
	mov es, ax ; to clear es register
	mov ss, ax ; to clear ss register
	mov dx, ax ; to clear dx register
	mov sp, SECOND_STAGE_BASE ; set stack pointer to below stage 2

read_part:
	; in order to read the fat file system,
	; portions of the information are contained in the boot sector
	mov dl, [drive] ; Drive number - not really important
	mov al, [fats] ; Number of FATs (my case 2)
	mul word [sc_p_fat] ; * Sectors per FAT (my case 18)
	add ax, [sc_b4_fat] ; + Sectors before FAT (my case 19)
	push ax ; preserving ax = logical sector of root directory

compute_max_root:
	mov ax, [MaxRootEntries] ;setting ax to max root entries
	shl ax, 5 ; multiplying bx by 32 (size of each directory entry)
	shr ax, 9 ; dividing by 512
	mov cx, ax
	pop ax
	mov [first_data_sector], ax
	add [first_data_sector], cx
	mov bx, sector_buffer_base
.cmr1:
	push cx
	call read_disk
	pop cx
	inc ax
	add bx, 0x200
	loop .cmr1 
	; looped to read cx sectors into sector_buffer_base
	mov cx, WORD [MaxRootEntries]
	; cx set for loop to compare filenames
	mov di, sector_buffer_base  
	; segment where disk was read to, needed for lodsb
.cmr2:
	push cx ; protecting CX from rep statement 
	mov cx, 0x000B  ; length of filename
	mov si, filename2 ; si = filename to compare to
	push di ; preserve di from increment cmpsb
	rep cmpsb
	je prepare_to_open_file ; jmp out of loop 
	pop di ; get DI prior to increment
	pop cx ; get previous CX for loop
	add di, 0x0020 ; length of entry
	loop .cmr2 ; do CX times
	jmp error

prepare_to_open_file:
	call get_disk_read_information ; ax = cluster number of start of file
	call read_fat_into_memory ; read FAT into sector_buffer_base
	push es
	push ax ; save fat cluster
	xor bx, bx ; clear BX for initial read of kernel
	add bx, (KERNEL_SEG - 0x0100)
	; this is an evil trick
	; instead of loading the ELF image somewhere and moving
	; the code and data segments to their final destinations,
	; since the code segment starts at 0x1000 in the file,
	; we just load the ELF image at 0x3000, and the code segment
	; ends up at 0x4000 where it's supposed to be XD
	; 0x3000 is safe to overwrite, since it's just the end
	; of the second FAT, which is not used anyway
	mov es, bx
	xor bx, bx
	call disk_to_mem
	pop ax ; get fat cluster for further information
	call next_cluster_in_chain
	pop es

	; the data segment is not necessarily offset the same way,
	; so let's move it to its correct location
	; 0x3058 is the offset of the data segment from 0x3000
	; 0x3060 is where it should be in memory
	; 0x3064 is size of data segment in ELF file
	push es
	push ds
	mov eax, [KERNEL_BASE - 0x1000 + 0x58] ; eax = offset
	add eax, (KERNEL_BASE - 0x1000) ; eax = current location of data segment
	mov ebx, [KERNEL_BASE - 0x1000 + 0x60] ; ebx = correct location of data segment
	cmp eax, ebx
	je donemovingdata
	mov edx, [KERNEL_BASE - 0x1000 + 0x64] ; edx = length of data segment
	je donemovingdata
	; source segment and offset
	mov ecx, eax
	shr ecx, 4
	mov ds, cx
	and eax, 0x0F
	add eax, edx
	dec eax
	mov esi, eax
	; destination segment and offset
	mov ecx, ebx
	shr ecx, 4
	mov es, cx
	and ebx, 0x0F
	add ebx, edx
	dec ebx
	mov edi, ebx
	; copy
	mov ecx, edx
	std
	rep movsb
	cld
donemovingdata:
	pop ds
	pop es

	; we still need to zero out the part of the data area that's not in the file
	; 0x3060 is address of data segment
	; 0x3064 is size of data segment in ELF file (how many bytes were actually initialized)
	; 0x3068 is size of data segment in memory (how many bytes need to be initialized)
	push es
	mov ecx, [KERNEL_BASE - 0x1000 + 0x68] ; ecx = size of data in memory
	mov ebx, [KERNEL_BASE - 0x1000 + 0x64] ; ebx = size of data on disk
	sub ecx, ebx ; ecx = size of data in memory - size of data on disk = number of bytes to clear
	mov eax, [KERNEL_BASE - 0x1000 + 0x60] ; eax = address of data segment
	mov ebx, [KERNEL_BASE - 0x1000 + 0x64] ; ebx = size of data on disk
	add eax, ebx ; eax = address of data segment + size of data on disk = first byte to clear
	mov ebx, eax ; ebx = first byte to clear
	and ebx, 0x0F ; ebx = first byte to clear & 0x0F = starting offset
	shr eax, 4 ; eax = first byte to clear >> 4 = segment to clear
	mov es, ax
	mov edi, ebx
	xor eax, eax
	rep stosb
	pop es

	; configure serial port
	; information available at:
	; http://www.clipx.net/ng/bios/ng2231e.php
	xor ax, ax
	mov al, 10100111b ; 2400 baud, no parity, 2 start bits?, 8 data bits
	xor dx, dx
	int 14h

	; send dummy commands
	call send_dummy_command
	call send_dummy_command
	call send_dummy_command    
	call send_dummy_command
	call send_dummy_command

	; set train speed to 0
	mov al, 'L'
	call send_char_com1
	mov al, '2'
	call send_char_com1
	mov al, '0'
	call send_char_com1
	mov al, 'S'
	call send_char_com1
	mov al, '0'
	call send_char_com1
	mov al, 015q
	call send_char_com1
	call delay

	;set zamboni speed to 5
	mov al, 'L'
	call send_char_com1
	mov al, '2'
	call send_char_com1
	mov al, '4'
	call send_char_com1
	mov al, 'S'
	call send_char_com1
	mov al, '6'
	call send_char_com1
	mov al, 015q
	call send_char_com1
	call delay

	; reset disk controller
	xor ax, ax
	int 13h

	; disable interrupts because we'd like to get away with some stuff
	cli

	; load GDT pointer into the GTD register!
	lgdt [gdt_ptr]

	; switch to protected mode!
	smsw ax
	or ax, 1
	lmsw ax

	; clear pre-fetch queue and start the kernel!
	jmp flush

flush:
	db 066h, 0eah ; 066h - instruction size override; 0eah - far jump
	dw (KERNEL_BASE & 0xFFFF), (KERNEL_BASE >> 16) ; offset to TOS (lo then hi)
	dw 8 ; code selector

; delay loop for serial communications

delay:
	mov ah, 0
	int 1Ah ; get current time in dx
	add dx, 4 ; go 4 ticks into the future
	push dx
.delay1:
	mov ah, 0
	int 1Ah ; get current time
	pop ax
	push ax
	cmp ax, dx ; compare it with future value
	jnz .delay1 ; if we have not yet arrived in the future, loop again
	pop ax
	ret

; send dummy command to com1

send_dummy_command:
	mov al, 'M'
	call send_char_com1
	mov al, 015q
	call send_char_com1
	call delay
	ret

; send al to com1
; information at:
; http://www.clipx.net/ng/bios/ng22614.php

send_char_com1:
	mov ah, 01h
	mov dx, 0
	int 14h
	ret

; global descriptor tables for segments - do not touch!

align 8, db 0

real_gdt:
	;; First 8 bytes are all zero
	;; null segment descriptor
	db 0x00 ; segment size
	db 0x00 ; segment size
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x00 ; presence, ring, system, type
	db 0x00 ; granularity, size, reserved, segment size
	db 0x00 ; base address
	;; OS text descriptor
	db 0xff ; segment size
	db 0xff ; segment size
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x9a ; 9 = present, ring 0, system; a = executable, readable
	db 0xcf ; c = coarse-grain, 32-bit; f = segment size
	db 0x00 ; base address
	;; OS Data Descriptor
	db 0xff ; segment size
	db 0xff ; segment size
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x92 ; 9 = present, ring 0, system; 2 = read/write
	db 0xcf ; c = coarse-grain, 32-bit; f = segment size
	db 0x00 ; base address
	;; Global data descriptor
	db 0xff ; segment size
	db 0xff ; segment size
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x00 ; base address
	db 0x92 ; 9 = present, ring 0, system; 2 = read/write
	db 0xcf ; c = coarse-grain, 32-bit; f = segment size
	db 0x00 ; base address

gdt_ptr:
	dw 0x001f ; upper 16 bits of all selector limits
	dw real_gdt ; address of the first gdt entry (low word)
	dw 0x0000 ; address of the first gdt entry (high word)

error:
	mov si, errmsg2
	call print
	jmp reboot

%include "utils.s"

; error message for disk failure
errmsg2 db 10, "Cannot find TOS.IMG", 13, 10, 0

; name of TOS kernel
filename2 db "TOS     IMG"
