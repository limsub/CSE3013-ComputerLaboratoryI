CC = gcc
target = main
objects = main.o cal_func.o pnt_func.o

$(target) : $(objects)
	$(CC) -o $(target) $(objects)

$(objects) : main.h

./PHONY : clean
clean :
	rm $(target) $(objects)