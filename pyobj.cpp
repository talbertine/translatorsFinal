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

//Allows us to keep sane maps
struct cmpPyObj {
    bool operator()(const struct pyobj*& a, const struct pyobj*& b) const {
        if (a->type != b->type)
        	return a->type < b->type;
        switch (a->type){
        case PY_INT:
        	return *(int *)a->value < *(int *)b->value;
        case PY_FLOAT:
        	return *(double *)a->value < *(double *)b->value;
        case PY_BOOL:
        	return *(bool *)a->value < *(bool *)b->value;
        case PY_NONE:
        	return false;
        default:
        	throw "Cannot index by a list or dict. That's just dumb."
        }
    }
};

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

void pyobjFree(struct pyobj *obj){
	switch (obj->type){
	case PY_INT:
	case PY_FLOAT:
	case PY_BOOL:
		free(*obj->value)
		free(obj);
		break;
	case PY_LIST:
		vector<struct pyobj **> data = *(vector<struct pyobj**> *)obj->value;
		for (int i = 0; i < data.size(); i++){
			pyobjDecRef(*data[i]);
			free(data[i]);
		}
		delete data;
		free(obj);
		break;
	case PY_DICT:
		map<struct pyobj *, struct pyobj **, cmpPyObj> data = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)obj->value;
		for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = data.begin(); itr != data.end(); ++itr){
			pyobjDecRef(itr->first);
			pyobjDecRef(*itr->second);
			free(itr->second);
		}
		delete data;
		free(obj);
		break
	case PY_NONE:
		free(obj);
		break;
	default:
		throw "Unexpected type in pyobjFree";
	}
}

string pyobjToString(struct pyobj *value){
	string retval = ""
	stringstream ss;
	switch (value->type){
	case PY_INT:
		ss << *(int *) value->value;
		ss >> retval;
	case PY_FLOAT:
		ss << *(double *)value->value;
		ss >> retval;
	case PY_BOOL:
		if (*(bool *) value->value){
			retval = "True";
		} else {
			retval = "False";
		}
	case PY_LIST:
		retval = "["
		vector<struct pyobj **> data = *(vector<struct pyobj **> *) value->value;
		for (int i = 0; i < data.size(); ++i){
			retval += pyobjToString(*data[i]);
			if (data.size() - 1 != i){
				retval += ", "
			}
		}
		retval += "]"
	case PY_DICT:
		map<struct pyobj *, struct pyobj **, cmpPyObj> data = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)obj->value;
		vector<string> entries = vector<string>
		for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = data.start(); itr != data.end(); ++itr){
			string entry = pyobjToString
			entries.push_back()
		}
	case PY_NONE:
		retval = "None"
	}
	return retval;
}

//Statements:
//Corresponds to print
void pyobjPrint(vector<struct pyobj *> values, bool newline){
	string printVal = string("");

}

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