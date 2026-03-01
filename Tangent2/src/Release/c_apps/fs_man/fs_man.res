[toolchain]lccu16
[output_dir]"./c_apps/fs_man/"
[output_filename]"fs_man.asm"
[file_c]"../c_apps/fs_man/fs_man.c"
[option_lccu16]-mcmodel=large -mdmodel=far -type=ML620909 -std=c99 -fno-signed-char -fpack-struct -fshort-enums -I"C:/LAPIS/LEXIDE/eclipse/../Inc/" -I"C:/LAPIS/LEXIDE/eclipse/../BuildTools/Ver.20251017/Inc/U16/Picolibc/" -O0 -lapisomf -Weverything -S -o"c_apps/fs_man/"
