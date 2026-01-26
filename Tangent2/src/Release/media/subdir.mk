################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../media/media.c 

PRNS += \
./media/media.prn 

PRNS__QUOTED += \
"./media/media.prn" 

ASMS += \
./media/media.asm 

ASMS__QUOTED += \
"./media/media.asm" 

IS += \
./media/media.i 

IS__QUOTED += \
"./media/media.i" 

RESS += \
./media/media.res 

RESS__QUOTED += \
"./media/media.res" 

OBJS += \
./media/media.obj 

OBJS__QUOTED += \
"./media/media.obj" 


# Each subdirectory must supply rules for building sources it contributes
media/media.asm: ../media/media.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./media/media.res"
	@echo 'Finished building: $<'
	@echo ' '

media/media.res: media/media.asm
media/media.i: media/media.asm

media/media.obj: ./media/media.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"media/" -WRPEAT -PR"media/media.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

media/media.prn: media/media.obj


