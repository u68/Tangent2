################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../startup.asm 

PRNS += \
./startup.prn 

PRNS__QUOTED += \
"./startup.prn" 

OBJS += \
./startup.obj 

OBJS__QUOTED += \
"./startup.obj" 


# Each subdirectory must supply rules for building sources it contributes
startup.obj: ../startup.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -ND -DN -ML -O"./" -WRPEAST -PR"startup.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup.prn: startup.obj


