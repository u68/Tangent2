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
-include input/subdir.mk
-include desktop/subdir.mk
-include debug/subdir.mk
-include c_apps/text_editor/subdir.mk
-include c_apps/paint/subdir.mk
-include c_apps/media_viewer/subdir.mk
-include c_apps/games/snake/subdir.mk
-include c_apps/games/minesweeper/subdir.mk
-include c_apps/games/ball/subdir.mk
-include c_apps/games/2048/subdir.mk
-include c_apps/fs_man/subdir.mk
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
	rlu8 -D -SD -NS -CC -ABW $(OBJS__QUOTED),"tui.abs",,"PLU16100LWFPK.LIB";
	@echo 'Finished building: $@'
	@echo ' '

tui.map: tui.abs $(OBJS)

# Other Targets
clean:
	-$(RM) "./c_apps/fs_man/fs_man.prn" "./c_apps/games/2048/2048.prn" "./c_apps/games/ball/ball.prn" "./c_apps/games/minesweeper/minesweeper.prn" "./c_apps/games/snake/snake.prn" "./c_apps/media_viewer/media_view.prn" "./c_apps/paint/paint.prn" "./c_apps/text_editor/text_edit.prn" "./debug/bsod.prn" "./debug/debug.prn" "./desktop/desktop.prn" "./input/input.prn" "./libcw.prn" "./main.prn" "./startup.prn" "./media/generated_media_asset.prn" "./media/media.prn" "./tui/glib.prn" "./vm/vm.prn" "./tuiRA.log" "./c_apps/fs_man/fs_man.asm" "./c_apps/games/2048/2048.asm" "./c_apps/games/ball/ball.asm" "./c_apps/games/minesweeper/minesweeper.asm" "./c_apps/games/snake/snake.asm" "./c_apps/media_viewer/media_view.asm" "./c_apps/paint/paint.asm" "./c_apps/text_editor/text_edit.asm" "./debug/bsod.asm" "./debug/debug.asm" "./desktop/desktop.asm" "./input/input.asm" "./libcw.asm" "./main.asm" "./media/generated_media_asset.asm" "./media/media.asm" "./tui/glib.asm" "./vm/vm.asm" "./c_apps/fs_man/fs_man.i" "./c_apps/games/2048/2048.i" 
	-$(RM) "./c_apps/games/ball/ball.i" "./c_apps/games/minesweeper/minesweeper.i" "./c_apps/games/snake/snake.i" "./c_apps/media_viewer/media_view.i" "./c_apps/paint/paint.i" "./c_apps/text_editor/text_edit.i" "./debug/bsod.i" "./debug/debug.i" "./desktop/desktop.i" "./input/input.i" "./libcw.i" "./main.i" "./media/generated_media_asset.i" "./media/media.i" "./tui/glib.i" "./vm/vm.i" "./tui.abs" "./tuiRB.s" "./tuiRA.s" "./c_apps/fs_man/fs_man.res" "./c_apps/games/2048/2048.res" "./c_apps/games/ball/ball.res" "./c_apps/games/minesweeper/minesweeper.res" "./c_apps/games/snake/snake.res" "./c_apps/media_viewer/media_view.res" "./c_apps/paint/paint.res" "./c_apps/text_editor/text_edit.res" "./debug/bsod.res" "./debug/debug.res" "./desktop/desktop.res" "./input/input.res" "./libcw.res" "./main.res" "./media/generated_media_asset.res" "./media/media.res" "./tui/glib.res" "./vm/vm.res" "./tui.HEX" "./c_apps/fs_man/fs_man.obj" "./c_apps/games/2048/2048.obj" "./c_apps/games/ball/ball.obj" "./c_apps/games/minesweeper/minesweeper.obj" 
	-$(RM) "./c_apps/games/snake/snake.obj" "./c_apps/media_viewer/media_view.obj" "./c_apps/paint/paint.obj" "./c_apps/text_editor/text_edit.obj" "./debug/bsod.obj" "./debug/debug.obj" "./desktop/desktop.obj" "./input/input.obj" "./libcw.obj" "./main.obj" "./startup.obj" "./media/generated_media_asset.obj" "./media/media.obj" "./tui/glib.obj" "./vm/vm.obj" "./tui.map" "./tui.sc" 
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
