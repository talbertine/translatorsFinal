#include <vector>

using std::vector;

/*
 * In general, reference count is decreased when an object is "consumed". When you read from a name, 
 * it temporarily increases the reference count, because it is used in an expression.
 */ 


//Data type for every data object in the python code
struct pyobj;

//Call before anything happens
void pyobjInit();

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
struct pyobj *pyobjDecRef(struct pyobj *val)


