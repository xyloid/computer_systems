#  
# Check system default gcc version
GCCVER =$(shell gcc -dumpversion)
$(info Detected gcc version $(GCCVER))

GCC_ALTER = gcc-4.8 gcc-4.8.5 

ifeq "$(GCCVER)" "4.8.5"
 $(info "Default gcc version is the same as the Khoury server's.")
 COMPILER = gcc
endif

#COMPILER_temp := $(foreach c, $(GCC_ALTER),$(shell which $(c)))
#$(info $(COMPILER_temp))

ifndef COMPILER
 ifeq "/usr/bin/gcc-4.8" "$(shell which gcc-4.8)"
  $(info "Forced to use same gcc version as the Khoury server's.")
  COMPILER = gcc-4.8
 endif
endif

ifndef COMPILER
 ifeq "/usr/bin/gcc-4.8.5" "$(shell which gcc-4.8.5)"
  $(info "Forced to use same gcc version as the Khoury server's.")
  COMPILER = gcc-4.8.5
 endif
endif

ifndef COMPILER
 $(info "gcc-4.8 not found, using gcc $(GCCVER) instead.")
 COMPILER = gcc
endif


SRC = main.c


all : main.o trace.o
	$(COMPILER) main.o trace.o -o main

memchk : main.o trace.o
	$(COMPILER)   main.o trace.o -o memchk
	valgrind --leak-check=full ./memchk

main.o : main.c
	$(COMPILER) -Wall -finstrument-functions -g -c -o main.o main.c

trace.o : trace.c
	$(COMPILER) -Wall -g -c -o trace.o trace.c

.PHONY : clean
clean :
	rm memchk main *.o

