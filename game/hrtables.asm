; hrtables.asm
; Originally HRTABLES.S
; Hires Tables

.hrtables
\org = $e000
\ tr on
\ lst off
\*-------------------------------
\ org org
\*-------------------------------
\*
\* YLO/YHI
\*
\* Index: Screen Y-coord (0-191, 0 = top)
\* Returns base address on hires page 1 (add $2000 for page 2)
\*
\*-------------------------------

\\ Needs to be changed for Beeb screen addressing!

\\ Apple II hi-res
\\ For Scanline y address = $2000 + ((y DIV 8) * $80) + ((y MOD 8) * $400)

; Would ideally be PAGE_ALIGN
.YLO
FOR y,0,191,1
\address = $2000 + ((y DIV 8) * $80) + ((y MOD 8) * $400)
address = beeb_screen_addr + ((y DIV 8) * BEEB_SCREEN_ROW_BYTES) + (y MOD 8)
EQUB LO(address)
NEXT
\ hex 00000000000000008080808080808080
\ hex 00000000000000008080808080808080
\ hex 00000000000000008080808080808080
\ hex 00000000000000008080808080808080

\ hex 2828282828282828A8A8A8A8A8A8A8A8
\ hex 2828282828282828A8A8A8A8A8A8A8A8
\ hex 2828282828282828A8A8A8A8A8A8A8A8
\ hex 2828282828282828A8A8A8A8A8A8A8A8

\ hex 5050505050505050D0D0D0D0D0D0D0D0
\ hex 5050505050505050D0D0D0D0D0D0D0D0
\ hex 5050505050505050D0D0D0D0D0D0D0D0
\ hex 5050505050505050D0D0D0D0D0D0D0D0

; Would ideally be PAGE_ALIGN
.YHI
FOR y,0,191,1
\address = $2000 + ((y DIV 8) * $80) + ((y MOD 8) * $400)
address = beeb_screen_addr + ((y DIV 8) * BEEB_SCREEN_ROW_BYTES) + (y MOD 8)
EQUB HI(address)
NEXT
\ hex 2024282C3034383C2024282C3034383C
\ hex 2125292D3135393D2125292D3135393D
\ hex 22262A2E32363A3E22262A2E32363A3E
\ hex 23272B2F33373B3F23272B2F33373B3F

\ hex 2024282C3034383C2024282C3034383C
\ hex 2125292D3135393D2125292D3135393D
\ hex 22262A2E32363A3E22262A2E32363A3E
\ hex 23272B2F33373B3F23272B2F33373B3F

\ hex 2024282C3034383C2024282C3034383C
\ hex 2125292D3135393D2125292D3135393D
\ hex 22262A2E32363A3E22262A2E32363A3E
\ hex 23272B2F33373B3F23272B2F33373B3F

\*-------------------------------
\*
\* SHIFTn/CARRYn
\*
\* n = # of pixels to shift right (0-6)
\* Index: byte value w/hibit clr (0-127)
\*
\* SHIFT returns shifted byte w/hibit set
\* CARRY returns carryover to next byte w/hibit clr
\*
\*-------------------------------

IF _NOT_BEEB

PAGE_ALIGN

.SHIFT0
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 0)
\NEXT
FOR byte,0,255,1
\EQUB LO(byte >> 0)
NEXT
\ hex 808182838485868788898A8B8C8D8E8F
\ hex 909192939495969798999A9B9C9D9E9F
\ hex A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
\ hex B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF

\ hex C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF
\ hex D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF
\ hex E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF
\ hex F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF

.SHIFT1
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 1)
\NEXT
FOR byte,0,255,1
EQUB LO(byte >> 1)
NEXT
\ hex 80828486888A8C8E90929496989A9C9E
\ hex A0A2A4A6A8AAACAEB0B2B4B6B8BABCBE
\ hex C0C2C4C6C8CACCCED0D2D4D6D8DADCDE
\ hex E0E2E4E6E8EAECEEF0F2F4F6F8FAFCFE

\ hex 80828486888A8C8E90929496989A9C9E
\ hex A0A2A4A6A8AAACAEB0B2B4B6B8BABCBE
\ hex C0C2C4C6C8CACCCED0D2D4D6D8DADCDE
\ hex E0E2E4E6E8EAECEEF0F2F4F6F8FAFCFE

.SHIFT2
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 2)
\NEXT
FOR byte,0,255,1
EQUB LO(byte >> 2)
NEXT
\ hex 8084888C9094989CA0A4A8ACB0B4B8BC
\ hex C0C4C8CCD0D4D8DCE0E4E8ECF0F4F8FC
\ hex 8084888C9094989CA0A4A8ACB0B4B8BC
\ hex C0C4C8CCD0D4D8DCE0E4E8ECF0F4F8FC

