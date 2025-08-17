################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

-include ../makefile.defs

# Export environment variables
export DCL=C:/lapis/LEXIDE/Dcl
export INCLU8=C:/lapis/LEXIDE/Inc;C:/lapis/LEXIDE/BuildTools/Ver.20241126/Inc/U16
export LIBU8=C:/lapis/LEXIDE/BuildTools/Ver.20241126/Lib/U16
export PATH=C:/lapis/LEXIDE/Bin;C:/lapis/LEXIDE/BuildTools/Ver.20241126/Bin;C:/lapis/LEXIDE/Utilities/Bin

# Add inputs and outputs from these tool invocations to the build variables 
OBJHEX += \
Tangent2.HEX \

ABS += \
Tangent2.abs \

MAP += \
Tangent2.map \


# All Target
all: $(OBJHEX)

# Tool invocations
Tangent2.HEX: $(ABS)
	@echo 'Building target: $@'
	@echo 'Invoking: Object converter'
	CnvOHU8 -H $(ABS) "Tangent2.HEX";
	@echo 'Finished building target: $@'
	@echo ' '

Tangent2.abs: $(OBJS)
	@echo 'Invoking: Linker'
	rlu8 -D -SD -NS -CC -ABW $(OBJS__QUOTED),"Tangent2.abs",,"LU8100LW.LIB";
	@echo 'Finished building: $@'
	@echo ' '

Tangent2.map: Tangent2.abs $(OBJS)

# Other Targets
clean:
	-$(RM) "./Tangent2.sc" "./src/io.prn" "./src/main.prn" "./src/vm.prn" "./startup.prn" "./Tangent2.abs" "./Tangent2RB.hex" "./Tangent2RA.hex" "./Tangent2RA.log" "./src/io.asm" "./src/main.asm" "./src/vm.asm" "./Tangent2.HEX" "./src/io.obj" "./src/main.obj" "./src/vm.obj" "./startup.obj" "./src/io.i" "./src/main.i" "./src/vm.i" "./Tangent2.map" 
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
