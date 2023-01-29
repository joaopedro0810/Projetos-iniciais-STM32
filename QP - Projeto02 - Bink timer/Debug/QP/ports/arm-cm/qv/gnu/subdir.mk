################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QP/ports/arm-cm/qv/gnu/qv_port.c 

OBJS += \
./QP/ports/arm-cm/qv/gnu/qv_port.o 

C_DEPS += \
./QP/ports/arm-cm/qv/gnu/qv_port.d 


# Each subdirectory must supply rules for building sources it contributes
QP/ports/arm-cm/qv/gnu/%.o QP/ports/arm-cm/qv/gnu/%.su: ../QP/ports/arm-cm/qv/gnu/%.c QP/ports/arm-cm/qv/gnu/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto02 - Bink timer/QP/include" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto02 - Bink timer/QP/ports/arm-cm/qv/gnu" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto02 - Bink timer/QP/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QP-2f-ports-2f-arm-2d-cm-2f-qv-2f-gnu

clean-QP-2f-ports-2f-arm-2d-cm-2f-qv-2f-gnu:
	-$(RM) ./QP/ports/arm-cm/qv/gnu/qv_port.d ./QP/ports/arm-cm/qv/gnu/qv_port.o ./QP/ports/arm-cm/qv/gnu/qv_port.su

.PHONY: clean-QP-2f-ports-2f-arm-2d-cm-2f-qv-2f-gnu

