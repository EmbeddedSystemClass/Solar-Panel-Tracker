################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L5_Application/source/handlers.cpp \
../L5_Application/source/high_level_init.cpp \
../L5_Application/source/tasks.cpp 

C_SRCS += \
../L5_Application/source/misc.c 

OBJS += \
./L5_Application/source/handlers.o \
./L5_Application/source/high_level_init.o \
./L5_Application/source/misc.o \
./L5_Application/source/tasks.o 

C_DEPS += \
./L5_Application/source/misc.d 

CPP_DEPS += \
./L5_Application/source/handlers.d \
./L5_Application/source/high_level_init.d \
./L5_Application/source/tasks.d 


# Each subdirectory must supply rules for building sources it contributes
L5_Application/source/%.o: ../L5_Application/source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C++ Compiler'
	arm-none-eabi-g++ -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L0_LowLevel" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L1_FreeRTOS\include" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L1_FreeRTOS\portable" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L2_Drivers" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L3_Utils" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L3_Utils\tlm" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L4_IO" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L4_IO\fat" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L5_Application" -Os -Wall -Wa,-adhlns="$@.lst" -fno-exceptions -fno-rtti -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

L5_Application/source/%.o: ../L5_Application/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L0_LowLevel" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L1_FreeRTOS/include" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L1_FreeRTOS\portable" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L2_Drivers" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L3_Utils" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L3_Utils\tlm" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L4_IO" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L4_IO\fat" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS_final_solar\L5_Application" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


