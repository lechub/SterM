################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/_write.c 

CPP_SRCS += \
../src/HBridge.cpp \
../src/HMI.cpp \
../src/Hamulec.cpp \
../src/Hardware.cpp \
../src/Menu.cpp \
../src/Praca.cpp \
../src/Silnik230VAC.cpp \
../src/Silnik24VDC.cpp \
../src/SilnikNapedu.cpp \
../src/Sterownik.cpp \
../src/main.cpp 

OBJS += \
./src/HBridge.o \
./src/HMI.o \
./src/Hamulec.o \
./src/Hardware.o \
./src/Menu.o \
./src/Praca.o \
./src/Silnik230VAC.o \
./src/Silnik24VDC.o \
./src/SilnikNapedu.o \
./src/Sterownik.o \
./src/_write.o \
./src/main.o 

C_DEPS += \
./src/_write.d 

CPP_DEPS += \
./src/HBridge.d \
./src/HMI.d \
./src/Hamulec.d \
./src/Hardware.d \
./src/Menu.d \
./src/Praca.d \
./src/Silnik230VAC.d \
./src/Silnik24VDC.d \
./src/SilnikNapedu.d \
./src/Sterownik.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m0 -mthumb -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DNDEBUG -DSTM32F072 -DHSE_VALUE=8000000 -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../lbdrivers -I../src -std=gnu++14 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -Wabi -Wctor-dtor-privacy -Wnoexcept -Wnon-virtual-dtor -Wstrict-null-sentinel -Wsign-promo -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DNDEBUG -DSTM32F072 -DHSE_VALUE=8000000 -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../lbdrivers -I../src -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


