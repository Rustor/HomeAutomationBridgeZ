################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hpdaemon/homeport.c \
../hpdaemon/hpd_avahi_publish.c \
../hpdaemon/hpd_avahi_publish_core.c \
../hpdaemon/hpd_configure.c \
../hpdaemon/hpd_events.c \
../hpdaemon/hpd_http_web_server.c \
../hpdaemon/hpd_log.c \
../hpdaemon/hpd_parameters.c \
../hpdaemon/hpd_server_sent_events.c \
../hpdaemon/hpd_services.c \
../hpdaemon/hpd_web_server.c \
../hpdaemon/hpd_xml.c 

OBJS += \
./hpdaemon/homeport.o \
./hpdaemon/hpd_avahi_publish.o \
./hpdaemon/hpd_avahi_publish_core.o \
./hpdaemon/hpd_configure.o \
./hpdaemon/hpd_events.o \
./hpdaemon/hpd_http_web_server.o \
./hpdaemon/hpd_log.o \
./hpdaemon/hpd_parameters.o \
./hpdaemon/hpd_server_sent_events.o \
./hpdaemon/hpd_services.o \
./hpdaemon/hpd_web_server.o \
./hpdaemon/hpd_xml.o 

C_DEPS += \
./hpdaemon/homeport.d \
./hpdaemon/hpd_avahi_publish.d \
./hpdaemon/hpd_avahi_publish_client.d \
./hpdaemon/hpd_avahi_publish_core.d \
./hpdaemon/hpd_configure.d \
./hpdaemon/hpd_events.d \
./hpdaemon/hpd_http_web_server.d \
./hpdaemon/hpd_https_web_server.d \
./hpdaemon/hpd_log.d \
./hpdaemon/hpd_parameters.d \
./hpdaemon/hpd_server_sent_events.d \
./hpdaemon/hpd_services.d \
./hpdaemon/hpd_web_server.d \
./hpdaemon/hpd_xml.d 


# Each subdirectory must supply rules for building sources it contributes
hpdaemon/%.o: ../hpdaemon/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DHPD_HTTP -I"/opt/build/hpbridge/hpdaemon" -I"/opt/build/ihaplite" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


