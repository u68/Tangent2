################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vm/vm.c 

PRNS += \
./vm/vm.prn 

PRNS__QUOTED += \
"./vm/vm.prn" 

ASMS += \
./vm/vm.asm 

ASMS__QUOTED += \
"./vm/vm.asm" 

IS += \
./vm/vm.i 

IS__QUOTED += \
"./vm/vm.i" 

RESS += \
./vm/vm.res 

RESS__QUOTED += \
"./vm/vm.res" 

OBJS += \
./vm/vm.obj 

OBJS__QUOTED += \
"./vm/vm.obj" 


# Each subdirectory must supply rules for building sources it contributes
vm/vm.asm: ../vm/vm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./vm/vm.res"
	@echo 'Finished building: $<'
	@echo ' '

vm/vm.res: vm/vm.asm
vm/vm.i: vm/vm.asm

vm/vm.obj: ./vm/vm.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"vm/" -WRPEAT -PR"vm/vm.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

vm/vm.prn: vm/vm.obj


