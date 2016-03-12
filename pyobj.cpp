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
		map<struct pyobj *, struct pyobj **> data = *(map<struct pyobj *, struct pyobj **> *)obj->value;
		for (map<struct pyobj *, struct pyobj **>::iterator itr = data.begin(); itr != data.end(); ++itr){
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
	stringstream
	switch (value->type){
	case PY_INT:
		retval = 
	case PY_FLOAT:
	case PY_BOOL:
	case PY_LIST:
	case PY_DICT:
	case PY_NONE:
	}
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
struct pyobj *pyobjAnd(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjOr(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjAdd(struct pyobj *left, struct pyobj *right);
struct pyobj *pyobjSub(struct pyobj *left, struct pyobj *right);
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


//Hailey's Shit
struct pyobj *pyobjUAdd(struct pyobj *val){
    struct pyobj *retval;
    double valData;
    
    if (val->type == PY_INT){
        int valData = *(int*)val->value;
        pyobj *retval = pyobjInt(valData);
    }
    
    else if (val->type == PY_BOOL){
        if (*(bool*)val->value == false){
            valData = 0;
        }
        else {
            valData = 1;
        }
        retval = pyobjInt(valData);
    }
    
    else if (val->type == PY_FLOAT){
        valData = *(double*)val->value;
        pyobj *retval = pyobjFloat(valData);
    }
    
    else {
        cout << "Error: UAdd on Dict, List, or None not possible";
        return 0;
    }
    
    pyobjDecRef(val);
    return retval;
}

struct pyobj *pyobjUSub(struct pyobj *val){
    struct pyobj *retval;
    double valData;
    
    if (val->type == PY_INT){
        valData = *(int*)val->value;
        retval = pyobjInt(-valData);
    }
    
    else if (val->type == PY_BOOL){
        if (*(bool*)val->value == false){
            valData = 0;
        }
        else {
            valData = -1;
        }
        retval = pyobjInt(valData);
    }
    
    else if (val->type == PY_FLOAT){
        valData = *(double*)val->value;
        retval = pyobjFloat(-valData);
    }
    
    else {
        cout << "Error: USub on Dict, List, or None not possible";
        return 0;
    }
    
    pyobjDecRef(val);
    return retval;
}


struct pyobj *pyobjNot(struct pyobj *val){
    struct pyobj *retval;
    bool valData;
    
    if (val->type == PY_INT){
        if (*(int*)val->value == 0){
            valData = true;
        }
        
        else {
           valData = false;
        }
        
        pyobj *retval = pyobjBool(valData);
    }
    
    else if val->type == PY_BOOL{
        if (*(bool*)val->value == false) {
            valData = true;
        }
        
        else {
            valData = false;
        }
        
        pyobj *retval = pyobjBool(valData);
    }
    
    else if val->type == PY_FLOAT{
        valData = *(double*)val->value;
        retval = pyobjBool(valData);
    }
    
    else if val->type == PY_LIST{
        vector<struct pyobj **> data = *(vector<struct pyobj **> *) val->value;
        if (data.size == 0) {
            valData = true;
        }
        else {
            valData = false;
        }
    }
    
    else if val->type == PY_DICT{
        map<struct pyobj *, struct pyobj **> data = *(map<struct pyobj *, struct pyobj **> *) val->value;
        if (data.size == 0) {
            valData = true;
        }
        else {
            valData = false;
        }
    }
    
    else if val->type == PY_NONE{
        valData = true;
        retval = pyobjBool(valData);
    }
    
    else {
        cout << "Error: pyobjNot";
        return 0;
    }
    
    pyobjDecRef(val);
    return retval;
}


struct pyobj *pyobjMult(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    
    if (left->type == PY_INT){
        
        int leftData = *(int*)left->value;
        
        if (right->type == PY_INT){
            int rightData = *(int*)right->value;
            int resData = leftData * rightData;
            retval = pyobjInt(resData);
        }
        
        else if (right->type == PY_BOOL){
            if (*(bool*)right->value == false){
                int rightData = 0;
            }
            else {
                int rightData = 1;
            }
            int resData = leftData * rightData;
            retval = pyobjInt(resData);
        }
        
        else if (right->type == PY_FLOAT){
            double rightData = *(double*)right->value;
            double resData = leftData * rightData;
            retval = pyobjFloat(resData);
        }
        
        else if (right->type == PY_LIST){
            vector<struct pyobj **> data = *(vector<struct pyobj **> *) val->value;
            double resData = leftData * data;
            retval = pyobjList(resData);
        }
        
    else if (right->type == PY_INT){
        int rightData = *(int*)right->value;
        
        if (left->type == PY_INT){
            int leftData = *(int*)left->value;
            int resData = leftData * rightData;
            retval = pyobjInt(resData);
        }
            
        else if (left->type == PY_BOOL){
            if (*(bool*)left->value == false){
                int leftData = 0;
            }
            else {
                int leftData = 1;
            }
            int resData = leftData * rightData;
            retval = pyobjInt(resData);
        }
            
        else if (left->type == PY_FLOAT){
            double leftData = *(double*)left->value;
            double resData = leftData * rightData;
            retval = pyobjFloat(resData);
        }
            
        else if (left->type == PY_LIST){
            vector<struct pyobj **> data = *(vector<struct pyobj **> *) val->value;
            vector<struct pyobj **> resData = leftData * data;
            retval = pyobjList(resData);
        }

    else if ((left->type && right->type) == PY_FLOAT){
        double leftData = *(double*)left->value;
        double rightData = *(double*)right->value;
        double resData = leftData * rightData;
        retval = pyobjFloat(resData);
    }
        
    else if ((left->type && right->type) == PY_BOOL){
        if (*(bool*)left->value == false){
            int leftData = 0;
        }
        else {
            int leftData = 1;
        }
        if (*(bool*)right->value == false){
            int rightData = 0;
        }
        else {
            int rightData = 1;
        }
        int resData = leftData * rightData;
        retval = pyobjInt(resData);
    }
        
    else {
        cout << "Error: pyobjMult trying to multiple incorrect types";
        return 0;
    }
    
	pyobjDecRef(left);
	pyobjDecRef(right);
	return retval;
}








