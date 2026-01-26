[toolchain]lccu16
[output_dir]"./time/"
[output_filename]"time.asm"
[file_c]"../time/time.c"
[option_lccu16]-mcmodel=large -mdmodel=far -type=ML620909 -std=c99 -fno-signed-char -I"C:/LAPIS/LEXIDE/eclipse/../Inc/" -I"C:/LAPIS/LEXIDE/eclipse/../BuildTools/Ver.20251017/Inc/U16/Picolibc/" -O0 -g -lapisomf -S -o"time/"
