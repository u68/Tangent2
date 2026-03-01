################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/paint/paint.c 

PRNS += \
./c_apps/paint/paint.prn 

PRNS__QUOTED += \
"./c_apps/paint/paint.prn" 

ASMS += \
./c_apps/paint/paint.asm 

ASMS__QUOTED += \
"./c_apps/paint/paint.asm" 

IS += \
./c_apps/paint/paint.i 

IS__QUOTED += \
"./c_apps/paint/paint.i" 

RESS += \
./c_apps/paint/paint.res 

RESS__QUOTED += \
"./c_apps/paint/paint.res" 

OBJS += \
./c_apps/paint/paint.obj 

OBJS__QUOTED += \
"./c_apps/paint/paint.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/paint/paint.asm: ../c_apps/paint/paint.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/paint/paint.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/paint/paint.res: c_apps/paint/paint.asm
c_apps/paint/paint.i: c_apps/paint/paint.asm

c_apps/paint/paint.obj: ./c_apps/paint/paint.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/paint/" -WRPEAT -PR"c_apps/paint/paint.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/paint/paint.prn: c_apps/paint/paint.obj


