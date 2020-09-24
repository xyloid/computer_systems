# Commented lines that are ignored starte with the '#' mark.
# A helpful tutorial for more information on make
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

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

executables = tcpserver tcpclient

all : $(executables)

# By default, we generate randomized color for each artist.
$(executables) : % : %.c
	$(COMPILER) -Wall -g $< -o $@


clean :
	rm  $(executables)

