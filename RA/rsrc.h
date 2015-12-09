#ifndef RSRC_H
#define RSRC_H
/*******************************************************************

  OS Exercises - Homework 5 - HOST dispatcher

    rsrc - process resource allocation functions for HOST dispatcher

    int rsrcChk (RsrcPtr available, Rsrc claim) - check that resources are available now
    int rsrcChkMax (Rsrc claim) - check that resources will be available sometime
    int rsrcAlloc (RsrcPtr available, Rsrc claim) - allocate resources
    void rsrcFree (RsrcPtr available, Rsrc claim) - free resources

    see rsrc.c for fuller description of function arguments and returns

********************************************************************

  history:
     v1.0: Original for this exercises and full project dispatcher

*******************************************************************/
#ifndef FALSE   
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* Resource management *****************************/

#define MAX_PRINTERS 2
#define MAX_SCANNERS 1
#define MAX_MODEMS 1
#define MAX_CDS 2

struct rsrc {
    int printers;
    int scanners;
    int modems;
    int cds;
};

typedef struct rsrc Rsrc;
typedef Rsrc * RsrcPtr;

/* prototypes  ************************************/

int rsrcChk(RsrcPtr, Rsrc);
int rsrcChkMax(Rsrc);
int rsrcAlloc(RsrcPtr, Rsrc);
void rsrcFree(RsrcPtr,Rsrc);

#endif
