#pragma once
//Code from Compiler Design in C (Alan I Holub 1990)
typedef		unsigned	short	_SETTYPE;						//one cell in a bit map (WORD size)
#define		_BITS_IN_WORD		16				
#define		_BYTES_IN_ARRAY(x)	(x << 1)						//# of bytes in bit map x^2 (assumes 16 bit unsigned short)
#define		_DIV_WSIZE(x)		((unsigned)(x) >> 4)			//divide by 16 (2^4 = 16)
#define		_MOD_WSIZE(x)		((x) & 0xf)						//modulo of 16 
#define		_DEFWORDS			8								//elements in default set 
#define		_DEFBITS			(_DEFWORDS * _BITS_IN_WORD)		//bits in default set 8 * 16 (512 bits)
#define		_ROUND(bit)			(((_DIV_WSIZE(bit)+8)>>3)<<3)	//

typedef	struct _set_
{
	unsigned char		nwords;									//Number of words in map
	unsigned char		complement;								//is a negative true set if true (compl - orig. name is c++ keyword for ~)
	unsigned			nbits;									//Number of bits in map
	_SETTYPE*			map;									//pointer to the map
	_SETTYPE			defmap[_DEFWORDS];						//The map itself
}SET, * PSET;

extern	int		_addset			P((SET*, int				));
extern	void	delset			P((SET*));
extern	SET*	dupset			P((SET*));
extern	void	invert			P((SET*));
extern	SET*	newset			P((void));
extern	int		next_member		P((SET*));
extern	int		num_ele			P((SET*));
extern	void	pset			P((SET*, int(*)(void*,const char*,int), void*));
extern	void	_set_op			P((int, SET*, SET*));
extern	int		_set_test		P((SET*, SET*));
extern	int		setcmp			P((SET*, SET*));
extern	unsigned sethash		P((SET*));
extern	int		subset			P((SET*, SET*));
extern	void	truncate		P((SET*));

#define	_UNION		0										//x is in s1 or s2
#define	_INTERSECT	1										//x is in s1 and s2
#define	_DIFFERENCE	2										//(x in s1) and (x not in s2)
#define	_ASSIGN		4										//s1 = s2

#define	UNION(d,s)				_set_op(_UNION,d,s)
#define	INTERSECT(d,s)			_set_op(_INTERSECT,d,s)
#define DIFFERENCE(d,s)			_set_op(_DIFFERNECE,d,s)
#define	ASSIGN(d,s)				_set_op(_ASSIGN,d,s)

#define CLEAR(s)				memset((s)->map,0,(s)->nwords * size(_SETTTYPE))
#define FILL(s)					memset((s)->map,~0,(s)->nwords * sizeof(_SETTYPE))
#define	COMPLEMENT(s)			((s)->complement = ~(s)->complement)

#define _SET_EQUIV				0						//value returned from _set_test, equivalent
#define	_SET_DISJ				1						//disjoint
#define _SET_INTER				2						//intersect

#define IS_DISJOINT(s1,s2)		(_set_test(s1,s2)==_SET_DISJ)
#define	IS_INTERSECTING(s1,s2)	(_set_test(s1,s2)==_SET_INTER)
#define	IS_EQUIVALENT(a,b)		(setcmp((a),(b))==0)
#define IS_EMPTY(s)				(num_ele(s) == 0)

//the follow have side-effects

#define	_GBIT(s,x,op)	(((s)->map)[_DIV_WSIZE(x)] op (1 << _MOD_WSIZE(x)))

#define REMOVE(s,x)		(((x) >= (s)->nbits) ? 0			: _GBIT(s,x,&= ~))	//set &=~bit (remove bit)
#define	ADD(s,x)		(((x) >= (s)->nbits) ? _addset(s,x)	: _GBIT(s,x, |=))	//set |= bit (or bit)
#define MEMBER(s,x)		(((x) >= (s)->nbits) ? 0			: _GBIT(s,x,&))		//set & bit  (test bit)
#define	TEST(s,x)		((MEMBER(s,x))		? !(s)->complement:(s)->complement) //if set, return true not in complement set

				