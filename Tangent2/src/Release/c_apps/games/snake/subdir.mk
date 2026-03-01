################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/games/snake/snake.c 

PRNS += \
./c_apps/games/snake/snake.prn 

PRNS__QUOTED += \
"./c_apps/games/snake/snake.prn" 

ASMS += \
./c_apps/games/snake/snake.asm 

ASMS__QUOTED += \
"./c_apps/games/snake/snake.asm" 

IS += \
./c_apps/games/snake/snake.i 

IS__QUOTED += \
"./c_apps/games/snake/snake.i" 

RESS += \
./c_apps/games/snake/snake.res 

RESS__QUOTED += \
"./c_apps/games/snake/snake.res" 

OBJS += \
./c_apps/games/snake/snake.obj 

OBJS__QUOTED += \
"./c_apps/games/snake/snake.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/games/snake/snake.asm: ../c_apps/games/snake/snake.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/games/snake/snake.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/snake/snake.res: c_apps/games/snake/snake.asm
c_apps/games/snake/snake.i: c_apps/games/snake/snake.asm

c_apps/games/snake/snake.obj: ./c_apps/games/snake/snake.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/games/snake/" -WRPEAT -PR"c_apps/games/snake/snake.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/snake/snake.prn: c_apps/games/snake/snake.obj


