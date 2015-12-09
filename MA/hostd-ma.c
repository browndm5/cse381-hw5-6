/*
	MEMORY ALLOCATION (MA)
	hostd-ma.c 
	Group 4:
		Dakota Brown
		Kyosuke Iwayama
		Kaelah Selby

	gcc -c hostd-ma.c -o hostd-ma
	./hostd-ma memory.txt
		
*/


/*******************************************************************

OS Eercises - Homework 5 - HOST dispatcher - Exercise Memory Allocation

    hostd

        hostd is a three level feedback 'dispatcher' that reads in a list
        of 'jobs' from a file and 'dispatches' them in feedback mode
        (see below).

        implements memory allocation as selected in command line.

        time resolution is one second (although this can be changed).

    usage

        hostd [-mf|-mn|-mb|-mw] <dispatch file>

        where
            <dispatch file> is list of process parameters as specified
                for assignment 2.
            -mx is optional selection of memory allocation algorithm
                -mf First Fit (default)
                -mn Next Fit
                -mb Best Fit
                -mw Worst Fit

    functionality

    1. Initialize dispatcher queue;
    2. Fill dispatcher queue from dispatch list file;
    3. Start dispatcher timer;
    4. While there's anything in any of the queues or there is a currently running process:
        i. Unload any pending processes from the input queue:
           While (head-of-input-queue.arrival-time <= dispatcher timer)
           dequeue process from input queue and enqueue on user job queue;
       ii. Unload pending processes from the user job queue:
           While (head-of-user-job-queue.mbytes can be allocated)
           dequeue process from user job queue, allocate memory to the process and
           enqueue on highest priority feedback queue (assigning it the appropriate
           priority);
      iii. If a process is currently running:
            a. Decrement process remainingcputime;
            b. If times up:
                A. Send SIGINT to the process to terminate it;
                B. Free memory we have allocated to the process;
                C. Free up process structure memory;
            c. else if other processes are waiting in any of the feedback queues:
                A. Send SIGTSTP to suspend it;
                B. Reduce the priority of the process (if possible) and enqueue it on
                   the appropriate feedback queue;
       iv. If no process currently running && feedback queues are not empty:
            a. Dequeue process from the highest priority feedback queue that is not empty
            b. If already started but suspended, restart it (send SIGCONT to it)
               else start it (fork & exec)
            c. Set it as currently running process;
        v. sleep for one second;
       vi. Increment dispatcher timer;
      vii. Go back to 4.
    5. Exit

********************************************************************

history:
   v1.0: Original simple FCFS dispatcher 
   v1.1: Simple round-robin dispatcher 
   v1.2: Simple three level feedback dispatcher add CheckQueues fn
   v1.3: Add memory block allocation (this exercise)
*******************************************************************/

#include "hostd.h"

#define VERSION "1.3"

/******************************************************
 
   internal functions
   
 ******************************************************/

int CheckQueues(PcbPtr *);
char * StripPath(char*);
void PrintUsage(FILE *, char *);
void SysErrMsg(char *, char *);
void ErrMsg(char *, char *);

/******************************************************

global variables

******************************************************/

Mab  memory = { 0, MEMORY_SIZE, FALSE, NULL, NULL }; // memory arena

/******************************************************/

