from ast import *

def getNewIdNum():
    retval = getNewIdNum.nextNum
    getNewIdNum.nextNum += 1
    return retval
getNewIdNum.nextNum = 0

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
                          "True__char = 1;\nFalse__char = 0;",
                          "\n".join(genList), 
                          "return 0;",
                          "}"])
    #Type literals
    
    else:
        raise sys.exit('Error in generate_c: unrecognized AST node: %s' % n)

if __name__ == "__main__":

    try:
        mylexer = MyLexer()
        mylexer.input(sys.stdin.read())
        tree = parse_module()

        dec_vars = set()

        print generate_c(tree, dec_vars)

    except Exception as e:
        print >> logs, e
        traceback.print_exc(file=logs)