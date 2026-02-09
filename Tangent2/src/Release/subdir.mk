################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../startup.asm 

C_SRCS += \
../libcw.c \
../main.c 

PRNS += \
./libcw.prn \
./main.prn \
./startup.prn 

PRNS__QUOTED += \
"./libcw.prn" \
"./main.prn" \
"./startup.prn" 

ASMS += \
./libcw.asm \
./main.asm 

ASMS__QUOTED += \
"./libcw.asm" \
"./main.asm" 

IS += \
./libcw.i \
./main.i 

IS__QUOTED += \
"./libcw.i" \
"./main.i" 

RESS += \
./libcw.res \
./main.res 

RESS__QUOTED += \
"./libcw.res" \
"./main.res" 

OBJS += \
./libcw.obj \
./main.obj \
./startup.obj 

OBJS__QUOTED += \
"./libcw.obj" \
"./main.obj" \
"./startup.obj" 


# Each subdirectory must supply rules for building sources it contributes
libcw.asm: ../libcw.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./libcw.res"
	@echo 'Finished building: $<'
	@echo ' '

libcw.res: libcw.asm
libcw.i: libcw.asm

libcw.obj: ./libcw.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"./" -WRPEAT -PR"libcw.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

libcw.prn: libcw.obj

main.asm: ../main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./main.res"
	@echo 'Finished building: $<'
	@echo ' '

main.res: main.asm
main.i: main.asm

main.obj: ./main.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"./" -WRPEAT -PR"main.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.prn: main.obj

startup.obj: ../startup.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -D -DF -ML -O"./" -WRPEAT -PR"startup.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup.prn: startup.obj


