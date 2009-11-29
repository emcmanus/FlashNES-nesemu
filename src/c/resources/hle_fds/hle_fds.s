;ЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛ
; HLE FDS BIOS 6502 implementation
;ЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛЛ

.MEMORYMAP
SLOTSIZE $2000
DEFAULTSLOT 0
SLOT 0 $E000
.ENDME

.ROMBANKMAP
BANKSTOTAL 1
BANKSIZE $2000
BANKS 1
.ENDRO

.BANK 0 SLOT 0

.ORG $0000

	;some data stolen from fds bios...needs to be updated with my own

	.db	$38,$4C,$C6,$C6,$C6,$64,$38,$00,$18,$38,$18,$18,$18,$18,$7E,$00
	.db	$7C,$C6,$0E,$3C,$78,$E0,$FE,$00,$7E,$0C,$18,$3C,$06,$C6,$7C,$00
	.db	$1C,$3C,$6C,$CC,$FE,$0C,$0C,$00,$FC,$C0,$FC,$06,$06,$C6,$7C,$00
	.db	$3C,$60,$C0,$FC,$C6,$C6,$7C,$00,$FE,$C6,$0C,$18,$30,$30,$30,$00
	.db	$7C,$C6,$C6,$7C,$C6,$C6,$7C,$00,$7C,$C6,$C6,$7E,$06,$0C,$78,$00
	.db	$38,$6C,$C6,$C6,$FE,$C6,$C6,$00,$FC,$C6,$C6,$FC,$C6,$C6,$FC,$00
	.db	$3C,$66,$C0,$C0,$C0,$66,$3C,$00,$F8,$CC,$C6,$C6,$C6,$CC,$F8,$00
	.db	$FE,$C0,$C0,$FC,$C0,$C0,$FE,$00,$FE,$C0,$C0,$FC,$C0,$C0,$C0,$00
	.db	$3E,$60,$C0,$DE,$C6,$66,$7E,$00,$C6,$C6,$C6,$FE,$C6,$C6,$C6,$00
	.db	$7E,$18,$18,$18,$18,$18,$7E,$00,$1E,$06,$06,$06,$C6,$C6,$7C,$00
	.db	$C6,$CC,$D8,$F0,$F8,$DC,$CE,$00,$60,$60,$60,$60,$60,$60,$7E,$00
	.db	$C6,$EE,$FE,$FE,$D6,$C6,$C6,$00,$C6,$E6,$F6,$FE,$DE,$CE,$C6,$00
	.db	$7C,$C6,$C6,$C6,$C6,$C6,$7C,$00,$FC,$C6,$C6,$C6,$FC,$C0,$C0,$00
	.db	$7C,$C6,$C6,$C6,$DE,$CC,$7A,$00,$FC,$C6,$C6,$CE,$F8,$DC,$CE,$00
	.db	$78,$CC,$C0,$7C,$06,$C6,$7C,$00,$7E,$18,$18,$18,$18,$18,$18,$00
	.db	$C6,$C6,$C6,$C6,$C6,$C6,$7C,$00,$C6,$C6,$C6,$EE,$7C,$38,$10,$00
	.db	$C6,$C6,$D6,$FE,$FE,$EE,$C6,$00,$C6,$EE,$7C,$38,$7C,$EE,$C6,$00
	.db	$66,$66,$66,$3C,$18,$18,$18,$00,$FE,$0E,$1C,$38,$70,$E0,$FE,$00
	.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$30,$30,$20,$00
	.db	$00,$00,$00,$00,$30,$30,$00,$00,$00,$00,$00,$00,$6C,$6C,$08,$00
	.db	$38,$44,$BA,$AA,$B2,$AA,$44,$38

.ORG $0149
;131 clock cycle delay
Delay131:
	PHA
	LDA #$16
	SEC
DelayLoop:
	SBC #$01
	BCS DelayLoop
	PLA
	RTS

;+-----+
;|Delay|
;+-----+
;Entry point:	$E153
;Y on call:	delay in milliseconds
;description:	a time delay generator.

.ORG $0153
;millisecond delay timer. Delay	in clock cycles	is: 1790*Y+5.
MilSecTimer:
	LDX $00
	LDX #$FE
MilSecLoop:
	NOP
	DEX
	BNE MilSecLoop
	CMP $00
	DEY
	BNE MilSecTimer
	RTS

