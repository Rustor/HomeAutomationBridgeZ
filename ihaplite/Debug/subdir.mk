################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../auxfunctions.c \
../iptask.c \
../msghandler.c \
../parser.c \
../sessionmanager.c \
../tcpclient.c \
../tcpserver.c \
../timemanager.c \
../udpserver.c \
../utils.c 

OBJS += \
./auxfunctions.o \
./iptask.o \
./msghandler.o \
./parser.o \
./sessionmanager.o \
./tcpclient.o \
./tcpserver.o \
./timemanager.o \
./udpserver.o \
./utils.o 

C_DEPS += \
./auxfunctions.d \
./iptask.d \
./msghandler.d \
./parser.d \
./sessionmanager.d \
./tcpclient.d \
./tcpserver.d \
./timemanager.d \
./udpserver.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D__USE_GNU -D_GNU_SOURCE -D__UNIX__ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


