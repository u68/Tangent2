################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include vm/subdir.mk
-include tui/subdir.mk
-include time/subdir.mk
-include media/subdir.mk
-include heap/subdir.mk
-include fs/subdir.mk
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
tangent.HEX \

ABS += \
tangent.abs \

MAP += \
tangent.map \

# All Target
all: $(OBJHEX)

# Tool invocations
tangent.HEX: $(ABS)
	@echo 'Building target: $@'
	@echo 'Invoking: Object converter'
	CnvOHU8 -H $(ABS) "tangent.HEX";
	@echo 'Finished building target: $@'
	@echo ' '

tangent.abs: $(OBJS)
	@echo 'Invoking: Linker'
	rlu8 -D -SD -NS -CC -ABW $(OBJS__QUOTED),"tangent.abs",,"PLU16100LWF.LIB";
	@echo 'Finished building: $@'
	@echo ' '

tangent.map: tangent.abs $(OBJS)
# Other Targets
clean:
	-$(RM) "./debug/bsod.prn" "./debug/debug.prn" "./desktop/desktop.prn" "./fs/fs.prn" "./heap/heap.prn" "./main.prn" "./startup.prn" "./media/media.prn" "./time/time.prn" "./tui/glib.prn" "./tui/graphics.prn" "./tui/lut.prn" "./tui/ysglfonts.prn" "./vm/vm.prn" "./tuiRA.log" "./debug/bsod.asm" "./debug/debug.asm" "./desktop/desktop.asm" "./fs/fs.asm" "./heap/heap.asm" "./main.asm" "./media/media.asm" "./time/time.asm" "./tui/glib.asm" "./tui/graphics.asm" "./tui/lut.asm" "./tui/ysglfonts.asm" "./vm/vm.asm" "./debug/bsod.i" "./debug/debug.i" "./desktop/desktop.i" "./fs/fs.i" "./heap/heap.i" "./main.i" "./media/media.i" "./time/time.i" "./tui/glib.i" "./tui/graphics.i" "./tui/lut.i" "./tui/ysglfonts.i" "./vm/vm.i" "./tui.abs" "./tuiRB.s" "./tuiRA.s" "./debug/bsod.res" "./debug/debug.res" "./desktop/desktop.res" "./fs/fs.res" "./heap/heap.res" "./main.res" "./media/media.res" "./time/time.res" "./tui/glib.res" "./tui/graphics.res" "./tui/lut.res" "./tui/ysglfonts.res" "./vm/vm.res" "./tui.HEX" "./debug/bsod.obj" 
	-$(RM) "./debug/debug.obj" "./desktop/desktop.obj" "./fs/fs.obj" "./heap/heap.obj" "./main.obj" "./startup.obj" "./media/media.obj" "./time/time.obj" "./tui/glib.obj" "./tui/graphics.obj" "./tui/lut.obj" "./tui/ysglfonts.obj" "./vm/vm.obj" "./tui.map" "./tui.sc" 
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