.ORG $0161
;disable playfield & objects
DisPfOBJ:
	LDA $FE
	AND #$e7
Write2001:
	STA $FE
	STA $2001;	[NES] PPU setup	#2
	RTS

.ORG $016B
;enable playfield & objects
EnPfOBJ:
	LDA $FE
	ORA #$18
	BNE Write2001

.ORG $0171
;disable objects
DisOBJs:
	LDA $FE
	AND #$ef
	JMP Write2001

.ORG $0178
;enable objects
EnOBJs:
	LDA $FE
	ORA #$10
	BNE Write2001

.ORG $017E
;disable playfield
DisPF:
	LDA $FE
	AND #$f7
	JMP Write2001

.ORG $0185
;enable playfield
EnPF:
	LDA $FE
	ORA #$08
	BNE Write2001

.ORG $01B2
;----------------------------------------------------------------------------
;wait for VINT
VINTwait:
	PHA;	save A
	LDA $0100
	PHA;	save old NMI pgm ctrl byte
	LDA #$00
	STA $0100;	set NMI pgm ctrl byte to 0

;enable VINT
	LDA $FF
	ORA #$80
	STA $FF
	STA $2000;	[NES] PPU setup	#1

;infinite loop
infiniteloop:
	BNE infiniteloop

;+----------+
;|Load Files|
;+----------+
;Entry point:	$E1F8
;RETaddr:	pointer to DiskID
;RETaddr+2:	pointer to LoadList
;A on return:	error code
;Y on return:	count of files actually found
;Description:	Loads files specified by DiskID into memory from disk. Load 
;addresses are decided by the file's header.

.ORG $01F8
loadfiles:
	lda	#0				;load function number
	sta	$4028			;store into hle function number register
	sta	$4029			;execute hle function
	rts					;return control to program

;+-----------+
;|Append File|
;+-----------+
;entry point:	$E237
;RETaddr:	pointer to DiskID
;RETaddr+2:	pointer to FileHeader
;A on return:	error code
;special error:	#$26 if verification stage fails
;Description:	appends the file data given by DiskID to the disk. This means 
;that the file is tacked onto the end of the disk, and the disk file count is 
;incremented. The file is then read back to verify the write. If an error 
;occurs during verification, the disk's file count is decremented (logically 
;hiding the written file).

.ORG $0237
	lda #$FF

;+----------+
;|Write File|
;+----------+
;Entry point:	$E239
;RETaddr:	pointer to DiskID
;RETaddr+2:	pointer to FileHeader
;A on call:	file sequence # for file to write
;A on return:	error code
;special error:	#$26 if verification fails
;Description:	same as "Append File", but instead of writing the file to the 
;end of the disk, A specifies the sequential position on the disk to write 
;the file (0 is the first). This also has the effect of setting the disk's 
;file count to the A value, therefore logically hiding any other files that 
;may reside after the written one.

.ORG $0239
	;call our hle writefile function
	lda	#1
	sta	$4028
	sta	$4029
	;return control to program
	rts

;+----------------+
;|Check File count|
;+----------------+
;Entry point:	$E2B7
;RETaddr:	pointer to DiskID
;A on call:	number to set file count to
;A on return:	error code
;Special error:	#$31 if A is less than the disk's file count
;Description:	reads in disk's file count, compares it to A, then sets the 
;disk's file count to A.

.ORG $02B7
	jmp checkfilecount

;+-----------------+
;|Adjust File count|
;+-----------------+
;Entry point:	$E2BB
;RETaddr:	pointer to DiskID
;A on call:	number to reduce current file count by
;A on return:	error code
;Special error:	#$31 if A is less than the disk's file count
;Description:	reads in disk's file count, decrements it by A, then writes the 
;new value back.

.ORG $02BB
	jmp adjfilecount

;+-----------------------+
;|Set File count (alt. 1)|
;+-----------------------+
;Entry point:	$E305
;RETaddr:	pointer to DiskID
;A on call:	number to set file count to
;A on return:	error code
;Description:	sets the disk's file count to A.

.ORG $0305
	jmp setfilecount1

;+-----------------------+
;|Set File count (alt. 2)|
;+-----------------------+
;entry point:	$E301
;RETaddr:	pointer to DiskID
;A on call:	number to set file count to minus 1
;A on return:	error code
;Description:	sets the disk's file count to A+1.

