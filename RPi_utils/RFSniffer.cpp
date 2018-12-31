/*
  File    RFSniffer.cpp

  source code based on https://github.com/ninjablocks/433Utils.git
*/

#include "../rc-switch/RCSwitch.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "helper.h"


#define VERSION "v0.1.0"
#define RELEASE "2018-12-31"
#define GPIOPIN 22


RCSwitch mySwitch;



std::string ulongToBinary(unsigned long value, unsigned length=0) {

  std::string binary;

  while(value != 0 || length) {

    binary = (value % 2 == 0 ? "0" : "1" ) + binary;
    value /= 2;
    if(length) { length--; }
  }

  return binary;
}



int main(int argc, char *argv[]) {

  // option variables
  int flag_binary = 0;
  int flag_debug = 0;
  int flag_info = 0;
  int flag_hex = 0;
  int gpio_pin = GPIOPIN;

  // variables for getopt
  int c;      // option code
  int index;

  opterr = 0;

  while ((c = getopt (argc, argv, "Vbdg:hix")) != -1) {

    switch (c) {

      case 'b':
        flag_binary = 1;
      break;

      case 'd':
        flag_debug = 1;
      break;

      case 'g':
        gpio_pin = atoi(optarg);
        if((gpio_pin < 1) || (gpio_pin > 29)) {
          fprintf(stderr, "ERROR: GPIO pin must be in range 1..29!\n");
          return 1;
        }
        break;

      case 'i':
        flag_info = 1;
      break;

      case 'h':
        printf("Usage: RFSniffer [OPTIONS]\n");
        printf("\n");
        printf("OPTIONS\n");
        printf("-b      Displays values as binary number with leading 'b'.\n");
        printf("-d      Displays debug information.\n");
        printf("-g PIN  Sets the used GPIO pin number, default is %d. PIN must be in range 1..29\n", GPIOPIN);
        printf("-h      Show this help information.\n");
        printf("-i      Additional received block information, with bit length, protocol and delay.\n");
        printf("-x      Displays values as hexadecimal number leading 'h'.\n");
        printf("-V      Displays program version.\n");
        printf("\n");
        printf("Protocol Typ  Pulse length  Sync bit  Wave form '0'  Wave form '1'\n");
        printf("       1          350us      1 / 31      1 /  3         3 / 1\n");
        printf("       2          650us      1 / 10      1 /  2         2 / 1\n");
        printf("       3          100us      3 / 71      4 / 11         9 / 6\n");
        printf("       4          380us      1 /  6      1 /  3         3 / 1\n");
        printf("       5          500us      6 / 14      1 /  2         2 / 1\n");
        printf("\n");
        printf("Example\n");
        printf("  Display all received codes with full information setup:\n");
        printf("  RFSniffer -bix\n");
        printf("\n");
        return 0;
        exit(0);
        break;

      case 'x':
        flag_hex = 1;
      break;

      case 'V':
        printf("%s (released %s)\n", VERSION, RELEASE);
        return 0;
        break;

      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;

      default:
        abort();
      }
  }

  // debug: show defined option values
  if(flag_debug) {
    printf ("gpio_pin = %d\n", gpio_pin);
  }


  /*
   * GPIO setup
   */
  if(wiringPiSetup() == -1) {
    fprintf (stderr, "ERROR: GPIO hardware setup failed!\n");
    return 0;
  }

  mySwitch = RCSwitch();
  mySwitch.enableReceive(gpio_pin);  // Receiver on interrupt 0


  /*
   * Receive loop
   */
  while(1) {

    if (mySwitch.available()) {

      unsigned long value = mySwitch.getReceivedValue();
      unsigned int bitlen = mySwitch.getReceivedBitlength();
      unsigned int protocol = mySwitch.getReceivedProtocol();
      unsigned int delay = mySwitch.getReceivedDelay();
      unsigned int *rawvalues = mySwitch.getReceivedRawdata();
      mySwitch.resetAvailable();

      if (value == 0) {

        printf("Unknown encoding\n");

      } else {

        // handle hexadecimal value
        char hexdecimal[128];
        if(flag_hex) {
          sprintf(hexdecimal, "  x%lx", value);
        } else {
          hexdecimal[0] = 0;
        }

        // handle binary value
        char binary[128];
        if(flag_binary) {
          sprintf(binary, "  b%s", ulongToBinary(value, bitlen).c_str());
        } else {
          binary[0] = 0;
        }

        // handle additional information
        char info[128];
        if(flag_info) {
          sprintf(info, "  len:%i  prt:%i  dly:%i", bitlen, protocol, delay);
        } else {
          info[0] = 0;
        }

        printf("%s -> d%lu%s%s%s\n", getTimeStamp(), value, hexdecimal, binary, info);

      }

      fflush(stdout);
    }

    usleep(100);

  }

  return 0;

}
// vim: autoindent tabstop=2 shiftwidth=2 expandtab softtabstop=2
