#include <stdio.h>
#include "pyobj.h"
int main()
{
struct pyobj * a_;
struct pyobj * prime_;
struct pyobj * c_;
struct pyobj * b_;
struct pyobj * False_;
struct pyobj * d_;
struct pyobj * i_;
struct pyobj * True_;

pyobjInit();
True_ = pyobjTrue();
False_ = pyobjFalse();
pyobjDecRef(pyobjAssign(&a_, pyobjInt(1)));

pyobjDecRef(pyobjAssign(&b_, pyobjFloat(1.0)));

pyobjDecRef(pyobjAssign(&c_, pyobjLoadName(True_)));

pyobjDecRef(pyobjAssign(&d_, pyobjList([pyobjLoadName(a_), pyobjLoadName(b_), pyobjLoadName(c_)], 3)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_), pyobjLoadName(b_), pyobjLoadName(c_), pyobjLoadName(d_)], true);

pyobjPrint(vector<pyobj *>([pyobjAdd(pyobjLoadName(a_), pyobjLoadName(a_))], true);

pyobjPrint(vector<pyobj *>([pyobjAdd(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjAdd(pyobjLoadName(a_), pyobjLoadName(True_))], true);

pyobjPrint(vector<pyobj *>([pyobjMult(pyobjLoadName(a_), pyobjLoadName(False_))], true);

pyobjPrint(vector<pyobj *>([pyobjMult(pyobjLoadName(a_), pyobjLoadName(a_))], true);

pyobjPrint(vector<pyobj *>([pyobjMult(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjMult(pyobjLoadName(a_), pyobjLoadName(True_))], true);

pyobjPrint(vector<pyobj *>([pyobjMult(pyobjLoadName(a_), pyobjLoadName(False_))], true);

pyobjPrint(vector<pyobj *>([pyobjMult(pyobjLoadName(a_), pyobjLoadName(d_))], true);

pyobjPrint(vector<pyobj *>([pyobjSub(pyobjLoadName(a_), pyobjLoadName(a_))], true);

pyobjPrint(vector<pyobj *>([pyobjSub(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_), pyobjSub(pyobjLoadName(a_), pyobjLoadName(c_)), pyobjLoadName(b_), pyobjLoadName(c_)], true);

pyobjPrint(vector<pyobj *>([pyobjIs(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjSubscriptLoad(pyobjLoadName(d_), pyobjInt(0))], true);

pyobjPrint(vector<pyobj *>([pyobjOr(pyobjLoadName(True_), pyobjLoadName(False_))], true);

pyobjPrint(vector<pyobj *>([pyobjAnd(pyobjLoadName(False_), pyobjLoadName(True_))], true);

pyobjPrint(vector<pyobj *>([pyobjGt(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjGt(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjGtE(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjGtE(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjLt(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjLt(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjLtE(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjLtE(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjAssign(&a_, pyobjAdd(pyobjLoadName(a_), pyobjLoadName(True_)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_)], true);

pyobjAssign(&a_, pyobjAdd(pyobjLoadName(a_), pyobjLoadName(b_)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_)], true);

pyobjAssign(&a_, pyobjSub(pyobjLoadName(a_), pyobjLoadName(c_)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_)], true);

pyobjAssign(&a_, pyobjSub(pyobjLoadName(a_), pyobjLoadName(b_)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_)], true);

pyobjPrint(vector<pyobj *>([pyobjEq(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjEq(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjNotEq(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjNotEq(pyobjLoadName(a_), pyobjLoadName(True_))], true);

pyobjPrint(vector<pyobj *>([pyobjIs(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjIs(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjIs(pyobjLoadName(a_), pyobjLoadName(d_))], true);

pyobjPrint(vector<pyobj *>([pyobjIsNot(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjIsNot(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjIsNot(pyobjLoadName(a_), pyobjLoadName(d_))], true);

pyobjPrint(vector<pyobj *>([pyobjNotIn(pyobjLoadName(a_), pyobjLoadName(d_))], true);

pyobjPrint(vector<pyobj *>([pyobjIn(pyobjLoadName(a_), pyobjLoadName(d_))], true);

pyobjPrint(vector<pyobj *>([pyobjUAdd(pyobjLoadName(a_))], true);

pyobjPrint(vector<pyobj *>([pyobjUSub(pyobjLoadName(a_))], true);

pyobjPrint(vector<pyobj *>([pyobjPow(pyobjLoadName(a_), pyobjLoadName(b_))], true);

pyobjPrint(vector<pyobj *>([pyobjPow(pyobjLoadName(a_), pyobjLoadName(c_))], true);

pyobjPrint(vector<pyobj *>([pyobjNone()], true);

pyobjDecRef(pyobjAssign(&a_, pyobjInt(0)));

while (pyobjToBool(pyobjLt(pyobjLoadName(a_), pyobjInt(3)))){
if (pyobjToBool(pyobjEq(pyobjLoadName(a_), pyobjLoadName(True_)))){
pyobjAssign(&a_, pyobjAdd(pyobjLoadName(a_), pyobjInt(1)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(d_)], true);
}
else {
if (pyobjToBool(pyobjEq(pyobjLoadName(a_), pyobjLoadName(False_)))){
pyobjAssign(&a_, pyobjAdd(pyobjLoadName(a_), pyobjInt(1)));

pyobjPrint(vector<pyobj *>([pyobjLoadName(c_)], true);
}
else {
pyobjPrint(vector<pyobj *>([pyobjLoadName(a_)], true);

break;
}
}
}

for ( int i_0 = 0; pyobjGetLen(pyobjGetItr(pyobjList([pyobjInt(1), pyobjInt(2), pyobjInt(3)], 3))) > i_0; ++i_0){
pyobjDecRef(pyobjAssign(&&i_, pyobjIndex(pyobjGetItr(pyobjList([pyobjInt(1), pyobjInt(2), pyobjInt(3)], 3)), pyobjInt(i_0))));
pyobjPrint(vector<pyobj *>([pyobjLoadName(i_)], true);
}

pyobjDecRef(pyobjAssign(&a_, pyobjIfExp(pyobjToBool(pyobjLoadName(True_)), pyobjFloat(3.0), pyobjLoadName(False_))));

pyobjDecRef(pyobjAssign(&b_, pyobjFloat(0.0)));

pyobjDecRef(pyobjAssign(&a_, pyobjFloat(1.0)));

pyobjDecRef(pyobjAssign(&c_, pyobjList([], 0)));

while (pyobjToBool(pyobjLt(pyobjLoadName(a_), pyobjInt(100)))){
pyobjAssign(&a_, pyobjAdd(pyobjLoadName(a_), pyobjInt(1)));

if (pyobjToBool(pyobjEq(pyobjFloorDiv(pyobjLoadName(a_), pyobjInt(2)), pyobjDiv(pyobjLoadName(a_), pyobjInt(2))))){
;
}

pyobjDecRef(pyobjAssign(&b_, pyobjFloat(2.0)));

pyobjDecRef(pyobjAssign(&prime_, pyobjLoadName(True_)));

while (pyobjToBool(pyobjLtE(pyobjPow(pyobjLoadName(b_), pyobjInt(2)), pyobjLoadName(a_)))){
if (pyobjToBool(pyobjEq(pyobjDiv(pyobjLoadName(a_), pyobjLoadName(b_)), pyobjFloorDiv(pyobjLoadName(a_), pyobjLoadName(b_))))){
pyobjDecRef(pyobjAssign(&prime_, pyobjLoadName(False_)));

break;
}

pyobjAssign(&b_, pyobjAdd(pyobjLoadName(b_), pyobjInt(1)));
}

if (pyobjToBool(pyobjLoadName(prime_))){
pyobjAssign(&c_, pyobjAdd(pyobjLoadName(c_), pyobjList([pyobjLoadName(a_)], 1)));
}
else {
continue;
}
}

pyobjPrint(vector<pyobj *>([pyobjLoadName(c_)], true);

pyobjDecRef(pyobjAssign(&a_, pyobjAssign(&b_, pyobjLoadName(c_))));

pyobjPrint(vector<pyobj *>([pyobjLoadName(a_), pyobjLoadName(b_), pyobjLoadName(c_)], true);

pyobjDecRef(pyobjAssign(&a_, pyobjInt(10)));

pyobjDecRef(pyobjAssign(&b_, pyobjList([pyobjInt(0), pyobjInt(1), pyobjInt(2)], 3)));

pyobjDecRef(pyobjAssign(&c_, pyobjInt(2)));

pyobjDecRef(pyobjAssign(pyobjSubscriptStore(pyobjLoadName(b_), pyobjLoadName(c_)), pyobjLoadName(a_)));

return 0;
}
