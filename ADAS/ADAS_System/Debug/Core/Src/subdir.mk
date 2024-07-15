################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BLU.c \
../Core/Src/BUZZER_program.c \
../Core/Src/Communication.c \
../Core/Src/DC_Motor.c \
../Core/Src/LCD_program.c \
../Core/Src/LDR.c \
../Core/Src/LED_program.c \
../Core/Src/LIne_Detector.c \
../Core/Src/LM35.c \
../Core/Src/UltraSonic_program_.c \
../Core/Src/adc.c \
../Core/Src/freertos.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/main.c \
../Core/Src/mpu6050.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/BLU.o \
./Core/Src/BUZZER_program.o \
./Core/Src/Communication.o \
./Core/Src/DC_Motor.o \
./Core/Src/LCD_program.o \
./Core/Src/LDR.o \
./Core/Src/LED_program.o \
./Core/Src/LIne_Detector.o \
./Core/Src/LM35.o \
./Core/Src/UltraSonic_program_.o \
./Core/Src/adc.o \
./Core/Src/freertos.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/main.o \
./Core/Src/mpu6050.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/BLU.d \
./Core/Src/BUZZER_program.d \
./Core/Src/Communication.d \
./Core/Src/DC_Motor.d \
./Core/Src/LCD_program.d \
./Core/Src/LDR.d \
./Core/Src/LED_program.d \
./Core/Src/LIne_Detector.d \
./Core/Src/LM35.d \
./Core/Src/UltraSonic_program_.d \
./Core/Src/adc.d \
./Core/Src/freertos.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/main.d \
./Core/Src/mpu6050.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/BLU.cyclo ./Core/Src/BLU.d ./Core/Src/BLU.o ./Core/Src/BLU.su ./Core/Src/BUZZER_program.cyclo ./Core/Src/BUZZER_program.d ./Core/Src/BUZZER_program.o ./Core/Src/BUZZER_program.su ./Core/Src/Communication.cyclo ./Core/Src/Communication.d ./Core/Src/Communication.o ./Core/Src/Communication.su ./Core/Src/DC_Motor.cyclo ./Core/Src/DC_Motor.d ./Core/Src/DC_Motor.o ./Core/Src/DC_Motor.su ./Core/Src/LCD_program.cyclo ./Core/Src/LCD_program.d ./Core/Src/LCD_program.o ./Core/Src/LCD_program.su ./Core/Src/LDR.cyclo ./Core/Src/LDR.d ./Core/Src/LDR.o ./Core/Src/LDR.su ./Core/Src/LED_program.cyclo ./Core/Src/LED_program.d ./Core/Src/LED_program.o ./Core/Src/LED_program.su ./Core/Src/LIne_Detector.cyclo ./Core/Src/LIne_Detector.d ./Core/Src/LIne_Detector.o ./Core/Src/LIne_Detector.su ./Core/Src/LM35.cyclo ./Core/Src/LM35.d ./Core/Src/LM35.o ./Core/Src/LM35.su ./Core/Src/UltraSonic_program_.cyclo ./Core/Src/UltraSonic_program_.d ./Core/Src/UltraSonic_program_.o ./Core/Src/UltraSonic_program_.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mpu6050.cyclo ./Core/Src/mpu6050.d ./Core/Src/mpu6050.o ./Core/Src/mpu6050.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

