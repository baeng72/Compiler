#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "set.h"

PUBLIC SET* newset()
{
	SET* p;
	if (!(p = (SET*)malloc(sizeof(SET))))
	{
		fprintf(stderr, "Can't get memory to create set\n");
		raise(SIGABRT);
		return NULL;
	}
	memset(p, 0, sizeof(SET));
	p->map		=	p->defmap;
	p->nwords	= _DEFWORDS;
	p->nbits	= _DEFBITS;
	return p;
}

PUBLIC void delset(SET* set)
{
	if (set->map != set->defmap)
		free(set->map);
	free(set);
}

PUBLIC SET* dupset(SET* set)
{
	SET* newSet;

	if (!(newSet = (SET*)malloc(sizeof(SET))))
	{
		fprintf(stderr,"Can't get memory to duplicate set\n");
		exit(1);
	}

	memset(newSet, 0, sizeof(SET));
	newSet->complement = set->complement;
	newSet->nwords = set->nwords;
	newSet->nbits = set->nbits;

	if (set->map == set->defmap)				//default bit map in use?
	{
		newSet->map = newSet->defmap;
		memcpy(newSet->defmap, set->defmap, _DEFWORDS * sizeof(_SETTYPE));
	}
	else
	{ 
		newSet->map =(_SETTYPE*)malloc(set->nwords * sizeof(_SETTYPE));
		if (!newSet->map)
		{
			fprintf(stderr, "Can't get memory to duplicate set bit map\n");
			exit(1);
		}
		memcpy(newSet->map, set->map, set->nwords * sizeof(_SETTYPE));
	}
	return newSet;
}

PRIVATE void enlarge(int need, SET* set)
{
	_SETTYPE* pNew;

	if (!set || need <= set->nwords)
		return;

	D(printf("enlarging %d word map to %d words\n", set->nwords, need));

	if (!(pNew = (_SETTYPE*)malloc(need * sizeof(_SETTYPE))))
	{
		fprintf(stderr, "Can't get memory to expand set\n");
		exit(1);
	}
	memcpy(pNew, set->map, set->nwords * sizeof(_SETTYPE));
	memset(pNew + set->nwords, 0, (need - set->nwords) * sizeof(_SETTYPE));

	if (set->map != set->defmap)
		free(set->map);

	set->map = pNew;
	set->nwords = (unsigned char)need;
	set->nbits = need * _BITS_IN_WORD;
}

PUBLIC int _addset(SET* set, int bit)
{
	enlarge(_ROUND(bit), set);
	return _GBIT(set, bit, |=);
}

PUBLIC int num_ele(SET* set)
{
	static unsigned char nbits[] =
	{
		0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,		//0-15
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,		//16-31
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,		//32-47
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,		//48-63
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,		//64-79
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,		//80-95
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,		//96-111
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,		//112-127
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,		//128-143
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,		//144-159
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,6,6,		//160-175
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,		//176-191
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,		//192-207
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,		//208-223
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,		//224-239
		4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8			//240-255
	};
	int					i;
	unsigned int		count = 0;
	unsigned char* p;

	if (!set)
		return 0;

	p = (unsigned char*)set->map;
	for (i = _BYTES_IN_ARRAY(set->nwords); --i >= 0;)
		count += nbits[*p++];
	return count;
}

PUBLIC int _set_test(SET* set1, SET* set2)
{
	//compares two sets. returns as follows:
	//
	//_SET_EQUIV		Sets are equivalent
	//_SET_INTER		Sets intersect but aren't equivalent
	//_SET_DISJ			Sets are disjoint
	//
	//The smaller set is made larger if the two sets are different sizes.
	int			i, rval = _SET_EQUIV;
	_SETTYPE* p1, * p2;
	i = max(set1->nwords, set2->nwords);
	enlarge(i, set1);
	enlarge(i, set2);
	p1 = set1->map;
	p2 = set2->map;
	for (; --i >= 0; p1++, p2++)
	{
		if (*p1 != *p2)
		{
			//You get here if the sets aren't equivalent. You can return
			//immediately if the sets intersect but have to keep going in the case
			// of disjoint sets (because the sets might acutally intersect at some byte, as yet unseen)
			if (*p1 & *p2)
				return _SET_INTER;
			else
				rval = _SET_DISJ;
		}
	}
	return rval;
}

PUBLIC int setcmp(SET* set1, SET* set2)
{
	//Works like strcmp(), returning 0 if the sets are equivalent, <0 if set1<set2 and >0 if set1>set2.
	int		i, j;
	_SETTYPE* p1, * p2;

	i = j = min(set1->nwords, set2->nwords);

	for (p1 = set1->map, p2 = set2->map; --j >= 0; p1++, p2++)
	{
		if (*p1 != *p2)
			return *p1 - *p2;
	}
	//you get here only if all words that exist in both sets are the same.
	//check tail end of the larger array for all zeros.
	if ((j = set1->nwords - i) > 0)//set1 is larger
	{
		while (--j >= 0)
		{
			if (*p1++)
				return 1;
		}
	}
	else if ((j = set2->nwords - i) > 0)//set 2 is larger
	{
		while (--j >= 0)
		{
			if (*p2++)
				return -1;
		}
	}
	return 0;

}

