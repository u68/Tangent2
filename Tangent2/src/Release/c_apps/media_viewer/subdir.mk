################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../c_apps/media_viewer/media_view.c 

PRNS += \
./c_apps/media_viewer/media_view.prn 

PRNS__QUOTED += \
"./c_apps/media_viewer/media_view.prn" 

ASMS += \
./c_apps/media_viewer/media_view.asm 

ASMS__QUOTED += \
"./c_apps/media_viewer/media_view.asm" 

IS += \
./c_apps/media_viewer/media_view.i 

IS__QUOTED += \
"./c_apps/media_viewer/media_view.i" 

RESS += \
./c_apps/media_viewer/media_view.res 

RESS__QUOTED += \
"./c_apps/media_viewer/media_view.res" 

OBJS += \
./c_apps/media_viewer/media_view.obj 

OBJS__QUOTED += \
"./c_apps/media_viewer/media_view.obj" 


# Each subdirectory must supply rules for building sources it contributes
c_apps/media_viewer/media_view.asm: ../c_apps/media_viewer/media_view.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./c_apps/media_viewer/media_view.res"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/media_viewer/media_view.res: c_apps/media_viewer/media_view.asm
c_apps/media_viewer/media_view.i: c_apps/media_viewer/media_view.asm

c_apps/media_viewer/media_view.obj: ./c_apps/media_viewer/media_view.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"c_apps/media_viewer/" -WRPEAT -PR"c_apps/media_viewer/media_view.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

c_apps/media_viewer/media_view.prn: c_apps/media_viewer/media_view.obj


