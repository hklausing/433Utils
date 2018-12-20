/*
  File    send.cpp

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
#define GPIOPIN 21
#define WAITTIME 20



int main(int argc, char *argv[]) {

  // option variables
  int flag_debug = 0;
  int flag_quiet = 0;
  int gpio_pin = GPIOPIN;
  int pulse_length = 0;
  int wait_time = WAITTIME;       // wait time in milliseconds
  int protocol_id = 1;            // protocol ID
  int repeat_transmits = 10;      // repeat transmissions

  // variables for getopt
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
        printf("Usage: send [OPTIONS] <system_code> <unit_code> <command> [,] ...\n");
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
        printf("  <system_code>  First five settings of Type A 10 pole DIP switch, e.g. 11111\n");
        printf("  <unit_code>    Switch number [1 .. 5], [A .. F] or [10000 .. 00001]\n");
        printf("  <command>      0|f|F for OFF and 1|t|T for ON\n");
        printf("The amount of arguments can be repeated up to 100 times.\n");
        printf("Between argument sequences a comma with leading & trailing spaces can be used\n");
        printf("to separated the list.\n");
        printf("\n");
        return 0;
        break;

      case 'l':
        pulse_length = atoi(optarg);
        if(pulse_length < 1) {
          fprintf(stderr, "ERROR: Pulse length must be greater 0!\n");
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
  int transmits = (argc - optind) / 3;
  if((transmits == 0) && ((argc - optind) == 0)) {
    fprintf (stderr, "ERROR: no argument sequence found!\n");
    return 1;
  }

  // check if send argument sequence is complete
  if((argc - optind) % 3 != 0) {
    fprintf (stderr, "ERROR: amount of send arguments is not completer!\n");
    fprintf (stderr, "Please check if the sequence of '<system_code> <unit_code> <command>' is complete for each transmits.\n");
    return 1;
  }


  int MAX_ARRAY = 100;
  struct {
    char *system_code;
    int unit_code;
    int command;
  } argument_list[MAX_ARRAY];

  for(int id = 0, index = optind; index < argc; index++) {

    transmits = id / 3;

    if(transmits >= MAX_ARRAY) {
      fprintf (stderr, "ERROR: too much argument sequences found!\n");
      return 1;
    }

    // check for separation characters and ignore them
    if(argv[index][0] == ',') {
      continue;
    }

    switch(id % 3) {

      case 0:
        {
          int error = 0;
          if(strlen(argv[index]) == 5) {
            for(int i = 0; i < 5; i++) {
              if(!((argv[index][i] == '0') || (argv[index][i] == '1'))) {
                error = 1;
              }
            }
            argument_list[transmits].system_code = argv[index];
          } else {
            error = 1;
          }
          if(error) {
            fprintf(stderr, "ERROR: wrong system_code information (%s) in argument sequence %d found!\n"
                  , argv[index], transmits + 1);
            return 1;
          }
        }
        break;

      case 1:
        {
          int error = 0;
          if(strlen(argv[index]) == 1) {

            switch(argv[index][0]){
              case '1': case 'a': case 'A': argument_list[transmits].unit_code = 1; break;
              case '2': case 'b': case 'B': argument_list[transmits].unit_code = 2; break;
              case '3': case 'c': case 'C': argument_list[transmits].unit_code = 3; break;
              case '4': case 'd': case 'D': argument_list[transmits].unit_code = 4; break;
              case '5': case 'e': case 'E': argument_list[transmits].unit_code = 5; break;
              default:
                error = 1;
            }

          } else if(strlen(argv[index]) == 5) {

            if(strcmp(argv[index], "10000") == 0) {
              argument_list[transmits].unit_code = 1;
            } else if(strcmp(argv[index], "01000") == 0) {
              argument_list[transmits].unit_code = 2;
            } else if(strcmp(argv[index], "00100") == 0) {
              argument_list[transmits].unit_code = 3;
            } else if(strcmp(argv[index], "00010") == 0) {
              argument_list[transmits].unit_code = 4;
            } else if(strcmp(argv[index], "00001") == 0) {
              argument_list[transmits].unit_code = 5;
            } else {
              error = 1;
            }

          } else {
            error = 1;
          }

          if(error) {
            fprintf (stderr, "ERROR: wrong unit_code information (%s) in argument sequence %d found!\n"
                , argv[index], transmits + 1);
            return 1;
          }
        }
        break;

      case 2:
        switch(argv[index][0]){
          case '0': case 'f': case 'F': argument_list[transmits].command = 0; break;
          case '1': case 't': case 'T': argument_list[transmits].command = 1; break;
          default:
            fprintf (stderr, "ERROR: wrong command character (%s) in argument sequences %d found!\n"
                , argv[index], transmits + 1);
            return 1;
        }
        break;
    }

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

    printf ("Stored argument sequences\n");
    for(int i = 0; i < transmits; i++) {
      printf("  %d. sequence: %s %d %d\n", (i+1)
          , argument_list[i].system_code
          , argument_list[i].unit_code
          , argument_list[i].command);
    }
  }


  /*
   * GPIO setup
   */
  if (wiringPiSetup () == -1){
    fprintf (stderr, "ERROR: GPIO hardware setup failed!\n");
    return 1;
  }

  RCSwitch mySwitch = RCSwitch();
  mySwitch.setProtocol(protocol_id);
  if(pulse_length > 0) {
      mySwitch.setPulseLength(pulse_length);
  }
  mySwitch.setRepeatTransmit(repeat_transmits);
  mySwitch.enableTransmit(gpio_pin);

  /*
   * loop over all argument sequences
   */
  for(int i = 0; i < transmits; i++) {

    if(!flag_quiet) {
      printf("%s sending system_code[%s] unit_code[%d] command[%d]\n"
            , getTimeStamp()
            , argument_list[i].system_code
            , argument_list[i].unit_code
            , argument_list[i].command);
    }

    switch(argument_list[i].command) {
      case 1:
        mySwitch.switchOn(argument_list[i].system_code, argument_list[i].unit_code);
        break;
      case 0:
        mySwitch.switchOff(argument_list[i].system_code, argument_list[i].unit_code);
        break;
      default:
        printf("command[%i] is unsupported\n", argument_list[i].command);
        return 1;
    }

    // wait after a transmit if another transmit follows
    if(wait_time && (i + 1 - transmits)) {
      ms_sleep(wait_time);
    }

  }

  return 0;
}

// vim: autoindent tabstop=2 shiftwidth=2 expandtab softtabstop=2
