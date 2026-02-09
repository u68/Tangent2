################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include vm/subdir.mk
-include tui/subdir.mk
-include media/subdir.mk
-include desktop/subdir.mk
-include debug/subdir.mk
-include subdir.mk
-include objects.mk

-include ../makefile.defs

# Export environment variables
export DCL=C:/LAPIS/LEXIDE/Dcl
export INCLU8=C:/LAPIS/LEXIDE/Inc;C:/LAPIS/LEXIDE/BuildTools/Ver.20251017/Inc/U16
export LIBU8=C:/LAPIS/LEXIDE/BuildTools/Ver.20251017/Lib/U16
export PATH=C:/LAPIS/LEXIDE/Bin;C:/LAPIS/LEXIDE/BuildTools/Ver.20251017/Bin;C:/LAPIS/LEXIDE/Utilities/Bin

# Add inputs and outputs from these tool invocations to the build variables 
OBJHEX += \
tui.HEX \

ABS += \
tui.abs \

MAP += \
tui.map \


# All Target
all: $(OBJHEX)

# Tool invocations
tui.HEX: $(ABS)
	@echo 'Building target: $@'
	@echo 'Invoking: Object converter'
	CnvOHU8 -H $(ABS) "tui.HEX";
	@echo 'Finished building target: $@'
	@echo ' '

tui.abs: $(OBJS)
	@echo 'Invoking: Linker'
	rlu8 -D -SD -NS -CC -ABW $(OBJS__QUOTED),"tui.abs",,"PLU16100LWF.LIB";
	@echo 'Finished building: $@'
	@echo ' '

tui.map: tui.abs $(OBJS)

# Other Targets
clean:
	-$(RM) "./debug/bsod.prn" "./debug/debug.prn" "./desktop/desktop.prn" "./libcw.prn" "./main.prn" "./startup.prn" "./media/media.prn" "./tui/glib.prn" "./vm/vm.prn" "./tuiRA.log" "./debug/bsod.asm" "./debug/debug.asm" "./desktop/desktop.asm" "./libcw.asm" "./main.asm" "./media/media.asm" "./tui/glib.asm" "./vm/vm.asm" "./debug/bsod.i" "./debug/debug.i" "./desktop/desktop.i" "./libcw.i" "./main.i" "./media/media.i" "./tui/glib.i" "./vm/vm.i" "./tui.abs" "./tuiRB.s" "./tuiRA.s" "./debug/bsod.res" "./debug/debug.res" "./desktop/desktop.res" "./libcw.res" "./main.res" "./media/media.res" "./tui/glib.res" "./vm/vm.res" "./tui.HEX" "./debug/bsod.obj" "./debug/debug.obj" "./desktop/desktop.obj" "./libcw.obj" "./main.obj" "./startup.obj" "./media/media.obj" "./tui/glib.obj" "./vm/vm.obj" "./tui.map" "./tui.sc" 
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
