from ast import *
import sys

def getNewIdNum():
    retval = getNewIdNum.nextNum
    getNewIdNum.nextNum += 1
    return retval
getNewIdNum.nextNum = 0

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
      retval += ');'
      return retval

    #loops
    elif isinstance(n, While):
      retval = "while (pyobjToBool(" + generate_c(n.test) + ")){\n"
      body = []
      for i in n.body:
        body.append(generate_c(i))
      retval += "\n".join(body)
      retval += "}"
    elif isinstance(n, Assign):
      


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