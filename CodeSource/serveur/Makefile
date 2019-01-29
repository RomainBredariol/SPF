
SRCS =	./serveur.c	\
	./mainServeur.c \
	./lib.c
#===============================================================================

CC			= gcc
CFLAGS  = -Wall
CFLAGS += -Werror=implicit-function-declaration
all : output

output : $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ && ./$@ 

clean :
	$(RM) output

