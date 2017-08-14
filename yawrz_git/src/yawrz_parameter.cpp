
#include "yawrz.h"

#include <sqlite3.h>

//using namespace std;

DEFINE_string(path,"./tbl/parameters.db","path");

static sqlite3 *the_prmtr_db = NULL;
    
static int prmtr_db_open() {
    int rc = sqlite3_open(FLAGS_path.c_str(),&the_prmtr_db);
    if (rc) {
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
        sqlite3_close(the_prmtr_db);
        the_prmtr_db = NULL;
        return -1;
    }
    return 0;
}

static int prmtr_set(char *name,char *value) {
    if (the_prmtr_db == NULL) {
        prmtr_db_open();
    }
    
    char *zErrMsg = 0;    
    char sql_update[PATH_MAX];

    sprintf(sql_update,"UPDATE DEVVAR SET VALUE='%s' WHERE NAME='%s';",value,name);
    int rc = sqlite3_exec(the_prmtr_db,sql_update,NULL,NULL,&zErrMsg);
    if (rc != SQLITE_OK) {
        LOG(INFO) << zErrMsg; // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,zErrMsg);        
        sqlite3_free(zErrMsg);
    }

    return 0;
}

static char *prmtr_get(char *name,char *value) {
    if (the_prmtr_db == NULL) {
        prmtr_db_open();
    }

    // SELECT * FROM DEVVAR WHERE NAME LIKE 'Device%'; 
    char sql_select[PATH_MAX];
    sprintf(sql_select,"SELECT VALUE FROM DEVVAR WHERE NAME='%s';",name);
        
    sqlite3_stmt *statement;
    int rc = sqlite3_prepare_v2(the_prmtr_db,sql_select,-1,&statement,NULL);
    if (rc != SQLITE_OK) {
        LOG(INFO) << __FUNCTION__; // printf("%s:%d %d",__FUNCTION__,__LINE__,rc);        
    }
    
    value[0] = 0;
    while (sqlite3_step(statement)==SQLITE_ROW) {
        const unsigned char *valoo = sqlite3_column_text(statement,0);
        strncpy(value,(const char *)valoo,PATH_MAX);
    }
    
    sqlite3_finalize(statement);

    return value;
}

namespace yawrz {
    
int parameter_set(char *name,char *value) {
    return prmtr_set(name,value);
}

int parameter_set_int(char *name,int n) {
    char value[PATH_MAX];
    sprintf(value,"%d",n);
    return prmtr_set(name,value);
}

int parameter_set_bool(char *name, bool b) {
    char value[4];
    if (b) {
        value[0] = '1'; // true        
    }
    else {
        value[0] = '0'; // false        
    }
    value[1] = '\0'; // null
    return prmtr_set(name,value);    
}

char *parameter_get(char *name,char *value) {
    return prmtr_get(name,value);
}

int parameter_get_int(char *name,int default_value) {
    char value[PATH_MAX];
    prmtr_get(name,value);
    return strtol(value,NULL,10);
}

bool parameter_get_bool(char *name) {
    char value[PATH_MAX];
    value[0] = 0;
    prmtr_get(name,value); // if (memchr("tTyY1\0", value[0], 6) != NULL) { return true; }
    if (value[0] == '0' || value[0] == 'f' || value[0] == 'F') {
        return false;
    }
    return true;
}

} // namespace yawrz
