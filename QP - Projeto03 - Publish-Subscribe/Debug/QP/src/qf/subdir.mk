################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QP/src/qf/qep_hsm.c \
../QP/src/qf/qep_msm.c \
../QP/src/qf/qf_act.c \
../QP/src/qf/qf_actq.c \
../QP/src/qf/qf_defer.c \
../QP/src/qf/qf_dyn.c \
../QP/src/qf/qf_mem.c \
../QP/src/qf/qf_ps.c \
../QP/src/qf/qf_qact.c \
../QP/src/qf/qf_qeq.c \
../QP/src/qf/qf_qmact.c \
../QP/src/qf/qf_time.c 

OBJS += \
./QP/src/qf/qep_hsm.o \
./QP/src/qf/qep_msm.o \
./QP/src/qf/qf_act.o \
./QP/src/qf/qf_actq.o \
./QP/src/qf/qf_defer.o \
./QP/src/qf/qf_dyn.o \
./QP/src/qf/qf_mem.o \
./QP/src/qf/qf_ps.o \
./QP/src/qf/qf_qact.o \
./QP/src/qf/qf_qeq.o \
./QP/src/qf/qf_qmact.o \
./QP/src/qf/qf_time.o 

C_DEPS += \
./QP/src/qf/qep_hsm.d \
./QP/src/qf/qep_msm.d \
./QP/src/qf/qf_act.d \
./QP/src/qf/qf_actq.d \
./QP/src/qf/qf_defer.d \
./QP/src/qf/qf_dyn.d \
./QP/src/qf/qf_mem.d \
./QP/src/qf/qf_ps.d \
./QP/src/qf/qf_qact.d \
./QP/src/qf/qf_qeq.d \
./QP/src/qf/qf_qmact.d \
./QP/src/qf/qf_time.d 


# Each subdirectory must supply rules for building sources it contributes
QP/src/qf/%.o QP/src/qf/%.su: ../QP/src/qf/%.c QP/src/qf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto03 - Publish-Subscribe/QP/include" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto03 - Publish-Subscribe/QP/ports/arm-cm/qv/gnu" -I"C:/Users/joaop/Documents/WorkspaceEstudo/QP - Projeto03 - Publish-Subscribe/QP/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QP-2f-src-2f-qf

clean-QP-2f-src-2f-qf:
	-$(RM) ./QP/src/qf/qep_hsm.d ./QP/src/qf/qep_hsm.o ./QP/src/qf/qep_hsm.su ./QP/src/qf/qep_msm.d ./QP/src/qf/qep_msm.o ./QP/src/qf/qep_msm.su ./QP/src/qf/qf_act.d ./QP/src/qf/qf_act.o ./QP/src/qf/qf_act.su ./QP/src/qf/qf_actq.d ./QP/src/qf/qf_actq.o ./QP/src/qf/qf_actq.su ./QP/src/qf/qf_defer.d ./QP/src/qf/qf_defer.o ./QP/src/qf/qf_defer.su ./QP/src/qf/qf_dyn.d ./QP/src/qf/qf_dyn.o ./QP/src/qf/qf_dyn.su ./QP/src/qf/qf_mem.d ./QP/src/qf/qf_mem.o ./QP/src/qf/qf_mem.su ./QP/src/qf/qf_ps.d ./QP/src/qf/qf_ps.o ./QP/src/qf/qf_ps.su ./QP/src/qf/qf_qact.d ./QP/src/qf/qf_qact.o ./QP/src/qf/qf_qact.su ./QP/src/qf/qf_qeq.d ./QP/src/qf/qf_qeq.o ./QP/src/qf/qf_qeq.su ./QP/src/qf/qf_qmact.d ./QP/src/qf/qf_qmact.o ./QP/src/qf/qf_qmact.su ./QP/src/qf/qf_time.d ./QP/src/qf/qf_time.o ./QP/src/qf/qf_time.su

.PHONY: clean-QP-2f-src-2f-qf

