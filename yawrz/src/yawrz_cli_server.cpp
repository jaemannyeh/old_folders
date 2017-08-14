#include "yawrz.h"

#include <zmq.hpp> 

DEFINE_int32(port,45060,"port");

static int command_server_do(char *request,std::ostringstream& sout) {
    yawrz::command_execute(request,sout);
    return 0;
}

static int command_server_using_socket() {
    int sd = socket(AF_INET,SOCK_STREAM,0); // On error, -1 is returned
    if (sd<0) {
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
	    return -1;
    }

    int optval = 1;
    // need to resolve FIN_WAIT2, TIME_WAIT, CLOSE_WAIT issues.
	setsockopt(sd, SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval));
		
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
	sa.sin_port = htons(FLAGS_port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
    if (bind(sd,(struct sockaddr *)&sa,sizeof(sa))<0) { // On error, -1 is returned
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
        close(sd);
        sd = -1;
        return -1;
    }
	
    listen(sd,1); //
	
    //struct sockaddr peer;
    //socklen_t peerlen = sizeof(peer);
    //fd = accept(sd,&peer,&peerlen); // On error, -1 is returned
        
    int fd = accept(sd,NULL,NULL); // don't care who it's from // On error, -1 is returned
    if (fd<0) {
        LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
        close(sd);
        sd = -1;
        return -1;
    }
    else {
        // only accept one connection at one time.
        // the new coming connection will be rejected immediately
        // printf("%s:%d\n",__FUNCTION__,__LINE__);
        close(sd);
        sd = -1;
    }

    while (fd != -1) {
        uint32_t len;
        uint32_t len_in_network_byte_order;
        int nread;
            
        // Format is <len>data
        nread = recv(fd,&len_in_network_byte_order,sizeof(len_in_network_byte_order),0);
        if (nread < 0) {
            LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
            close(fd);
            fd = -1;
            break;
        }            
        else if (nread == 0) {
            LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
            close(fd);
            fd = -1;
            break;
        }
        
        len = ntohl(len_in_network_byte_order);
        if (len<=0) {
            LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
            close(fd);
            fd = -1;
            break;
        }
        
        int offset = 0;
        char request[PATH_MAX];

        while (len > 0) {
            nread = recv(fd,&request[offset],len,0); /* On error, -1 is returned */
            if (nread < 0) {
                LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
                close(fd);
                fd = -1;
                break;
            }
            else if (nread == 0) {
                LOG(INFO) << strerror(errno); // syslog(LOG_DEBUG,"%s:%d %s",__FUNCTION__,__LINE__,strerror(errno));
                close(fd);
                fd = -1;
                break;
            }
            offset += nread;
            len -= nread;
        }
        request[offset] = 0;
            
        if (fd != -1) {
            std::ostringstream sout;
            command_server_do(request,sout);
            const char *reply = sout.str().c_str();
                
            len = strlen(reply);
            len_in_network_byte_order = htonl(len);
            send(fd,&len_in_network_byte_order,sizeof(len_in_network_byte_order),0);
            send(fd,reply,len,0);
        }
	}
	
    close(sd);
    sd = -1;

    return 0;
}

static int command_server_using_zmq() {
    zmq::context_t context(1);
    zmq::socket_t socket(context,ZMQ_REP);
    
    socket.bind ("tcp://*:45063"); // FLAGS_port
    
    while (1) {
        zmq::message_t request;
        socket.recv(&request);
        std::cout << (char *)request.data() << std::endl;
        
        std::ostringstream sout;
        command_server_do((char *)request.data(),sout);

        zmq::message_t reply(sout.str().length());
        memcpy (reply.data(),sout.str().c_str(),sout.str().length());
        socket.send (reply);
    }
    
    return 0;
}

namespace yawrz {


int cli_server_using_socket() {
    command_server_using_socket();
    return 0;
}

int cli_server_using_zmq() {
    command_server_using_zmq();
    return 0;
}

} // namespace yawarz
