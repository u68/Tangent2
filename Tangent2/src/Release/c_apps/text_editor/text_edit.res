[toolchain]lccu16
[output_dir]"./c_apps/text_editor/"
[output_filename]"text_edit.asm"
[file_c]"../c_apps/text_editor/text_edit.c"
[option_lccu16]-mcmodel=large -mdmodel=far -type=ML620909 -std=c99 -fno-signed-char -fpack-struct -fshort-enums -I"C:/LAPIS/LEXIDE/eclipse/../Inc/" -I"C:/LAPIS/LEXIDE/eclipse/../BuildTools/Ver.20251017/Inc/U16/Picolibc/" -O0 -lapisomf -Weverything -S -o"c_apps/text_editor/"
