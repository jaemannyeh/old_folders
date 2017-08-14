# See route_guide

HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXX = g++
CPPFLAGS += -I/usr/local/include -pthread
CXXFLAGS += -std=c++11
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++`            \
           -lgrpc++_reflection \
           -lprotobuf -lpthread -ldl
else
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++`            \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl
endif

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

# PROTOS_PATH = ../../protos
PROTOS_PATH = ./

vpath %.proto $(PROTOS_PATH)

all: root_gahyd_client root_gahyd_server

root_gahyd_client: root_gahyd.pb.o root_gahyd.grpc.pb.o helper.o root_gahyd_client.o
	$(CXX) $^ $(LDFLAGS) -o $@
	
root_gahyd_server: root_gahyd.pb.o root_gahyd.grpc.pb.o helper.o root_gahyd_server.o
	$(CXX) $^ $(LDFLAGS) -o $@
	
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
	
clean:
	rm -f *.o *.pb.cc *.pb.h root_gahyd_client root_gahyd_server