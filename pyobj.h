#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdarg>

using std::map;
using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;


#define PY_INT 0
#define PY_FLOAT 1
#define PY_BOOL 2
#define PY_LIST 3
#define PY_DICT 4
#define PY_NONE 5

/*
 * In general, reference count is decreased when an object is "consumed". When you read from a name, 
 * it temporarily increases the reference count, because it is used in an expression.
 */ 

//Debugging functions
long pyobjGetRef(struct pyobj *var);
char pyobjGetType(struct pyobj *var);
void * pyobjGetData(struct pyobj *var);


//Data type for every data object in the python code
struct pyobj;

//Call before anything happens
void pyobjInit();
void pyobjFree(struct pyobj *);

//Statements:
//Corresponds to print
void pyobjPrint(bool newline, int size ...);

//Assignment
// Does not decrement reference count in order to facilitate chaining. Needs to be done manually at the end.
struct pyobj *pyobjAssign(struct pyobj **var,  pyobj *val);

//Increments reference count
struct pyobj *pyobjIncRef(struct pyobj *val);

//Decrements reference count
//Used internally so that reference counting is neat
struct pyobj *pyobjDecRef(struct pyobj *val);

//Handy for for loops
bool pyobjToBool(struct pyobj *val); 
struct pyobj *pyobjIndex(struct pyobj *itr, struct pyobj *index);
struct pyobj *pyobjGetItr(struct pyobj *obj);
int pyobjGetLen(struct pyobj *obj);

//Literals
struct pyobj *pyobjInt(int n);
struct pyobj *pyobjBool(bool n);
struct pyobj *pyobjFloat(double n);
struct pyobj *pyobjList(int size, ...); //Size, then values as struct pyobj *
struct pyobj *pyobjList(struct pyobj **vals, int size);
struct pyobj *pyobjDict(int size, ...); //Size, then key value pairs as struct pyobj *, alternating between each

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

struct pyobj *pyobjIfExp(struct pyobj *cond, struct pyobj *body, struct pyobj *orelse);
