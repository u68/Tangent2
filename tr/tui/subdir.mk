################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tui/glib.c 

PRNS += \
./tui/glib.prn 

PRNS__QUOTED += \
"./tui/glib.prn" 

ASMS += \
./tui/glib.asm 

ASMS__QUOTED += \
"./tui/glib.asm" 

IS += \
./tui/glib.i 

IS__QUOTED += \
"./tui/glib.i" 

RESS += \
./tui/glib.res 

RESS__QUOTED += \
"./tui/glib.res" 

OBJS += \
./tui/glib.obj 

OBJS__QUOTED += \
"./tui/glib.obj" 


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