PUBLIC unsigned sethash(SET* set1)
{
	//Hash the set by summing together the words in the bit map
	_SETTYPE* p;
	unsigned	total;
	int			j;
	total = 0;
	j = set1->nwords;
	p = set1->map;

	while (--j >= 0)
		total += *p++;

	return total;
}

PUBLIC int	subset(SET* set, SET* possible_subset)
{
	//Return 1 is "possible_subset" is a subset of "set".
	//One is returned if it's a subset, zero otherwise.
	//Empty sets are subsets of everything.
	//The routine silently malfunctions if give a NULL set, however.
	//If the "possible_subset" is larger than "set", then the extra bytes must be all zeros.
	_SETTYPE* subsetp, * setp;
	int	common;						//This many bytes in potential subset
	int	tail;
	if (possible_subset->nwords > set->nwords)
	{
		common = set->nwords;
		tail = possible_subset->nwords - common;
	}
	else
	{
		common = possible_subset->nwords;
		tail = 0;
	}
	subsetp = possible_subset->map;
	setp = set->map;

	for (; --common >= 0; subsetp++, setp++)
	{
		if ((*subsetp & *setp) != *subsetp)
			return 0;	
	}

	while (--tail >= 0)
	{
		if (*subsetp++)
			return 0;
	}
	return 1;

}

PUBLIC void _set_op(int op,SET* dest,SET* src)
{
	//Performs binary operations depending on op:
	//
	// _UNION:				dest = union of source and dest
	// _INTERSECT:			dest = intersection of src and dest
	// _DIFFERENCE:			dest = symmetric difference of src and dest
	// _ASSIGN:				dest = src;

	//The sizes of the destination set is adjusted so that it's the same size
	//as the source set.

	_SETTYPE* d;			//Pointer to the destination map
	_SETTYPE* s;			//Pointer to the map in src
	int			ssize;		//Number of words in src set
	int			tail;		//dest set is this much bigger

	ssize = src->nwords;

	if ((unsigned)dest->nwords < (unsigned)ssize)	//make sure dest set is at least as big as the src set
		enlarge(ssize, dest);

	tail = dest->nwords - ssize;
	d = dest->map;
	s = src->map;

	switch (op)
	{
	case _UNION:
		while (--ssize >= 0)
			*d++ |= *s++;
		break;
	case _INTERSECT:
		while (--ssize >= 0)
			*d++ &= *s++;		
		while (--tail >= 0)
			*d++ = 0;
		break;
	case _DIFFERENCE:
		while (--ssize >= 0)
			*d++ ^= *s++;
		break;
	case _ASSIGN:
		while (--ssize >= 0)
			*d++ = *s++;
		while (--tail >= 0)
			*d++ = 0;
		break;
	}
}

PUBLIC void insert(SET* set)
{
	//Physically inver the bits in the set.
	//Compare with the COMPLEMENT() macro,
	//which just modifies the complement bit.
	_SETTYPE* p, * end;

	for (p = set->map, end = p + set->nwords; p < end; p++)
		*p = ~*p;
}


PUBLIC void truncate(SET* set)
{
	//Clears the set but also sets it back to the original default size.
	//Compare this routine with the CLEAR() macro which clears all bits in the map
	//but doesn't modify the size.
	if (set->map != set->defmap)
	{
		free(set->map);
		set->map = set->defmap;
	}
	set->nwords = _DEFWORDS;
	set->nbits = _DEFBITS;
	memset(set->defmap, 0, sizeof(set->defmap));
}

PUBLIC int next_member(SET* set)
{
	//set == NULL						reset
	//set changed from last call		reset and return first element
	//otherwise							return next element or -1 if none

	static SET* oset = NULL;			//"set" arg in last call
	static unsigned	current_member = 0;		//last-accessed member of cur. set
	_SETTYPE*	map;

	if (!set)
		return ((int)(oset = NULL));

	if (oset != set)
	{
		oset = set;
		current_member = 0;
		for (map = set->map; *map == 0 && current_member < set->nbits; ++map)
		{
			current_member += _BITS_IN_WORD;
		}
	}
		//The increment must put into the test because, if the TEST() invocation
		//evaluates to true, then an increment on the right of a for() statement
		//would never be executed.

	while (current_member++ < set->nbits)
	{
		if (TEST(set, current_member - 1))
		{
			return (current_member - 1);
		}
	}
	return -1;
	

}


PUBLIC void pset(SET* set, int(*output_routine)(void*,const char*,int), void* param)
{
	int i, did_something = 0;

	if (!set)
		(*output_routine)(param, "null", -1);
	else
	{
		next_member(NULL);
		while ((i = next_member(set)) >= 0)
		{
			did_something++;
			(*output_routine)(param, "%d ", i);
		}
		next_member(NULL);

		if (!did_something)
			(*output_routine)(param, "empty", -2);
	}

}