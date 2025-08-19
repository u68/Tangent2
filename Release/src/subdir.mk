################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/io.c \
../src/legacy_vm.c \
../src/main.c \
../src/vm.c 

PRNS += \
./src/io.prn \
./src/legacy_vm.prn \
./src/main.prn \
./src/vm.prn 

PRNS__QUOTED += \
"./src/io.prn" \
"./src/legacy_vm.prn" \
"./src/main.prn" \
"./src/vm.prn" 

ASMS += \
./src/io.asm \
./src/legacy_vm.asm \
./src/main.asm \
./src/vm.asm 

ASMS__QUOTED += \
"./src/io.asm" \
"./src/legacy_vm.asm" \
"./src/main.asm" \
"./src/vm.asm" 

OBJS += \
./src/io.obj \
./src/legacy_vm.obj \
./src/main.obj \
./src/vm.obj 

OBJS__QUOTED += \
"./src/io.obj" \
"./src/legacy_vm.obj" \
"./src/main.obj" \
"./src/vm.obj" 

IS += \
./src/io.i \
./src/legacy_vm.i \
./src/main.i \
./src/vm.i 

IS__QUOTED += \
"./src/io.i" \
"./src/legacy_vm.i" \
"./src/main.i" \
"./src/vm.i" 


# Each subdirectory must supply rules for building sources it contributes
src/io.asm: ../src/io.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	ccu8 -SD -ABW -TML620909 -ML -near -Fa"src/" -Ot -Oa -W1 "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/io.i: src/io.asm

src/io.obj: ./src/io.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -ND -DN -ML -O"src/" -WRPEAST -PR"src/io.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/io.prn: src/io.obj

src/legacy_vm.asm: ../src/legacy_vm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	ccu8 -SD -ABW -TML620909 -ML -near -Fa"src/" -Ot -Oa -W1 "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/legacy_vm.i: src/legacy_vm.asm

src/legacy_vm.obj: ./src/legacy_vm.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -ND -DN -ML -O"src/" -WRPEAST -PR"src/legacy_vm.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/legacy_vm.prn: src/legacy_vm.obj

src/main.asm: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	ccu8 -SD -ABW -TML620909 -ML -near -Fa"src/" -Ot -Oa -W1 "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.i: src/main.asm

src/main.obj: ./src/main.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -ND -DN -ML -O"src/" -WRPEAST -PR"src/main.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.prn: src/main.obj

src/vm.asm: ../src/vm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	ccu8 -SD -ABW -TML620909 -ML -near -Fa"src/" -Ot -Oa -W1 "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/vm.i: src/vm.asm

src/vm.obj: ./src/vm.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -ND -DN -ML -O"src/" -WRPEAST -PR"src/vm.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/vm.prn: src/vm.obj


