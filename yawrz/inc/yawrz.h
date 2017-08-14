
//#pragma once

#ifndef YAWRZ_H
#define YAWRZ_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cstring>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>
#include <syslog.h>

#include <glog/logging.h> //#include <gflags/gflags.h> // sudo apt-get install libgoogle-glog-dev

namespace yawrz {

int server(int argc, char *argv[]);

int cli_server_using_socket();
int cli_server_using_zmq();

int cli_main(int argc, char *argv[]);    

int command_execute(char *statement,std::ostringstream &sout); // On error, -1 is returned.

const char* version();

int parameter_set(char *name,char *value);
int parameter_set_int(char *name,int n);
int parameter_set_bool(char *name,int n);
char *parameter_get(char *name,char *value);
int parameter_get_int(char *name,int default_value);
int parameter_get_bool(char *name,int default_value);

int tables_main(int argc, char *argv[]);  

} // namespace yawarz

#endif /* YAWRZ_H */


