################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
../src/SILABS_STARTUP.A51 

C_SRCS += \
../src/InitDevice.c \
../src/function_generator.c \
../src/main.c 

OBJS += \
./src/InitDevice.OBJ \
./src/SILABS_STARTUP.OBJ \
./src/function_generator.OBJ \
./src/main.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	wine "/Applications/Simplicity Studio.app/Contents/Eclipse/developer/toolchains/keil_8051/9.53/BIN/C51" "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/InitDevice.OBJ: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/EFM8BB3/inc/SI_EFM8BB3_Register_Enums.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/InitDevice.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/function_generator.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/EFM8BB3/inc/SI_EFM8BB3_Defs.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/si_toolchain.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/stdint.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/stdbool.h

src/%.OBJ: ../src/%.A51
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	wine "/Applications/Simplicity Studio.app/Contents/Eclipse/developer/toolchains/keil_8051/9.53/BIN/AX51" "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/function_generator.OBJ: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/bsp/bsp.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_memory_lcd/inc/tick.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_memory_lcd/inc/disp.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_memory_lcd/inc/render.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_joystick/joystick.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_memory_lcd/inc/graphics/thinfont.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/function_generator.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/sine.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/square.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/triangle.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/sawtooth.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/windowed_sine.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/nav_up.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/nav_down.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/nav_left.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/graphics/nav_right.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/waveform_tables.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_retargetserial/retargetserial.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/EFM8BB3_SLSTK2022A/config/bsp_config.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_memory_lcd/inc/config/render_config.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/config/joystick_config.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/toolchains/keil_8051/9.53/INC/STDIO.H /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/EFM8BB3/inc/SI_EFM8BB3_Register_Enums.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/config/memory_lcd_config.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/EFM8BB3/inc/SI_EFM8BB3_Defs.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/si_toolchain.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/stdint.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/stdbool.h

src/main.OBJ: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/bsp/bsp.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/InitDevice.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_memory_lcd/inc/disp.h /Users/liaohu/SimplicityStudio/v4_workspace/Final\ project_1/inc/function_generator.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/common/drivers/efm8_retargetserial/retargetserial.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/kits/EFM8BB3_SLSTK2022A/config/bsp_config.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/toolchains/keil_8051/9.53/INC/STDIO.H /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/EFM8BB3/inc/SI_EFM8BB3_Register_Enums.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/EFM8BB3/inc/SI_EFM8BB3_Defs.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/si_toolchain.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/stdint.h /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/8051/v4.0.3/Device/shared/si8051Base/stdbool.h


