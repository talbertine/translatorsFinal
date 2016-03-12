#include "pyobj.h"
#include <cstdlib>
#include <cmath>

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
        retval = "{"
		map<struct pyobj *, struct pyobj **, cmpPyObj> data = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)obj->value;
		vector<string> entries = vector<string>();
		for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = data.start(); itr != data.end(); ++itr){
			string entry = pyobjToString(itr->first) + ": " + pyobjToString(*itr->second);
			entries.push_back(entry);
		}
        for (int i = 0; i < entries.size(); i++){
            retval += entries[i]
            if (i != entries.size() - 1){
                retval += ", "
            }
        }
        retval += "}"
	case PY_NONE:
		retval = "None"
	}
	return retval;
}

//Statements:
//Corresponds to print
void pyobjPrint(vector<struct pyobj *> values, bool newline){
	string printVal = string("");
    for (int i = 0; i < values.size(); i++){
        printVal += pyobjToString(values[i]);
        if (i != values.size() - 1){
            printVal += " ";
        }
    }
    if (newline){
        cout << printVal << endl;
    } else {
        cout << printVal;
    }
    
}

//Assignment
// Does not decrement reference count in order to facilitate chaining. Needs to be done manually afterwards.
struct pyobj *pyobjAssign(struct pyobj **var,  pyobj *val){
    pyobjIncRef(val);
    pyobjDecRef(*var);
    (*var) = val;
    return val;
}

//Increments reference count
struct pyobj *pyobjIncRef(struct pyobj *val){
    val->reference++;
    return val;
}

//Decrements reference count
//Used internally so that reference counting is neat
struct pyobj *pyobjDecRef(struct pyobj *val){
    val->reference--;
    if (val->reference < 1){
        //Nobody needs it anymore :'(
        pyobjFree(val);
        return NULL;
    }
    return val;
}

//Handy for for loops, but should not be used elsewhere.
struct pyobj *pyobjIndex(struct pyobj *itr, struct pyobj *index){
    //itr must be a list
    vector<struct pyobj **> list = *(vector<struct pyobj **> *)itr->value;
    //index must be an int
    int i = *(int *)index->value;
    struct pyobj *retval = *list[i];
    pyobjIncRef(retval);
    pyobjDecRef(itr);
    pyobjDecRef(index);
    return *list[i];
}

struct pyobj *pyobjGetItr(struct pyobj *obj){
    struct pyobj *retval;
    if (obj->type == PY_LIST){
        retval = obj;
    } else if (obj->type == PY_DICT){
        vector<struct pyobj **>newList = vector<struct pyobj **>newList;
        map<struct pyobj *, struct pyobj **, cmpPyObj> data = *(map<struct pyobj *, struct pyobj **, cmpPyObj>)obj->value;
        for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = data.begin(); itr != data.end(); itr++){
            struct pyobj **newEntry = new struct pyobj *;
            *newEntry = itr->first;
            newList.push_back(newEntry);
        }
        retval = pyobjList(newList.data(), newList.size());
        pyobjDecRef(obj);
    } else {
        throw "You can't iterate over things that aren't collections."
    }
    return retval;
}

int pyobjGetLen(struct pyobj *obj){
    //obj has to be a list
    int retval = ((vector<struct pyobj **> *)obj->value)->size();
    pyobjDecRef(obj);
}

