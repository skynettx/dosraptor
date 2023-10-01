#ifndef _TYPES_H
#define _TYPES_H

typedef enum
{
	FALSE,
	TRUE
}BOOL;

#define LOCAL static
#define PRIVATE static
#define PUBLIC
#define TSMCALL
#define SPECIAL
#define MACRO
#define NUL (VOID *)0
#define EMPTY ~0
#define ASIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FMUL32(a) ( ( a ) << 5 )

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef void VOID;
typedef char CHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef int	INT;
typedef unsigned int UINT;

int random(int);
#define random( x ) ( rand() % x )

#endif