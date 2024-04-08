################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/application.c \
../src/gpio.c \
../src/int2.c \
../src/keypad.c \
../src/lcd.c \
../src/main.c \
../src/timer1.c \
../src/uart.c 

OBJS += \
./src/application.o \
./src/gpio.o \
./src/int2.o \
./src/keypad.o \
./src/lcd.o \
./src/main.o \
./src/timer1.o \
./src/uart.o 

C_DEPS += \
./src/application.d \
./src/gpio.d \
./src/int2.d \
./src/keypad.d \
./src/lcd.d \
./src/main.d \
./src/timer1.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\Youssef\Desktop\Youssef_Ahmed_85_Final_Project\Youssef_Ahmed_85_Final_Project_HMI_ECU\inc" -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


