all: YSLog

OBJ_DIR = ./obj
SRC_DIR = ./src
INC_DIR = ./h

OBJECTS = $(OBJ_DIR)/Main.o \
	$(OBJ_DIR)/LogSource.o \
	$(OBJ_DIR)/SimpleLogSink.o

INCLUDES = -I$(INC_DIR)
OPTIONS = -Wall -g -c -O
CFLAGS = $(OPTIONS) $(INCLUDES)

YSLog: $(OBJECTS)
	g++ -o YSLog $(OBJECTS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	g++ $(CFLAGS) -o $@ $<

$(OBJ_DIR)/Main.o: $(INC_DIR)/LogSource.h $(INC_DIR)/LogPublisher.h $(INC_DIR)/LogListener.h $(INC_DIR)/LoggingEvent.h

$(OBJ_DIR)/LogSource.o: $(INC_DIR)/LogSource.h $(INC_DIR)/LogPublisher.h $(INC_DIR)/LogListener.h $(INC_DIR)/LoggingEvent.h $(INC_DIR)/Portability.h

$(OBJ_DIR)/SimpleLogSink.o: $(INC_DIR)/SimpleLogSink.h $(INC_DIR)/LogListener.h $(INC_DIR)/LoggingEvent.h

clean:
	rm -rf $(OBJ_DIR)/*.o YSLog

