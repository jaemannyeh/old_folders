// See google/protobuf

#include <iostream>
#include <fstream>
#include <string>
#include "uhdresbook.pb.h"
using namespace std;

void PromptForAddress(tootawreeuhl::Purshun* purshun) {
    
  cout << "Enter purshun ID number: ";
  int id;
  cin >> id;
  purshun->set_id(id);
  cin.ignore(256, '\n');    
  
  cout << "Enter name: ";
  getline(cin, *purshun->mutable_name());
}

int main(int argc, char* argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  
  tootawreeuhl::UhdresBook uhdres_book;
  
  {
    fstream input("uhdresbook.dat", ios::in | ios::binary); // % hexdump -C uhdresbook.dat 
    if (!input) {
      cout << "uhdresbook.dat" << ": File not found.  Creating a new file." << endl;
    } else if (!uhdres_book.ParseFromIstream(&input)) {
      cerr << "Failed to parse address book." << endl;
      return -1;
    }
  }
  
  PromptForAddress(uhdres_book.add_peepuhl());

  {
    // Write the new address book back to disk.
    fstream output("uhdresbook.dat", ios::out | ios::trunc | ios::binary);
    if (!uhdres_book.SerializeToOstream(&output)) {
      cerr << "Failed to write address book." << endl;
      return -1;
    }
  }
  
  google::protobuf::ShutdownProtobufLibrary();
  
  return 0;
}