\ hex 8084888C9094989CA0A4A8ACB0B4B8BC
\ hex C0C4C8CCD0D4D8DCE0E4E8ECF0F4F8FC
\ hex 8084888C9094989CA0A4A8ACB0B4B8BC
\ hex C0C4C8CCD0D4D8DCE0E4E8ECF0F4F8FC

.SHIFT3
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 3)
\NEXT
FOR byte,0,255,1
EQUB LO(byte >> 3)
NEXT
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8

\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8
\ hex 80889098A0A8B0B8C0C8D0D8E0E8F0F8

.SHIFT4
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 4)
\NEXT
FOR byte,0,255,1
EQUB LO(byte >> 4)
NEXT
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0

\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0
\ hex 8090A0B0C0D0E0F08090A0B0C0D0E0F0

.SHIFT5
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 5)
\NEXT
FOR byte,0,255,1
EQUB LO(byte >> 5)
NEXT
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0

\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0
\ hex 80A0C0E080A0C0E080A0C0E080A0C0E0

.SHIFT6
\FOR byte,0,127,1
\EQUB $80 OR LO(byte << 6)
\NEXT
FOR byte,0,255,1
EQUB LO(byte >> 6)
NEXT
\ hex 80C080C080C080C080C080C080C080C0
\ hex 80C080C080C080C080C080C080C080C0
\ hex 80C080C080C080C080C080C080C080C0
\ hex 80C080C080C080C080C080C080C080C0

\ hex 80C080C080C080C080C080C080C080C0
\ hex 80C080C080C080C080C080C080C080C0
\ hex 80C080C080C080C080C080C080C080C0
\ hex 80C080C080C080C080C080C080C080C0

.SHIFT7
FOR byte,0,255,1
EQUB LO(byte >> 7)
NEXT


.CARRY0
\FOR byte,0,127,1
\EQUB ((LO(byte << 0) AND $80)>>7) OR (HI(byte << 0)<<1)
\NEXT
FOR byte,0,255,1
EQUB LO(byte << 8)
NEXT
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000

\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000

.CARRY1
\FOR byte,0,127,1
\EQUB ((LO(byte << 1) AND $80)>>7) OR (HI(byte << 1)<<1)
\NEXT
FOR byte,0,255,1
EQUB LO(byte << 7)
NEXT
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000

\ hex 01010101010101010101010101010101
\ hex 01010101010101010101010101010101
\ hex 01010101010101010101010101010101
\ hex 01010101010101010101010101010101

.CARRY2
\FOR byte,0,127,1
\EQUB ((LO(byte << 2) AND $80)>>7) OR (HI(byte << 2)<<1)
\NEXT
FOR byte,0,255,1
EQUB LO(byte << 6)
NEXT
\ hex 00000000000000000000000000000000
\ hex 00000000000000000000000000000000
\ hex 01010101010101010101010101010101
\ hex 01010101010101010101010101010101

\ hex 02020202020202020202020202020202
\ hex 02020202020202020202020202020202
\ hex 03030303030303030303030303030303
\ hex 03030303030303030303030303030303

.CARRY3
\FOR byte,0,127,1
\EQUB ((LO(byte << 3) AND $80)>>7) OR (HI(byte << 3)<<1)
\NEXT
FOR byte,0,255,1
EQUB LO(byte << 5)
NEXT
\ hex 00000000000000000000000000000000
\ hex 01010101010101010101010101010101
\ hex 02020202020202020202020202020202
\ hex 03030303030303030303030303030303

\ hex 04040404040404040404040404040404
\ hex 05050505050505050505050505050505
\ hex 06060606060606060606060606060606
\ hex 07070707070707070707070707070707

.CARRY4
\FOR byte,0,127,1
\EQUB ((LO(byte << 4) AND $80)>>7) OR (HI(byte << 4)<<1)
\NEXT
FOR byte,0,255,1
\EQUB LO(byte << 4)
NEXT
\ hex 00000000000000000101010101010101
\ hex 02020202020202020303030303030303
\ hex 04040404040404040505050505050505
\ hex 06060606060606060707070707070707

\ hex 08080808080808080909090909090909
\ hex 0A0A0A0A0A0A0A0A0B0B0B0B0B0B0B0B
\ hex 0C0C0C0C0C0C0C0C0D0D0D0D0D0D0D0D
\ hex 0E0E0E0E0E0E0E0E0F0F0F0F0F0F0F0F

