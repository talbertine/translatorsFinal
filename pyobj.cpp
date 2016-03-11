#include "pyobj.h"
#include <cstdlib>

#define PY_INT 0
#define PY_FLOAT 1
#define PY_BOOL 2
#define PY_LIST 3
#define PY_DICT 4
#define PY_NONE 5

//Data type for every data object in the python code
struct pyobj{
	char type;
	void *value;
	long reference;
};
struct pyobj *trueConst;
struct pyobj *falseConst;
struct pyobj *noneConst;

//Call before anything happens
void pyobjInit(){
	trueConst = malloc(sizeof(struct pyobj));
	trueConst->type = PY_BOOL;
	trueConst->value = malloc(sizeof(bool));
	*trueConst->value = true;
	trueConst->reference = 2;

	falseConst = malloc(sizeof(struct pyobj));
	falseConst->type = PY_BOOL;
	falseConst->value = malloc(sizeof(bool));
	*falseConst->value = false;
	falseConst->reference = 2;

	noneConst = malloc(sizeof(struct pyobj));
	noneConst->type = PY_NONE;
	noneConst->reference = 2;
}

void pyobjFree(struct pyobj *){
	switch (pyobj->type){
	case PY_INT:
		
	case PY_FLOAT:

	case PY_BOOL:

	case PY_LIST:

	case PY_DICT:

	case PY_NONE:
	}
}

//Statements:
//Corresponds to print
void pyobjPrint(vector<struct pyobj *> values, bool newline);

//Assignment
// Does not decrement reference count in order to facilitate chaining. Needs to be done manually at the end.
struct pyobj *pyobjAssign(struct pyobj **var,  pyobj *val);

//Increments reference count
struct pyobj *pyobjIncRef(struct pyobj *val);

//Decrements reference count
//Used internally so that reference counting is neat
struct pyobj *pyobjDecRef(struct pyobj *val);

//Handy for for loops
struct pyobj *pyobjIndex(struct pyobj *itr, struct pyobj *index);
struct pyobj *pyobjGetItr(struct pyobj *obj);
int pyobjGetLen(struct pyobj *obj);

//Literals
struct pyobj *pyobjInt(int n);
struct pyobj *pyobjBool(bool n);
struct pyobj *pyobjFloat(double n);
struct pyobj *pyobjList(struct pyobj vals[], int size);
struct pyobj *pyobjDict(struct pyobj *keys[], struct pyobj *vals[], int size);

//Global Constants
//Not used by local names, but used when returning booleans from functions, for example
//That is, only used internally
struct pyobj *pyobjTrue();
struct pyobj *pyobjFalse();
struct pyobj *pyobjNone();

//Loading names
struct pyobj *pyobjLoadName(struct pyobj *var);
struct pyobj *pyobjSubscriptLoad(struct pyobj *ls, struct pyobj *idx);

//Storing stuff
struct pyobj **pyobjSubscriptStore(struct pyobj *ls, struct pyobj *idx);

//Operations
struct pyobj *pyobjUAdd(struct pyobj *val);
struct pyobj *pyobjUSub(struct pyobj *val);
struct pyobj *pyobjNot(struct pyobj *val);
struct pyobj *pyobjAnd(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjOr(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjAdd(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjSub(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjMult(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjDiv(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjFloorDiv(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjMod(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjPow(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjIs(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjIsNot(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjIn(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjNotIn(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjEq(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjNotEq(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjLt(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjLtE(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjGt(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjGtE(struct pyobj *left, struct pyobj *right);