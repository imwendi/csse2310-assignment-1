# Inspired by example C++ makefiles from 
# https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html#zz-1.8

CC = gcc
FLAGS = -pedantic -Wall --std=gnu99
OBJS = wordList.o readDict.o searchMethods.o search.o
TARGET = search

# Compile the target
$(TARGET) : $(OBJS)
	$(CC) $(FLAGS) -o $@ $^

# Pattern rule for compiling a .o object given a .c file
%.o : %.c
	$(CC) $(FLAGS) -o $@ -c $<

# Dependency rules
search.o: wordList.h
readDict.o: readDict.h wordList.h
wordList.o : wordList.h
searchMethods.o : searchMethods.h wordList.h

clean:
	rm *.o search





