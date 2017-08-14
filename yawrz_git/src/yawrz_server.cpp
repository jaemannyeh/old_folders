
#include "yawrz.h"

//using namespace std;

static void usage(char *argv0) {
    printf("Usage: %s -h -p 1234 -v\n",argv0);
}

static int process_args(int argc, char *argv[]) {
    while (1) {
        int c;
        
        c = getopt(argc, argv, "hp:v");
        if (c == -1) {
            break;
        }
        
        switch (c) {
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'p':
                printf("%s\n",optarg);
                break;
            case 'v':
                printf("%s\n",yawrz::version());
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }
    
    return 0;
}

static int become_daemon() {
    // http://shahmirj.com/blog/beginners-guide-to-creating-a-daemon-in-linux
    // http://shahmirj.com/blog/the-initd-script
    // http://man7.org/tlpi/code/online/dist/daemons/become_daemon.c.html
    
    pid_t pid = fork();
    if (pid < 0) {
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno)); // perror("")
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0); // change file mask // why?

    pid_t sid = setsid(); // run a program in a new session
    if (sid < 0) {
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno)); // perror("")
        exit(EXIT_FAILURE);
    }
    
    if (chdir("/")<0) { // on error, -1 is returned
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno)); // perror("")
        exit(EXIT_FAILURE);        
    };

    close(STDIN_FILENO); // 0
    close(STDOUT_FILENO); // 1
    close(STDERR_FILENO); // 2
    
    return 0;
}

static void *cli_server_using_socket_thread(void *arg) {
    while (1) {
        //syslog(LOG_DEBUG,"%s:%d",__FUNCTION__,__LINE__);    
        yawrz::cli_server_using_socket();
        sleep(1);
    }
    return NULL;
}

static void *cli_server_using_zmq_thread(void *arg) {
    while (1) {
        //syslog(LOG_DEBUG,"%s:%d",__FUNCTION__,__LINE__);    
        yawrz::cli_server_using_zmq();
        sleep(1);
    }
    return NULL;
}
namespace yawrz {

int server(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler(); // see glog/logging.h
    
    google::SetVersionString(yawrz::version());
    google::SetUsageMessage("");    
    google::ParseCommandLineFlags(&argc, &argv, true); //  see logging_unittest.cc

    process_args(argc,argv);

    become_daemon();

    pthread_t thread_s;
    pthread_create(&thread_s, NULL, cli_server_using_socket_thread, NULL); // cli_server_run();
    
    pthread_t thread_z;
    pthread_create(&thread_z, NULL, cli_server_using_zmq_thread, NULL); // cli_server_run();
    
    while (1) {
        sleep(1);
    }
    
    return 0;
}

} // namespace yawarz

    // openlog(DAEMON_NAME,LOG_CONS| LOG_NDELAY|LOG_PERROR|LOG_PID,LOG_USER);
    // std::clog << __FUNCTION__ << __LINE__ << std::endl;
    // see /var/log/syslog
    // setlogmask(LOG_UPTO(LOG_DEBUG));
    // syslog(LOG_DEBUG,"%s:%d started",__FUNCTION__,__LINE__);