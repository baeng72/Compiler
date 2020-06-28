#pragma once

typedef struct BUCKET
{
	struct BUCKET* next;
	struct BUCKET** prev;
}BUCKET,*PBUCKET;

typedef struct hash_tab_
{
	int		size;			//Max number of elements in table
	int		numsyms;		//number of elements currently in tabl
	unsigned (*hash)(void*);		//hash function
	int		(*cmp)(void*,void*);		//comparison funct, cmp(name,bucket_p)
	BUCKET* table[1];		//First element of actual hash table
}HASH_TAB;
