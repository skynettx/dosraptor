#include <stdio.h>
#include <stdlib.h>
// #include <assert.h>
// #define ASSERT	assert

#include "types.h"
#include "exitapi.h"
#include "vmemapi.h"

void WalkHeap(void);

#define MAX_BLOCKS  63

typedef struct
{
	DWORD		size;
	VM_OWNER	*owner;
} MCB;

typedef struct
{
   BYTE		*blk[ MAX_BLOCKS ];
   MCB      *last_mcb[ MAX_BLOCKS ];
   DWORD    blocks;
   MCB      *rover;
	DWORD		discarded;
	DWORD		age;
} POOL;

#define BLK_FREE    0x80000000L
#define BLK_SIZE    0x00FFFFFFL
#define BLK_ID      0x7F000000L

PRIVATE POOL    	pool;

/*************************************************************************
 VM_InitMemory - Assign memory block to be used as virtual memory.
               - Can be called multiple times to add additional memory.
 *************************************************************************/
PUBLIC void
VM_InitMemory(
	BYTE		*memory,        // INPUT : Memory to be added to the pool
	DWORD		size            // INPUT : Size of memory
	)
{
	MCB			*mcb;

	ASSERT( pool.blocks < MAX_BLOCKS );
   ASSERT( size > 1024 );

   pool.blk[ pool.blocks ] = memory;
/*
* Create 1st Memory Block, the size of the block does include
* the MCB header.
*/
   mcb = ( MCB * )memory;
   mcb->size = size - sizeof( MCB );
   mcb->size |= BLK_FREE;
   mcb->size |= pool.blocks << 24;
   mcb->owner = NULL;
   if ( pool.rover == NULL )
      pool.rover = mcb;
/*
* Create Last Memory Block
*/
    mcb = ( MCB * )(( BYTE * ) mcb + ( mcb->size & BLK_SIZE ) );
    mcb->size = 0;
    mcb->owner = NULL;
    pool.last_mcb[ pool.blocks ] = mcb;

/*
* Chain previous last MCB to this block
*/
    if ( pool.blocks > 0 )
    {
        mcb = pool.last_mcb[ pool.blocks - 1 ];
        mcb->size = pool.blocks << 24;
    }
    pool.blocks++;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 vm_ColaceMem - Colace small fragments of memory into larger blocks.
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
PUBLIC DWORD
vm_ColaceMem(
    MCB 			*mcb
    )
{
	MCB  			*next_mcb;
	DWORD			mcb_size;

	mcb_size = ( DWORD )( mcb->size & BLK_SIZE );
/*
* Merge all free contiguous blocks.
*/
	next_mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
	while ( next_mcb->size & BLK_FREE )
	{
		mcb_size += ( DWORD ) ( next_mcb->size & BLK_SIZE );
		next_mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
	}
	mcb->size = (DWORD)( BLK_FREE | ( mcb->size & ~BLK_SIZE ) | mcb_size );
	return mcb_size;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 vm_DiscardMem - Disacard infrequently used memory 
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
PUBLIC MCB *
vm_DiscardMem(
	DWORD       size
	)
{
   MCB         *mcb;
   MCB         *free_mcb;
	VM_OWNER		*owner;
   DWORD       mcb_size;
   BOOL        mem_freed;
   DWORD    	lowcnt;
   DWORD    	lowsize;
	DWORD			oldage;
   MCB      	*low_mcb;

   do
   {
   	oldage = lowsize = lowcnt = ( DWORD ) ~0;
   	low_mcb = NULL;
   	mcb = ( MCB * )( pool.blk[ 0 ] );
      mem_freed = FALSE;
		pool.age--;

      for ( ;; )
      {
         mcb_size = mcb->size & BLK_SIZE;
			if ( mcb_size == 0 )
			{
         /*
         * At end of block, use the block # encoded in the size to
         * advance (or wrap) to the next block of memory.
         */
            mcb_size = mcb->size >> 24;
            mcb = ( MCB * )( pool.blk[ mcb_size ] );
            if ( mcb_size == 0 )
            {
               if ( low_mcb != NULL )
					{
						pool.discarded++;
               	low_mcb->owner->obj = NULL;
						vm_ColaceMem( low_mcb );
						return low_mcb;
					}
               break;
            }
				continue;
			}

			if ( ! ( mcb->size & BLK_FREE ) )
			{
				if ( ( owner = mcb->owner ) != NULL )
				{
         	/*
         	* Keep track of the least recently used memory block that is
				* large enough to use as we are searching for a free block.
         	*/
					if ( owner->age )
						owner->age--;

					if ( mcb_size >= size &&
							( owner->age < lowcnt ||
							( owner->age == lowcnt && lowsize > mcb_size )
						  )
						)
            	{
			 	 		lowsize = mcb_size;
               	lowcnt  = owner->age;
               	low_mcb = mcb;
            	}
					else if ( owner->age < oldage )
					{
						oldage = owner->age;
					}
         	}
			}
         mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
      }
   /*
   * Now go through and colace the heap
   */
      pool.rover = mcb = ( MCB * )( pool.blk[ 0 ] );
		free_mcb = NULL;
		oldage += 2;

      for ( ;; )
      {
         mcb_size = ( mcb->size & BLK_SIZE );
         if ( mcb->size & BLK_FREE )
         {
				free_mcb = mcb;
            mcb_size = vm_ColaceMem( mcb );
				if ( mcb_size >= size )
					return mcb;

            mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
				continue;
         }
			else if ( mcb_size )
			{
				if ( mcb->owner != NULL && mcb->owner->age <= oldage )
				{
					pool.discarded++;
            	mcb->size |= BLK_FREE;
            	mcb->owner->obj = NULL;
            	mcb->owner = NULL;
            	mem_freed = TRUE;
					if ( free_mcb )
					{
						mcb = free_mcb;
						continue;
					}
					free_mcb = mcb;
				}
				else
				{
					free_mcb = NULL;
				}
		      mcb = ( MCB * )( ( BYTE * ) mcb + ( mcb_size & BLK_SIZE ) );
			}
			else
	      {
         /*
         * At end of block, use the block # encoded in the size to
         * advance (or wrap) to the next block of memory.
         */
				free_mcb = NULL;
				mcb_size = mcb->size >> 24;
            mcb = ( MCB * )( pool.blk[ mcb_size ] );
            if ( mcb_size == 0 )
				{
               break;
				}
	      }
      }
   } while ( mem_freed == TRUE );
   return NULL;
}

/*************************************************************************
 VM_Malloc - Allocates a block of memory - swapping out other blocks if
             needed.
 *************************************************************************/
PUBLIC VOID *
VM_Malloc(
	UINT		size,       // INPUT : Size of object
	VM_OWNER	*owner,     // INPUT : Owner Structure, NULL=Locked
	BOOL		discard		// INPUT : Discard memory to satisfy request.
	)
{
   MCB      *mcb;
   MCB      *next_mcb;
   DWORD    mcb_size;
   BOOL     all_locked;

   ASSERT( pool.blocks > 0 );
/*
* Round block up to next DWORD in size, and add in overhead of MCB
*/
   size = ( ( size + 3 ) & ~3 ) + sizeof( MCB );

/*
* Search for free memory block across all pools...
*/
   all_locked = TRUE;

   mcb = pool.rover;
   do
   {
      while ( ! ( mcb->size & BLK_FREE ) )
      {
         mcb_size = ( DWORD )( mcb->size & BLK_SIZE );
			if ( all_locked == TRUE && mcb->owner != NULL && discard )
				all_locked = FALSE;

         if ( mcb_size > 0 )
            mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
         else
         {
         /*
         * At end of block, use the block # encoded in the size to
         * advance (or wrap) to the next block of memory.
         */
            mcb = ( MCB * )( pool.blk[ mcb->size >> 24 ] );
         }

         if ( mcb == pool.rover )
         {
            if ( all_locked == TRUE )
               return NULL;
            /*
            * Walked all of memory with no luck, start discarding least recently
            * used memory.
            */
            if ( ( mcb = vm_DiscardMem( size ) ) == NULL )
               return NULL;

				mcb_size = mcb->size & BLK_SIZE;
            goto FOUND_MCB;
         }
      }
      mcb_size = vm_ColaceMem( mcb );
		if ( mcb_size < size )
		{
		/*
		* Block is not big enough to satisfy request, goto next block
		*/
         mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
		}
   }
   while ( mcb_size < size && mcb != pool.rover );

   if ( mcb_size < size )
      return NULL;

FOUND_MCB:
   if ( mcb_size - size < ( sizeof( MCB ) + 4 ) )
	{
      size = mcb_size;
      next_mcb = ( MCB * )( ( BYTE * ) mcb + size );
	}
   else
   {
      next_mcb = ( MCB * )( ( BYTE * ) mcb + size );
      next_mcb->size = ( DWORD )( BLK_FREE | ( mcb->size & BLK_ID ) | ( mcb_size - size ) );
      next_mcb->owner = NULL;
   }
   pool.rover = next_mcb;
   mcb->size = (DWORD)( ( mcb->size & BLK_ID ) | size );
   mcb->owner = owner;
   if ( owner != NULL )
   {
      owner->obj = ( BYTE * ) mcb + sizeof( MCB );
      owner->age = ++pool.age;
   }
   return ( VOID * )( ( BYTE * ) mcb + sizeof( MCB ) );
}
	
/*************************************************************************
* VM_Touch - touch a peice of memory to keep track of most recently used.
 *************************************************************************/
PUBLIC VOID
VM_Touch(
	VM_OWNER	*owner		// INPUT : Owner of memory to touch.
	)
{
	if ( owner )
	{
		owner->age = ++pool.age;
	}
}

/*************************************************************************
 VM_Free - frees a block of memory allocated by VM_Malloc
 *************************************************************************/
PUBLIC VOID
VM_Free(
   VOID     *mem        // INPUT : Memory Object to Free
	)
{
   MCB      *mcb;

   if ( mem != NULL )
   {
      mcb = ( MCB * )( ( BYTE * ) mem - sizeof( MCB ) );
      if ( ! ( mcb->size & BLK_FREE ) )
      {
         if ( mcb->owner != NULL )
            mcb->owner->obj = NULL;
         mcb->owner = NULL;
         vm_ColaceMem( mcb );
         pool.rover = mcb;
      }
   }
}


/*************************************************************************
 VM_Lock - Locks a block of memory from being swapped out.
 *************************************************************************/
PUBLIC VOID
VM_Lock(
   VOID     *mem        // INPUT : Memory Object to Free
	)
{
   MCB      *mcb;

   ASSERT( mem != NULL );

   mcb = ( MCB * )( ( BYTE * ) mem - sizeof( MCB ) );
   if ( ! ( mcb->size & BLK_FREE ) )
   {
      mcb->owner = NULL;
   }
}
   

/*************************************************************************
 VM_Unlock - Unlocks a block of memory allowing it to be swapped out.
 *************************************************************************/
PUBLIC VOID
VM_Unlock(
   VOID     *mem,       // INPUT : Memory Object to Free
	VM_OWNER *owner      // INPUT : Owner Structure, NULL=Locked
	)
{
   MCB      *mcb;

   if ( mem != NULL )
   {
      mcb = ( MCB * )( ( BYTE * ) mem - sizeof( MCB ) );
      if ( ! ( mcb->size & BLK_FREE ) )
      {
         mcb->owner = owner;
			if ( owner != NULL )
			{
			   owner->obj = ( BYTE * ) mcb + sizeof( MCB );
				owner->age = pool.age;
			}
      }
   }
}
  

/*************************************************************************
 VM_GetCoreInfo - Get information on core resource
 *************************************************************************/
PUBLIC VOID
VM_GetCoreInfo(
	DWORD			*largest,		// OUTPUT: Largest block free
	DWORD			*totalfree,		// OUTPUT: Total amount free
	DWORD			*totallocked,	// OUTPUT: Total amount locked
	DWORD			*totalused,		// OUTPUT: Total amount used
	DWORD			*discarded		// OUTPUT: Number of items discarded
	)
{
	MCB			*mcb;
	MCB         *next_mcb;
	DWORD       mcb_size;
	DWORD			mem_size;
	DWORD			l_free;
	DWORD			l_used;
	DWORD			l_largest;
	DWORD			l_locked;

	l_locked = l_free = l_used = l_largest = 0;
	mcb = ( MCB * ) pool.blk[ 0 ];
	while ( mcb != NULL )
	{
		mcb_size = ( DWORD )( mcb->size & BLK_SIZE );
		if ( mcb->size & BLK_FREE )
		{
			mem_size = mcb_size - 8;
			l_free += mem_size;
			if ( l_largest < mem_size )
				l_largest = mem_size;
			next_mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
		}
		else
		{
			if ( mcb_size == 0 )
			{
         /*
         * At end of block, use the block # encoded in the size to
         * advance (or wrap) to the next block of memory.
         */
            mcb_size = mcb->size >> 24;
            next_mcb = ( MCB * )( pool.blk[ mcb_size ] );
			}
			else
			{
				l_used += mcb_size;
				if ( mcb->owner == NULL )
					l_locked += mcb_size;

				next_mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
			}
		}
		if ( next_mcb == ( MCB * ) pool.blk[ 0 ] )
			break;
		mcb = next_mcb;
	}
	if ( largest )
		*largest = l_largest;
	if ( totalfree )
		*totalfree = l_free;
	if ( totallocked )
		*totallocked = l_locked;
	if ( totalused )
		*totalused = l_used;
	if ( discarded )
		*discarded = pool.discarded;
}


#if 0
#define MAX_OBJECTS		256

VM_OWNER	obj_owner[ MAX_OBJECTS ];
BYTE	cmp_buf[ 32768 ];

PUBLIC VOID
WalkHeap(
	void
	)
{
	MCB			*mcb;
	MCB         *next_mcb;
	DWORD       mcb_size;

	mcb = ( MCB * ) pool.blk[ 0 ];
	while ( mcb != NULL )
	{
		mcb_size = ( DWORD )( mcb->size & BLK_SIZE );
		if ( mcb->size & BLK_FREE )
		{
			printf( "    FREE MEMORY of %4d bytes\n", mcb_size );
			next_mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
		}
		else
		{
			if ( mcb_size == 0 )
			{
         /*
         * At end of block, use the block # encoded in the size to
         * advance (or wrap) to the next block of memory.
         */
            mcb_size = mcb->size >> 24;
				printf( "END OF ARENA - NEXT ARENA IS %d\n",  mcb_size );

            next_mcb = ( MCB * )( pool.blk[ mcb_size ] );
			}
			else
			{
				int		j;
				BYTE		*mem;

				if ( mcb->owner == NULL )
					printf( "  LOCKED MEMORY of %4d bytes belongs to: ", mcb_size - sizeof( MCB ) );
				else
					printf( "UNLOCKED MEMORY of %4d bytes belongs to: ", mcb_size - sizeof( MCB ) );

				mem = ( BYTE * )mcb + sizeof( MCB );
				for ( j = 0; j < MAX_OBJECTS; j++ )
				{
					if ( obj_owner[ j ].obj == mem )
						break;
				}
				if ( j == MAX_OBJECTS )
				{
					printf( ">>>>>>>>> NO BODY <<<<<<<< LAST USED: %d\n", mcb->owner->id );
	  //				return;
				}
				else
				{
					if ( mcb->owner == NULL )
						printf( "object %3d\n", j );
					else
					{
						printf( "object %3d  age:%d  %s\n",
							j, obj_owner[j].age,
							( obj_owner[j].obj == mem ? "ok" : ">>>> OWNER CONFLICT <<<<" )
							);
					}
				}
				next_mcb = ( MCB * )( ( BYTE * ) mcb + mcb_size );
			}
		}
		if ( next_mcb == ( MCB * ) pool.blk[ 0 ] )
			break;
		mcb = next_mcb;
	}
	printf( "--------------MEMORY COMPLETE--------------\n" );
}


dumpbuf( char *buf, int len )
{
	int		j;
	int		k;

	printf( "\ndump of %d bytes:\n", len );
	for ( j = 0; j < len;  )
	{
		for ( k = 0; k < 16; k++ )
		{
			if ( k + j < len )
				printf( "%02x%c", buf[ j + k ],  ( k == 7 ) ? '-':' ' );
			else
				printf( "   " );
		}
		j += k;
		printf( "\n" );
	}
}

void main( void )
{
	DWORD	 		largest;
	DWORD	 		totalfree;
	DWORD	 		totallocked;
	DWORD	 		totalused;
	DWORD	 		discarded;
	char			*mem1;
	int			j;
	int			obj_size;
	void			*obj;

	if ( ( mem1 = malloc( 8192 ) ) == NULL )
	{
		printf( "\nMalloc of mem1 failed.\n" );
		return ;
	}
	memset( mem1, 0xFF, 8192 );
	VM_InitMemory( mem1, 8192 );

	if ( ( mem1 = malloc( 4096 ) ) == NULL )
	{
		printf( "\nMalloc of mem2 failed.\n" );
		return ;
	}
	memset( mem1, 0xFF, 4096 );
	VM_InitMemory( mem1, 4096 );
	WalkHeap();
	for ( j = 0; j < 8; j++ )
	{
	  	obj_size = 64 << ( j & 3 );
		VM_Malloc( obj_size, &obj_owner[ j ], TRUE );
		obj_owner[ j ].id = j;
		if ( obj_owner[ j ].obj != NULL )
		{
			printf( "Locking obj %d for %d bytes...\n", j, obj_size );
			memset( obj_owner[ j ].obj, j, obj_size );
			VM_Lock( obj_owner[ j ].obj );
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
	WalkHeap();

	printf( "Caching %d objects\n", MAX_OBJECTS - j );
	for ( ; j < 48; j++ )
	{
		obj_size = 64 << ( j & 3 );
		printf( "Caching obj %d for %d bytes...", j, obj_size );
		if ( ( obj = VM_Malloc( obj_size, &obj_owner[ j ], FALSE ) ) != NULL )
		{
			obj_owner[ j ].id = j;
//			obj_owner[ j ].age = ( ( j * 1527 * obj_size >> 1 ) ) & 0x00000FFF;
			memset( obj, j, obj_size );
			printf( "done.\n" );
		}
		else
		{
			printf( "failed.\n" );
			break;
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
	WalkHeap();
#if 0
	for ( j = 0; j < MAX_OBJECTS; j++ )
	{
		obj_size = 64 << ( j & 3 );
		printf( "Verifying obj %d for %d bytes...", j, obj_size );
		if ( obj_owner[ j ].obj != NULL )
		{
			memset( cmp_buf, j, obj_size );
			if ( memcmp( cmp_buf, obj_owner[ j ].obj, obj_size ) != 0 )
			{
				printf( "FAILED!\n" );
				dumpbuf( obj_owner[ j ].obj, obj_size );
				break;
			}
	  		printf( "ok.\n" );
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
#endif
	printf( "Allocating %d objects\n", MAX_OBJECTS - (MAX_OBJECTS/8));
	for ( j = 48; j < 96; j++ )
	{
		obj_size = 64 << ( j & 3 );
		printf( "Allocating obj %d for %d bytes...", j, obj_size );
		if ( ( obj = VM_Malloc( obj_size, &obj_owner[ j ], TRUE ) ) != NULL )
		{
			obj_owner[ j ].id = j;
			memset( obj, j, obj_size );
			printf( "done.\n" );
		}
		else
		{
			printf( "failed.\n" );
			break;
		}
	}
	WalkHeap();
	for ( j = 0; j < 96; j++ )
	{
		obj_size = 64 << ( j & 3 );
		printf( "Verifying obj %d for %d bytes...", j, obj_size );
		if ( obj_owner[ j ].obj == NULL )
			printf( "discarded.\n" );
		else
		{
			obj_owner[ j ].id = j;
			memset( cmp_buf, j, obj_size );
			if ( memcmp( cmp_buf, obj_owner[ j ].obj, obj_size ) != 0 )
			{
				printf( "FAILED!\n" );
				dumpbuf( obj_owner[ j ].obj, obj_size );
				break;
			}
 	  		printf( "ok.\n" );
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );

	for ( j = 8; j < 96; j++ )
	{
		if ( obj_owner[ j ].obj != NULL )
		{
			obj_size = 64 << ( j & 3 );
			printf( "Locking obj %d for %d bytes...\n", j, obj_size );
			VM_Lock( obj_owner[ j ].obj );
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
	WalkHeap();

/*
* DEBUG HERE
*/
	printf( "Allocating %d objects\n", MAX_OBJECTS );
	for ( j = 96; j < 144; j++ )
	{
		obj_size = 128 << ( j & 3 );
		printf( "Allocating obj %d for %d bytes...", j, obj_size );
		if ( ( obj = VM_Malloc( obj_size, &obj_owner[ j ], TRUE ) ) != NULL )
		{
			memset( obj, j, obj_size );
			printf( "done.\n" );
		}
		else
		{
			printf( "failed.\n" );
			break;
		}
	}
	WalkHeap();
	for ( j = 0; j < 144; j++ )
	{
		obj_size = 64 << ( j & 3 );
		printf( "Verifying obj %d for %d bytes...", j, obj_size );
		if ( obj_owner[ j ].obj == NULL )
			printf( "discarded.\n" );
		else
		{
			memset( cmp_buf, j, obj_size );
			if ( memcmp( cmp_buf, obj_owner[ j ].obj, obj_size ) != 0 )
			{
				printf( "FAILED!\n" );
				dumpbuf( obj_owner[ j ].obj, obj_size );
				break;
			}
	  		printf( "ok.\n" );
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );

	for ( j = 0; j < MAX_OBJECTS; j++ )
	{
		if ( obj_owner[ j ].obj != NULL )
		{
			obj_size = 64 << ( j & 3 );
			printf( "freeing obj %d for %d bytes...\n", j, obj_size );
			VM_Free( obj_owner[ j ].obj );
		}
	}
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
	WalkHeap();
	printf( "Allocating %d objects\n", MAX_OBJECTS );
	for ( j = 0; j < 144; j++ )
	{
		obj_size = 128 << ( j & 3 );
		printf( "Allocating obj %d for %d bytes...", j, obj_size );
		if ( ( obj = VM_Malloc( obj_size, &obj_owner[ j ], TRUE ) ) != NULL )
		{
			memset( obj, j, obj_size );
			printf( "done.\n" );
		}
		else
		{
			printf( "failed.\n" );
			break;
		}
	}
	WalkHeap();
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
	for ( j = 0; j < MAX_OBJECTS; j++ )
	{
		if ( obj_owner[ j ].obj != NULL )
		{
			obj_size = 64 << ( j & 3 );
			printf( "freeing obj %d for %d bytes...\n", j, obj_size );
			VM_Free( obj_owner[ j ].obj );
		}
	}
	WalkHeap();
	VM_GetCoreInfo( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
}
#endif
