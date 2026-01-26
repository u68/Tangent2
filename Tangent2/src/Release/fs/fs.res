[toolchain]lccu16
[output_dir]"./fs/"
[output_filename]"fs.asm"
[file_c]"../fs/fs.c"
[option_lccu16]-mcmodel=large -mdmodel=far -type=ML620909 -std=c99 -fno-signed-char -I"C:/LAPIS/LEXIDE/eclipse/../Inc/" -I"C:/LAPIS/LEXIDE/eclipse/../BuildTools/Ver.20251017/Inc/U16/Picolibc/" -O0 -g -lapisomf -S -o"fs/"