.ORG $0301
	jmp setfilecount2

;+--------------------+
;|Get Disk Information|
;+--------------------+
;Entry point:	$E32A
;RETaddr:	pointer to DiskInfo
;A on return:	error code
;Description:	fills DiskInfo up with data read off the current disk.

.ORG $032A
	jmp getdiskinfo

;+----------+
;|WaitForRdy|
;+----------+
;Entry Point:	$E64D
;Description:	used to initilalize the disk drive for data transfers.
;Logic:
;	[$4025] = 0010x110;	stop motor (if it was already running)
;	Delay(512)
;	[$4025] = 0010x111;	no effect
;	[$4025] = 0010x101;	scan disk
;	Delay(150);		allow pleanty of time for motor to turn on
;	[$4026] = 1xxxxxxx;	enable battery checking
;	if ([$4033] = 0xxxxxxx);check battery good bit
;	 then Error($02)
;	[$4025] = 0010x110;	stop motor again
;	[$4025] = 0010x111;	no effect
;	[$4025] = 0010x101;	scan disk again
;	repeat
;	 if ([$4032] = xxxxxxx1)
;	  then Error($01);	constantly examine disk set
;	until ([$4032] = xxxxxx0x);wait for ready flag to activate
;	return

.ORG $064D
	jmp xferbyte

;+------------+
;|CheckBlkType|
;+------------+
;Entry point:	$E68F
;A on call:	expected block type
;Description:	compares the first byte in a new data block to the one passed 
;in A. Generates an error if test fails.
;Logic:
;	Delay(5);		advance 5 ms into GAP period
;	[$4025] = x1xxxxxx;	wait for block start mark to appear
;	[$0101] = 01000000;	set IRQ mode to disk byte transfer
;	[$4025] = 1xxxxxxx;	enable disk transfer IRQs
;	if (XferByte <> BlockType);test first byte in block
;	 then Error($21+BlockType)
;	return

.ORG $068F
	jmp checkblktype

;+------------+
;|WriteBlkType|
;+------------+
;Entry point:	$E6B0
;A on call:	block type to create
;Description:	creates a new data block, and writes the passed block type to 
;it as the first byte.
;Logic:
;	[$4025] = 00x0x0xx;	set transfer direction to write
;	Delay(10);		create a 10 ms GAP period
;	[$4024] = 00000000;	zero out write data register
;	[$4025] = 01x0x0xx;	start writing data via $4024 to disk
;	[$0101] = 01000000;	set IRQ mode to disk byte transfer
;	[$4025] = 1xxxxxxx;	enable disk transfer IRQs
;	XferByte($80);		write out block start mark
;	XferByte(BlockType);	write out specified block type
;	return

.ORG $06B0
	jmp writeblktype

;+------------+
;|EndOfBlkRead|
;+------------+
;Entry point:	$E706
;Description:	called when all (no more and no less) data from a block has 
;been read in. Tests CRC bit to verify data block's integrity.
;Logic:
;	XferByte;		dummy read in CRC byte 1
;	if ([$4030] = x1xxxxxx)
;	 then Error($28)
;	[$4025] = xxx1xxxx;	activate process CRC bit
;	XferByte;		dummy read in CRC byte 2
;	if ([$4030] = xxx1xxxx);test (CRC accumulator = zero) status
;	 then Error($27)
;	[$4025] = 00x0x1xx;	disable further disk IRQ's, etc.
;	if ([$4032] = xxxxxxx1);check disk set status
;	 then Error($01)
;	return

.ORG $0706
	jmp endofblkread

;+-------------+
;|EndOfBlkWrite|
;+-------------+
;Entry point:	$E729
;Description:	called when all data has been written to a block. Writes out 
;16-bit CRC value generated by the FDS hardware as last 2 bytes of file.
;Logic:
;	XferByte;		pipes last byte written into shift registers
;	if ([$4030] = x1xxxxxx)
;	 then Error($29)
;	[$4025] = xxx1xxxx;	activate process CRC bit
;	Delay(0.5);		contents of CRC register written for 0.5 ms
;	if ([$4032] = xxxxxx1x);check drive ready status
;	 then Error($30);	disk head reached end of disk
;	[$4025] = 00x0x1xx;	disable further disk IRQ's, etc.
;	if ([$4032] = xxxxxxx1);check disk set status
;	 then Error($01)
;	return

