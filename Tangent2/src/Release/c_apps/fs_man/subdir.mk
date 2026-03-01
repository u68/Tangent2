################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/fs_man/fs_man.c 

PRNS += \
./c_apps/fs_man/fs_man.prn 

PRNS__QUOTED += \
"./c_apps/fs_man/fs_man.prn" 

ASMS += \
./c_apps/fs_man/fs_man.asm 

ASMS__QUOTED += \
"./c_apps/fs_man/fs_man.asm" 

IS += \
./c_apps/fs_man/fs_man.i 

IS__QUOTED += \
"./c_apps/fs_man/fs_man.i" 

RESS += \
./c_apps/fs_man/fs_man.res 

RESS__QUOTED += \
"./c_apps/fs_man/fs_man.res" 

OBJS += \
./c_apps/fs_man/fs_man.obj 

OBJS__QUOTED += \
"./c_apps/fs_man/fs_man.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/fs_man/fs_man.asm: ../c_apps/fs_man/fs_man.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/fs_man/fs_man.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/fs_man/fs_man.res: c_apps/fs_man/fs_man.asm
c_apps/fs_man/fs_man.i: c_apps/fs_man/fs_man.asm

c_apps/fs_man/fs_man.obj: ./c_apps/fs_man/fs_man.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/fs_man/" -WRPEAT -PR"c_apps/fs_man/fs_man.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/fs_man/fs_man.prn: c_apps/fs_man/fs_man.obj


