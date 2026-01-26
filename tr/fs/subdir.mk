################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fs/fs.c 

PRNS += \
./fs/fs.prn 

PRNS__QUOTED += \
"./fs/fs.prn" 

ASMS += \
./fs/fs.asm 

ASMS__QUOTED += \
"./fs/fs.asm" 

IS += \
./fs/fs.i 

IS__QUOTED += \
"./fs/fs.i" 

RESS += \
./fs/fs.res 

RESS__QUOTED += \
"./fs/fs.res" 

OBJS += \
./fs/fs.obj 

OBJS__QUOTED += \
"./fs/fs.obj" 


# Each subdirectory must supply rules for building sources it contributes
fs/fs.asm: ../fs/fs.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./fs/fs.res"
	@echo 'Finished building: $<'
	@echo ' '

fs/fs.res: fs/fs.asm
fs/fs.i: fs/fs.asm

fs/fs.obj: ./fs/fs.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"fs/" -WRPEAT -PR"fs/fs.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

fs/fs.prn: fs/fs.obj


