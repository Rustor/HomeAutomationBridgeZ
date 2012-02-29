INPUT=`./udplisten;` 
IPADDR=`echo "$INPUT" | grep ipaddr: | cut -d ":" -f 2  `
MYIP=`ifconfig | grep "inet addr:" | cut -d ':' -f 2 | grep Bcast | cut -d " " -f 1`

echo $IPADDR

echo $MYIP

./dvlcoauto 60001 $MYIP $IPADDR 
