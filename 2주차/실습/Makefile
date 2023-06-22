CC = gcc
CFLAGS = -g $(INC)
target = test
objects= mylib.o myarr.o main.o

$(target): $(objects)
	$(CC) -o $(target) $(objects)

myarr.o : myarr.h
mylib.o : mylib.h

.PHONY : clean
clean :
	rm -rf $(target) $(objects)
