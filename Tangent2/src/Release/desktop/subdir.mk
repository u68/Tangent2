################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../desktop/desktop.c 

PRNS += \
./desktop/desktop.prn 

PRNS__QUOTED += \
"./desktop/desktop.prn" 

ASMS += \
./desktop/desktop.asm 

ASMS__QUOTED += \
"./desktop/desktop.asm" 

IS += \
./desktop/desktop.i 

IS__QUOTED += \
"./desktop/desktop.i" 

RESS += \
./desktop/desktop.res 

RESS__QUOTED += \
"./desktop/desktop.res" 

OBJS += \
./desktop/desktop.obj 

OBJS__QUOTED += \
"./desktop/desktop.obj" 


# Each subdirectory must supply rules for building sources it contributes
desktop/desktop.asm: ../desktop/desktop.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./desktop/desktop.res"
	@echo 'Finished building: $<'
	@echo ' '

desktop/desktop.res: desktop/desktop.asm
desktop/desktop.i: desktop/desktop.asm

desktop/desktop.obj: ./desktop/desktop.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"desktop/" -WRPEAT -PR"desktop/desktop.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

desktop/desktop.prn: desktop/desktop.obj