//Literals
struct pyobj *pyobjInt(int n){
    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_INT;
    int *data = new int;
    newObj->value = data;
    *data = n;
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjBool(bool n){
    struct pyobj *newObj;
    if (n){
        newObj = pyobjTrue();
    } else {
        newObj = pyobjFalse();
    }
    return newObj;
}
struct pyobj *pyobjFloat(double n){
    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_FLOAT;
    double *newData = new double;
    newObj->value = newData
    *newData = n;
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjList(struct pyobj *vals[], int size){
    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_LIST;
    vector<struct pyobj **> *newData = new vector<struct pyobj **>;
    newObj->value = newData;
    for (int i = 0; i < size; i++){
        struct pyobj **newEntry = new struct pyobj *;
        *newEntry = vals[i];
        *newData->push_back(newEntry);
    }
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjDict(struct pyobj *keys[], struct pyobj *vals[], int size){
    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_DICT;
    map<struct pyobj *, struct pyobj **, cmpPyObj> *newData = new map<struct pyobj *, struct pyobj **, cmpPyObj>;
    newObj->value = newData;
    for (int i = 0; i < size; i++){
        struct pyobj *newKey = keys[i];
        struct pyobj **newVal = new struct pyobj *;
        *newVal = vals[i];
        (*newObj->value)[newKey] = newVal;
    }
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjTrue(){
    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_BOOL;
    bool *data = new bool;
    newObj->value = data;
    *data = true;
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjFalse(){
    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_BOOL;
    bool *data = new bool;
    newObj->value = data;
    *data = true;
    newObj->reference = 1;
    return newObj;
}

//Global Constants
//Not used by local names, but used when returning booleans from functions, for example
//That is, only used internally
struct pyobj *pyobjNone(){
    pyobjIncRef(noneConst);
    return noneConst;
}

//Loading names
struct pyobj *pyobjLoadName(struct pyobj *var){
    pyobjIncRef(var);
    return var;
}
struct pyobj *pyobjSubscriptLoad(struct pyobj *ls, struct pyobj *idx){
    struct pyobj *retval;
    if (ls->type == PY_LIST){
        if (idx->type == PY_INT){
            int idx = *(int *)idx->value;
            retval = *(*(*(vector<struct pyobj **> *)ls->value)[idx]);
        } else {
            throw "Cannot index a list with anything other than an int."
        }
    } else if(ls->type == PY_DICT){
        retval = *((*(map<struct pyobj *, struct pyobj **, cmpPyObj>)ls->value)[idx])
    } else {
        throw "Tried to index something other than a collection."
    }
    pyobjIncRef(retval);
    pyobjDecRef(ls);
    pyobjDecRef(idx);
    return retval;
}

//Storing stuff
struct pyobj **pyobjSubscriptStore(struct pyobj *ls, struct pyobj *idx){
    struct pyobj **retval;
    if (ls->type == PY_LIST){
        if (idx->type == PY_INT){
            int idx = *(int *)idx->value;
            retval = *(*(vector<struct pyobj **> *)ls->value)[idx];
        } else {
            throw "Cannot index a list with anything other than an int."
        }
    } else if(ls->type == PY_DICT){
        retval = (*(map<struct pyobj *, struct pyobj **, cmpPyObj>)ls->value)[]
    } else {
        throw "Tried to index something other than a collection."
    }
    pyobjDecRef(ls);
    pyobjDecRef(idx);
    return retval;
}

//Operations
struct pyobj *pyobjDiv(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    switch (left->type){
    case PY_INT:
        int leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjInt(leftInt / rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(leftInt / rightFloat);
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftInt);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation /"
        }
        break;
    case PY_FLOAT:
        double leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjFloat(leftFloat / rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(leftFloat / rightFloat);
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(leftFloat);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation /"
        }
        break;
    case PY_BOOL:
        bool leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(leftBool / rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(leftBool / rightFloat);
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftBool / rightBool);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation /"
        }
        break;
    default:
        throw "Invalid l-value for operation /"
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjFloorDiv(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    switch (left->type){
    case PY_INT:
        int leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjInt(leftInt / rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(floor(leftInt / rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftInt);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    case PY_FLOAT:
        double leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjFloat(floor(leftFloat / rightInt));
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(floor(leftFloat / rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(floor(leftFloat));
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    case PY_BOOL:
        bool leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(leftBool / rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(floor(leftBool / rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftBool / rightBool);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    default:
        throw "Invalid l-value for operation //"
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjMod(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    switch (left->type){
    case PY_INT:
        int leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjInt(leftInt % rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(fmod(leftInt, rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftInt % rightBool);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    case PY_FLOAT:
        double leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjFloat(fmod(leftFloat, rightInt));
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(fmod(leftFloat, rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(fmod(leftFloat, rightBool));
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    case PY_BOOL:
        bool leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(leftBool % rightInt);
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(fmod(leftBool, rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftBool % rightBool);
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    default:
        throw "Invalid l-value for operation //"
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjPow(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    switch (left->type){
    case PY_INT:
        int leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjInt(pow(leftInt, rightInt));
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(pow(leftInt, rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            retval = pyobjInt(pow(leftInt, rightBool);
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    case PY_FLOAT:
        double leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            retval = pyobjFloat(pow(leftFloat, rightInt));
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(pow(leftFloat, rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(fmod(leftFloat, rightBool));
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    case PY_BOOL:
        bool leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            int rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(pow(leftBool, rightInt));
            break;
        case PY_FLOAT:
            double rightFloat = *(double *)right->value;
            retval = pyobjFloat(pow(leftBool, rightFloat));
            break;
        case PY_BOOL:
            bool rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(pow(leftBool, rightBool));
            } else {
                throw "Division by zero"
            }
            break;
        default:
            throw "Invalid r-value for operation //"
        }
        break;
    default:
        throw "Invalid l-value for operation //"
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

//Won't behave like python's is necessarily, because python's is operator is weird.
//By that I mean, it's undefined which objects are reused.
struct pyobj *pyobjIs(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    if (left == right){
        retval = pyobjBool(true);
    } else {
        retval = pyobjBool(false);
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

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
    
    else if (val->type == PY_BOOL){
        if (*(bool*)val->value == false) {
            valData = true;
        }
        
        else {
            valData = false;
        }
        
        pyobj *retval = pyobjBool(valData);
    }
    
    else if (val->type == PY_FLOAT){
        valData = *(double*)val->value;
        retval = pyobjBool(valData);
    }
    
    else if (val->type == PY_LIST){
        vector<struct pyobj **> data = *(vector<struct pyobj **> *) val->value;
        if (data.size == 0) {
            valData = true;
        }
        else {
            valData = false;
        }
    }
    
    else if (val->type == PY_DICT){
        map<struct pyobj *, struct pyobj **> data = *(map<struct pyobj *, struct pyobj **> *) val->value;
        if (data.size == 0) {
            valData = true;
        }
        else {
            valData = false;
        }
    }
    
    else if (val->type == PY_NONE){
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

struct pyobj *pyobjAnd(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    
    if(left->type == PY_NONE){
        retval = pyobjNone();
    }
    
    else if (*(bool*)left->value == false){
        bool data = false;
        retval = pyobjBool(data);
    }
    
    else if (*(int*)left->value == 0){
        int data = 0;
        retval = pyobjInt(data);
    }
    
    else {
        if (right->type == PY_INT){
            int data = *(int*)right->value;
            retval = pyobjInt(data);
        }
        else if (right->type == PY_BOOL){
            if (*(bool*)right->value == false){
                bool data = false;
                retval = pyobjBool(data);
            }
            else {
                bool data = true;
                retval = pyobjBool(data);
            }
            
        }
        else if (right->type == PY_FLOAT){
            double data = *(double*)right->value;
            retval = pyobjFloat(data);
        }
        else if (right->type == PY_LIST){
            vector<struct pyobj **> data = *(int*)right->value;
            retval = pyobjList(data);
        }
        else if (right->type == PY_DICT){
            map<struct pyobj *, struct pyobj **> data = *(int*)right->value;
            retval = pyobjDict(data);
        }
        else if (right->type == PY_NONE){
            retval = pyobjNone();
        }
    }
    
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjOr(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    
    if((left->type == PY_NONE) || (*(bool*)left->value == false) || (*(int*)left->value == 0)){
        if (right->type == PY_INT){
            int data = *(int*)right->value;
            retval = pyobjInt(data);
        }
        else if (right->type == PY_BOOL){
            if (*(bool*)right->value == false){
                bool data = false;
                retval = pyobjBool(data);
            }
            else {
                bool data = true;
                retval = pyobjBool(data);
            }
        }
        else if (right->type == PY_FLOAT){
            double data = *(double*)right->value;
            retval = pyobjFloat(data);
        }
        else if (right->type == PY_LIST){
            vector<struct pyobj **> data = *(int*)right->value;
            retval = pyobjList(data);
        }
        else if (right->type == PY_DICT){
            map<struct pyobj *, struct pyobj **> data = *(int*)right->value;
            retval = pyobjDict(data);
        }
        else if (right->type == PY_NONE){
            retval = pyobjNone();
        }
    }

    else if (left->type == PY_INT){
            int data = *(int*)left->value;
            retval = pyobjInt(data);
        }
    else if (left->type == PY_BOOL){
        if (*(bool*)left->value == false){
            bool data = false;
            retval = pyobjBool(data);
        }
        else {
            bool data = true;
            retval = pyobjBool(data);
        }
    }
    else if (left->type == PY_FLOAT){
        double data = *(double*)left->value;
        retval = pyobjFloat(data);
    }
    else if (left->type == PY_LIST){
        vector<struct pyobj **> data = *(int*)left->value;
        retval = pyobjList(data);
    }
    else if (left->type == PY_DICT){
        map<struct pyobj *, struct pyobj **> data = *(int*)left->value;
        retval = pyobjDict(data);
    }
    
    pyobjDecRef(left);
    pyobjDecRef(right);
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
            int rightData;
            if (*(bool*)right->value == false){
                rightData = 0;
            }
            else {
                rightData = 1;
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
            vector<struct pyobj **> resData = leftData * data;
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
            int leftData;
            
            if (*(bool*)left->value == false){
                leftData = 0;
            }
            else {
                leftData = 1;
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

    else if ((left->type == PY_FLOAT) && (right->type == PY_FLOAT)){
        double leftData = *(double*)left->value;
        double rightData = *(double*)right->value;
        double resData = leftData * rightData;
        retval = pyobjFloat(resData);
    }
        
    else if ((left->type == PY_BOOL) && (right->type == PY_BOOL)){
        int leftData;
        int rightData;
        
        if (*(bool*)left->value == false){
            leftData = 0;
        }
        else {
            leftData = 1;
        }
        if (*(bool*)right->value == false){
            rightData = 0;
        }
        else {
            rightData = 1;
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

struct pyobj *pyobjAdd(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    
    if (left->type == PY_INT){
        int leftData = *(int*)left->value;
    
        if (right->type == PY_INT){
            int rightData = *(int*)right->value;
            int resData = leftData + rightData;
            retval = pyobjInt(resData);
        }
        
        else if (right->type == PY_BOOL){
            int rightData;
            if (*(bool*)right->value == false){
                rightData = 0;
            }
            else {
                rightData = 1;
            }
            int resData = leftData + rightData;
            retval = pyobjInt(resData);
        }
        
        else if (right->type == PY_FLOAT){
            double rightData = *(double*)right->value;
            double resData = leftData + rightData;
            retval = pyobjFloat(resData);
        }
        
    else if (right->type == PY_INT){
        int rightData = *(int*)right->value;
            
        if (left->type == PY_INT){
            int leftData = *(int*)left->value;
            int resData = leftData + rightData;
            retval = pyobjInt(resData);
        }
            
        else if (left->type == PY_BOOL){
            int leftData;
            
            if (*(bool*)left->value == false){
                leftData = 0;
            }
            else {
                leftData = 1;
            }
            int resData = leftData + rightData;
            retval = pyobjInt(resData);
        }
            
        else if (left->type == PY_FLOAT){
            double leftData = *(double*)left->value;
            double resData = leftData + rightData;
            retval = pyobjFloat(resData);
        }
            
    else if ((left->type == PY_FLOAT) && (right->type == PY_FLOAT)){
        double leftData = *(double*)left->value;
        double rightData = *(double*)right->value;
        double resData = leftData + rightData;
        retval = pyobjFloat(resData);
    }
            
    else if ((left->type == PY_BOOL) && (right->type == PY_BOOL)){
        int leftData;
        int rightData;
        
        if (*(bool*)left->value == false){
            leftData = 0;
        }
        else {
            leftData = 1;
        }
        if (*(bool*)right->value == false){
            rightData = 0;
        }
        else {
            rightData = 1;
        }
            
        int resData = leftData + rightData;
        retval = pyobjInt(resData);
    }
        
    else if ((left->type == PY_LIST) && (right->type == PY_LIST)){
            vector<struct pyobj **> leftData = *(vector<struct pyobj **> *) val->value;
            vector<struct pyobj **> rightData = *(vector<struct pyobj **> *) val->value;
            vector<struct pyobj **> resData = leftData + rightData;
            retval = pyobjList(resData);
    }
        
    else {
        cout << "Error: pyobjMult trying to multiple incorrect types";
        return 0;
    }
            
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
    
}

struct pyobj *pyobjSub(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    
    if (left->type == PY_INT){
        int leftData = *(int*)left->value;
        
        if (right->type == PY_INT){
            int rightData = *(int*)right->value;
            int resData = leftData - rightData;
            retval = pyobjInt(resData);
        }
        
        else if (right->type == PY_BOOL){
            int rightData;
            if (*(bool*)right->value == false){
                rightData = 0;
            }
            else {
                rightData = 1;
            }
            int resData = leftData - rightData;
            retval = pyobjInt(resData);
        }
        
        else if (right->type == PY_FLOAT){
            double rightData = *(double*)right->value;
            double resData = leftData - rightData;
            retval = pyobjFloat(resData);
        }
        
    else if (right->type == PY_INT){
        int rightData = *(int*)right->value;
        
        if (left->type == PY_INT){
            int leftData = *(int*)left->value;
            int resData = leftData - rightData;
            retval = pyobjInt(resData);
        }
            
        else if (left->type == PY_BOOL){
            int leftData;
        
            if (*(bool*)left->value == false){
                leftData = 0;
            }
            else {
                leftData = 1;
            }
            int resData = leftData - rightData;
            retval = pyobjInt(resData);
        }
            
        else if (left->type == PY_FLOAT){
            double leftData = *(double*)left->value;
            double resData = leftData - rightData;
            retval = pyobjFloat(resData);
        }
            
    else if ((left->type == PY_FLOAT) && (right->type == PY_FLOAT)){
        double leftData = *(double*)left->value;
        double rightData = *(double*)right->value;
        double resData = leftData - rightData;
        retval = pyobjFloat(resData);
    }
            
    else if ((left->type == PY_BOOL) && (right->type == PY_BOOL)){
        int leftData;
        int rightData;
                
        if (*(bool*)left->value == false){
            leftData = 0;
        }
        else {
            leftData = 1;
        }
        if (*(bool*)right->value == false){
            rightData = 0;
        }
        else {
            rightData = 1;
        }
        
        int resData = leftData - rightData;
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




