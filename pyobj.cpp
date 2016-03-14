#include "pyobj.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <set>

using std::set;
string pyobjToString(struct pyobj *value, set <struct pyobj *> seen);
string pyobjToString(struct pyobj *value);

//Data type for every data object in the python code
struct pyobj{
	char type;
	void *value;
	long reference;
};
struct pyobj *noneConst;

//Debugging functions
long pyobjGetRef(struct pyobj *var){
    return var->reference;
}

char pyobjGetType(struct pyobj *var){
    return var->type;
}

void * pyobjGetData(struct pyobj *var){
    return var->value;
}

//Allows us to keep sane maps
struct cmpPyObj : public std::binary_function<struct pyobj *, struct pyobj *, bool>{
    bool operator()(struct pyobj* const&a, struct pyobj* const&b) const{
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
        	throw "Cannot index by a list or dict. That's just dumb.";
        }
    }
};

//Call before anything happens
void pyobjInit(){
	noneConst = (struct pyobj *)malloc(sizeof(struct pyobj));
	noneConst->type = PY_NONE;
	noneConst->reference = 2;
}

void pyobjFree(struct pyobj *obj){
    vector<struct pyobj **> *vecData;
    map<struct pyobj *, struct pyobj **, cmpPyObj> *mapData;
	switch (obj->type){
	case PY_INT:
        free((int *)obj->value);
        free(obj);
        break;
	case PY_FLOAT:
        free((double *)obj->value);
        free(obj);
        break;
	case PY_BOOL:
		free((bool *)obj->value);
		free(obj);
		break;
	case PY_LIST:
		vecData = (vector<struct pyobj**> *)obj->value;
		for (int i = 0; i < vecData->size(); i++){
			pyobjDecRef(*(*vecData)[i]);
			free((*vecData)[i]);
		}
		delete vecData;
		free(obj);
		break;
	case PY_DICT:
		mapData = (map<struct pyobj *, struct pyobj **, cmpPyObj> *)obj->value;
		for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = mapData->begin(); itr != mapData->end(); ++itr){
			pyobjDecRef(itr->first);
			pyobjDecRef(*itr->second);
			free(itr->second);
		}
		delete mapData;
		free(obj);
		break;
	case PY_NONE:
		free(obj);
		break;
	default:
		throw "Unexpected type in pyobjFree";
	}
}

bool pyobjToBool(struct pyobj *val){
    bool retval;
    switch (val->type){
        case PY_INT:
        retval = *(int *)val->value != 0;
        break;
        case PY_FLOAT:
        retval = *(double *)val->value != 0;
        break;
        case PY_BOOL:
        retval = *(bool *)val->value;
        break;
        case PY_LIST:
        retval = ((vector<struct pyobj **> *)val->value)->size();
        break;
        case PY_DICT:
        retval = ((map<struct pyobj *, struct pyobj **, cmpPyObj> *)val->value)->size();
        break;
        case PY_NONE:
        retval = false;
        break;
    }
    pyobjDecRef(val);
    return retval;
}

string pyobjToString(struct pyobj *value){
    set<struct pyobj *> seen;
    return pyobjToString(value, seen);
}

string pyobjToString(struct pyobj *value, set <struct pyobj *> seen){
	string retval;
	stringstream ss;
    vector<struct pyobj **> vecData;
    map<struct pyobj *, struct pyobj **, cmpPyObj> mapData;
    vector<string> entries;
    double temp;
    char tempStr[100];

    //From Dr. Huang's code
    char outstr[128];
    char *p;
    char printed_0;
    char printed_0_neg;

	switch (value->type){
	case PY_INT:
		ss << (*(int *) value->value);
		ss >> retval;
        break;
	case PY_FLOAT:
		temp = *(double *)value->value;

        snprintf(outstr, 128, "%.12g", temp);
        p = outstr;
        if(temp == 0.0)
        {
            if(printed_0 == 0)
            {
                printed_0 = 1;
                printed_0_neg = *p == '-'; //see if we incremented for negative
            }
            else
            {
                if (printed_0_neg) 
                    retval = "-0.0";
                else
                    retval = "0.0";
                break;
            }
        }
        if(*p == '-')
            p++;
        while(*p && isdigit(*p))
            p++;
        if (*p){
            retval = outstr;
        } else {
            retval = outstr;
            retval += ".0";
        }
        break;
	case PY_BOOL:
		if (*(bool *) value->value){
			retval = "True";
		} else {
			retval = "False";
		}
        break;
	case PY_LIST:
		retval = "[";
		vecData = *(vector<struct pyobj **> *) value->value;
		for (int i = 0; i < vecData.size(); ++i){
            if (seen.count(*vecData[i]) > 0){
                retval += "[...]";
            } else {
                set<struct pyobj *> derp = set<struct pyobj *>(seen);
                derp.insert(value);
                retval += pyobjToString(*vecData[i], derp);
            }
			if (vecData.size() - 1 != i){
				retval += ", ";
			}
		}
		retval += "]";
        break;
	case PY_DICT:
        retval = "{";
		mapData = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)value->value;
		entries = vector<string>();
		for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = mapData.begin(); itr != mapData.end(); ++itr){
            string entry;
            if (seen.count(*itr->second) > 0){
                entry = pyobjToString(itr->first) + ": {...}";
            } else {
                set<struct pyobj *> derp = set<struct pyobj *>(seen);
                derp.insert(value);
                entry = pyobjToString(itr->first) + ": " + pyobjToString(*itr->second, derp);
            }
			entries.push_back(entry);
		}
        for (int i = 0; i < entries.size(); i++){
            retval += entries[i];
            if (i != entries.size() - 1){
                retval += ", ";
            }
        }
        retval += "}";
        break;
	case PY_NONE:
		retval = "None";
	}
	return retval;
}