.ORG $0729
	jmp endofblkwrite

xferdone:
	lda	#$14
	sta	$4028
	sta	$4029
	cli
	rts

.ORG $0778
	jmp	xferdone

;+-----+
;|Error|
;+-----+
;Entry point:	$E781
;X on call:	error code
;A,X on return:	error code
;Description:	restores stack to a prior state before returning, and 
;terminates data transfer.
;Logic:
;	S = [$04];		restore stack frame to a previous state
;	[$4025] = 0010x11x;	disable scan disk bit
;	A = X = ErrorCode
;	return

.ORG $0781
	jmp error

;+--------+
;|XferByte|
;+--------+
;Entry point:	$E7A3
;A on call:	byte to write to disk
;A on return:	byte read from disk
;description:	Waits for an IRQ to occur, then reads [$4031] & writes [$4024]. 
;Only the current status of the write flag ($4025.2) determines if data is 
;actually written to disk, or if valid data is read off the disk.
;Logic:
;	(Wait for IRQ occurence)
;	temp=[$4031]
;	[$4024]=A
;	A=temp
;	return

.ORG $07A3
	ldx	$4031
	sta	$4024
	txa
	rts

;ppu data processor
.ORG $07BB
	lda	#$D
	sta	$4028
	sta	$4029
	rts

;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;fetches hardcoded 16-bit value	after second return address into [$00] & [$01]
;that return address is then incremented by 2.
.ORG $0844
GetHCparam:
	TSX
	LDA $0103,X
	STA $05
	LDA $0104,X
	STA $06
	LDY #$01
	LDA ($05),Y
	STA $00
	INY
	LDA ($05),Y
	STA $01
	CLC
	LDA #$02
	ADC $05
	STA $0103,X
	LDA #$00
	ADC $06
	STA $0104,X
	RTS

.ORG $086A
	LDA $FF
	AND #$fb
	STA $2000;	[NES] PPU setup	#1
	STA $FF
	LDX $2002;	[NES] PPU status
	LDY #$00
	BEQ E8A5
E87A:
	PHA
	STA $2006;	[NES] VRAM address select
	INY
	LDA $0302,Y
	STA $2006;	[NES] VRAM address select
	INY
	LDX $0302,Y
E889:
	INY
	LDA $0302,Y
	STA $2007;	[NES] VRAM data
	DEX
	BNE E889
	PLA
	CMP #$3f
	BNE E8A4
	STA $2006;	[NES] VRAM address select
	STX.W $2006;	[NES] VRAM address select
	STX.W $2006;	[NES] VRAM address select
	STX.W $2006;	[NES] VRAM address select
E8A4:
	INY
E8A5:
	LDA $0302,Y
	BPL E87A
	STA $0302
	LDA #$00
	STA $0301
	RTS

.ORG $08D2
	STA $02
	STX $03
	STY $04
	JSR GetHCparam
	LDY #$ff
	LDA #$01
	BNE E8F6
	STA $03
	STX $02
	JSR GetHCparam
	LDY #$00
	LDA ($00),Y
	AND #$0f
	STA $04
	LDA ($00),Y
	LSR A
	LSR A
	LSR A
	LSR A
E8F6	STA $05
	LDX $0301
E8FB	LDA $03
	STA $0302,X
	JSR E93C
	LDA $02
	STA $0302,X
	JSR E93C
	LDA $04
	STA $06
	STA $0302,X
E912	JSR E93C
	INY
	LDA ($00),Y
	STA $0302,X
	DEC $06
	BNE E912
	JSR E93C
	STX.W $0301
	CLC
	LDA #$20
	ADC $02
	STA $02
	LDA #$00
	ADC $03
	STA $03
	DEC $05
	BNE E8FB
	LDA #$ff
	STA $0302,X
	RTS
E93C:
	INX
	CPX $0300
	BCC E94E
	LDX $0301
	LDA #$ff
	STA $0302,X
	PLA
	PLA
	LDA #$01
E94E	RTS

.ORG $0970
	LDA $00
	STA $0300,Y
	INY
	LDA $01
	STA $0300,Y
	SEC
	RTS

