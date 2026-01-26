################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tui/glib.c \
../tui/graphics.c \
../tui/lut.c \
../tui/ysglfonts.c 

PRNS += \
./tui/glib.prn \
./tui/graphics.prn \
./tui/lut.prn \
./tui/ysglfonts.prn 

PRNS__QUOTED += \
"./tui/glib.prn" \
"./tui/graphics.prn" \
"./tui/lut.prn" \
"./tui/ysglfonts.prn" 

ASMS += \
./tui/glib.asm \
./tui/graphics.asm \
./tui/lut.asm \
./tui/ysglfonts.asm 

ASMS__QUOTED += \
"./tui/glib.asm" \
"./tui/graphics.asm" \
"./tui/lut.asm" \
"./tui/ysglfonts.asm" 

IS += \
./tui/glib.i \
./tui/graphics.i \
./tui/lut.i \
./tui/ysglfonts.i 

IS__QUOTED += \
"./tui/glib.i" \
"./tui/graphics.i" \
"./tui/lut.i" \
"./tui/ysglfonts.i" 

RESS += \
./tui/glib.res \
./tui/graphics.res \
./tui/lut.res \
./tui/ysglfonts.res 

RESS__QUOTED += \
"./tui/glib.res" \
"./tui/graphics.res" \
"./tui/lut.res" \
"./tui/ysglfonts.res" 

OBJS += \
./tui/glib.obj \
./tui/graphics.obj \
./tui/lut.obj \
./tui/ysglfonts.obj 

OBJS__QUOTED += \
"./tui/glib.obj" \
"./tui/graphics.obj" \
"./tui/lut.obj" \
"./tui/ysglfonts.obj" 


# Each subdirectory must supply rules for building sources it contributes
tui/glib.asm: ../tui/glib.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./tui/glib.res"
	@echo 'Finished building: $<'
	@echo ' '

tui/glib.res: tui/glib.asm
tui/glib.i: tui/glib.asm

tui/glib.obj: ./tui/glib.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"tui/" -WRPEAT -PR"tui/glib.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

tui/glib.prn: tui/glib.obj

tui/graphics.asm: ../tui/graphics.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./tui/graphics.res"
	@echo 'Finished building: $<'
	@echo ' '

tui/graphics.res: tui/graphics.asm
tui/graphics.i: tui/graphics.asm

tui/graphics.obj: ./tui/graphics.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"tui/" -WRPEAT -PR"tui/graphics.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

tui/graphics.prn: tui/graphics.obj

tui/lut.asm: ../tui/lut.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./tui/lut.res"
	@echo 'Finished building: $<'
	@echo ' '

tui/lut.res: tui/lut.asm
tui/lut.i: tui/lut.asm

tui/lut.obj: ./tui/lut.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"tui/" -WRPEAT -PR"tui/lut.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

tui/lut.prn: tui/lut.obj

tui/ysglfonts.asm: ../tui/ysglfonts.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./tui/ysglfonts.res"
	@echo 'Finished building: $<'
	@echo ' '

tui/ysglfonts.res: tui/ysglfonts.asm
tui/ysglfonts.i: tui/ysglfonts.asm

tui/ysglfonts.obj: ./tui/ysglfonts.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"tui/" -WRPEAT -PR"tui/ysglfonts.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

tui/ysglfonts.prn: tui/ysglfonts.obj


