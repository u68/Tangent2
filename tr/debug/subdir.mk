################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../debug/bsod.c \
../debug/debug.c 

PRNS += \
./debug/bsod.prn \
./debug/debug.prn 

PRNS__QUOTED += \
"./debug/bsod.prn" \
"./debug/debug.prn" 

ASMS += \
./debug/bsod.asm \
./debug/debug.asm 

ASMS__QUOTED += \
"./debug/bsod.asm" \
"./debug/debug.asm" 

IS += \
./debug/bsod.i \
./debug/debug.i 

IS__QUOTED += \
"./debug/bsod.i" \
"./debug/debug.i" 

RESS += \
./debug/bsod.res \
./debug/debug.res 

RESS__QUOTED += \
"./debug/bsod.res" \
"./debug/debug.res" 

OBJS += \
./debug/bsod.obj \
./debug/debug.obj 

OBJS__QUOTED += \
"./debug/bsod.obj" \
"./debug/debug.obj" 


# Each subdirectory must supply rules for building sources it contributes
debug/bsod.asm: ../debug/bsod.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./debug/bsod.res"
	@echo 'Finished building: $<'
	@echo ' '

debug/bsod.res: debug/bsod.asm
debug/bsod.i: debug/bsod.asm

debug/bsod.obj: ./debug/bsod.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"debug/" -WRPEAT -PR"debug/bsod.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

debug/bsod.prn: debug/bsod.obj

debug/debug.asm: ../debug/debug.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./debug/debug.res"
	@echo 'Finished building: $<'
	@echo ' '

debug/debug.res: debug/debug.asm
debug/debug.i: debug/debug.asm

debug/debug.obj: ./debug/debug.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"debug/" -WRPEAT -PR"debug/debug.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

debug/debug.prn: debug/debug.obj


