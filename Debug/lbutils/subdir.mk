################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lbutils/eeprom.c \
../lbutils/myUtils.c \
../lbutils/stm32f0xx_flash.c 

CPP_SRCS += \
../lbutils/Fifo.cpp \
../lbutils/FrameBuffer.cpp \
../lbutils/QuickTask.cpp \
../lbutils/RTClock.cpp \
../lbutils/SoftStart.cpp \
../lbutils/VEprom.cpp 

OBJS += \
./lbutils/Fifo.o \
./lbutils/FrameBuffer.o \
./lbutils/QuickTask.o \
./lbutils/RTClock.o \
./lbutils/SoftStart.o \
./lbutils/VEprom.o \
./lbutils/eeprom.o \
./lbutils/myUtils.o \
./lbutils/stm32f0xx_flash.o 

C_DEPS += \
./lbutils/eeprom.d \
./lbutils/myUtils.d \
./lbutils/stm32f0xx_flash.d 

CPP_DEPS += \
./lbutils/Fifo.d \
./lbutils/FrameBuffer.d \
./lbutils/QuickTask.d \
./lbutils/RTClock.d \
./lbutils/SoftStart.d \
./lbutils/VEprom.d 


# Each subdirectory must supply rules for building sources it contributes
lbutils/%.o: ../lbutils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m0 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F072 -DHSE_VALUE=8000000 -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../lbdrivers -I../src -std=gnu++14 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -Wabi -Wctor-dtor-privacy -Wnoexcept -Wnon-virtual-dtor -Wstrict-null-sentinel -Wsign-promo -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lbutils/%.o: ../lbutils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F072 -DHSE_VALUE=8000000 -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../lbdrivers -I../src -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


