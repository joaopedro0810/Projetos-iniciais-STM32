################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/GPIO/gpio.c 

OBJS += \
./BSP/GPIO/gpio.o 

C_DEPS += \
./BSP/GPIO/gpio.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/GPIO/%.o BSP/GPIO/%.su: ../BSP/GPIO/%.c BSP/GPIO/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/joaop/Documents/Workspace STM/Workspace Estudo/QP - Projeto04 - Eventos dinamicos/QP/include" -I"C:/Users/joaop/Documents/Workspace STM/Workspace Estudo/QP - Projeto04 - Eventos dinamicos/QP/ports/arm-cm/qv/gnu" -I"C:/Users/joaop/Documents/Workspace STM/Workspace Estudo/QP - Projeto04 - Eventos dinamicos/QP/src" -I"C:/Users/joaop/Documents/Workspace STM/Workspace Estudo/QP - Projeto04 - Eventos dinamicos/BSP/GPIO" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-GPIO

clean-BSP-2f-GPIO:
	-$(RM) ./BSP/GPIO/gpio.d ./BSP/GPIO/gpio.o ./BSP/GPIO/gpio.su

.PHONY: clean-BSP-2f-GPIO

