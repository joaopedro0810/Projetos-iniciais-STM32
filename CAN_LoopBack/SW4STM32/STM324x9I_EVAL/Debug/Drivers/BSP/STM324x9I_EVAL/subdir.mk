################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.c \
C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.c 

OBJS += \
./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.o \
./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.o 

C_DEPS += \
./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.d \
./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.o: C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.c Drivers/BSP/STM324x9I_EVAL/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM324x9I_EVAL -c -I../../../Inc -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM324x9I_EVAL -I../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -Wno-unused-variable -Wno-pointer-sign -Wno-main -Wno-format -Wno-address -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-parentheses -Wno-missing-braces -fstack-usage -MMD -MP -MF"Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.o: C:/Users/joaop/Documents/Workspace\ STM/Workspace\ Estudo/CAN_LoopBack/Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.c Drivers/BSP/STM324x9I_EVAL/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM324x9I_EVAL -c -I../../../Inc -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM324x9I_EVAL -I../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -Wno-unused-variable -Wno-pointer-sign -Wno-main -Wno-format -Wno-address -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-parentheses -Wno-missing-braces -fstack-usage -MMD -MP -MF"Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM324x9I_EVAL

clean-Drivers-2f-BSP-2f-STM324x9I_EVAL:
	-$(RM) ./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.d ./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.o ./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval.su ./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.d ./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.o ./Drivers/BSP/STM324x9I_EVAL/stm324x9i_eval_io.su

.PHONY: clean-Drivers-2f-BSP-2f-STM324x9I_EVAL

