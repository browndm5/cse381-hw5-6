/*******************************************************************

  OS Exercises - Homework 5 - HOST dispatcher

    rsrc - resource management routines for the GHOST dispatcher

    int rsrcChk (RsrcPtr available, Rsrc claim)
       - check that resources are available now 
      returns:
        TRUE or FALSE - no allocation is actally done

    int rsrcChkMax (Rsrc claim)
       - check that resources will ever be available
      returns:
        TRUE or FALSE

    int rsrcAlloc (RsrcPtr available, Rsrc claim)
       - allocate resources
      returns:
        TRUE or FALSE if not enough resources available

    void rsrcFree (RsrcPtr available, Rsrc claim);
       - free resources
      returns NONE

********************************************************************

  version: 1.0 (this exercise and full project dispatcher)
  history:
     v1.0: Original for this exercises and full project dispatcher

*******************************************************************/

#include "rsrc.h"
                                          
/*******************************************************
 * int rsrcChk (RsrcPtr available, Rsrc claim)
 *    - check that resources are available
 *
 * returns:
 *    TRUE or FALSE - no allocation is actally done
 *******************************************************/
int rsrcChk(RsrcPtr available, Rsrc claim)
{
    return ((available) &&
            (available->printers >= claim.printers) &&
            (available->scanners >= claim.scanners) &&
            (available->modems >= claim.modems) &&
            (available->cds >= claim.cds) ?
             TRUE : FALSE );
}
    
/*******************************************************
 * int rsrcChkMax (Rsrc claim)
 *    - check that resources could be available
 *
 * returns:
 *    TRUE or FALSE - no allocation is actally done
 *******************************************************/
int rsrcChkMax(Rsrc claim)
{
    return ((claim.printers <= MAX_PRINTERS) &&
            (claim.scanners <= MAX_SCANNERS) &&
            (claim.modems <= MAX_MODEMS) &&
            (claim.cds <= MAX_CDS) ?
             TRUE : FALSE );
}
    
/*******************************************************
 * int rsrcAlloc (RsrcPtr available, Rsrc claim)
 *    - allocate resources
 *
 * returns:
 *    TRUE or FALSE if not enough resources available
 *******************************************************/
int rsrcAlloc(RsrcPtr available, Rsrc claim)
{
    if (!rsrcChk(available, claim)) return FALSE;
    available->printers -= claim.printers;
    available->scanners -= claim.scanners;
    available->modems -= claim.modems;
    available->cds -= claim.cds;
    return TRUE;
}

/*******************************************************
 * void rsrcFree (RsrcPtr available, Rsrc claim);
 *    - free resources
 *
 * returns NONE
 *******************************************************/
void rsrcFree(RsrcPtr available, Rsrc claim)
{
    available->printers += claim.printers;
    available->scanners += claim.scanners;
    available->modems += claim.modems;
    available->cds += claim.cds;
}

