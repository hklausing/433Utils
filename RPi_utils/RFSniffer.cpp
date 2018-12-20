/*
  File    RFSniffer.cpp

  source code based on https://github.com/ninjablocks/433Utils.git
*/

#include "../rc-switch/RCSwitch.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "helper.h"


#define VERSION "v0.0.1"
#define RELEASE "2018-12-20"
#define GPIOPIN 22


RCSwitch mySwitch;


int main(int argc, char *argv[]) {

  // option variables
  int flag_debug = 0;
  int gpio_pin = GPIOPIN;
  int pulse_length = 0;
  int protocol_id = 1;            // protocol ID

  // variables for getopt
  int c;      // option code
  int index;

  opterr = 0;

  while ((c = getopt (argc, argv, "Vdg:hl:p:")) != -1) {

    switch (c) {

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

      case 'l':
        pulse_length = atoi(optarg);
        if(pulse_length < 1) {
          fprintf(stderr, "ERROR: Puls length must be greater 0!\n");
          return 1;
        }
        break;

      case 'h':
        printf("Usage: RFsniffer [OPTIONS]\n");
        printf("\n");
        printf("OPTIONS\n");
        printf("-d      Displays debug information\n");
        printf("-g PIN  Sets the used GPIO pin number, default is %d. PIN must be in range 1..29\n", GPIOPIN);
        printf("-h      Show this help information\n");
        printf("-l LEN  Defines pulse length in usec, default is 350 in protocol 1.\n");
        printf("        LEN must be greater 0.\n");
        printf("-p PROT Defines the used protocol ID, default is 1.\n");
        printf("        Allowed range is 1..5, see rc-switch documentation for details.\n");
        printf("-V      Displays program version\n");
        printf("\n");
        return 0;
        exit(0);
        break;

      case 'p':
        protocol_id = atoi(optarg);
        if((protocol_id < 1) || (protocol_id > 5)) {
          fprintf(stderr, "ERROR: Protocol ID must be in range 1..5!\n");
          return 1;
        }
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
    printf ("pulse_length = %d usec\n", pulse_length);
    printf ("protocol_id = %d\n", protocol_id);
  }


  /*
   * GPIO setup
   */
  if(wiringPiSetup() == -1) {
    fprintf (stderr, "ERROR: GPIO hardware setup failed!\n");
    return 0;
  }

  mySwitch = RCSwitch();
  if(protocol_id > 0){
    mySwitch.setProtocol(protocol_id);
  }
  if(pulse_length > 0){
    mySwitch.setPulseLength(pulse_length);
  }
  mySwitch.enableReceive(gpio_pin);  // Receiver on interrupt 0


  /*
   * Receive loop
   */
  while(1) {

    if (mySwitch.available()) {

      int value = mySwitch.getReceivedValue();
      if (value == 0) {

        printf("Unknown encoding\n");

      } else {

        printf("%s Received %i\n", getTimeStamp(), mySwitch.getReceivedValue() );

      }

      fflush(stdout);
      mySwitch.resetAvailable();
    }

    usleep(100);

  }

  return 0;

}
// vim: autoindent tabstop=2 shiftwidth=2 expandtab softtabstop=2
