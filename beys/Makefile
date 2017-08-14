
PROJECT := $(notdir $(CURDIR))

ifeq ($(BEYS_DIR),)
BEYS_DIR := .
endif

BIN_DIR = ./bin
INC_DIR = ./inc
SRC_DIR = ./src
TBL_DIR := $(BEYS_DIR)/tbl

ifeq ("$(wildcard $(BIN_DIR))","")
$(shell mkdir $(BIN_DIR))
endif

ifeq ("$(wildcard $(TBL_DIR))","")
$(shell mkdir $(TBL_DIR))
endif

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJIKTS := $(patsubst %.cpp, %.o, $(notdir $(SOURCES)))
OBJECTS := $(addprefix $(BIN_DIR)/, $(OBJIKTS))

DEFINES :=

INCLUDES := -I$(INC_DIR) -I$(SRC_DIR)

LAHYBREE := -L./bin -lbeys -lsqlite3 -lreadline

VPATH := $(SRC_DIR)

default: clean $(BIN_DIR)/lib$(PROJECT).a entry tables

entry:
	g++ -o $(BIN_DIR)/main $(BIN_DIR)/main.o $(LAHYBREE)

$(BIN_DIR)/lib$(PROJECT).a: $(OBJECTS)
	ar rs $@ $^
	ar d $@ main.o

$(BIN_DIR)/%.o: %.cpp
	g++ -std=c++11 -g -O -Wall $(DEFINES) $(INCLUDES) -c $< -o $@

tables:
	cat $(TBL_DIR)/tables.sql | sqlite3 $(TBL_DIR)/tables.db
	cat $(TBL_DIR)/beys.sql | sqlite3 $(TBL_DIR)/beys.db
	
clean:
	-rm $(BIN_DIR)/* $(TBL_DIR)/*.db

show:
	@echo PROJECT = $(PROJECT)
	@echo SOURCES = $(SOURCES)
	@echo OBJECTS = $(OBJECTS)
	@echo DEFINES = $(DEFINES)
	@echo INCLUDES = $(INCLUDES)
	@echo LAHYBREE = $(LAHYBREE)	