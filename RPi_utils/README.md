# About

The changes for this project based on https://github.com/ninjablocks/433Utils.
Thanks for this great job.

rcswitch-pi is for controlling rc remote controlled power sockets 
with the raspberry pi. Kudos to the projects [rc-switch](http://code.google.com/p/rc-switch)
and [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi).
I just adapted the rc-switch code to use the wiringpi library instead of
the library provided by the arduino.


## Installation

Execute the following items to install the tools to a Raspberry Pi:

* First you have to install the [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library.

* Clone this package to your drive, `$ git clone https://github.com/hklausing/433Utils.git`

* Change the directory to __433Utils/RPi\_utils__

* Compile the project: `$ make`

* Install the programs to your system, if required, `$ sudo make install`


## Usage

* All three programs supporting the option __-h__ and __-V__ to give some
  help and version information.
* All programs supporting the option __-g PIN__. With this option the port
  number can be controlled at program start. This makes it handy to
  change this information. The default number for _send_ & _codesend_ is
  21 (WiringPi count) and is connected to the header pin 29. E.g. `RFSniffer -g7` if the
  receiver is connected to header pin 7. The default pin for _RFSniffer_
  is 22 (WiringPi count).

### send

_send_ is used to transmit a code divided into system-code, unit-code
and command. Multiple codes can be listed on each program call.


### codesend

_codesend_ is used to transmit a decimal code number. Multiple codes
can be listed on each program call.

### RFSniffer

_RFSniffer_ is used to read code transmitted by a remote control.
This helps to find codes for the command _codesend_. If the options
__-b -i -x__ are used at program start some additional information,
like bit length or protocol type, will be displayed.

