################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../media/generated_media_asset.c \
../media/media.c 

PRNS += \
./media/generated_media_asset.prn \
./media/media.prn 

PRNS__QUOTED += \
"./media/generated_media_asset.prn" \
"./media/media.prn" 

ASMS += \
./media/generated_media_asset.asm \
./media/media.asm 

ASMS__QUOTED += \
"./media/generated_media_asset.asm" \
"./media/media.asm" 

IS += \
./media/generated_media_asset.i \
./media/media.i 

IS__QUOTED += \
"./media/generated_media_asset.i" \
"./media/media.i" 

RESS += \
./media/generated_media_asset.res \
./media/media.res 

RESS__QUOTED += \
"./media/generated_media_asset.res" \
"./media/media.res" 

OBJS += \
./media/generated_media_asset.obj \
./media/media.obj 

OBJS__QUOTED += \
"./media/generated_media_asset.obj" \
"./media/media.obj" 


# Each subdirectory must supply rules for building sources it contributes
media/generated_media_asset.asm: ../media/generated_media_asset.c
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	lccu16 @"./media/generated_media_asset.res"
	@echo 'Finished building: $<'
	@echo ' '

media/generated_media_asset.res: media/generated_media_asset.asm
media/generated_media_asset.i: media/generated_media_asset.asm

media/generated_media_asset.obj: ./media/generated_media_asset.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Assembler'
	rasu8 -CD -ABW -SD -DF -ML -O"media/" -WRPEAT -PR"media/generated_media_asset.prn" -L -NS -NR -NPL -NPW "$<"
	@echo 'Finished building: $<'
	@echo ' '

media/generated_media_asset.prn: media/generated_media_asset.obj

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


