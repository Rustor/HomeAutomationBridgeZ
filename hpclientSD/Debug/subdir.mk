################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HP_client.c \
../avahi_browse.c \
../avahi_browse_api.c \
../curl_fct.c \
../mainSD.c \
../request_service.c \
../xmlAPI.c 

OBJS += \
./HP_client.o \
./avahi_browse.o \
./avahi_browse_api.o \
./curl_fct.o \
./mainSD.o \
./request_service.o \
./xmlAPI.o 

C_DEPS += \
./HP_client.d \
./avahi_browse.d \
./avahi_browse_api.d \
./curl_fct.d \
./mainSD.d \
./request_service.d \
./xmlAPI.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DHPD_HTTP -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


