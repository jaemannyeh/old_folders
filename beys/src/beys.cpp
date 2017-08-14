
#include "beys.h"

Beys::Beys() {
    srandom(time(NULL));
    
    if(getenv("BEYS_DIR") == NULL) {
        setenv("BEYS_DIR",".",1);
    }

    strcpy(beys_db_path,getenv("BEYS_DIR"));
    strcat(beys_db_path,"/"); // linux allows multiple '/' in the path.
    strcat(beys_db_path,"tbl/beys.db");
    std::cout << time(NULL) << ":" << basename(__FILE__) << ":" << __LINE__ << ":";
    std::cout << beys_db_path << ":" << PATH_MAX;
    std::cout << std::endl;
}

Beys::~Beys() {
}

int Beys::sqlite3Callback(void *NotUsed, int argc, char **argv, char **azColName) {
   return 0;
}

void Beys::getDeviceData() {
    sqlite3* db;    
    int rc;

    rc = sqlite3_open(beys_db_path,&db);
    if (rc) {
        // see http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
        perror(beys_db_path);        
        perror(sqlite3_errmsg(db));
        db = NULL;
    }
    
    sqlite3_close(db);
    db = NULL;
}
    
void Beys::setDeviceData() {
    sqlite3* db;    
    int rc;
    char *zErrMsg = NULL;
    const char* data = "Callback function called";
    
    rc = sqlite3_open(beys_db_path,&db);
    if (rc) {
        // see http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
        perror(beys_db_path);        
        perror(sqlite3_errmsg(db));
        db = NULL;
    }

    const char *sql_update = "UPDATE DEVICE_DATA set VALUE='http://www.google.com' where NAME='evice.ManagementServer.URL';";
    rc = sqlite3_exec(db, sql_update, sqlite3Callback, (void*)data, &zErrMsg);
    if ( rc != SQLITE_OK ){
        perror(zErrMsg);
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
   
    sqlite3_close(db);
    db = NULL;
}

Beys TheBeys;
Beys *theBeys = &TheBeys;

// local objects -------------------------------------------------------------
sqlite3* the_beys_sqlite3_database_connection_object = NULL;
// The prepared statement object: sqlite3_stmt

//***************************************************************************/
/// @description
///
int beys_open() {
    srandom(time(NULL));
    
    if(getenv("BEYS_DIR") == NULL) {
        setenv("BEYS_DIR",".",1);
    }
}

//***************************************************************************/
/// @description
///
int beys_close() {
}

//***************************************************************************/
/// @description
///
static int beys_sqlite3_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i=0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

//***************************************************************************/
/// @description
///
int beys_set(const char *name,const char *value) {
    return 0;
}

//***************************************************************************/
/// @description
///
char *beys_get(const char *name,char *value)
{
    return NULL;
}

//***************************************************************************/
/// @description
///
int beys_get_as_int(const char *name,int default_value) {
  return 0;  
}

//***************************************************************************/
/// @description
///
void beys_usage() {
    std::cout << "Usage:" << std::endl;
}

//***************************************************************************/
/// @description
///
int beys_process_args(int argc, char *argv[]) {
    while (1) {
        // see https://www.cs.swarthmore.edu/~newhall/unixhelp/C_cool_utils.html
        int c;
        
        c = getopt(argc, argv, "hp:");
        if (c == -1) {
            break;
        }
        
        switch (c) {
            case 'h':
                beys_usage();
                exit(0);
                break;
            case 'p':
                std::cout << optarg << std::endl;
                break;
            default:
                break;
        }
        
    }
    
    return 0;
}

//***************************************************************************/
/// @description
///
int beys_main(int argc, char *argv[]) {
    
    beys_process_args(argc,argv);

    return 0;
}
