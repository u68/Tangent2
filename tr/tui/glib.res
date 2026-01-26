[toolchain]lccu16
[output_dir]"./tui/"
[output_filename]"glib.asm"
[file_c]"../tui/glib.c"
[option_lccu16]-mcmodel=large -mdmodel=far -type=ML620909 -std=c99 -fno-signed-char -I"C:/LAPIS/LEXIDE/eclipse/../Inc/" -I"C:/LAPIS/LEXIDE/eclipse/../BuildTools/Ver.20251017/Inc/U16/Picolibc/" -O0 -g -lapisomf -S -o"tui/"
