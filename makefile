flags = -std=c++0x
CC = clang++

default: project

main.cpp: test.py
	cat test.py | python translate.py > main.cpp

pyobj.o: pyobj.cpp pyobj.h
	$(CC)  -c pyobj.cpp

project: main.cpp pyobj.h pyobj.o
	$(CC)  main.cpp pyobj.o -o prog

clean:
	-@rm -f main.cpp
	-@rm -f pyobj.o
	-@rm -f project
