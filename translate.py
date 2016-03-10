from ast import *
import sys

def getNewIdNum():
		retval = getNewIdNum.nextNum
		getNewIdNum.nextNum += 1
		return retval
getNewIdNum.nextNum = 0

def toVarName(name):
	return name + "_"

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
													"int main()",
													"{", 
													variableDeclaration,
													"\n".join(genList), 
													"return 0;",
													"}"])
		elif isinstance(n, Print) #Print Statements
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
				retval += "pyobjAssign(&" + generate_c(i) + ", "
			retval += generate_c(n.value)
			retval += "".join(stack) + ");\n"
			return retval

		#Flow control
		elif isinstance(n, If):
			retval = "if (pyobjToBool(" + generate_c(n.test) + ")){\n"
			body = []
			for i in n.body:
				body.append(generate_c(i))
			retval += "\n".join(body)
			retval += '}\n'
			if len(n.orelse) > 0:
				retval += "else {\n"
				body = []
				for i in n.orelse:
					body.append(generate_c(i))
				retval += "\n".join(body)
				retval += '}\n'
			return retval

		elif isinstance(n, While):
			retval = "while (pyobjToBool(" + generate_c(n.test) + ")){\n"
			body = []
			for i in n.body:
				body.append(generate_c(i))
			retval += "\n".join(body)
			retval += "}\n"
			return retval

		elif isinstance(n, For):
			iterVar = "i_" + str(getNewIdNum())
			retval = "for ( int " + iterVar " = 0; pyobjGetLen(pyobjGetItr(" + generate_c(n.iter) + ")) > " + iterVar + "; ++" + iterVar + "){\n"
			retval += "pyobjDecRef(pyobjAssign(&" + generate_c(target) + ", pyobjIndex(pyobjGetItr(" + generate_c(n.iter) + "), pyobjInt(" + iterVar + "))));\n"
			body = []
			for i in n.body:
				body.append(generate_c(i))
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
			return generate_c(n.value) + ';\n'

		# Types
		elif isinstance(n, Num):
			if type(n.n) == int:
				return "pyobjInt(" + str(n.n) + ")"
			elif type(n.n) == float
				return "pyobjFloat(" + str(n.n) + ")"
			else:
				raise SyntaxError("Complex type not supported")

		elif isinstance(n, List):
			retval = "pyobjList(["
			listElms = []
			for i in n.elts:
				listElms.append(generate_c(i))
			retval += ", ".join(listElms)
			retval += '], ' + str(len(n.elts)) + ")"
			return retval

		elif isinstance(n, Dict):
			retval = "pyobjDict(["
			keys = []
			vals = []
			for i in range(len(n.keys)):
				keys.append(generate_c(n.keys[i]))
				vals.append(generate_c(n.vals[i]))
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
				return toVarName(n.id)

		#Operations
		elif isinstance(n, UnaryOp):
			op = generate_c(n.op)
			retval = op + "(" + generate_c(n.operand) + ")"
			return retval

		elif isinstance(n, BinOp):
			op = generate_c(n.op)
			retval = op + "(" + generate_c(n.left) + ", " + generate_c(n.right) + ")"
			return retval

		elif isinstance(n, BoolOp):
			op = generate_c(n.op)
			retval = ""
			stk = []
			for i in n.values[:-1]:
				retval += op + "(" + generate_c(i) + ", "
				stk.append(")")
			retval += generate_c(n.values[-1])
			retval += "".join(stk)
			return retval

		elif isinstance(n, Compare):
			retval = ""
			prev = generate_c(n.left)
			cmpops = []
			for i in range(len(n.ops)):
				op = generate_c(n.ops[i])
				cur = generate_c(n.comparators[i])
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

		#Operators
		elif isinstance(n, UAdd):
			return "pyobjUAdd"





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

				print generate_c(tree, dec_vars)

		except Exception as e:
				print >> logs, e
				traceback.print_exc(file=logs)