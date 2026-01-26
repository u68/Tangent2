################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../time/time.c 

PRNS += \
./time/time.prn 

PRNS__QUOTED += \
"./time/time.prn" 

ASMS += \
./time/time.asm 

ASMS__QUOTED += \
"./time/time.asm" 

IS += \
./time/time.i 

IS__QUOTED += \
"./time/time.i" 

RESS += \
./time/time.res 

RESS__QUOTED += \
"./time/time.res" 

OBJS += \
./time/time.obj 

OBJS__QUOTED += \
"./time/time.obj" 


# Each subdirectory must supply rules for building sources it contributes
time/time.asm: ../time/time.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./time/time.res"
	@echo 'Finished building: $<'
	@echo ' '

time/time.res: time/time.asm
time/time.i: time/time.asm

time/time.obj: ./time/time.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"time/" -WRPEAT -PR"time/time.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

time/time.prn: time/time.obj