int main (int argc, char *argv[])
{
    char * inputfile = NULL;      // job dispatch file
    FILE * inputliststream;
    PcbPtr inputqueue = NULL;     // input queue buffer
    PcbPtr userjobqueue = NULL;   // arrived processes
    PcbPtr fbqueue[N_FB_QUEUES];  // feedback queues
    PcbPtr currentprocess = NULL; // current process
    PcbPtr process = NULL;        // working pcb pointer
    int timer = 0;                // dispatcher timer
    int quantum = QUANTUM;        // current time-slice quantum
    int i;                        // working index

//  0. Parse command line

    i = 0;
    while (++i < argc) {
        if (!strcmp(argv[i],"-mf")) {
	/*FOR BEST FIT*/
        } else
        if (!strcmp(argv[i],"-mn")) {
	/*FOR NEXT FIT*/
        } else
        if (!strcmp(argv[i],"-mb")) {
	/*FOR BEST FIT*/
        } else
        if (!strcmp(argv[i],"-mw")) {
	/*FOR WORST FIT*/
        } else
        if (!inputfile) {
            inputfile = argv[i];
        } else {
             PrintUsage(stdout, argv[0]);
        }
    }
    if (!inputfile) PrintUsage(stdout, argv[0]);    

//  1. Initialize dispatcher queues (all others already initialised) ;

    i = 0;

    for (i = 0; i < N_FB_QUEUES; fbqueue[i++] = NULL);
    
//  2. Fill dispatcher queue from dispatch list file;
   
    if (!(inputliststream = fopen(inputfile, "r"))) { // open it
          SysErrMsg("could not open dispatch list file:", inputfile);
          exit(2);
    }

    while (!feof(inputliststream)) {  // put processes into input_queue
        process = createnullPcb();
        if (fscanf(inputliststream,"%d, %d, %d, %d, %d, %d, %d, %d",
             &(process->arrivaltime), &(process->priority),
             &(process->remainingcputime), &(process->mbytes),
             &(process->req.printers), &(process->req.scanners),
             &(process->req.modems), &(process->req.cds)) != 8) {
            free(process);
            continue;
        }
        process->status = PCB_INITIALIZED;
        inputqueue = enqPcb(inputqueue, process);
    }


//  3. Start dispatcher timer;
//     (already set to zero above)
        
//  4. While there's anything in any of the queues or there is a currently running process:

    while (inputqueue || userjobqueue || CheckQueues(fbqueue) >= 0 || currentprocess) {
        while (inputqueue && inputqueue->arrivaltime <= timer) {
            process = deqPcb(&inputqueue);
            userjobqueue = enqPcb(userjobqueue, process);
        }

//     ii. Unload pending processes from the user job queue:
//         While (head-of-user-job-queue.mbytes can be allocated)
//         dequeue process from user job queue, allocate memory to the process and
//         enqueue on highest priority feedback queue (assigning it the appropriate
//         priority);


        while (userjobqueue) {
            process = deqPcb(&userjobqueue);
            process->status = PCB_READY;
            process->priority = 0;
            fbqueue[process->priority] = enqPcb(fbqueue[process->priority], process);
        }
//    iii. If a process is currently running;
        if (currentprocess) {
//          a. Decrement process remainingcputime;
            currentprocess->remainingcputime--;
//          b. If times up:
           if (currentprocess->remainingcputime <= 0) {
//             A. Send SIGINT to the process to terminate it;
               terminatePcb(currentprocess);
//             B. Free up process structure memory
               free(currentprocess);
               currentprocess = NULL;
           }
//         c. else if other processes are waiting in feedback queues:
           else if (CheckQueues(fbqueue) >= 0) {
//             A. Send SIGTSTP to suspend it;
               suspendPcb(currentprocess);
//             B. Reduce the priority of the process (if possible) and enqueue it on
//                the appropriate feedback queue;;

               if (++(currentprocess->priority) >= N_FB_QUEUES)
                   currentprocess->priority = N_FB_QUEUES - 1;
                   fbqueue[currentprocess->priority] = enqPcb(fbqueue[currentprocess->priority], currentprocess);
                   currentprocess = NULL;
           }
        }
//     iv. If no process currently running && feedback queues are not empty:
       if (!currentprocess && (i = CheckQueues(fbqueue)) >= 0) {
//         a. Dequeue process from RR queue
           currentprocess = deqPcb(&fbqueue[i]);
//              else start it (fork & exec)
//         c. Set it as currently running process;
           currentprocess = startPcb(currentprocess);
       }
//       v. sleep for quantum;

        quantum = currentprocess && currentprocess->remainingcputime < QUANTUM ? currentprocess->remainingcputime :
                   ! (currentprocess) ? 1 : QUANTUM;
        sleep(quantum);
//      vi. Increment dispatcher timer;
        timer += quantum;
//     vii. Go back to 4.
    }
//    5. Exit
    exit (0);
}

/*******************************************************************

int CheckQueues(PcbPtr * queues)

  check array of dispatcher queues

  return priority of highest non-empty queue
          -1 if all queues are empty
*******************************************************************/
int CheckQueues(PcbPtr * queues)
{
    int n;

    for (n = 0; n < N_FB_QUEUES; n++)
        if (queues[n]) return n;
    return -1;
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
"  %s [-mf|-mn|-mb|-mw] <dispatch file>\n"
" \n"
"  where \n"
"    <dispatch file> is list of process parameters \n"
"    -mx is optional selection of memory allocation algorithm \n"
"      -mf First Fit (default) \n"
"      -mn Next Fit \n"
"      -mb Best Fit \n"
"      -mw Worst Fit \n\n",
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
                                          