.ORG $097D
	LDA #$08
	STA $00
	LDA $02
	ASL A
	ROL $00
	ASL A
	ROL $00
	AND #$e0
	STA $01
	LDA $03
	LSR A
	LSR A
	LSR A
	ORA $01
	STA $01
	RTS


;random number generator
.ORG $09B1
	lda	#$10
	sta	$4028
	sta	$4029
	rts

.ORG $09C8
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;sprite dma

	LDA #$00
	STA $2003;	[NES] SPR-RAM address select
	LDA #$02
	STA $4014;	[NES] Sprite DMA trigger
	RTS

.ORG $09D3
	STX $00
	DEC $00,X
	BPL E9DE
	LDA #$09
	STA $00,X
	TYA
E9DE:
	TAX
E9DF:
	LDA $00,X
	BEQ E9E5
	DEC $00,X
E9E5:
	DEX
	CPX $00
	BNE E9DF
	RTS

.ORG $09EB
ReadCtrlrs:
	lda	#$12
	sta	$4028
	sta	$4029
	rts


;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;controller OR function

;[$F5]|=[$00]
;[$F6]|=[$01]

.ORG $0A0D
ORctrlrRead:
	LDA $00
	ORA $F5
	STA $F5
	LDA $01
	ORA $F6
	STA $F6
	RTS

.ORG $0A1A
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;get controller	status

;- returns status of controller	buttons in [$F7] (CI) and [$F8]	(CII)
;- returns which new buttons have been pressed since last update in
;  [$F5] (CI) and [$F6] (CII)

GetCtrlrSts:
	JSR ReadCtrlrs
	BEQ GetCtrlrStsSkip;	always branches	because ReadCtrlrs sets zero flag
	JSR ReadCtrlrs;	this instruction is not used
	JSR ORctrlrRead;this instruction is not used
GetCtrlrStsSkip:
	LDX #$01
GetCtrlrStsLoop:
	LDA $F5,X
	TAY
	EOR $F7,X
	AND $F5,X
	STA $F5,X
	STY $F7,X
	DEX
	BPL GetCtrlrStsLoop
	RTS

;ReadCntrlrs
.ORG $0A36
	JSR ReadCtrlrs
a39:
	LDY $F5
	LDA $F6
	PHA
	JSR ReadCtrlrs
	PLA
	CMP $F6
	BNE a39
	CPY $F5
	BNE a39
	BEQ GetCtrlrStsSkip
	JSR ReadCtrlrs
	JSR ORctrlrRead
a52:
	LDY $F5
	LDA $F6
	PHA
	JSR ReadCtrlrs
	JSR ORctrlrRead
	PLA
	CMP $F6
	BNE a52
	CPY $F5
	BNE a52
	BEQ GetCtrlrStsSkip
	JSR ReadCtrlrs
	LDA $00
	STA $F7
	LDA $01
	STA $F8
	LDX #$03
readbpl:
	LDA $F5,X
	TAY
	EOR $F1,X
	AND $F5,X
	STA $F5,X
	STY $F1,X
	DEX
	BPL readbpl
	RTS

.ORG $0A84
VRAMfill:
	pha
	lda	#$F
	sta	$4028
	pla
	sta	$4029
	rts

.ORG $0AD2
MemFill:
	pha
	lda	#$E
	sta	$4028
	pla
	sta	$4029
	rts

.ORG $0AEA
	;hle ppu restore command
	lda	#$C
	;write command register
	sta	$4028
	;write execute command register
	sta	$4029
	;return
	rts

.ORG $0AFD
	ASL A
	TAY
	INY
	PLA
	STA $00
	PLA
	STA $01
	LDA ($00),Y
	TAX
	INY
	LDA ($00),Y
	STA $01
	STX $00
	JMP ($0000)

.ORG $0BAF
	pha
	lda	#$11
	sta	$4028
	pla
	sta	$4029
	rts

;ripped directly from fds bios, unknown function...
.ORG $0C21
EC21:
	RTS
	LDY #$0b
	LDA ($00),Y
	STA $02
	LDA #$02
	STA $03
	DEY
	LDA ($00),Y
	LSR A
	LSR A
	LSR A
	LSR A
	BEQ EC21
	STA $04
	STA $0C
	LDA ($00),Y
	AND #$0f
	BEQ EC21
	STA $05
	LDY #$01
	LDA ($00),Y
	TAX
	DEY
	LDA ($00),Y
	BEQ EC4F
	BPL EC21
	LDX #$f4