//Statements:
//Corresponds to print
void pyobjPrint(bool newline, int size, ...){
    vector<struct pyobj *> values;
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size; i++){
        values.push_back(va_arg(args, struct pyobj *));
    }
    va_end(args);

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
        vector<struct pyobj *>newList = vector<struct pyobj *>();
        map<struct pyobj *, struct pyobj **, cmpPyObj> data = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)obj->value;
        for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = data.begin(); itr != data.end(); itr++){
            struct pyobj *newEntry = itr->first;
            newList.push_back(newEntry);
        }
        retval = pyobjList(newList.data(), newList.size());
        pyobjDecRef(obj);
    } else {
        throw "You can't iterate over things that aren't collections.";
    }
    return retval;
}

int pyobjGetLen(struct pyobj *obj){
    //obj has to be a list
    int retval = ((vector<struct pyobj **> *)obj->value)->size();
    pyobjDecRef(obj);
    return retval;
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
    newObj->value = newData;
    *newData = n;
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjList(int size, ...){
    struct pyobj **vals = (struct pyobj **) malloc(sizeof(struct pyobj *) * size);
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size; i++){
        vals[i] = va_arg(args, struct pyobj *);
    }
    va_end(args);

    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_LIST;
    vector<struct pyobj **> *newData = new vector<struct pyobj **>;
    newObj->value = newData;
    for (int i = 0; i < size; i++){
        struct pyobj **newEntry = new struct pyobj *;
        *newEntry = vals[i];
        newData->push_back(newEntry);
    }
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjList(struct pyobj **vals, int size){

    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_LIST;
    vector<struct pyobj **> *newData = new vector<struct pyobj **>;
    newObj->value = newData;
    for (int i = 0; i < size; i++){
        struct pyobj **newEntry = new struct pyobj *;
        *newEntry = vals[i];
        newData->push_back(newEntry);
    }
    newObj->reference = 1;
    return newObj;
}
struct pyobj *pyobjDict(int size, ...){
    struct pyobj **keys = (struct pyobj **)malloc(sizeof(struct pyobj *) * size);
    struct pyobj **vals = (struct pyobj **)malloc(sizeof(struct pyobj *) * size);
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size * 2; i++){
        if (i % 2 == 0){
            keys[i / 2] = va_arg(args, struct pyobj *);
        } else {
            vals[i / 2] = va_arg(args, struct pyobj *);
        }
    }
    va_end(args);

    struct pyobj *newObj = new struct pyobj;
    newObj->type = PY_DICT;
    map<struct pyobj *, struct pyobj **, cmpPyObj> *newData = new map<struct pyobj *, struct pyobj **, cmpPyObj>;
    newObj->value = newData;
    for (int i = 0; i < size; i++){
        struct pyobj *newKey = keys[i];
        struct pyobj **newVal = new struct pyobj *;
        *newVal = vals[i];
        (*newData)[newKey] = newVal;
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
        (*newData)[newKey] = newVal;
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
    *data = false;
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
            int i = *(int *)(idx->value);
            retval = *(*(vector<struct pyobj **> *)ls->value)[i];
        } else {
            throw "Cannot index a list with anything other than an int.";
        }
    } else if(ls->type == PY_DICT){

        retval = *(*(map<struct pyobj *, struct pyobj **, cmpPyObj> *)ls->value)[idx];
    } else {
        throw "Tried to index something other than a collection.";
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
            int i = *(int *)idx->value;
            retval = (*(vector<struct pyobj **> *)ls->value)[i];
        } else {
            throw "Cannot index a list with anything other than an int.";
        }
    } else if(ls->type == PY_DICT){
        retval = (*(map<struct pyobj *, struct pyobj **, cmpPyObj> *)ls->value)[idx];
    } else {
        throw "Tried to index something other than a collection.";
    }
    pyobjDecRef(ls);
    pyobjDecRef(idx);
    return retval;
}

