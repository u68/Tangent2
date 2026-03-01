################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/games/2048/2048.c 

PRNS += \
./c_apps/games/2048/2048.prn 

PRNS__QUOTED += \
"./c_apps/games/2048/2048.prn" 

ASMS += \
./c_apps/games/2048/2048.asm 

ASMS__QUOTED += \
"./c_apps/games/2048/2048.asm" 

IS += \
./c_apps/games/2048/2048.i 

IS__QUOTED += \
"./c_apps/games/2048/2048.i" 

RESS += \
./c_apps/games/2048/2048.res 

RESS__QUOTED += \
"./c_apps/games/2048/2048.res" 

OBJS += \
./c_apps/games/2048/2048.obj 

OBJS__QUOTED += \
"./c_apps/games/2048/2048.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/games/2048/2048.asm: ../c_apps/games/2048/2048.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/games/2048/2048.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/2048/2048.res: c_apps/games/2048/2048.asm
c_apps/games/2048/2048.i: c_apps/games/2048/2048.asm

c_apps/games/2048/2048.obj: ./c_apps/games/2048/2048.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/games/2048/" -WRPEAT -PR"c_apps/games/2048/2048.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/2048/2048.prn: c_apps/games/2048/2048.obj


