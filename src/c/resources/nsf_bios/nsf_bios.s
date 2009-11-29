;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; NSF BIOS
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.MEMORYMAP
SLOTSIZE $1000
DEFAULTSLOT 0
SLOT 0 $3000
.ENDME

.ROMBANKMAP
BANKSTOTAL 1
BANKSIZE $1000
BANKS 1
.ENDRO

.BANK 0 SLOT 0
.ORG $0000

.define BIOS_IDLE $00
.define BIOS_INIT $01
.define BIOS_PLAY $02

init_addr:
	.dw 0

play_addr:
	.dw 0

bios_cmd:
	.db 0

.ORG $0100

forever:
	jmp	forever

reset:
	sei
	ldx	#$FF
	txs
	jmp	forever

irq:
	rti
	
nmi:
	rti

.ORG $FFA
	.dw nmi
	.dw reset
	.dw irq