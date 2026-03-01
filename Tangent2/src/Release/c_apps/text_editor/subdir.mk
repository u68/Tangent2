################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/text_editor/text_edit.c 

PRNS += \
./c_apps/text_editor/text_edit.prn 

PRNS__QUOTED += \
"./c_apps/text_editor/text_edit.prn" 

ASMS += \
./c_apps/text_editor/text_edit.asm 

ASMS__QUOTED += \
"./c_apps/text_editor/text_edit.asm" 

IS += \
./c_apps/text_editor/text_edit.i 

IS__QUOTED += \
"./c_apps/text_editor/text_edit.i" 

RESS += \
./c_apps/text_editor/text_edit.res 

RESS__QUOTED += \
"./c_apps/text_editor/text_edit.res" 

OBJS += \
./c_apps/text_editor/text_edit.obj 

OBJS__QUOTED += \
"./c_apps/text_editor/text_edit.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/text_editor/text_edit.asm: ../c_apps/text_editor/text_edit.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/text_editor/text_edit.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/text_editor/text_edit.res: c_apps/text_editor/text_edit.asm
c_apps/text_editor/text_edit.i: c_apps/text_editor/text_edit.asm

c_apps/text_editor/text_edit.obj: ./c_apps/text_editor/text_edit.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/text_editor/" -WRPEAT -PR"c_apps/text_editor/text_edit.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/text_editor/text_edit.prn: c_apps/text_editor/text_edit.obj


