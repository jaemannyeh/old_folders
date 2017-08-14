
# Makefile for Reason

PROJECT=reason
REASON=./

CC=g++

INC=-Isrc -Isrc/$(PROJECT) -I$(REASON)lib/sqlite/include -I$(REASON)lib/zlib/include -I$(REASON)lib/openssl/include -I$(REASON)lib/mysql/include -I$(REASON)src -I$(REASON)lib/postgres/include 

LIB=-lm -lpthread -lz -L$(REASON) -lrt -lcrypt -ldl 

OBJ=$(REASON)lib/sqlite/libsqlite3.a $(REASON)lib/openssl/libssl.a $(REASON)lib/openssl/libcrypto.a $(REASON)lib/mysql/libmysqlclient.a $(REASON)lib/postgres/libpq.a


OPTARCH=-march=pentium4
OPTFLAG=-fpermissive -fno-operator-names -fno-rtti -fno-exceptions

#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -Os 
#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O3 
CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O3 -ggdb 
#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O0 -ggdb -D_DEBUG 

SRCDIR=src
OBJDIR=build

#SRCFILES=$(shell find $(SRCDIR)/ -maxdepth 3 -name "*.cpp")
SRCFILES=$(shell find $(SRCDIR)/ -name "*.cpp")
INCFILES=$(shell find $(SRCDIR)/ -name "*.d")
#SUBFILES=$(wildcard *.d)
OBJFILES=$(patsubst %.cpp,%.o,$(SRCFILES))
DEPFILES=$(patsubst %.cpp,%.d,$(SRCFILES))

default: all

clean:
	rm -f $(OBJFILES)
	rm -f $(DEPFILES)

all: $(PROJECT)

$(PROJECT): $(OBJFILES)	 
	$(CC) $(OBJFILES) $(OBJ) $(LIB) -o $@

$(OBJFILES):  
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

depend: $(DEPFILES)


library: $(OBJFILES)
	rm -f libreason.a
	ar rcs libreason.a $(filter-out reason% test%,$(OBJFILES)) 


$(DEPFILES): 
	$(warning $< $@)
	$(CC) $(INC) $(patsubst %.d,%.cpp,$@) -E -MM -MT $(patsubst src/%.d,src/%.o,$@) -MF $@ > /dev/null
	
include $(INCFILES)
