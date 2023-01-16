################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app_module.c \
../src/mqtt_module.c \
../src/stat_module.c \
../src/ubus_module.c \
../src/uci_module.c \
../src/wimark_test_exercise.c 

C_DEPS += \
./src/app_module.d \
./src/mqtt_module.d \
./src/stat_module.d \
./src/ubus_module.d \
./src/uci_module.d \
./src/wimark_test_exercise.d 

OBJS += \
./src/app_module.o \
./src/mqtt_module.o \
./src/stat_module.o \
./src/ubus_module.o \
./src/uci_module.o \
./src/wimark_test_exercise.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/vanaluk/Work/test_exercise/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/app_module.d ./src/app_module.o ./src/mqtt_module.d ./src/mqtt_module.o ./src/stat_module.d ./src/stat_module.o ./src/ubus_module.d ./src/ubus_module.o ./src/uci_module.d ./src/uci_module.o ./src/wimark_test_exercise.d ./src/wimark_test_exercise.o

.PHONY: clean-src

