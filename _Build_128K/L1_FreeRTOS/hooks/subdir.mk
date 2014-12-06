################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L1_FreeRTOS/hooks/hooks.c 

OBJS += \
./L1_FreeRTOS/hooks/hooks.o 

C_DEPS += \
./L1_FreeRTOS/hooks/hooks.d 


# Each subdirectory must supply rules for building sources it contributes
L1_FreeRTOS/hooks/%.o: ../L1_FreeRTOS/hooks/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L0_LowLevel" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L1_FreeRTOS/include" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L1_FreeRTOS\portable" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L2_Drivers" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L3_Utils" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L3_Utils\tlm" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L4_IO" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L4_IO\fat" -I"C:\Users\Montoya\Documents\Classes\CmpE146\Lab\CmpE146Package_LPC1758\projects\lpc17xx_CPP_FreeRTOS\L5_Application" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


