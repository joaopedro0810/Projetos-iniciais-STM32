################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QP/ports/freertos/qf_port.c 

OBJS += \
./QP/ports/freertos/qf_port.o 

C_DEPS += \
./QP/ports/freertos/qf_port.d 


# Each subdirectory must supply rules for building sources it contributes
QP/ports/freertos/%.o QP/ports/freertos/%.su: ../QP/ports/freertos/%.c QP/ports/freertos/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto05 - Port para FreeRTOS/QP/include" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto05 - Port para FreeRTOS/QP/ports/freertos" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto05 - Port para FreeRTOS/QP/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QP-2f-ports-2f-freertos

clean-QP-2f-ports-2f-freertos:
	-$(RM) ./QP/ports/freertos/qf_port.d ./QP/ports/freertos/qf_port.o ./QP/ports/freertos/qf_port.su

.PHONY: clean-QP-2f-ports-2f-freertos

