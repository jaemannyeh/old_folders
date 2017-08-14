PROJECT := $(notdir $(CURDIR))

BIN_DIR = ./bin
INC_DIR = ./inc
LOG_DIR = ./log
SRC_DIR = ./src
TBL_DIR = ./tbl

ifeq ("$(wildcard $(BIN_DIR))","")
$(shell mkdir $(BIN_DIR))
endif

ifeq ("$(wildcard $(INC_DIR))","")
$(shell mkdir $(INC_DIR))
endif

ifeq ("$(wildcard $(LOG_DIR))","")
$(shell mkdir $(LOG_DIR))
endif

ifeq ("$(wildcard $(SRC_DIR))","")
$(shell mkdir $(SRC_DIR))
endif

ifeq ("$(wildcard $(TBL_DIR))","")
$(shell mkdir $(TBL_DIR))
endif

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJIKTS := $(patsubst %.cpp, %.o, $(notdir $(SOURCES)))
OBJECTS := $(addprefix $(BIN_DIR)/, $(OBJIKTS))

DEFINES :=

INCLUDES := -I$(INC_DIR) -I$(SRC_DIR) -I/usr/include/libxml2

LAHYBREE := -L./bin -l$(PROJECT) -lsqlite3 -lxml2 -lzmq -lcurl -lprotobuf -lglog -lgflags -lreadline -lpthread

VPATH := $(SRC_DIR) $(TBL_DIR)

default: default1 default2 

default1: clean $(BIN_DIR)/lib$(PROJECT).a

default2: $(BIN_DIR)/$(PROJECT)_main

$(BIN_DIR)/lib$(PROJECT).a: $(OBJECTS)
	ar rs $@ $^
	ar d $@ $(PROJECT)_cli_main.o $(PROJECT)_server_main.o $(PROJECT)_tables_main.o

$(BIN_DIR)/%.o: %.cpp
	g++ -std=c++11 -g -O -Wall $(DEFINES) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/$(PROJECT)_main:
	g++ -std=c++11 -o $@ $@.o $(LAHYBREE)

migrate:
ifneq ($(PROJECT),kwiz)
	rename -v 's/kwiz/$(PROJECT)_/' ./inc/*
	-sed -i -e 's/kwiz/$(PROJECT)_/' ./inc/*
	rename -v 's/kwiz/$(PROJECT)_/' ./src/*
	-sed -i -e 's/kwiz/$(PROJECT)_/' ./src/*	
	rename -v 's/kwiz/$(PROJECT)_/' ./tbl/*
endif	

install:
	sudo cp $(SRC_DIR)/$(PROJECT)_server.sh /etc/init.d/
	sudo chmod a+x /etc/init.d/$(PROJECT)_server.sh

-include Makefile.tbl # simply ignore a makefile which does not exist 

clean:
	-rm -f $(BIN_DIR)/* $(LOG_DIR)/* 
	-rm -f $(TBL_DIR)/cwmp_parameters.sql $(TBL_DIR)/*.db $(TBL_DIR)/*.pb.*

show:
	@echo PROJECT = $(PROJECT)
	@echo SOURCES = $(SOURCES)
	@echo OBJECTS = $(OBJECTS)
	@echo DEFINES = $(DEFINES)
	@echo INCLUDES = $(INCLUDES)
	@echo LAHYBREE = $(LAHYBREE)

