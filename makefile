

project: skeleton.c hashtable_itr.c hashtable_itr.h hashtable.h hashtable_private.h hw6_compiler.py
	cat test.py | python hw6_compiler.py > main.c
	gcc main.c skeleton.c hashtable_itr.c hashtable_itr.h hashtable.h hashtable_private.h -o prog

