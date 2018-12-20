# About

rcswitch-pi is for controlling rc remote controlled power sockets 
with the raspberry pi. Kudos to the projects [rc-switch](http://code.google.com/p/rc-switch)
and [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi).
I just adapted the rc-switch code to use the wiringpi library instead of
the library provided by the arduino.


## Usage

First you have to install the [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library.
After that you can compile the example program *send* by executing *make*. 
It uses wiringPi pin no 21 by default. The GPIO pin can be controlled by the option -g GPIOPIN at program start. (Good Resource for Pin Details https://pinout.xyz/pinout/wiringpi)

All three program supporting the option -h to give some help information.

The programs send and codesend are able to transmit multiple data packages.

## Note
The 'RF\_Sniffer' code is as yet untested.  It _should_ work, but it is still being tested thoroughly.  It's provided to allow you to start playing with it now.
