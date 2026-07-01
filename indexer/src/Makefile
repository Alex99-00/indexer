
TNAME = indexer
SHELL = /bin/sh
OPT = -O3
INCLUDES = -I. -I../include
CFLAGS = -c -pedantic -Wall -Werror -ansi $(INCLUDES)
CFLAGSD = -c -pedantic -Wall -Werror -ansi -DNDEBUG $(INCLUDES)

CFILES = \
	alloc.c \
	austab.c \
	hashtbl.c \
	param.c \
	datei.c \
	indexer.c \


OBJ = $(CFILES:.c=.o)


# Immer Komlettes Rebuild
debug: cleanall $(TNAME) ; strip $(TNAME)


# Immer Komlettes Rebuild
all: cleanall $(TNAME) ; strip $(TNAME)


# all oder debug ? -> CFLAGS oder CFLAGSD
ifeq ($(MAKECMDGOALS),all)
.c.o:	
	$(CC) $(CFLAGS) $(OPT) $<
else
.c.o:	
	$(CC) $(CFLAGSD) $(OPT) $<
endif

$(TNAME): $(OBJ)
	$(CC) $(OPT) -o $(TNAME) $(OBJ)


clean: 
	rm -f *\.o

cleanall: 
	rm -f *\.o indexer
