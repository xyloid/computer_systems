#  
# Check system default gcc version
GCCVER =$(shell gcc -dumpversion)
$(info Detected gcc version $(GCCVER))

GCC_ALTER = gcc-4.8 gcc-4.8.5 

ifeq "$(GCCVER)" "4.8.5"
 $(info "Default gcc version is the same as the Khoury server's.")
 COMPILER = gcc
endif

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

# HW Specified Part
executables = trace simulator

#COMPILER=clang

all : $(executables)

# -lm should be in the end of line, otherwise gcc can not use ld correctly.
trace : % : %.c 
	$(COMPILER) -std=c11 $< -o $@ -lm

simulator : % : %.c
	$(COMPILER) -std=c11 $< -o $@ -lm

.PHONY : clean
clean :
	rm $(executables) *.o
