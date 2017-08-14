# See google/protobuf

.PHONY: all cpp

all: cpp

cpp: add_purshun_cpp list_peepuhl_cpp

clean:
	rm -f add_purshun_cpp list_peepuhl_cpp
	rm -f protoc_middleman uhdresbook.pb.cc uhdresbook.pb.h
	rm -f uhdresbook.dat

protoc_middleman: uhdresbook.proto
	protoc --cpp_out=. uhdresbook.proto
	@touch protoc_middleman
	
add_purshun_cpp: add_purshun.cc protoc_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ add_purshun.cc uhdresbook.pb.cc -o add_purshun_cpp `pkg-config --cflags --libs protobuf`

list_peepuhl_cpp: list_peepuhl.cc protoc_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ list_peepuhl.cc uhdresbook.pb.cc -o list_peepuhl_cpp `pkg-config --cflags --libs protobuf`
	