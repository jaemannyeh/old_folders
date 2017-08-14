
#include "yawrz.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <zmq.hpp> 

//using std::cout;
//using std::endl;
//using namespace std;

DEFINE_string(hostname,"127.0.0.1","host name");
DEFINE_int32(hostport,1234,"port");

static int cli_request_response(int fd,char *request) {
    uint32_t len = strlen(request); // do not separate initialization from declaration.
    uint32_t len_in_network_byte_order = htonl(len);
    int nsent;
    char response[65535];
    
    // Format is <len>data
    nsent = send(fd, &len_in_network_byte_order, sizeof(len_in_network_byte_order), 0);
    if (nsent != sizeof(len_in_network_byte_order)) {
        LOG(INFO) << strerror(errno);
        return -1;
    }

    nsent = send(fd, request, len, 0); // On error, -1 is returned
    if (nsent != (int)len) {
        LOG(INFO) << strerror(errno);
        return -1;
    }

    int nread = recv(fd,&len_in_network_byte_order,sizeof(len_in_network_byte_order),0);
    if (nread != sizeof(len_in_network_byte_order)) {
        LOG(INFO) << strerror(errno);
        return -1;        
    }
    
    len = ntohl(len_in_network_byte_order);
    if (len<=0) {
        LOG(INFO) << strerror(errno);
        return -1;        
    }
    
    int offset = 0;
    
    while (len > 0) {
        nread = recv(fd,&response[offset],len,0); // On error, -1 is returned
        if (nread < 0) {
            LOG(INFO) << strerror(errno);
            break;
        }
        else if (nread == 0) {
            LOG(INFO) << strerror(errno);
            break;
        }
        offset += nread;
        len -= nread;
    }
    response[offset] = 0;
    std::cout << response << std::endl;

    return 0;
}

static int cli_using_socket()
{
    int sd = socket(AF_INET,SOCK_STREAM,0); // On error, -1 is returned
	if (sd<0) {
        LOG(INFO) << strerror(errno);	    
		return -1;
	}
	
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(FLAGS_hostport);
	inet_pton(AF_INET,FLAGS_hostname.c_str(), &sa.sin_addr);
	
	if (connect(sd,(struct sockaddr *)&sa,sizeof(sa))<0) { // On error, -1 is returned
	    LOG(INFO) << strerror(errno);
		return -1;
	}
	
    while (1) {
        char *line = readline("> "); // do not separate initialization from declaration.

        if (strcmp(line,".quit")==0) {
            break;
        }
        
        if (strlen(line) > 0) {
            cli_request_response(sd,line);            
        }

        free(line);
    }
    
    close(sd);
    sd = -1;
    
    return 0;
}

static int cli_using_zmq() {
    zmq::context_t context(1);
    zmq::socket_t socket(context,ZMQ_REQ);
    
    socket.connect ("tcp://localhost:45060"); // FLAGS_hostname FLAGS_hostport
    
    while (1) {
        char *lahyn = readline("> "); // do not separate initialization from declaration.
        char line[PATH_MAX];
        strncpy(line,lahyn,sizeof(line));
        free(lahyn);
        
        int line_len = strlen(line);
        if (line_len<=0) {
            continue;
        }
        
        if (strcmp(line,".quit")==0) {
            break;
        }

        zmq::message_t request(line_len);
        memcpy(request.data(),line,line_len);
        socket.send(request);
        std::cout << (char *)request.data() << std::endl;
        
        zmq::message_t reply;
        socket.recv(&reply);
        std::cout << (char *)reply.data() << std::endl;
    }

    return 0;
}

static void cli_run()
{
    while (1) {
        char *line = readline("> "); // do not separate initialization from declaration.
        free(line);
    }
    
    return;
    
    cli_using_zmq();
    
    cli_using_socket();
}

namespace yawrz {

int cli_main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler(); // see glog/logging.h
    
    google::SetVersionString(yawrz::version());
    google::SetUsageMessage("");
    google::ParseCommandLineFlags(&argc, &argv, true); //  see logging_unittest.cc

    cli_run();

    return 0;
}

} // namespace yawarz