.CARRY5
\FOR byte,0,127,1
\EQUB ((LO(byte << 5) AND $80)>>7) OR (HI(byte << 5)<<1)
\NEXT
FOR byte,0,255,1
\EQUB LO(byte << 3)
NEXT
\ hex 00000000010101010202020203030303
\ hex 04040404050505050606060607070707
\ hex 08080808090909090A0A0A0A0B0B0B0B
\ hex 0C0C0C0C0D0D0D0D0E0E0E0E0F0F0F0F

\ hex 10101010111111111212121213131313
\ hex 14141414151515151616161617171717
\ hex 18181818191919191A1A1A1A1B1B1B1B
\ hex 1C1C1C1C1D1D1D1D1E1E1E1E1F1F1F1F

.CARRY6
\FOR byte,0,127,1
\EQUB ((LO(byte << 6) AND $80)>>7) OR (HI(byte << 6)<<1)
\NEXT
FOR byte,0,255,1
\EQUB LO(byte << 2)
NEXT
\ hex 00000101020203030404050506060707
\ hex 080809090A0A0B0B0C0C0D0D0E0E0F0F
\ hex 10101111121213131414151516161717
\ hex 181819191A1A1B1B1C1C1D1D1E1E1F1F

\ hex 20202121222223232424252526262727
\ hex 282829292A2A2B2B2C2C2D2D2E2E2F2F
\ hex 30303131323233333434353536363737
\ hex 383839393A3A3B3B3C3C3D3D3E3E3F3F

.CARRY7
FOR byte,0,255,1
EQUB LO(byte << 1)
NEXT


\*-------------------------------
\*
\* MIRROR
\*
\* Index: byte value w/hibit clr (0-127)
\* Returns mirrored byte w/hibit set
\*
\* BEEB - just reverse all 8 bits
\*
\*-------------------------------

.MIRROR
FOR byte,0,255,1
b0=byte AND 1
b1=byte AND 2
b2=byte AND 4
b3=byte AND 8
b4=byte AND 16
b5=byte AND 32
b6=byte AND 64
b7=byte AND 128
EQUB (b0<<7) OR (b1<<5) OR (b2<<3) OR (b3<<1) OR (b4>>1) OR (b5>>3) OR (b6>>5) OR (b7>>7)
NEXT
\ hex 80C0A0E090D0B0F088C8A8E898D8B8F8
\ hex 84C4A4E494D4B4F48CCCACEC9CDCBCFC
\ hex 82C2A2E292D2B2F28ACAAAEA9ADABAFA
\ hex 86C6A6E696D6B6F68ECEAEEE9EDEBEFE

\ hex 81C1A1E191D1B1F189C9A9E999D9B9F9
\ hex 85C5A5E595D5B5F58DCDADED9DDDBDFD
\ hex 83C3A3E393D3B3F38BCBABEB9BDBBBFB
\ hex 87C7A7E797D7B7F78FCFAFEF9FDFBFFF

\*-------------------------------
\*
\* MASKTAB
\*
\* Index: byte value w/hibit clr (0-127)
\* Returns mask byte w/hibit set
\*
\*-------------------------------

\.MASKTAB
\ EQUB $FF,$FC,$F8,$F8,$F1,$F0,$F0,$F0
\ EQUB $E3,$E0,$E0,$E0,$E1,$E0,$E0,$E0
\ EQUB $C7,$C4,$C0,$C0,$C1,$C0,$C0,$C0
\ EQUB $C3,$C0,$C0,$C0,$C1,$C0,$C0,$C0
\
\ EQUB $8F,$8C,$88,$88,$81,$80,$80,$80
\ EQUB $83,$80,$80,$80,$81,$80,$80,$80
\ EQUB $87,$84,$80,$80,$81,$80,$80,$80
\ EQUB $83,$80,$80,$80,$81,$80,$80,$80
\
\ EQUB $9F,$9C,$98,$98,$91,$90,$90,$90
\ EQUB $83,$80,$80,$80,$81,$80,$80,$80
\ EQUB $87,$84,$80,$80,$81,$80,$80,$80
\ EQUB $83,$80,$80,$80,$81,$80,$80,$80
\
\ EQUB $8F,$8C,$88,$88,$81,$80,$80,$80
\ EQUB $83,$80,$80,$80,$81,$80,$80,$80
\ EQUB $87,$84,$80,$80,$81,$80,$80,$80
\ EQUB $83,$80,$80,$80,$81,$80,$80,$80

.MASKTAB
FOR byte,0,255,1

