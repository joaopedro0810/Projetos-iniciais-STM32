################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/Components/stmpe1600/stmpe1600.c \
C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/Components/stmpe811/stmpe811.c \
C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/Components/ts3510/ts3510.c 

OBJS += \
./Drivers/BSP/Components/stmpe1600.o \
./Drivers/BSP/Components/stmpe811.o \
./Drivers/BSP/Components/ts3510.o 

C_DEPS += \
./Drivers/BSP/Components/stmpe1600.d \
./Drivers/BSP/Components/stmpe811.d \
./Drivers/BSP/Components/ts3510.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/stmpe1600.o: C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/Components/stmpe1600/stmpe1600.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM324x9I_EVAL -c -I../../../Inc -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM324x9I_EVAL -I../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -Wno-unused-variable -Wno-pointer-sign -Wno-main -Wno-format -Wno-address -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-parentheses -Wno-missing-braces -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/stmpe1600.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/stmpe811.o: C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/Components/stmpe811/stmpe811.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM324x9I_EVAL -c -I../../../Inc -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM324x9I_EVAL -I../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -Wno-unused-variable -Wno-pointer-sign -Wno-main -Wno-format -Wno-address -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-parentheses -Wno-missing-braces -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/stmpe811.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/ts3510.o: C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/Components/ts3510/ts3510.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM324x9I_EVAL -c -I../../../Inc -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM324x9I_EVAL -I../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -Wno-unused-variable -Wno-pointer-sign -Wno-main -Wno-format -Wno-address -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-parentheses -Wno-missing-braces -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/ts3510.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/stmpe1600.d ./Drivers/BSP/Components/stmpe1600.o ./Drivers/BSP/Components/stmpe1600.su ./Drivers/BSP/Components/stmpe811.d ./Drivers/BSP/Components/stmpe811.o ./Drivers/BSP/Components/stmpe811.su ./Drivers/BSP/Components/ts3510.d ./Drivers/BSP/Components/ts3510.o ./Drivers/BSP/Components/ts3510.su

.PHONY: clean-Drivers-2f-BSP-2f-Components

