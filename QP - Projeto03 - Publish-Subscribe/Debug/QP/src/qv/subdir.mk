################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QP/src/qv/qv.c 

OBJS += \
./QP/src/qv/qv.o 

C_DEPS += \
./QP/src/qv/qv.d 


# Each subdirectory must supply rules for building sources it contributes
QP/src/qv/%.o QP/src/qv/%.su: ../QP/src/qv/%.c QP/src/qv/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto03 - Publish-Subscribe/QP/include" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto03 - Publish-Subscribe/QP/ports/arm-cm/qv/gnu" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto03 - Publish-Subscribe/QP/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QP-2f-src-2f-qv

clean-QP-2f-src-2f-qv:
	-$(RM) ./QP/src/qv/qv.d ./QP/src/qv/qv.o ./QP/src/qv/qv.su

.PHONY: clean-QP-2f-src-2f-qv

