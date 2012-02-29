killall mdnsd;
# zigbee bridge ip can be changed with -i ipaddr
./Debug/hpbridge -l outside -d2 power_meter -d1 light_source -d3 temperature_sensor -d4 switch -d5 light_sensor -d6 thermostat