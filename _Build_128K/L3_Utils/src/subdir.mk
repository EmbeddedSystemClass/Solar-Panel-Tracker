################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L3_Utils/src/command_handler.cpp \
../L3_Utils/src/cpp_task.cpp \
../L3_Utils/src/file_logger.cpp \
../L3_Utils/src/freertos_timer.cpp \
../L3_Utils/src/logger.cpp \
../L3_Utils/src/str.cpp \
../L3_Utils/src/utilities.cpp 

C_SRCS += \
../L3_Utils/src/rit_isr.c \
../L3_Utils/src/rtc_alarm.c 

OBJS += \
./L3_Utils/src/command_handler.o \
./L3_Utils/src/cpp_task.o \
./L3_Utils/src/file_logger.o \
./L3_Utils/src/freertos_timer.o \
./L3_Utils/src/logger.o \
./L3_Utils/src/rit_isr.o \
./L3_Utils/src/rtc_alarm.o \
./L3_Utils/src/str.o \
./L3_Utils/src/utilities.o 

C_DEPS += \
./L3_Utils/src/rit_isr.d \
./L3_Utils/src/rtc_alarm.d 

CPP_DEPS += \
./L3_Utils/src/command_handler.d \
./L3_Utils/src/cpp_task.d \
./L3_Utils/src/file_logger.d \
./L3_Utils/src/freertos_timer.d \
./L3_Utils/src/logger.d \
./L3_Utils/src/str.d \
./L3_Utils/src/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
L3_Utils/src/%.o: ../L3_Utils/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C++ Compiler'
	arm-none-eabi-g++ -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L0_LowLevel" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L1_FreeRTOS\include" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L1_FreeRTOS\portable" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L2_Drivers" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L3_Utils" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L3_Utils\tlm" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L4_IO" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L4_IO\fat" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L5_Application" -Os -Wall -Wa,-adhlns="$@.lst" -fno-exceptions -fno-rtti -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

L3_Utils/src/%.o: ../L3_Utils/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L0_LowLevel" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L1_FreeRTOS/include" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L1_FreeRTOS\portable" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L2_Drivers" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L3_Utils" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L3_Utils\tlm" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L4_IO" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L4_IO\fat" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L5_Application" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