//Operations

struct pyobj *pyobjIfExp(struct pyobj *cond, struct pyobj *body, struct pyobj *orelse){
    struct pyobj *retval;
    if(pyobjToBool(cond)){
        retval = body;
        pyobjDecRef(orelse);
    } else {
        retval = orelse;
        pyobjDecRef(body);
    }
    return retval;
}

struct pyobj *pyobjDiv(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    int rightInt;
    double rightFloat;
    bool rightBool;
    switch (left->type){
    case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjInt(leftInt / rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(leftInt / rightFloat);
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftInt);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation /";
        }
        break;
    case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjFloat(leftFloat / rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(leftFloat / rightFloat);
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(leftFloat);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation /";
        }
        break;
    case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(leftBool / rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(leftBool / rightFloat);
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftBool / rightBool);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation /";
        }
        break;
    default:
        throw "Invalid l-value for operation /";
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjFloorDiv(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    int rightInt;
    double rightFloat;
    bool rightBool;
    switch (left->type){
    case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjInt(leftInt / rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(floor(leftInt / rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftInt);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjFloat(floor(leftFloat / rightInt));
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(floor(leftFloat / rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(floor(leftFloat));
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(leftBool / rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(floor(leftBool / rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftBool / rightBool);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    default:
        throw "Invalid l-value for operation //";
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjMod(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    int rightInt;
    double rightFloat;
    bool rightBool;
    switch (left->type){
    case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjInt(leftInt % rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(fmod(leftInt, rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftInt % rightBool);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjFloat(fmod(leftFloat, rightInt));
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(fmod(leftFloat, rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(fmod(leftFloat, rightBool));
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(leftBool % rightInt);
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(fmod(leftBool, rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(leftBool % rightBool);
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    default:
        throw "Invalid l-value for operation //";
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjPow(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    int rightInt;
    double rightFloat;
    bool rightBool;
    switch (left->type){
    case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjInt(pow(leftInt, rightInt));
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(pow(leftInt, rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjInt(pow(leftInt, rightBool));
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjFloat(pow(leftFloat, rightInt));
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(pow(leftFloat, rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjFloat(fmod(leftFloat, rightBool));
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
        case PY_INT:
            rightInt = *(int *)right->value;
            // booleans are promoted to ints when divided like this
            retval = pyobjInt(pow(leftBool, rightInt));
            break;
        case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjFloat(pow(leftBool, rightFloat));
            break;
        case PY_BOOL:
            rightBool = *(bool *)right->value;
            if (rightBool){
                retval = pyobjInt(pow(leftBool, rightBool));
            } else {
                throw "Division by zero";
            }
            break;
        default:
            throw "Invalid r-value for operation //";
        }
        break;
    default:
        throw "Invalid l-value for operation //";
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

struct pyobj *pyobjIsNot(struct pyobj *left, struct pyobj *right){
    return pyobjNot(pyobjIs(left, right));
}

struct pyobj *pyobjIn(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval = pyobjFalse();
    vector<struct pyobj **> dataVec;
    map<struct pyobj *, struct pyobj **, cmpPyObj>dataMap;
    switch (right->type){
    case PY_LIST:
        dataVec = *(vector<struct pyobj **> *)right->value;
        for (vector<struct pyobj **>::iterator itr = dataVec.begin(); itr != dataVec.end(); itr++){
            pyobjIncRef(left);
            pyobjIncRef(**itr);
            retval = pyobjOr(pyobjEq(left, **itr), retval);
            if (*(bool *)retval->value){
                break;
            }
        }
        break;
    case PY_DICT:
        dataMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)right->value;
        for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = dataMap.begin(); itr != dataMap.end(); itr++){
            pyobjIncRef(left);
            pyobjIncRef(itr->first);
            retval = pyobjOr(pyobjEq(left, itr->first), retval);
            if (*(bool *)retval->value){
                break;
            }
        }
        break;
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjNotIn(struct pyobj *left, struct pyobj *right){
    return pyobjNot(pyobjIn(left, right));
}

struct pyobj *pyobjEq(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    vector<struct pyobj **>leftList;
    map<struct pyobj *, struct pyobj **, cmpPyObj>leftMap;
    int rightInt;
    double rightFloat;
    bool rightBool;
    vector<struct pyobj **>rightList;
    map<struct pyobj *, struct pyobj **, cmpPyObj>rightMap;
    switch (left->type){
        case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftInt == rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftInt == rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftInt == rightBool);
            break;
            case PY_LIST: //An int can never be equal to any of these
            case PY_DICT:
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftFloat == rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftFloat == rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftFloat == rightBool);
            break;
            case PY_LIST: //A float can never be equal to any of these
            case PY_DICT:
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftBool == rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftBool == rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftBool == rightBool);
            break;
            case PY_LIST: //A bool can never be equal to any of these
            case PY_DICT:
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_LIST:
        leftList = *(vector<struct pyobj **> *)left->value;
        switch (right->type){
            case PY_LIST: 
            rightList = *(vector<struct pyobj **> *)right->value;
            if (leftList.size() != rightList.size()){
                retval = pyobjBool(false);
                break;
            }
            retval = pyobjBool(true);
            for (int i = 0; i < leftList.size(); i++){
                struct pyobj *leftDatum = *leftList[i];
                struct pyobj *rightDatum = *rightList[i];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                retval = pyobjAnd(pyobjEq(leftDatum, rightDatum), retval);
                if (! *(bool *)retval->value){
                    break;
                }
            }
            break;
            case PY_INT: //A list can never be equal to anything but a list
            case PY_FLOAT:
            case PY_BOOL:
            case PY_DICT:
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_DICT:
        leftMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)left->value;
        switch (right->type){
            case PY_DICT: 
            rightMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)right->value;
            if (leftMap.size() != rightMap.size()){
                retval = pyobjBool(false);
                break;
            }
            retval = pyobjBool(true);
            for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = leftMap.begin(); itr != leftMap.end(); itr++){
                struct pyobj *key = itr->first;
                if (rightMap.count(key) < 1 || leftMap.count(key) < 1){
                    //one map is missing at least one key
                    retval = pyobjBool(false);
                    break;
                }
                struct pyobj **leftVal = leftMap[key];
                struct pyobj **rightVal = rightMap[key];
                pyobjIncRef(*leftVal);
                pyobjIncRef(*rightVal);
                retval = pyobjAnd(pyobjEq(*leftVal, *rightVal), retval);
                if (! *(bool *)retval->value){
                    break;
                }
            }
            break;
            case PY_INT: //A dict can never be equal to anything but a dict
            case PY_FLOAT:
            case PY_BOOL:
            case PY_LIST:
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_NONE:
        if (left->type == right->type){
            retval = pyobjBool(true);
        } else {
            retval = pyobjBool(false);
        }
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjNotEq(struct pyobj *left, struct pyobj *right){
    return pyobjNot(pyobjEq(left, right));
}

//A helper function that gets the list of keys
vector<struct pyobj *> getKeys(map<struct pyobj *, struct pyobj **, cmpPyObj> src){
    vector<struct pyobj *> keys = vector<struct pyobj *>();
    for (map<struct pyobj *, struct pyobj **, cmpPyObj>::iterator itr = src.begin(); itr != src.end(); itr++){
        keys.push_back(itr->first);
    }
    return keys;
}

struct pyobj *pyobjLt(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    vector<struct pyobj **>leftList;
    map<struct pyobj *, struct pyobj **, cmpPyObj>leftMap;
    int rightInt;
    double rightFloat;
    bool rightBool;
    vector<struct pyobj **>rightList;
    map<struct pyobj *, struct pyobj **, cmpPyObj>rightMap;
    int size;
    struct pyobj *temp1;
    struct pyobj *temp2;
    vector<struct pyobj *> leftKeys;
    vector<struct pyobj *> rightKeys;
    switch (left->type){
        case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftInt < rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftInt < rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftInt < rightBool);
            break;
            case PY_LIST: //An int can never be equal to any of these
            retval = pyobjBool(true);
            break;
            case PY_DICT:
            retval = pyobjBool(false);
            break;
            case PY_NONE:
            retval = pyobjBool(true);
        }
        break;
        case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftFloat < rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftFloat < rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftFloat < rightBool);
            break;
            case PY_LIST: //A float can never be equal to any of these
            retval = pyobjBool(true);
            break;
            case PY_DICT:
            retval = pyobjBool(false);
            break;
            case PY_NONE:
            retval = pyobjBool(true);
        }
        break;
        case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftBool < rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftBool < rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftBool < rightBool);
            break;
            case PY_LIST: //A bool can never be equal to any of these
            retval = pyobjBool(true);
            break;
            case PY_DICT:
            retval = pyobjBool(true);
            break;
            case PY_NONE:
            retval = pyobjBool(true);
        }
        break;
        case PY_LIST:
        leftList = *(vector<struct pyobj **> *)left->value;
        switch (right->type){
            case PY_LIST: 
            rightList = *(vector<struct pyobj **> *)right->value;
            retval = NULL;
            size = leftList.size();
            pyobj *temp1;
            pyobj *temp2;
            if (rightList.size() < size){
                size = rightList.size();
            }
            for (int i = 0; i < size; i++){
                struct pyobj *leftDatum = *leftList[i];
                struct pyobj *rightDatum = *rightList[i];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                temp1 = pyobjLt(leftDatum, rightDatum);
                temp2 = pyobjGt(leftDatum, rightDatum);
                bool lt = *(bool *)temp1->value;
                bool gt = *(bool *)temp2->value;
                pyobjDecRef(temp1);
                pyobjDecRef(temp2);
                if (lt){
                    retval = pyobjBool(true);
                    break;
                } else if (gt){
                    retval = pyobjBool(false);
                    break;
                }
            }
            if (retval == NULL){
                //We haven't found anything yet.
                //They're equal in terms of content so far, but not necessarily length
                if (leftList.size() < rightList.size()){
                    retval = pyobjBool(true);
                } else {
                    retval = pyobjBool(false);
                }
            }
            break;
            case PY_INT: //A list can never be equal to anything but a list
            retval = pyobjBool(false);
            break;
            case PY_FLOAT:
            retval = pyobjBool(false);
            break;
            case PY_BOOL:
            retval = pyobjBool(false);
            break;
            case PY_DICT:
            retval = pyobjBool(false);
            break;
            case PY_NONE:
            retval = pyobjBool(true);
        }
        break;
        case PY_DICT:
        leftMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)left->value;
        switch (right->type){
            case PY_DICT: 
            rightMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)right->value;
            retval = NULL;
            //First compare length
            if (leftMap.size() < rightMap.size()){
                retval = pyobjBool(true);
                break;
            } else if (leftMap.size() > rightMap.size()){
                retval = pyobjBool(false);
                break;
            }
            leftKeys = getKeys(leftMap);
            rightKeys = getKeys(rightMap);
            std::sort(leftKeys.begin(), leftKeys.end(), cmpPyObj());
            std::sort(rightKeys.begin(), rightKeys.end(), cmpPyObj());
            for (int i = 0; i < leftKeys.size(); i++){ //Now compare keys
                struct pyobj *leftDatum = leftKeys[i];
                struct pyobj *rightDatum = rightKeys[i];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                temp1 = pyobjLt(leftDatum, rightDatum);
                temp2 = pyobjGt(leftDatum, rightDatum);
                bool lt = *(bool *)temp1->value;
                bool gt = *(bool *)temp2->value;
                pyobjDecRef(temp1);
                pyobjDecRef(temp2);
                if (lt){
                    retval = pyobjBool(true);
                    break;
                } else if (gt){
                    retval = pyobjBool(false);
                    break;
                }
            }
            if (retval != NULL){
                break;
            }
            for (int i = 0; i < leftKeys.size(); i++){ //Now compare values
                struct pyobj *leftDatum = *leftMap[leftKeys[i]];
                struct pyobj *rightDatum = *rightMap[rightKeys[i]];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                temp1 = pyobjLt(leftDatum, rightDatum);
                temp2 = pyobjGt(leftDatum, rightDatum);
                bool lt = *(bool *)temp1->value;
                bool gt = *(bool *)temp2->value;
                pyobjDecRef(temp1);
                pyobjDecRef(temp2);
                if (lt){
                    retval = pyobjBool(true);
                    break;
                } else if (gt){
                    retval = pyobjBool(false);
                    break;
                }
            }
            if (retval == NULL){
                retval = pyobjBool(false);
            }
            break;
            case PY_INT: //A dict can never be equal to anything but a dict
            retval = pyobjBool(true);
            break;
            case PY_FLOAT:
            retval = pyobjBool(true);
            break;
            case PY_BOOL:
            retval = pyobjBool(false);
            break;
            case PY_LIST:
            retval = pyobjBool(true);
            break;
            case PY_NONE:
            retval = pyobjBool(true);
        }
        break;
        case PY_NONE:
        retval = pyobjBool(false);
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjLtE(struct pyobj *left, struct pyobj *right){
    return pyobjNot(pyobjGt(left, right));
}

struct pyobj *pyobjGt(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    vector<struct pyobj **>leftList;
    map<struct pyobj *, struct pyobj **, cmpPyObj>leftMap;
    vector<struct pyobj *> leftKeys;
    int rightInt;
    double rightFloat;
    bool rightBool;
    vector<struct pyobj **>rightList;
    map<struct pyobj *, struct pyobj **, cmpPyObj>rightMap;
    vector<struct pyobj *> rightKeys;
    int size;
    struct pyobj *temp1;
    struct pyobj *temp2;
    switch (left->type){
        case PY_INT:
        leftInt = *(int *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftInt > rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftInt > rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftInt > rightBool);
            break;
            case PY_LIST: //An int can never be equal to any of these
            retval = pyobjBool(false);
            break;
            case PY_DICT:
            retval = pyobjBool(true);
            break;
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_FLOAT:
        leftFloat = *(double *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftFloat > rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftFloat > rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftFloat > rightBool);
            break;
            case PY_LIST: //A float can never be equal to any of these
            retval = pyobjBool(false);
            break;
            case PY_DICT:
            retval = pyobjBool(true);
            break;
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_BOOL:
        leftBool = *(bool *)left->value;
        switch (right->type){
            case PY_INT:
            rightInt = *(int *)right->value;
            retval = pyobjBool(leftBool > rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *)right->value;
            retval = pyobjBool(leftBool > rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjBool(leftBool > rightBool);
            break;
            case PY_LIST: //A bool can never be equal to any of these
            retval = pyobjBool(false);
            break;
            case PY_DICT:
            retval = pyobjBool(false);
            break;
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_LIST:
        leftList = *(vector<struct pyobj **> *)left->value;
        switch (right->type){
            case PY_LIST: 
            rightList = *(vector<struct pyobj **> *)right->value;
            retval = NULL;
            size = leftList.size();
            if (rightList.size() < size){
                size = rightList.size();
            }
            for (int i = 0; i < size; i++){
                struct pyobj *leftDatum = *leftList[i];
                struct pyobj *rightDatum = *rightList[i];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                temp1 = pyobjLt(leftDatum, rightDatum);
                temp2 = pyobjGt(leftDatum, rightDatum);
                bool lt = *(bool *)temp1->value;
                bool gt = *(bool *)temp2->value;
                pyobjDecRef(temp1);
                pyobjDecRef(temp2);
                if (lt){
                    retval = pyobjBool(false);
                    break;
                } else if (gt){
                    retval = pyobjBool(true);
                    break;
                }
            }
            if (retval == NULL){
                //We haven't found anything yet.
                //They're equal in terms of content so far, but not necessarily length
                if (leftList.size() > rightList.size()){
                    retval = pyobjBool(true);
                } else {
                    retval = pyobjBool(false);
                }
            }
            break;
            case PY_INT: //A list can never be equal to anything but a list
            retval = pyobjBool(true);
            break;
            case PY_FLOAT:
            retval = pyobjBool(true);
            break;
            case PY_BOOL:
            retval = pyobjBool(true);
            break;
            case PY_DICT:
            retval = pyobjBool(true);
            break;
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_DICT:
        leftMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)left->value;
        switch (right->type){
            case PY_DICT: 
            rightMap = *(map<struct pyobj *, struct pyobj **, cmpPyObj> *)right->value;
            retval = NULL;
            //First compare length
            if (leftMap.size() < rightMap.size()){
                retval = pyobjBool(false);
                break;
            } else if (leftMap.size() > rightMap.size()){
                retval = pyobjBool(true);
                break;
            }
            leftKeys = getKeys(leftMap);
            rightKeys = getKeys(rightMap);
            std::sort(leftKeys.begin(), leftKeys.end(), cmpPyObj());
            std::sort(rightKeys.begin(), rightKeys.end(), cmpPyObj());
            for (int i = 0; i < leftKeys.size(); i++){ //Now compare keys
                struct pyobj *leftDatum = leftKeys[i];
                struct pyobj *rightDatum = rightKeys[i];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                temp1 = pyobjLt(leftDatum, rightDatum);
                temp2 = pyobjGt(leftDatum, rightDatum);
                bool lt = *(bool *)temp1->value;
                bool gt = *(bool *)temp2->value;
                pyobjDecRef(temp1);
                pyobjDecRef(temp2);
                if (lt){
                    retval = pyobjBool(false);
                    break;
                } else if (gt){
                    retval = pyobjBool(true);
                    break;
                }
            }
            if (retval != NULL){
                break;
            }
            for (int i = 0; i < leftKeys.size(); i++){ //Now compare values
                struct pyobj *leftDatum = *leftMap[leftKeys[i]];
                struct pyobj *rightDatum = *rightMap[rightKeys[i]];
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                pyobjIncRef(leftDatum);
                pyobjIncRef(rightDatum);
                temp1 = pyobjLt(leftDatum, rightDatum);
                temp2 = pyobjGt(leftDatum, rightDatum);
                bool lt = *(bool *)temp1->value;
                bool gt = *(bool *)temp2->value;
                pyobjDecRef(temp1);
                pyobjDecRef(temp2);
                if (lt){
                    retval = pyobjBool(false);
                    break;
                } else if (gt){
                    retval = pyobjBool(true);
                    break;
                }
            }
            if (retval == NULL){
                retval = pyobjBool(false);
            }
            break;
            case PY_INT: //A dict can never be equal to anything but a dict
            retval = pyobjBool(false);
            break;
            case PY_FLOAT:
            retval = pyobjBool(false);
            break;
            case PY_BOOL:
            retval = pyobjBool(true);
            break;
            case PY_LIST:
            retval = pyobjBool(false);
            break;
            case PY_NONE:
            retval = pyobjBool(false);
        }
        break;
        case PY_NONE:
        retval = pyobjBool(true);
    }
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}

struct pyobj *pyobjGtE(struct pyobj *left, struct pyobj *right){
    return pyobjNot(pyobjLt(left, right));
}


//Hailey's Wonderful Contribution
struct pyobj *pyobjUAdd(struct pyobj *val){
    struct pyobj *retval;
    double valData;
    
    if (val->type == PY_INT){
        int valData = *(int*)val->value;
        pyobj *retval = pyobjInt(valData);
    }
    
    else if (val->type == PY_BOOL){
        retval = pyobjInt(*(bool*)val->value);
    }
    
    else if (val->type == PY_FLOAT){
        valData = *(double*)val->value;
        pyobj *retval = pyobjFloat(valData);
    }
    
    else {
        throw "Error: UAdd on Dict, List, or None not possible";
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
        retval = pyobjInt(-(*(bool*)val->value));
    }
    
    else if (val->type == PY_FLOAT){
        valData = *(double*)val->value;
        retval = pyobjFloat(-valData);
    }
    
    else {
        throw "Error: USub on Dict, List, or None not possible";
    }
    
    pyobjDecRef(val);
    return retval;
}


struct pyobj *pyobjNot(struct pyobj *val){
    struct pyobj *retval;
    bool valData;
    
    if (val->type == PY_INT){
        valData = !(*(int*)val->value);
    }
    
    else if (val->type == PY_BOOL){
        valData = !(*(bool*)val->value);
    }
    
    else if (val->type == PY_FLOAT){
        valData = !(*(double*)val->value);
    }
    
    else if (val->type == PY_LIST){
        vector<struct pyobj **> data = *(vector<struct pyobj **> *) val->value;
        valData = data.size() == 0;
    }
    
    else if (val->type == PY_DICT){
        map<struct pyobj *, struct pyobj **> data = *(map<struct pyobj *, struct pyobj **> *) val->value;
        valData = data.size() == 0;
    }
    
    else if (val->type == PY_NONE){
        valData = true;
    }
    
    else {
        throw "Error: Type is not supported";
    }
    retval = pyobjBool(valData);
    pyobjDecRef(val);
    return retval;
}

struct pyobj *pyobjAnd(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    struct pyobj *copy;

    struct pyobj **keys;
    struct pyobj **values;
    map<struct pyobj *, struct pyobj **, cmpPyObj> valMap;
    int i;
    vector<struct pyobj **>valList;
    struct pyobj **vals;
    
    pyobjIncRef(left);
    bool leftVal = pyobjToBool(left);
    if (pyobjToBool(left)){
        pyobjDecRef(left);
        return right;
    } else {
        pyobjDecRef(right);
        return left;
    }
}

//Returns the reference 
struct pyobj *pyobjOr(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    struct pyobj *copy;

    map<struct pyobj *, struct pyobj **, cmpPyObj> valMap;
    struct pyobj **keys;
    struct pyobj **values;
    int i;
    vector<struct pyobj **>valList;
    struct pyobj **vals;

    pyobjIncRef(left);
    if (pyobjToBool(left)){
        pyobjDecRef(right);
        return left;
    } else {
        pyobjDecRef(left);
        return right;
    }
}

struct pyobj *pyobjMult(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    vector<struct pyobj **> data;
    vector<struct pyobj **> leftList;
    int rightInt;
    double rightFloat;
    bool rightBool;
    struct pyobj **resData;
    vector<struct pyobj **> rightList;

    switch(left->type){
        case PY_INT:
        leftInt = *(int*)left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int*)right->value;
            retval = pyobjInt(leftInt * rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double*)right->value;
            retval = pyobjFloat(leftInt * rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool*)right->value;
            retval = pyobjInt(leftInt * rightBool);
            break;
            case PY_LIST:
            data = *(vector<struct pyobj **> *) right->value;
            resData = (struct pyobj **) malloc(sizeof(struct pyobj *) * leftInt * data.size());
            for (int i = 0; i < leftInt * data.size(); i++){
                resData[i] = *data[i % data.size()];
            }
            retval = pyobjList(resData, leftInt * data.size());
            free(resData);
            break;
            default:
            throw "Invalid type in probMult";
        }
        break;
        case PY_FLOAT:
        leftFloat = *(double*)left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int*)right->value;
            retval = pyobjFloat(leftFloat * rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double*)right->value;
            retval = pyobjFloat(leftFloat * rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool*)right->value;
            retval = pyobjFloat(leftFloat * rightBool);
            break;
            default:
            throw "Invalid type in probMult";
        }
        break;
        case PY_BOOL:
        leftBool = *(bool*)left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int*)right->value;
            retval = pyobjInt(leftBool * rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double*)right->value;
            retval = pyobjFloat(leftBool * rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool*)right->value;
            retval = pyobjInt(leftBool * rightBool);
            break;
            case PY_LIST:
            data = *(vector<struct pyobj **> *) right->value;
            resData = (struct pyobj **) malloc(sizeof(struct pyobj *) * leftBool * data.size());
            for (int i = 0; i < leftBool * data.size(); i++){
                resData[i] = *data[i % data.size()];
            }
            retval = pyobjList(resData, leftBool * data.size());
            free(resData);
            break;
            default:
            throw "Invalid type in probMult";
        }
        break;
        case PY_LIST:
        leftList = *(vector<struct pyobj **>*)left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int*)right->value;
            resData = (struct pyobj **)malloc(sizeof(struct pyobj *) * rightInt * data.size());
            for (int i = 0; i < rightInt * data.size(); i++){
                resData[i] = *data[i % data.size()];
            }
            retval = pyobjList(resData, rightInt * data.size());
            free(resData);
            break;
            case PY_BOOL:
            rightBool = *(bool*)right->value;
            resData = (struct pyobj **)malloc(sizeof(struct pyobj *) * rightBool * data.size());
            for (int i = 0; i < rightBool * data.size(); i++){
                resData[i] = *data[i % data.size()];
            }
            retval = pyobjList(resData, rightBool * data.size());
            free(resData);
            break;
            default:
            throw "Invalid type in probMult";
        }
        default:
        throw "Invalid type in probMult";
    }
    
	pyobjDecRef(left);
	pyobjDecRef(right);
	return retval;
}

struct pyobj *pyobjAdd(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    vector<struct pyobj **>leftList;
    int rightInt;
    double rightFloat;
    bool rightBool;
    vector<struct pyobj **>rightList;
    struct pyobj **temp;

    switch(left->type){
        case PY_INT:
        leftInt = *(int *) left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int *) right->value;
            retval = pyobjInt(leftInt + rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *) right->value;
            retval = pyobjFloat(leftInt + rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjInt(leftInt + rightBool);
            break;
        }
        break;
        case PY_FLOAT:
        leftFloat = *(double *) left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int *) right->value;
            retval = pyobjFloat(leftFloat + rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *) right->value;
            retval = pyobjFloat(leftFloat + rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjFloat(leftFloat + rightBool);
            break;
        }
        break;
        case PY_BOOL:
        leftBool = *(bool *) left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int *) right->value;
            retval = pyobjInt(leftBool + rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *) right->value;
            retval = pyobjFloat(leftBool + rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjInt(leftBool + rightBool);
            break;
        }
        break;
        case PY_LIST:
        leftList = *(vector<struct pyobj **> *)left->value;
        if (right->type != PY_LIST){
            throw "list can only add to list";
        }
        rightList = *(vector<struct pyobj **> *)right->value;
        temp = (struct pyobj **)malloc(sizeof(struct pyobj *) * (rightList.size() + leftList.size()));
        for (int i = 0; i < leftList.size(); i++){
            temp[i] = *leftList[i];
        }
        for (int i = 0; i < rightList.size(); i++){
            temp[i + leftList.size()] = *rightList[i];
        }
        retval = pyobjList(temp, leftList.size() + rightList.size());
        free(temp);
        break;
        default:
        throw "Can't add specified type";
    }
            
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
    
}

struct pyobj *pyobjSub(struct pyobj *left, struct pyobj *right){
    struct pyobj *retval;
    int leftInt;
    double leftFloat;
    bool leftBool;
    int rightInt;
    double rightFloat;
    bool rightBool;
    
    switch(left->type){
        case PY_INT:
        leftInt = *(int *) left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int *) right->value;
            retval = pyobjInt(leftInt - rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *) right->value;
            retval = pyobjFloat(leftInt - rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjInt(leftInt - rightBool);
            break;
            default:
            throw "Can't subtract specified type";
        }
        break;
        case PY_FLOAT:
        leftFloat = *(double *) left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int *) right->value;
            retval = pyobjFloat(leftFloat - rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *) right->value;
            retval = pyobjFloat(leftFloat - rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjFloat(leftFloat - rightBool);
            break;
            default:
            throw "Can't subtract specified type";
        }
        break;
        case PY_BOOL:
        leftBool = *(bool *) left->value;
        switch(right->type){
            case PY_INT:
            rightInt = *(int *) right->value;
            retval = pyobjInt(leftBool - rightInt);
            break;
            case PY_FLOAT:
            rightFloat = *(double *) left->value;
            retval = pyobjFloat(leftBool - rightFloat);
            break;
            case PY_BOOL:
            rightBool = *(bool *)right->value;
            retval = pyobjInt(leftBool - rightBool);
            break;
            default:
            throw "Can't subtract specified type";
        }
        break;
        default:
        throw "Can't subtract specified type";
    }
            
    pyobjDecRef(left);
    pyobjDecRef(right);
    return retval;
}




