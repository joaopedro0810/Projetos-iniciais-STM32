################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Projeto 01 - FDCAN 1 Loopback
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Projeto\ 01\ -\ FDCAN\ 1\ Loopback.elf \

MAP_FILES += \
Projeto\ 01\ -\ FDCAN\ 1\ Loopback.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Projeto\ 01\ -\ FDCAN\ 1\ Loopback.list \


# All Target
all: main-build

# Main-build Target
main-build: Projeto\ 01\ -\ FDCAN\ 1\ Loopback.elf secondary-outputs

# Tool invocations
Projeto\ 01\ -\ FDCAN\ 1\ Loopback.elf Projeto\ 01\ -\ FDCAN\ 1\ Loopback.map: $(OBJS) $(USER_OBJS) C:\Users\joaop\Documents\Workspace\ STM\Workspace\ Estudo\Projeto\ 01\ -\ FDCAN\ 1\ Loopback\STM32H7A3ZITXQ_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Projeto 01 - FDCAN 1 Loopback.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"C:\Users\joaop\Documents\Workspace STM\Workspace Estudo\Projeto 01 - FDCAN 1 Loopback\STM32H7A3ZITXQ_FLASH.ld" --specs=nosys.specs -Wl,-Map="Projeto 01 - FDCAN 1 Loopback.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Projeto\ 01\ -\ FDCAN\ 1\ Loopback.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Projeto 01 - FDCAN 1 Loopback.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Projeto\ 01\ -\ FDCAN\ 1\ Loopback.elf Projeto\ 01\ -\ FDCAN\ 1\ Loopback.list Projeto\ 01\ -\ FDCAN\ 1\ Loopback.map Projeto\\ 01\\ -\\ FDCAN\\ 1\\ Loopback.elf default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
