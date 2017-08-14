// See google/protobuf

#include <iostream>
#include <fstream>
#include <string>
#include "uhdresbook.pb.h"
using namespace std;

void ListPeople(const tootawreeuhl::UhdresBook& uhdres_book) {
  for (int i = 0; i < uhdres_book.peepuhl_size(); i++) {
    const tootawreeuhl::Purshun& purshun = uhdres_book.peepuhl(i);
    
    cout << "Purshun ID: " << purshun.id() << endl;
    cout << "      Name: " << purshun.name() << endl;
  }
}

int main(int argc, char* argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  
  tootawreeuhl::UhdresBook uhdres_book;
  
  {
    fstream input("uhdresbook.dat", ios::in | ios::binary); // % hexdump -C uhdresbook.dat 
    if (!uhdres_book.ParseFromIstream(&input)) {
      cerr << "Failed to parse address book." << endl;
      return -1;
    }
  }
  
  ListPeople(uhdres_book);
  
  google::protobuf::ShutdownProtobufLibrary();
  
  return 0;
}
