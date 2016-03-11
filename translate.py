from ast import *
import sys
logs = sys.stderr
import traceback

def getNewIdNum():
		retval = getNewIdNum.nextNum
		getNewIdNum.nextNum += 1
		return retval
getNewIdNum.nextNum = 0

def toVarName(name):
	return name + "_"

def genVarDec(variables):
	retval = ""
	for i in variables:
		retval += "struct pyobj * " + toVarName(i) + ";\n"
	return retval

class Var:
	def __init__(self, name):
		self.name = name
		self.id = getNewIdNum

def generate_c(n, dec_vars):
		if isinstance(n, Module):

				genList = []
				for i in n.body:
						genList.append(generate_c(i, dec_vars))
				variableDeclaration = genVarDec(dec_vars)
				return "\n".join(["#include <stdio.h>",
								"#include \"pyobj.h\"",
													"int main()",
													"{", 
													variableDeclaration,
													"pyobjInit();",
													"True_ = pyobjTrue();",
													"False_ = pyobjFalse();",
													"\n".join(genList), 
													"return 0;",
													"}"])
		elif isinstance(n, Print): #Print Statements
			retval = "pyobjPrint(vector<pyobj *>(["
			values = []
			for i in n.values:
				values.append(generate_c(i, dec_vars))
			values = ", ".join(values)
			retval += values
			retval += "], "
			if n.nl:
				retval += 'true'
			else:
				retval += 'false'
			retval += ');\n'
			return retval


		#Assignment statement
		#This one's special because we have to wait until after the assignments to 
		# decrement the reference count, so the value is not deleted prematurely
		elif isinstance(n, Assign):
			retval = "pyobjDecRef("
			stack = []
			for i in n.targets:
				stack.append(')')
				retval += "pyobjAssign(" + generate_c(i, dec_vars) + ", "
			retval += generate_c(n.value, dec_vars)
			retval += "".join(stack) + ");\n"
			return retval

		elif isinstance(n, AugAssign):
			op = generate_c(n.op, dec_vars)
			dest = generate_c(n.target, dec_vars)
			n.target.ctx = Load()
			source = generate_c(n.target, dec_vars)
			retval = "pyobjAssign(" + dest + ", " + op + "(" + source + ", " + generate_c(n.value, dec_vars) + "));\n"
			return retval

		#Flow control
		elif isinstance(n, If):
			retval = "if (pyobjToBool(" + generate_c(n.test, dec_vars) + ")){\n"
			body = []
			for i in n.body:
				body.append(generate_c(i, dec_vars))
			retval += "\n".join(body)
			retval += '}\n'
			if len(n.orelse) > 0:
				retval += "else {\n"
				body = []
				for i in n.orelse:
					body.append(generate_c(i, dec_vars))
				retval += "\n".join(body)
				retval += '}\n'
			return retval

		elif isinstance(n, While):
			retval = "while (pyobjToBool(" + generate_c(n.test, dec_vars) + ")){\n"
			body = []
			for i in n.body:
				body.append(generate_c(i, dec_vars))
			retval += "\n".join(body)
			retval += "}\n"
			return retval

		elif isinstance(n, For):
			iterVar = "i_" + str(getNewIdNum())
			retval = "for ( int " + iterVar + " = 0; pyobjGetLen(pyobjGetItr(" + generate_c(n.iter, dec_vars) + ")) > " + iterVar + "; ++" + iterVar + "){\n"
			retval += "pyobjDecRef(pyobjAssign(&" + generate_c(n.target, dec_vars) + ", pyobjIndex(pyobjGetItr(" + generate_c(n.iter, dec_vars) + "), pyobjInt(" + iterVar + "))));\n"
			body = []
			for i in n.body:
				body.append(generate_c(i, dec_vars))
			retval += "\n".join(body)
			retval += "}\n"
			return retval

		elif isinstance(n, Pass):
			return ";\n"

		elif isinstance(n, Break):
			return "break;\n"

		elif isinstance(n, Continue):
			return "continue;\n"

		elif isinstance(n, Expr):
			return "pyobjDecRef(" + generate_c(n.value, dec_vars) + ');\n'

		# Types
		elif isinstance(n, Num):
			if type(n.n) == int:
				return "pyobjInt(" + str(n.n) + ")"
			elif type(n.n) == float:
				return "pyobjFloat(" + str(n.n) + ")"
			else:
				raise SyntaxError("Complex type not supported")

		elif isinstance(n, List):
			retval = "pyobjList(["
			listElms = []
			for i in n.elts:
				listElms.append(generate_c(i, dec_vars))
			retval += ", ".join(listElms)
			retval += '], ' + str(len(n.elts)) + ")"
			return retval

		elif isinstance(n, Dict):
			retval = "pyobjDict(["
			keys = []
			vals = []
			for i in range(len(n.keys)):
				keys.append(generate_c(n.keys[i], dec_vars))
				vals.append(generate_c(n.vals[i], dec_vars))
			retval += ", ".join(keys) + "], ["
			retval += ", ".join(vals) + "], " + str(len(i)) + ")"
			return retval
		
		elif isinstance(n, Name):
			if n.id == "None" and isinstance(n.ctx, Load):
				return "pyobjNone()"
			elif n.id == "None" and isinstance(n.ctx, Store):
				raise SyntaxError("assignment to None")
			elif isinstance(n.ctx, Load):
				return "pyobjLoadName(" + toVarName(n.id) + ")"
			elif isinstance(n.ctx, Store):
				if n.id not in dec_vars:
					dec_vars.add(n.id)
				return "&" + toVarName(n.id)

		#Operations
		elif isinstance(n, UnaryOp):
			op = generate_c(n.op, dec_vars)
			retval = op + "(" + generate_c(n.operand, dec_vars) + ")"
			return retval

		elif isinstance(n, BinOp):
			op = generate_c(n.op, dec_vars)
			retval = op + "(" + generate_c(n.left, dec_vars) + ", " + generate_c(n.right, dec_vars) + ")"
			return retval

		elif isinstance(n, BoolOp):
			op = generate_c(n.op, dec_vars)
			retval = ""
			stk = []
			for i in n.values[:-1]:
				retval += op + "(" + generate_c(i, dec_vars) + ", "
				stk.append(")")
			retval += generate_c(n.values[-1], dec_vars)
			retval += "".join(stk)
			return retval

		elif isinstance(n, Compare):
			retval = ""
			prev = generate_c(n.left, dec_vars)
			cmpops = []
			for i in range(len(n.ops)):
				op = generate_c(n.ops[i], dec_vars)
				cur = generate_c(n.comparators[i], dec_vars)
				cmpops.append(op + "(" + prev + ", " + cur + ")")
				prev = cur
			stk = []
			if len(cmpops) == 1:
				return cmpops[0]
			for i in cmpops[:-1]:
				retval =+ "pyobjAnd(" + i + ", "
				stk += [')']
			retval += cmpops[-1] + "".join(stk)
			return retval

		elif isinstance(n, Subscript):
			idx = generate_c(n.slice.value, dec_vars)
			if isinstance(n.ctx, Load):
				return "pyobjSubscriptLoad(" + generate_c(n.value, dec_vars) + ", " + idx + ")"
			elif isinstance(n.ctx, Store):
				return "pyobjSubscriptStore(" + generate_c(n.value, dec_vars) + ", " + idx + ")"
			else:
				raise SyntaxError("We don't support del")

		elif isinstance(n, IfExp):
			return "pyobjIfExp(pyobjToBool(" + generate_c(n.test, dec_vars) + "), " + generate_c(n.body, dec_vars) + ", " + generate_c(n.orelse, dec_vars) + ")"

		#Operators
		elif isinstance(n, UAdd):
			return "pyobjUAdd"

		elif isinstance(n, USub):
			return "pyobjUSub"
				
		elif isinstance(n, Not):
			return "pyobjNot"
				
		elif isinstance(n, And):
			return "pyobjAnd"
				
		elif isinstance(n, Or):
			return "pyobjOr"

		elif isinstance(n, Add):
			return "pyobjAdd"
				
		elif isinstance(n, Sub):
			return "pyobjSub"
				
		elif isinstance(n, Mult):
			return "pyobjMult"

		elif isinstance(n, Div):
			return "pyobjDiv"

		elif isinstance(n, FloorDiv):
			return "pyobjFloorDiv"

		elif isinstance(n, Mod):
			return "pyobjMod"
				
		elif isinstance(n, Pow):
			return "pyobjPow"
				
		elif isinstance(n, Is):
			return "pyobjIs"
				
		elif isinstance(n, IsNot):
			return "pyobjIsNot"
				
		elif isinstance(n, In):
			return "pyobjIn"
				
		elif isinstance(n, NotIn):
			return "pyobjNotIn"
				
		elif isinstance(n, Eq):
			return "pyobjEq"
				
		elif isinstance(n, NotEq):
			return "pyobjNotEq"
				
		elif isinstance(n, Lt):
			return "pyobjLt"
				
		elif isinstance(n, LtE):
			return "pyobjLtE"
				
		elif isinstance(n, Gt):
			return "pyobjGt"
				
		elif isinstance(n, GtE):
			return "pyobjGtE"




#expr * expr

#pyobj *pyobjMult(pyobj *left, pyobj *right){
#	//Assume ints
#	int leftData = left->data;
#	int rightData = right->data;
#	int resData = leftData * rightData;
#	pyobj *retval = pyobjInt(resData);
#	pyobjDecRef(left);
#	pyobjDecRef(right);
#	return retval;
#}


		else:
				raise sys.exit('Error in generate_c: unrecognized AST node: %s' % n)

if __name__ == "__main__":

		try:
				source = sys.stdin.read()
				tree = parse(source)

				dec_vars = set()
				dec_vars.add("True")
				dec_vars.add("False")

				print generate_c(tree, dec_vars)

		except Exception as e:
				print >> logs, e
				traceback.print_exc(file=logs)