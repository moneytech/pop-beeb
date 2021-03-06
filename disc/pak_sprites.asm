; sprites.asm

ORG 0
GUARD &FFFF

.sprites_start

ALIGN &100
.file0
INCBIN "disc/chtab9.pu.bin"

ALIGN &100
.file1
INCBIN "disc/chtab8.pu.bin"

ALIGN &100
.file2
INCBIN "disc/chtab7.pu.bin"

ALIGN &100
.file3
INCBIN "disc/chtab6.pu.bin"

ALIGN &100
.file4
INCBIN "disc/chtab5.pu.bin"

ALIGN &100
.file5
INCBIN "disc/fat.pu.bin"

ALIGN &100
.file6
INCBIN "disc/gd.pu.bin"

ALIGN &100
.file7
INCBIN "disc/shad.pu.bin"

ALIGN &100
.file8
INCBIN "disc/skel.pu.bin"

ALIGN &100
.file9
INCBIN "disc/viz.pu.bin"

ALIGN &100
.file10
INCBIN "disc/dun1a.pu.bin"

ALIGN &100
.file11
INCBIN "disc/dun1b.pu.bin"

ALIGN &100
.file12
INCBIN "disc/dun2.pu.bin"

ALIGN &100
.file13
INCBIN "disc/pal1a.pu.bin"

ALIGN &100
.file14
INCBIN "disc/pal1b.pu.bin"

ALIGN &100
.file15
INCBIN "disc/pal2.pu.bin"

ALIGN &100
.sprites_end

SAVE "disc/Sprites.bin", sprites_start, sprites_end, 0

MACRO CAT_ENTRY file_no, byte_offset, byte_end
{
    sector_offset = byte_offset DIV 256
    track_no = sector_offset DIV 10
    sector_no = sector_offset MOD 10
    byte_size = byte_end - byte_offset
    num_sectors = (byte_size + &FF) DIV 256

    PRINT "File #", file_no, "Byte offset =", ~byte_offset, "Byte size =", ~byte_size, "Track =", track_no, "Sector =", sector_no, "Num sectors =", num_sectors

    EQUB track_no, sector_no, num_sectors, 0
}
ENDMACRO

.catalog_start

CAT_ENTRY 0, file0, file1
CAT_ENTRY 1, file1, file2
CAT_ENTRY 2, file2, file3
CAT_ENTRY 3, file3, file4
CAT_ENTRY 4, file4, file5
CAT_ENTRY 5, file5, file6
CAT_ENTRY 6, file6, file7
CAT_ENTRY 7, file7, file8
CAT_ENTRY 8, file8, file9
CAT_ENTRY 9, file9, file10
CAT_ENTRY 10, file10, file11
CAT_ENTRY 11, file11, file12
CAT_ENTRY 12, file12, file13
CAT_ENTRY 13, file13, file14
CAT_ENTRY 14, file14, file15
CAT_ENTRY 15, file15, sprites_end

.catalog_end

SAVE "disc/Catalog.bin", catalog_start, catalog_end, 0
