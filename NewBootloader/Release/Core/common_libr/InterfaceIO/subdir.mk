################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/common_libr/InterfaceIO/InterfaceIO.cpp \
../Core/common_libr/InterfaceIO/UartIO.cpp \
../Core/common_libr/InterfaceIO/debug_swd.cpp 

OBJS += \
./Core/common_libr/InterfaceIO/InterfaceIO.o \
./Core/common_libr/InterfaceIO/UartIO.o \
./Core/common_libr/InterfaceIO/debug_swd.o 

CPP_DEPS += \
./Core/common_libr/InterfaceIO/InterfaceIO.d \
./Core/common_libr/InterfaceIO/UartIO.d \
./Core/common_libr/InterfaceIO/debug_swd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/common_libr/InterfaceIO/%.o Core/common_libr/InterfaceIO/%.su Core/common_libr/InterfaceIO/%.cyclo: ../Core/common_libr/InterfaceIO/%.cpp Core/common_libr/InterfaceIO/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"/home/ashish/Practice/NewBootloader/Core/common_libr/fifo" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/InterfaceStream" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/as_pkt" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/buffering" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/crc" -I"/home/ashish/Practice/NewBootloader/Core/common_libr/InterfaceIO" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-common_libr-2f-InterfaceIO

clean-Core-2f-common_libr-2f-InterfaceIO:
	-$(RM) ./Core/common_libr/InterfaceIO/InterfaceIO.cyclo ./Core/common_libr/InterfaceIO/InterfaceIO.d ./Core/common_libr/InterfaceIO/InterfaceIO.o ./Core/common_libr/InterfaceIO/InterfaceIO.su ./Core/common_libr/InterfaceIO/UartIO.cyclo ./Core/common_libr/InterfaceIO/UartIO.d ./Core/common_libr/InterfaceIO/UartIO.o ./Core/common_libr/InterfaceIO/UartIO.su ./Core/common_libr/InterfaceIO/debug_swd.cyclo ./Core/common_libr/InterfaceIO/debug_swd.d ./Core/common_libr/InterfaceIO/debug_swd.o ./Core/common_libr/InterfaceIO/debug_swd.su

.PHONY: clean-Core-2f-common_libr-2f-InterfaceIO

