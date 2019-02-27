################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lbdrivers/CallbackInterface.cpp \
../lbdrivers/I2C.cpp \
../lbdrivers/Password.cpp \
../lbdrivers/ST7032iFB.cpp 

OBJS += \
./lbdrivers/CallbackInterface.o \
./lbdrivers/I2C.o \
./lbdrivers/Password.o \
./lbdrivers/ST7032iFB.o 

CPP_DEPS += \
./lbdrivers/CallbackInterface.d \
./lbdrivers/I2C.d \
./lbdrivers/Password.d \
./lbdrivers/ST7032iFB.d 


# Each subdirectory must supply rules for building sources it contributes
lbdrivers/%.o: ../lbdrivers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m0 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F072 -DHSE_VALUE=8000000 -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../lbdrivers -I../src -std=gnu++14 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -Wabi -Wctor-dtor-privacy -Wnoexcept -Wnon-virtual-dtor -Wstrict-null-sentinel -Wsign-promo -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