EC4F:
	STX $08
	LDY #$08
	LDA ($00),Y
	LSR A
	AND #$08
	BEQ EC5C
	LDA #$80
EC5C:
	ROR A
	STA $09
	INY
	LDA ($00),Y
	AND #$23
	ORA $09
	STA $09
	LDY #$03
	LDA ($00),Y
	STA $0A
	LDA $05
	STA $07
	LDY #$00
	STY $0B
EC76:
	LDA $04
	STA $06
	LDX $08
EC7C:
	TXA
	STA ($02),Y
	CMP #$f4
	BEQ EC87
	CLC
	ADC #$08
	TAX
EC87:
	INY
	INY
	LDA $09
	STA ($02),Y
	INY
	LDA $0A
	STA ($02),Y
	INY
	INC $0B
	DEC $06
	BNE EC7C
	LDA $0A
	CLC
	ADC #$08
	STA $0A
	DEC $07
	BNE EC76
	LDY #$07
	LDA ($00),Y
	STA $07
	DEY
	LDA ($00),Y
	STA $08
	LDA #$00
	STA $0A
	CLC
	LDX $0B
	DEY
ECB7:
	LDA ($00),Y
	CLC
	ADC $07
	STA $07
	LDA #$00
	ADC $08
	STA $08
	DEX
	BNE ECB7
	INC $02
	LDY #$00
	LDA $08
	BNE ECD3
	DEC $0A
	LDY $07
ECD3	BIT $09
	BMI ECF5
	BVS ECF7
ECD9	LDA ($07),Y
	BIT $0A
	BPL ECE0
	TYA
ECE0	STA ($02,X)
	DEY
	BIT $09
	BMI ECE9
	INY
	INY
ECE9	LDA #$04
	CLC
	ADC $02
	STA $02
	DEC $0B
	BNE ECD9
	RTS
ECF5:
	BVC ED09
ECF7:
	TYA
	CLC
	ADC $0B
	TAY
	DEY
	BIT $09
	BMI ECD9
	LDA #$ff
	EOR $0C
	STA $0C
	INC $0C
ED09:
	TYA
	CLC
	ADC $0C
	TAY
	LDA $04
	STA $06
ED12:
	DEY
	BIT $09
	BMI ED19
	INY
	INY
ED19:
	LDA ($07),Y
	BIT $0A
	BPL ED20
	TYA
ED20:
	STA ($02,X)
	LDA #$04
	CLC
	ADC $02
	STA $02
	DEC $06
	BNE ED12
	TYA
	CLC
	ADC $0C
	TAY
	DEC $05
	BNE ED09
	RTS


.ORG $0E24

reset:
	;disable irq and clear decimal mode bit
	sei
	cld

	;init stack
	ldx #$FF
	txs

	;here we can do a fancy intro...
	jsr bios_intro

	;do boot (load boot files)
	lda #0
	jsr loadfiles
	.dw $FF00,$FF00

	;initialize bios vars
	lda	#$80
	sta $101
	lda	#$C0
	sta $100

	lda	#$80
	sta $FF
	lda #$06
	sta $FE
	lda #0
	sta $FD
	sta $FC
	sta $FB
	lda #$2E
	sta $FA
	lda #$FF
	sta $F9

	lda #$AC
	sta $102
	lda #$35
	sta $103

	;enable interrupts
	cli

	;transfer control to the disk program
	jmp ($DFFC)

	;loop forever
loop:
	jmp	loop

.ORG $1000


adjfilecount:

	;call our hle adjfilecount function
	lda	#2
	sta	$4028
	sta	$4029
	
	;return control to program
	rts

checkfilecount:

	;call our hle checkfilecount function
	lda	#3
	sta	$4028
	sta	$4029
	
	;return control to program
	rts

getdiskinfo:

	;call our hle getdiskinfo function
	lda	#4
	sta	$4028
	sta	$4029
	
	;return control to program
	rts

setfilecount1:

	;call our hle setfilecount1 function
	lda	#5
	sta	$4028
	sta	$4029
	
	;return control to program
	rts

