################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../heap/heap.c 

PRNS += \
./heap/heap.prn 

PRNS__QUOTED += \
"./heap/heap.prn" 

ASMS += \
./heap/heap.asm 

ASMS__QUOTED += \
"./heap/heap.asm" 

IS += \
./heap/heap.i 

IS__QUOTED += \
"./heap/heap.i" 

RESS += \
./heap/heap.res 

RESS__QUOTED += \
"./heap/heap.res" 

OBJS += \
./heap/heap.obj 

OBJS__QUOTED += \
"./heap/heap.obj" 


# Each subdirectory must supply rules for building sources it contributes
heap/heap.asm: ../heap/heap.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./heap/heap.res"
	@echo 'Finished building: $<'
	@echo ' '

heap/heap.res: heap/heap.asm
heap/heap.i: heap/heap.asm

heap/heap.obj: ./heap/heap.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"heap/" -WRPEAT -PR"heap/heap.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

heap/heap.prn: heap/heap.obj


