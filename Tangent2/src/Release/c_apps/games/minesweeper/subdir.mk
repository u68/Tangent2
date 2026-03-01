################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/games/minesweeper/minesweeper.c 

PRNS += \
./c_apps/games/minesweeper/minesweeper.prn 

PRNS__QUOTED += \
"./c_apps/games/minesweeper/minesweeper.prn" 

ASMS += \
./c_apps/games/minesweeper/minesweeper.asm 

ASMS__QUOTED += \
"./c_apps/games/minesweeper/minesweeper.asm" 

IS += \
./c_apps/games/minesweeper/minesweeper.i 

IS__QUOTED += \
"./c_apps/games/minesweeper/minesweeper.i" 

RESS += \
./c_apps/games/minesweeper/minesweeper.res 

RESS__QUOTED += \
"./c_apps/games/minesweeper/minesweeper.res" 

OBJS += \
./c_apps/games/minesweeper/minesweeper.obj 

OBJS__QUOTED += \
"./c_apps/games/minesweeper/minesweeper.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/games/minesweeper/minesweeper.asm: ../c_apps/games/minesweeper/minesweeper.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/games/minesweeper/minesweeper.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/minesweeper/minesweeper.res: c_apps/games/minesweeper/minesweeper.asm
c_apps/games/minesweeper/minesweeper.i: c_apps/games/minesweeper/minesweeper.asm

c_apps/games/minesweeper/minesweeper.obj: ./c_apps/games/minesweeper/minesweeper.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/games/minesweeper/" -WRPEAT -PR"c_apps/games/minesweeper/minesweeper.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/games/minesweeper/minesweeper.prn: c_apps/games/minesweeper/minesweeper.obj


