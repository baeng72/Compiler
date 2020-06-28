#pragma once
//NFA state

//Non-character values of NFA.edge
#define		EPSILON		-1
#define		CCL			-2
#define		EMPTY		-3

//Values of the anchor field
#define		NONE		0					//Not anchored
#define		START		1					//Anchored at start of line
#define		END			2					//Anchored at end of line
#define		BOTH		( START | END )		//Anchored in both places

typedef struct nfa
{
	int			edge;			//label for edge: character, CCL, EMPTY, or EPSILON
	SET*		bitset;			//Set to store character classes
	struct nfa* next;			//Next state (or NULL if none)
	struct nfa* next2;			//Another next state if edge == EPSILON, NULL if this state isn't used
	char*		accept;			//NULL if not an accepting state, else a point to the action string
	int			anchor;			//Says whether pattern is anchored and, if so, where (uses #defines above)
}NFA,*PNFA;
