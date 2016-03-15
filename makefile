CC = g++

default: 
	-@make clean
	-@make test

main.cpp: test.py
	cat test.py | python translate.py > main.cpp

pyobj.o: pyobj.cpp pyobj.h
	$(CC) -g -c pyobj.cpp

project: main.cpp pyobj.h pyobj.o
	$(CC) -g main.cpp pyobj.o -o project

clean:
	-@rm -f main.cpp
	-@rm -f pyobj.o
	-@rm -f project
	-@rm -f py.out
	-@rm -f c.out
	-@rm -f diff.out

test: project test.py
	python test.py > py.out
	./project > c.out
	-@diff py.out c.out >diff.out
	cat diff.out

