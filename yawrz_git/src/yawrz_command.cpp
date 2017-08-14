
#include "yawrz.h"

typedef void (*function_pointer_t)(int argc,char *argv[],std::ostringstream &sout);
static std::map<std::string,function_pointer_t> command_map;

static void command_add(const char *name,function_pointer_t funptr,const char *description) {
    command_map[name] = funptr;
}

static void parameter(int argc,char *argv[],std::ostringstream &sout) {
    char value[PATH_MAX];
    if (strcmp(argv[1],"get")==0) {
        sout << yawrz::parameter_get(argv[2],value);
    }
    else if (strcmp(argv[1],"set")==0) {
        yawrz::parameter_set(argv[2],argv[3]);
    }
}

static void help(int argc,char *argv[],std::ostringstream &sout) {
    sout << __FUNCTION__;
}

static void restart(int argc,char *argv[],std::ostringstream &sout) {
    sout << __FUNCTION__;
}

static void shutdown(int argc,char *argv[],std::ostringstream &sout) {
    sout << __FUNCTION__;    
}

static void uptime(int argc,char *argv[],std::ostringstream &sout) {
    time_t tahym = time(NULL); // as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)
    char buhfer[1024];

    sout << ctime_r(&tahym,buhfer);

    //struct tm lohkuhl_tahym;    
    // localtime_r(&tahym,&lohkuhl_tahym);
    //sout << asctime_r(&lohkuhl_tahym,buhfer);
    //gmtime_r(&tahym,&lohkuhl_tahym);
    //sout << asctime_r(&lohkuhl_tahym,buhfer);
}

static void version(int argc,char *argv[],std::ostringstream &sout) {
    sout << yawrz::version();
}

static void command_init() {

    // % sudo vi /etc/rsyslog.conf
    // % sudo vi /etc/rsyslog.d/40-daemon_alert.conf
    // % sudo /etc/init.d/rsyslog restart or % sudo service rsyslog restart
    // % logger
    // see /var/log/syslog

    LOG(INFO) << __FUNCTION__; // syslog(LOG_DEBUG,"%s:%d",__FUNCTION__,__LINE__);

    command_add("parameter",parameter,"");
    command_add("help",help,"");
    command_add("restart",restart,"");
    command_add("shutdown",shutdown,"");
    command_add("uptime",uptime,"");
    command_add("version",version,"");    
}

namespace yawrz {

int command_execute(char *statement,std::ostringstream &sout) {
    static bool command_initialized = false;
    if (command_initialized == false)
    {
        command_init();
        command_initialized = true;
    }
    
    int argc = 0;
    int argc_max = 8;
    char *argv[argc_max];
    char *saveptr = NULL;
    argv[argc] = strtok_r(statement," \t",&saveptr); //[0]
    for (argc=1; argc<argc_max; argc++) { // [1],...,[argc_max-1]
        argv[argc] = strtok_r(NULL," \t",&saveptr);
        if (argv[argc]==NULL) {
            break;
        }
    }

    function_pointer_t funptr = command_map[argv[0]];
    if (funptr == NULL) {
        sout << statement << ": command not found";
        return -1;
    }
    
    funptr(argc,argv,sout);
    return 0;
}

} // namespace yawarz