setfilecount2:

	;call our hle setfilecount2 function
	lda	#6
	sta	$4028
	sta	$4029
	
	;return control to program
	rts

xferbyte:

	;xferbyte writes the acumulator to disk if in write mode ($4025.2==0)
	;or reads from the disk back into accumulator ($4025.2==1)
	
	;read the read/write flag
	lda	#7
	sta	$4028
	sta	$4029
	
	;determine if we read or write
	cpx #0
	beq	xferbyte_write

xferbyte_read:
	lda	#1
	sta	$4028
	sta	$4029
	rts

xferbyte_write:
	lda	#7
	sta	$4028
	sta	$4029
	rts

checkblktype:

	;call our hle checkblktype function
	lda	#8
	sta	$4028
	sta	$4029

	rts

writeblktype:

	;call our hle writeblktype function
	lda	#9
	sta	$4028
	sta	$4029

	rts

endofblkread:

	;call our hle endofblkread function
	lda	#$A
	sta	$4028
	sta	$4029

	rts

endofblkwrite:

	;call our hle endofblkwrite function
	lda	#$B
	sta	$4028
	sta	$4029

	rts

error:

	;call our hle error function
	lda	#$13
	sta	$4028
	sta	$4029

	rts

readctrl2:
	LDX $FB
	INX
	TXA
	STA $4016
	DEX
	TXA
	STA $4016
	LDX #$08
ReadCtrlrsLoop:
	LDA $4016;	[NES] Joypad & I/O port for port #1
	LSR A
	ROL $F5
	LSR A
	ROL $00
	LDA $4017;	[NES] Joypad & I/O port for port #2
	LSR A
	ROL $F6
	LSR A
	ROL $01
	DEX
	BNE ReadCtrlrsLoop
	RTS

irq:
	BIT $0101
	BMI game_irq
	BVC disk_byte_skip

;disk transfer routine ([$0101]	= 01xxxxxx)
	LDX $4031
	STA $4024
	PLA
	PLA
	PLA
	TXA
	RTS

disk_byte_skip:
;disk byte skip	routine ([$0101] = 00nnnnnn; n is # of bytes to	skip)
;this is mainly	used when the CPU has to do some calculations while bytes
;read off the disk need to be discarded.
	PHA
	LDA $0101
	SEC
	SBC #$01
	BCC end_irq
	STA $0101
	LDA $4031
	PLA
end_irq:
	RTI

game_irq:
;[$0101] = 1Xxxxxxx
	BVC disk_irq
	JMP ($DFFE);	11xxxxxx

disk_irq:
;disk IRQ acknowledge routine ([$0101] = 10xxxxxx).
;don't know what this is used for, or why a delay is put here.
	PHA
;	LDA $4030
;	JSR Delay131
	PLA
	RTI

;[$0100]	program control	on NMI
;-------	----------------------
;00xxxxxx:	VINTwait was called; return PC to address that called VINTwait
;01xxxxxx:	use [$DFF6] vector
;10xxxxxx:	use [$DFF8] vector
;11xxxxxx:	use [$DFFA] vector

;NMI branch target
nmi:
	BIT $0100
	BPL game_nmi1
	BVC game_nmi2
	JMP ($DFFA);	11xxxxxx
game_nmi2:
	JMP ($DFF8);	10xxxxxx
game_nmi1:
	BVC no_game_nmi
	JMP ($DFF6);	01xxxxxx

no_game_nmi:
;disable further VINTs	00xxxxxx
	LDA $FF
	AND #$7f
	STA $FF
	STA $2000;	[NES] PPU setup	#1
	LDA $2002;	[NES] PPU status

;discard interrupted return address (should be $E1C5)
	PLA
	PLA
	PLA

;restore byte at [$0100]
	PLA
	STA $0100

;restore A
	PLA
	RTS


.ORG $1800
bios_intro:

	;for now, we have an hle intro even

waitvbl:
	lda	$2002
	bpl waitvbl

	;call hle intro frame function
	lda	#$1F
	sta	$4028
	sta	$4029

	;result of intro frame func is pushed to the stack, so retreive it	
	pla
	
	;if result was 0, then continue looping
	cmp #0
	beq bios_intro

	rts

.ORG $1F00
	.dw $FFFF
	.dw $FFFF

.ORG $1FFA
	.dw nmi
	.dw reset
	.dw irq
