#include "lex.h"
#include <stdio.h>
#include <assert.h>
int printfunc(void* parm, const char* msg, int code)
{
	return printf(msg, code);
}
void setTest()
{
	SET* set = newset();
	assert(set->nwords == _DEFWORDS);
	assert(set->nbits == _DEFBITS);
	assert(set->map == set->defmap);
	assert(IS_EMPTY(set));
	SET* set2 = dupset(set);
	assert(set->nwords == set2->nwords);
	assert(set->nbits == set2->nbits);
	ADD(set, 32);
	assert(set->map == set->defmap);
	assert(set->nwords == set2->nwords);
	assert(set->nbits == set2->nbits);
	assert(num_ele(set) == 1);
	ADD(set, 63);
	assert(set->map == set->defmap);
	assert(num_ele(set) == 2);
	assert(IS_DISJOINT(set, set2));//sets are disjoint
	assert(set->map == set->defmap);
	assert(!IS_INTERSECTING(set, set2));//sets don't intersect
	assert(set->map == set->defmap);
	delset(set2);
	set2 = dupset(set);
	assert(IS_EQUIVALENT(set, set2));
	truncate(set2);
	assert(set2->nbits == _DEFBITS);
	assert(set2->nwords == _DEFWORDS);
	assert(set2->defmap == set2->map);
	pset(set,printfunc,NULL);
}

int main(void)
{
	setTest();
	return 0;
}