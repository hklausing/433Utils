/*
  File    codesend.cpp

  source code based on https://github.com/ninjablocks/433Utils.git
*/


#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>

#include <ctype.h>
#include <unistd.h>
#include "helper.h"


#define VERSION "v0.0.1"
#define RELEASE "2018-12-20"
#define GPIOPIN 21
#define WAITTIME 20


int main(int argc, char *argv[]) {

  // option variables
  int flag_debug = 0;
  int flag_quiet = 0;
  int gpio_pin = GPIOPIN;
  int pulse_length = 0;
  int wait_time = WAITTIME;       // wait time in milli seconds
  int protocol_id = 1;            // protocol ID
  int repeat_transmits = 10;      // repeat transmittings

  // variables for getoption
  int c;      // option code
  int index;

  opterr = 0;

  while ((c = getopt (argc, argv, "Vdg:hl:p:qr:w:")) != -1) {

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

      case 'h':
        printf("Sending 433 MHz remote plug control codes, on selected wiringpi pin.\n");
        printf("\n");
        printf("Usage: codesend [OPTIONS] <code> ...\n");
        printf("\n");
        printf("OPTIONS:\n");
        printf("-d      Displays debug information.\n");
        printf("-g PIN  Sets the used GPIO pin number, default is %d.\n", GPIOPIN);
        printf("        PIN must be in rang 1..29.\n");
        printf("-h      Shows this help information.\n");
        printf("-l LEN  Defines pulse length in usec, default is 350 in protocol 1.\n");
        printf("        LEN must be greater 0.\n");
        printf("-p PROT Defines the used protocol ID, default is 1.\n");
        printf("        Allowed range is 1..5, see rc-switch documentation for details.\n");
        printf("-q      Disables any program output during execution.\n");
        printf("-r NUM  Defines the amount of repeat transmits, default is 10.\n");
        printf("        Allowed range is 1..50.\n");
        printf("-w TIME Defines the wait time in msec between transmits, default is %d.\n", WAITTIME);
        printf("        Allowed range is 1..60000.\n");
        printf("-V      Displays program version.\n");
        printf("\n");
        printf("Arguments:\n");
        printf("  <code>  data code what will be transmitted via 433MHz\n");
        printf("The amount of code arguments is limited to 100 times.\n");
        printf("\n");
        return 0;
        break;

      case 'l':
        pulse_length = atoi(optarg);
        if(pulse_length < 1) {
          fprintf(stderr, "ERROR: Puls length must be greater 0!\n");
          return 1;
        }
        break;

      case 'p':
        protocol_id = atoi(optarg);
        if((protocol_id < 1) || (protocol_id > 5)) {
          fprintf(stderr, "ERROR: Protocol ID must be in range 1..5!\n");
          return 1;
        }
        break;

      case 'q':
        flag_quiet = 1;
        break;

      case 'r':
        repeat_transmits = atoi(optarg);
        if((repeat_transmits < 1) || (repeat_transmits > 25)) {
          fprintf(stderr, "ERROR: Repeat transmits must be in range 1..25!\n");
          return 1;
        }
        break;

      case 'w':
        wait_time = atoi(optarg);
        if((wait_time < 0) || (wait_time > 60000)) {
          fprintf(stderr, "ERROR: Wait time value must be in range 1..60000 msec!\n");
          return 1;
        }
        break;

      case 'V':
        printf("%s (released %s)\n", VERSION, RELEASE);
        return 0;
        break;

      case '?':
        if ((optopt == 'g') || (optopt == 'l'))
          fprintf (stderr, "ERROR: Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;

      default:
        abort ();
      }
  }


  // check if argument sequence is specified
  int transmits = 0;
  if((transmits == 0) && ((argc - optind) == 0)) {
    fprintf (stderr, "ERROR: no code argument found!\n");
    return 1;
  }

  int MAX_ARRAY = 100;
  char *argument_list[MAX_ARRAY];
  for(int id = 0, index = optind; index < argc; index++) {

    transmits = id;

    if(transmits >= MAX_ARRAY) {
      fprintf (stderr, "ERROR: too much code arguments found!\n");
      return 1;
    }

    argument_list[transmits] = argv[index];

    id++;

  }

  // correct the amount of transmits
  transmits++;


  // debug: show defined option values
  if(flag_debug) {

    printf ("flag_quiet = %s\n", flag_quiet?"true":"false");
    printf ("gpio_pin = %d\n", gpio_pin);
    printf ("pulse_length = %d usec\n", pulse_length);
    printf ("wait_time = %d msec\n", wait_time);
    printf ("protocol_id = %d\n", protocol_id);
    printf ("repeat_transmits = %d\n", repeat_transmits);

    /*
    for (index = optind; index < argc; index++) {
      printf ("Non-option argument %s\n", argv[index]);
    }
    */

    printf ("Stored code arguments:\n");
    for(int i = 0; i < transmits; i++) {
      printf("  %d. code: %s\n", (i+1), argument_list[i]);
    }
  }


  /*
   * GPIO setup
   */
  if(wiringPiSetup() == -1) {
    fprintf (stderr, "ERROR: GPIO hardware setup failed!\n");
    return 1;
  }

  RCSwitch mySwitch = RCSwitch();
  if(protocol_id != 0){
    mySwitch.setProtocol(protocol_id);
  }
  if (pulse_length != 0){
    mySwitch.setPulseLength(pulse_length);
  }
  mySwitch.setRepeatTransmit(repeat_transmits);
  mySwitch.enableTransmit(gpio_pin);

  const unsigned int CODE_LENGTH = 24;

  /*
   * loop over all argument sequences
   */
  for(int i = 0; i < transmits; i++) {

    int code = atol(argument_list[i]);

    if(!flag_quiet) {
      printf("%s sending code[%i]\n", getTimeStamp(), code);
    }

    mySwitch.send(code, CODE_LENGTH);

    // wait after a transmit if another transmit follows
    if(wait_time && (i + 1 - transmits)){

      ms_sleep(wait_time);

    }

  }

  return 0;

}

// vim: autoindent tabstop=2 shiftwidth=2 expandtab softtabstop=2
