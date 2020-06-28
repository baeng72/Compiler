#pragma once
//Code from Compiler Design in C (Alan I Holub 1990)
#ifdef DEBUG
#define	PRIVATE
#define	D(x) x
#else
#define	PRIVATE	static
#define	D(x)
#endif // DEBUG

#define PUBLIC

#ifdef _WIN32		//some of the MS dos stuff assums 16:16 segmented addressing, so don't want or need that on 32bit or greater
#define MS(x)
#define US(x)
#define ANSI
//#elif MSDOS
//#define	MS(x) x
//#define	UX(x)
//#define	ANSI
//#define	_8086
#else
#define	MS(x)
#define	UX(x) x
#define	O_BINARY	0
typedef long time_t;	//for the VAX
typedef unsigned size_t;
extern char* strdup();
#endif // MSDOS

#ifdef ANSI
#define	P(x) x
#define VA_LIST ...
#else
#define P(x) ()
#define void char
#define VA_LIST _a_r_g_s
#endif

#ifdef _8086
#define SEG(p)		(((unsgined*)&(p))[1])	//the old seg:offset from dos days
#define	OFF(p)		(((unsigned*)&(p))[0])
#define PHYS(p)		(((unsigned long)OFF(p)) + ((unsigned long)SET(p)<<4)) // turn 16 bit seg and 16 bit offset into 32 bit mem. location
#endif

#define	NUMELE(a)		(sizeof(a)/sizeof(*(a)))				//array size in elements
#define	LASTELE(a)		((a) + (NUMELE(a)-1))					//pointer to the last element
#define	TOOHIGH(a,p)	((p) - (a) > (NUMELE(a)-1))				//out of bounds on high side?
#define	TOOLOW(a,p)		((p) - (a) < 0)							//out of bounds on low size?
#define	INBOUNDS(a,p)	(! (TOOHIGH(ap,p) || TOOLOW(a,p)))	//bounds checking

#define _IS(t,x)	(((t) 1 << (x)) != 0)	//evaluate true if width of a variable of type of t is < x. The !=0 assures that the answer is 1 or 0.

#define	NBITS(t)	(4 * (1 + _ISt,4) + _IS(t,8))+ _IS(t,12) + _IS(t,16) + _IS(t,20) + _IS(t,24) + _IS(t,28) + _IS(t,32)))

#define MAXINT (((unsigned)~0)>>1)

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define RANGE(a,b,c)	((a) <= (b) && (b) <= (c))
