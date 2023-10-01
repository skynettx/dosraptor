#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
#include "file0001.inc"

#define MAX_DERBIS 15

typedef struct DERB_S
{
   struct DERB_S *  prev;             // LINK LIST PREV
   struct DERB_S *  next;             // LINK LIST NEXT
   INT               dir;             // DIRECTION 0 - 7
   INT               x;
   INT               y;
}DERBS;

PRIVATE ESHOT     derbs [ MAX_DERBIS ];
  
PRIVATE ESHOT     first_derb;
PRIVATE ESHOT     last_derb;
PRIVATE ESHOT *   free_derb;

/***************************************************************************
DERB_Clear () - Clears out ESHOT Linklist
 ***************************************************************************/
VOID
DERB_Clear (
VOID
)
{
   INT loop;
  
   eshotnum = 0;

   first_derb.prev = NUL;
   first_derb.next = &last_derb;
  
   last_derb.prev  = &first_derb;
   last_derb.next  = NUL;
  
   free_derb       = eshots;
  
   memset ( eshots, 0, sizeof ( eshots ) );
  
   for ( loop = 0; loop < MAX_ESHOT-1; loop++ )
      eshots[ loop ].next = &eshots [ loop + 1 ];
}

/*-------------------------------------------------------------------------*
DERB_Get () - gets a Free ESHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE ESHOT *
DERB_Get (
VOID
)
{
   ESHOT * new;
  
   if ( !free_derb )
      return ( NUL );
  
   eshotnum++;
   if ( eshothigh < eshotnum )
      eshothigh = eshotnum;

   new = free_derb;
   free_derb = free_derb->next;
  
   memset ( new, 0 ,sizeof ( ESHOT ) );
  
   new->next = &last_derb;
   new->prev = last_derb.prev;
   last_derb.prev = new;
   new->prev->next = new;
  
   return ( new );
}
  
/*-------------------------------------------------------------------------*
DERB_Remove () - Removes SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE ESHOT *
DERB_Remove (
ESHOT * sh
)
{
   ESHOT * next;
  
   eshotnum--;

   next = sh->prev;
  
   sh->next->prev = sh->prev;
   sh->prev->next = sh->next;
  
   memset ( sh, 0, sizeof ( ESHOT ) );
  
   sh->next = free_derb;
  
   free_derb = sh;
  
   return ( next );
}
  
