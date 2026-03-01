################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/games/ball/ball.c 

PRNS += \
./c_apps/games/ball/ball.prn 

PRNS__QUOTED += \
"./c_apps/games/ball/ball.prn" 

ASMS += \
./c_apps/games/ball/ball.asm 

ASMS__QUOTED += \
"./c_apps/games/ball/ball.asm" 

IS += \
./c_apps/games/ball/ball.i 

IS__QUOTED += \
"./c_apps/games/ball/ball.i" 

RESS += \
./c_apps/games/ball/ball.res 

RESS__QUOTED += \
"./c_apps/games/ball/ball.res" 

OBJS += \
./c_apps/games/ball/ball.obj 

OBJS__QUOTED += \
"./c_apps/games/ball/ball.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/games/ball/ball.asm: ../c_apps/games/ball/ball.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/games/ball/ball.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/ball/ball.res: c_apps/games/ball/ball.asm
c_apps/games/ball/ball.i: c_apps/games/ball/ball.asm

c_apps/games/ball/ball.obj: ./c_apps/games/ball/ball.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/games/ball/" -WRPEAT -PR"c_apps/games/ball/ball.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/ball/ball.prn: c_apps/games/ball/ball.obj