\ Simpler way of providing a 1 pixel mask
\EQUB LO(byte OR byte<<1 OR byte>>1) EOR &FF

\ Potentially use a 2 pixel mask:
\EQUB LO(byte OR byte<<1 OR byte>>1 OR byte<<2 OR byte>>2) EOR &FF

 NEXT

\*-------------------------------
\*
\* SHIFTL-H/CARRYL-H
\*
\* Index: Bit offset (0-6)
\* Returns address of corresponding shift/carry table
\*
\*-------------------------------

.SHIFTL     ; should be 0
 EQUB LO(SHIFT0)    ;-$80
 EQUB LO(SHIFT1)    ;-$80
 EQUB LO(SHIFT2)    ;-$80
 EQUB LO(SHIFT3)    ;-$80
 EQUB LO(SHIFT4)    ;-$80
 EQUB LO(SHIFT5)    ;-$80
 EQUB LO(SHIFT6)    ;-$80
 EQUB LO(SHIFT7)    ;-$80

.SHIFTH
 EQUB HI(SHIFT0)    ;-$80
 EQUB HI(SHIFT1)    ;-$80
 EQUB HI(SHIFT2)    ;-$80
 EQUB HI(SHIFT3)    ;-$80
 EQUB HI(SHIFT4)    ;-$80
 EQUB HI(SHIFT5)    ;-$80
 EQUB HI(SHIFT6)    ;-$80
 EQUB HI(SHIFT7)    ;-$80

.CARRYL     ; should be 0
 EQUB LO(CARRY0)    ;-$80
 EQUB LO(CARRY1)    ;-$80
 EQUB LO(CARRY2)    ;-$80
 EQUB LO(CARRY3)    ;-$80
 EQUB LO(CARRY4)    ;-$80
 EQUB LO(CARRY5)    ;-$80
 EQUB LO(CARRY6)    ;-$80
 EQUB LO(CARRY7)    ;-$80

.CARRYH
 EQUB HI(CARRY0)    ;-$80
 EQUB HI(CARRY1)    ;-$80
 EQUB HI(CARRY2)    ;-$80
 EQUB HI(CARRY3)    ;-$80
 EQUB HI(CARRY4)    ;-$80
 EQUB HI(CARRY5)    ;-$80
 EQUB HI(CARRY6)    ;-$80
 EQUB HI(CARRY7)    ;-$80

\*-------------------------------
\*
\* AMASKS/BMASKS
\*
\* Index: Bit offset (0-6)
\* Returns appropriate mask bytes
\*
\*-------------------------------

.AMASKS EQUB %10000000
 EQUB %10000001
 EQUB %10000011
 EQUB %10000111
 EQUB %10001111
 EQUB %10011111
 EQUB %10111111

.BMASKS EQUB %11111111
 EQUB %11111110
 EQUB %11111100
 EQUB %11111000
 EQUB %11110000
 EQUB %11100000
 EQUB %11000000

 .CARRY_MASK
 FOR n,0,7,1
 EQUB (&FF >> n) EOR &FF
 NEXT

.CARRY_MASK1
 FOR n,0,7,1
 EQUB (&FE >> n) EOR &FF
 NEXT

\ Don't need carry masks above 1 as > 7 bits

 .SHIFT_MASK
 FOR n,0,7,1
 EQUB LO(&FF >> n)
 NEXT

.SHIFT_MASK1
 FOR n,0,7,1
 EQUB LO(&1FF >> n)
 NEXT

.SHIFT_MASK2
 FOR n,0,7,1
 EQUB LO(&3FF >> n)
 NEXT

.SHIFT_MASK3
 FOR n,0,7,1
 EQUB LO(&7FF >> n)
 NEXT

.SHIFT_MASK4
 FOR n,0,7,1
 EQUB LO(&FFF >> n)
 NEXT

.SHIFT_MASK5
 FOR n,0,7,1
 EQUB LO(&1FFF >> n)
 NEXT

.SHIFT_MASK6
 FOR n,0,7,1
 EQUB LO(&3FFF >> n)
 NEXT

ENDIF

\*-------------------------------
\*
\* OPCODE
\*
\* Index: OPACITY (0-5)
\* Returns opcode to put in self-mod code
\*
\*-------------------------------

.OPCODE EQUB $31 ;and (oper),Y
 EQUB $11 ;ora
 EQUB $91 ;sta
 EQUB $51 ;eor
 EQUB $31 ;and
 EQUB $91 ;sta

\*-------------------------------
\ lst
\ usr $a9,2,$0000,*-org
\ lst off
