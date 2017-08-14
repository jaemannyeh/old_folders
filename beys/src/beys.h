
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>

#include <sqlite3.h>

#include <readline/readline.h>
#include <readline/history.h>

class Beys {
private:
    char beys_db_path[PATH_MAX];

public:    
    Beys();
    ~Beys();
    
    static int sqlite3Callback(void *NotUsed, int argc, char **argv, char **azColName);
    void getDeviceData();
    void setDeviceData();
    
    int set(const char *name,const char *value);
    char *get(const char *name,char *value);
    int get_as_int(const char *name,int default_value);
};