#ifndef GHOST_H
#define GHOST_H
/*******************************************************************

  OS Exercises - Homework 5 - HOST dispatcher
 
   include file for HOST dispatcher

 *******************************************************************

  history:
     v1.0: original for previous exercises FCFS, RR, & Feedback
     v1.1: add memory allocation 
     v1.2: add resource allocation

 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "pcb.h"
#include "mab.h"
#include "rsrc.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define DEFAULT_NAME "hostd"
#define QUANTUM  1
#endif
