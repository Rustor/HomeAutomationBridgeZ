Description
===========

Turn Z-device on and off via homeport interface.

Package includes:
* A HP-server-bridge offering access to z-device 
	(webserver and bridge)
* HP-client and middleware-utils
* Example client using utils
* Config tool for D-GW 


Dependencies
============

Hardware Needed:
	A D-Z-gateway.
	A Z-on-off device.
	A network with DHCP.

Build deps:
	libmxml 2.7 (--disable-threads)
	libmicrohttpd 
	libconfig
	libuuid
	libavavi core (LIAB SG)

	libcurl

(for help installing see homeport-README)


Build
=====

The package (mostly) consist of Eclipse projects (import into a workspace). To build without eclipse you have to install the code in /opt/build and do the following steps:
 
cd DGWconfig
sh build.sh
 
cd into ihaplite/Debug
make clean; make
 
cd into rest projects debug dirs 
make clean; make


NOTE: the included object code is compatible with the LIAB SG openembedded (ARM).

Install (needed for running Example)
=======

cd hpclientSD/Debug
sudo cp hpclientSD /sbin

cd hpclientPUT/Debug
sudo cp hpclientPUT /sbin


cd Dzigbee-bridge/Debug
sudo cp zigbee-bridge /sbin

Usage
=====

Configure D-Z-gateway (if not already configured):
cd DGWconfig 
sh configdvgw.sh
Power cycle D-Z-gateway (off and on again)

First console:
cd /opt/build/hpbridge
sh start-hpbridge.sh

(wait 20 sec)

Second console:
cd /opt/build/example
./Debug/example On
or
./Debug/example Off


To add Z-devices 
================
To add Z-devices to D-Z-gateway, you have to use third-party D-tools! 





