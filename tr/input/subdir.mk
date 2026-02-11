################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../input/input.c 

PRNS += \
./input/input.prn 

PRNS__QUOTED += \
"./input/input.prn" 

ASMS += \
./input/input.asm 

ASMS__QUOTED += \
"./input/input.asm" 

IS += \
./input/input.i 

IS__QUOTED += \
"./input/input.i" 

RESS += \
./input/input.res 

RESS__QUOTED += \
"./input/input.res" 

OBJS += \
./input/input.obj 

OBJS__QUOTED += \
"./input/input.obj" 


# Each subdirectory must supply rules for building sources it contributes
input/input.asm: ../input/input.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./input/input.res"
	@echo 'Finished building: $<'
	@echo ' '

input/input.res: input/input.asm
input/input.i: input/input.asm

input/input.obj: ./input/input.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"input/" -WRPEAT -PR"input/input.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

input/input.prn: input/input.obj


