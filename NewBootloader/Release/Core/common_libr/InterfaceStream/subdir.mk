################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/common_libr/InterfaceStream/InterfaceStream.cpp 

OBJS += \
./Core/common_libr/InterfaceStream/InterfaceStream.o 

CPP_DEPS += \
./Core/common_libr/InterfaceStream/InterfaceStream.d 


# Each subdirectory must supply rules for building sources it contributes
Core/common_libr/InterfaceStream/%.o Core/common_libr/InterfaceStream/%.su Core/common_libr/InterfaceStream/%.cyclo: ../Core/common_libr/InterfaceStream/%.cpp Core/common_libr/InterfaceStream/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"/home/ashish/Practice/NewBootloader/Core/common_libr/fifo" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/InterfaceStream" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/as_pkt" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/buffering" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/crc" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/InterfaceIO" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-common_libr-2f-InterfaceStream

clean-Core-2f-common_libr-2f-InterfaceStream:
	-$(RM) ./Core/common_libr/InterfaceStream/InterfaceStream.cyclo ./Core/common_libr/InterfaceStream/InterfaceStream.d ./Core/common_libr/InterfaceStream/InterfaceStream.o ./Core/common_libr/InterfaceStream/InterfaceStream.su

.PHONY: clean-Core-2f-common_libr-2f-InterfaceStream

