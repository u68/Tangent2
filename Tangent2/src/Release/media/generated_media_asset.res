[toolchain]lccu16
[output_dir]"./media/"
[output_filename]"generated_media_asset.asm"
[file_c]"../media/generated_media_asset.c"
[option_lccu16]-mcmodel=large -mdmodel=far -type=ML620909 -std=c99 -fno-signed-char -fpack-struct -fshort-enums -I"C:/LAPIS/LEXIDE/eclipse/../Inc/" -I"C:/LAPIS/LEXIDE/eclipse/../BuildTools/Ver.20251017/Inc/U16/Picolibc/" -O0 -lapisomf -Weverything -S -o"media/"
