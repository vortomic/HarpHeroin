/*
   time_it - code to measure and report time used by a program
     or section of code.

   Author: Larry D. Pyeatt
   Date: 8/9/2005
   Copyright Larry D. Pyeatt, 2005 all rights reserved
   You may use this code for non-commercial purposes

   How to use:
     This code acts like a simple stopwatch.
     1. use time_it_start to begin tracking time usage.
     2. use time_it_stop to stop 
     3. use time_it_report to print the results

     Example:
       
       #include <time_it.h>
         .
         .
         .
       time_it function_time;  // create a  time_it variable
         .
         .
         .
       time_it_start(&function_time); // start tracking time usage
         .
         .   // execute the code you want to time
         .
       time_it_stop(&function_time); // stop tracking time usage
         .
         .
         .
       time_it_report(&function_time, "Timing results for my code:");
         .
         .
         .

*/

#ifndef TIME_IT_H
#define TIME_IT_H

#include <time.h>
#include <sys/times.h>
#include <unistd.h>

/* A structure to hold timing information */
typedef struct {
  clock_t start_time, end_time;
  struct tms start_cpu, end_cpu;
}time_it;

void time_it_start(time_it *t);

void time_it_stop(time_it *t);

void time_it_report(time_it *t, char *tag);

#endif
