################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
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


BUILD_ARTIFACT_NAME := Youssef_Ahmed_85_Final_Project_HMI_ECU
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Youssef_Ahmed_85_Final_Project_HMI_ECU.lss \

FLASH_IMAGE += \
Youssef_Ahmed_85_Final_Project_HMI_ECU.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: main-build

# Main-build Target
main-build: Youssef_Ahmed_85_Final_Project_HMI_ECU.elf secondary-outputs

# Tool invocations
Youssef_Ahmed_85_Final_Project_HMI_ECU.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Youssef_Ahmed_85_Final_Project_HMI_ECU.map -mmcu=atmega32 -o "Youssef_Ahmed_85_Final_Project_HMI_ECU.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Youssef_Ahmed_85_Final_Project_HMI_ECU.lss: Youssef_Ahmed_85_Final_Project_HMI_ECU.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Youssef_Ahmed_85_Final_Project_HMI_ECU.elf  >"Youssef_Ahmed_85_Final_Project_HMI_ECU.lss"
	@echo 'Finished building: $@'
	@echo ' '

Youssef_Ahmed_85_Final_Project_HMI_ECU.hex: Youssef_Ahmed_85_Final_Project_HMI_ECU.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Youssef_Ahmed_85_Final_Project_HMI_ECU.elf  "Youssef_Ahmed_85_Final_Project_HMI_ECU.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Youssef_Ahmed_85_Final_Project_HMI_ECU.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 Youssef_Ahmed_85_Final_Project_HMI_ECU.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Youssef_Ahmed_85_Final_Project_HMI_ECU.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents main-build

-include ../makefile.targets
