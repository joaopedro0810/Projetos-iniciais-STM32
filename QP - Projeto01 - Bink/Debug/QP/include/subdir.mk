################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QP/include/qstamp.c 

OBJS += \
./QP/include/qstamp.o 

C_DEPS += \
./QP/include/qstamp.d 


# Each subdirectory must supply rules for building sources it contributes
QP/include/%.o QP/include/%.su: ../QP/include/%.c QP/include/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/ST/Workspace STM/Workspace Estudo/Projetos-iniciais-STM32/QP - Projeto01 - Bink/QP/include" -I"C:/ST/Workspace STM/Workspace Estudo/Projetos-iniciais-STM32/QP - Projeto01 - Bink/QP/ports/arm-cm/qv/gnu" -I"C:/ST/Workspace STM/Workspace Estudo/Projetos-iniciais-STM32/QP - Projeto01 - Bink/QP/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QP-2f-include

clean-QP-2f-include:
	-$(RM) ./QP/include/qstamp.d ./QP/include/qstamp.o ./QP/include/qstamp.su

.PHONY: clean-QP-2f-include

