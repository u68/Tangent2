################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../startup.asm 

C_SRCS += \
../main.c 

PRNS += \
./main.prn \
./startup.prn 

PRNS__QUOTED += \
"./main.prn" \
"./startup.prn" 

ASMS += \
./main.asm 

ASMS__QUOTED += \
"./main.asm" 

IS += \
./main.i 

IS__QUOTED += \
"./main.i" 

RESS += \
./main.res 

RESS__QUOTED += \
"./main.res" 

OBJS += \
./main.obj \
./startup.obj 

OBJS__QUOTED += \
"./main.obj" \
"./startup.obj" 


# Each subdirectory must supply rules for building sources it contributes
main.asm: ../main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./main.res"
	@echo 'Finished building: $<'
	@echo ' '

main.res: main.asm
main.i: main.asm

main.obj: ./main.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"./" -WRPEAT -PR"main.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.prn: main.obj

startup.obj: ../startup.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -D -DF -ML -O"./" -WRPEAT -PR"startup.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup.prn: startup.obj


