/*
	FIRST COME FIRST SERVE (FCFS)
	hostd.c
	Group 4:
		Dakota Brown
		Kyosuke Iwayama
		Kaelah Selby

	gcc -c hostd.c -o hostd
	./hostd fcfs.txt
*/

/*******************************************************************

OS Eercises - Homework 5 - HOST dispatcher - Dispatcher Shell

    hostd

        hostd is fcfs 'dispatcher' that reads in a list of 'jobs' from a file
        and 'dispatches' them in a first-come-first-served manner.
ch
        time resolution is one second (although this can be changed). 

    usage
   
        hostd <dispatch file>
 
        where
            <dispatch file> is list of process parameters as specified in fcfs.txt.

    functionality

    1. Initialize dispatcher queue;
    2. Fill dispatcher queue from dispatch list file;
    3. Start dispatcher timer;
    4. While there's anything in the queue or there is a currently running process:
        i. If a process is currently running;
            a. Decrement process remainingcputime;
            b. If times up:
                A. Send SIGINT to the process to terminate it;
                B. Free up process structure memory
       ii. If no process currently running &&
            dispatcher queue is not empty &&
            arrivaltime of process at head of queue is <= dispatcher timer:
            a. Dequeue process and start it (fork & exec)
            b. Set it as currently running process;
      iii. sleep for one second;
       iv. Increment dispatcher timer;
        v. Go back to 4.
    5. Exit

********************************************************************/

#include "hostd.h"

#define VERSION "1.0"

/******************************************************
 
   internal functions
   
 ******************************************************/

char * StripPath(char*);
void PrintUsage(FILE *, char *);
void SysErrMsg(char *, char *);
void ErrMsg(char *, char *);

/* global storage */


/******************************************************/

int main (int argc, char *argv[])
{
    char * inputfile;             // job dispatch file
    FILE * inputliststream;
    PcbPtr inputqueue = NULL;     // input queue buffer
    PcbPtr currentprocess = NULL; // current process
    PcbPtr process = NULL;        // working pcb pointer
    PcbPtr pcbNext = NULL;
    int timer = 0;                // dispatcher timer 

//  0. Parse command line
    if (argc == 2) inputfile = argv[1];
    else PrintUsage (stderr, argv[0]);

/*  1. Initialize dispatcher queue;
     (already initialised in assignments above)

  2. Fill dispatcher queue from dispatch list file;
*/    if (!(inputliststream = fopen(inputfile, "r"))) { // open it
          SysErrMsg("ERROR LOADING DISPATCH FILE:", inputfile);
          exit(2);
    }

    while (!feof(inputliststream)) {  // processing the inputqueue
	pcbNext = createnullPcb();
        if (fscanf(inputliststream,"%d, %d, %d, %d, %d, %d, %d, %d", &(pcbNext->arrivaltime), &(pcbNext->priority),&(pcbNext->remainingcputime), &(pcbNext->mbytes),&(pcbNext->req.printers), &(pcbNext->req.scanners),&(pcbNext->req.modems), &(pcbNext->req.cds)) != 8) {
            free(pcbNext);
            continue;
        }
        pcbNext->status = PCB_INITIALIZED; //pcbNext is initialized
        inputqueue = enqPcb(inputqueue, pcbNext); //queus inputqueue with pcbNext
    }

/*  3. Start dispatcher timer;
     (already set to zero above)
*/
//  4. While there's anything in the queue or there is a currently running process:
	while (inputqueue || currentprocess ) {

//      i. If a process is currently running;
		if(currentprocess){


//          a. Decrement process remainingcputime;
			(currentprocess->remainingcputime) -= 1;

//          b. If times up:
			if(currentprocess->remainingcputime == 0){

//             A. Send SIGINT to the process to terminate it;
				terminatePcb(currentprocess);

//             B. Free up process structure memory
				free(currentprocess);
				currentprocess = NULL;
          		}
		}

/*     ii. If no process now currently running &&
           dispatcher queue is not empty &&
           arrivaltime of process at head of queue is <= dispatcher timer:
*/		if (!currentprocess && inputqueue && inputqueue->arrivaltime <= timer){
			currentprocess = deqPcb(&inputqueue);
			if(currentprocess) {
				startPcb(currentprocess);
    			}
		}
/*          a. Dequeue process and start it (fork & exec)
          b. Set it as currently running process;
     iii. sleep for one second;
*/		sleep(1);

//      iv. Increment dispatcher timer;
		timer++;

//       v. Go back to 4.
   	}

//    5. Exit
    exit (0);
}

/*******************************************************************

char * StripPath(char * pathname);

  strip path from file name

  pathname - file name, with or without leading path

  returns pointer to file name part of pathname
    if NULL or pathname is a directory ending in a '/'
        returns NULL
*******************************************************************/

char * StripPath(char * pathname)
{
    char * filename = pathname;\

    if (filename && *filename) {           // non-zero length string
        filename = strrchr(filename, '/'); // look for last '/'
        if (filename)                      // found it
            if (*(++filename))             //  AND file name exists
                return filename;
            else
                return NULL;
        else
            return pathname;               // no '/' but non-zero length string
    }                                      // original must be file name only
    return NULL;
}

/*******************************************************
 * print usage
 ******************************************************/
void PrintUsage(FILE * stream, char * progname)
{
    if(!(progname = StripPath(progname))) progname = DEFAULT_NAME;
    
    fprintf(stream,"\n"
"%s process dispatcher (version " VERSION "); usage:\n\n"
"  %s <dispatch file>\n"
" \n"
"  where <dispatch file> is list of process parameters \n\n",
    progname,progname);

    exit(127);
}
/********************************************************
 * print an error message on stderr
 *******************************************************/

void ErrMsg(char * msg1, char * msg2)
{
    if (msg2)
        fprintf(stderr,"ERROR - %s %s\n", msg1, msg2);
    else
        fprintf(stderr,"ERROR - %s\n", msg1);
    return;
}

/*********************************************************
 * print an error message on stderr followed by system message
 *********************************************************/

void SysErrMsg(char * msg1, char * msg2)
{
    if (msg2)
        fprintf(stderr,"ERROR - %s %s; ", msg1, msg2);
    else
        fprintf(stderr,"ERROR - %s; ", msg1);
    perror(NULL);
    return;
}
                                          
