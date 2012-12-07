[BITS 16]

; This is the location in memory of the first-stage boot loader after it is read from the boot sector
%define FIRST_STAGE_BASE 0x7C00
%define FIRST_STAGE_SEG 0x07C0

; This is the location in memory where the FAT information from the boot sector will be stored
; I tried using 0400h and Bochs decided to act funny.
%define boot_sector_base 0800h
%define boot_sector_seg 0080h

; This is the location in memory where the second-stage boot loader will be loaded
%define SECOND_STAGE_BASE 0x1000
%define SECOND_STAGE_SEG 0x0100

; This is the location in memory where further sectors will be read
%define sector_buffer_base 1800h
%define sector_buffer_seg 0180h

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

org FIRST_STAGE_BASE

entry:
	; The following code fills the short space between bytes 0-0Eh
	; The jmp is to clear the boot sector
	; The nop is to fill space
	jmp short begin
	nop

	; Zeros to fill the FAT information
	times 0x3B db 0

begin:
	; Copy the boot sector from its initial location to its final location
	mov ax, FIRST_STAGE_SEG
	mov ds, ax
	mov ax, boot_sector_seg
	mov es, ax
	mov cx, 256
	xor si, si
	xor di, di
	rep movsw

	; Clear registers
	xor ax, ax ; to clear the ax register (the accepted way to clear the register)
	mov ds, ax ; to clear ds register
	mov es, ax ; to clear es register
	mov ss, ax ; to clear ss register
	mov dx, ax ; to clear dx register
	mov sp, FIRST_STAGE_BASE ; set stack pointer to below stage 1
	
root_dir_read:
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
	mov si, filename ; si = filename to compare to
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
	add bx, SECOND_STAGE_SEG
	mov es, bx
	xor bx, bx
	call disk_to_mem
	pop ax ; get fat cluster for further information
	call next_cluster_in_chain
	pop es

jmp_to_stage2:
	jmp SECOND_STAGE_BASE

error:
	mov si, errmsg
	call print
	jmp reboot

%include "utils.s"

; error message for disk failure
errmsg db 10, "Cannot find STAGE2.BIN", 13, 10, 0

; fill remaining space with zeroes
size equ $ - entry
%if size+11+2 > 512
	%error "Size is too large."
%endif
times (512 - size - 11 - 2) db 0

; name of second-stage bootloader
filename db "STAGE2  BIN"

; boot signature
db 0x55, 0xAA
