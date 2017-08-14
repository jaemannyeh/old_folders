

#include "beys.h"

int beys_cli_do_command(int fd,char *cmd) {
    uint32_t len = strlen(cmd);
    uint32_t len_in_network_byte_order = htonl(len);
    char buf[65535];
    
    if (send(fd, &len_in_network_byte_order, sizeof(len_in_network_byte_order), 0) < 0) {
        perror("");
        return 0;
    }
    
    if (send(fd, cmd, len, 0) < 0) {
        perror("");
        return 0;
    }
    
    int nread = recv(fd,&len_in_network_byte_order,sizeof(len_in_network_byte_order),0);
    if (nread != sizeof(len_in_network_byte_order)) {
        perror("");
        return 0;        
    }
    
    len = ntohl(len_in_network_byte_order);
    int offset = 0;
    
    while (len > 0) {
        nread = recv(fd,&buf[offset],len,0);
        if (nread < 0) {
            perror("");
            break;
        }
        else if (nread == 0) {
            perror("");
            break;
        }
        offset += nread;
        len -= nread;
    }
    
    buf[nread] = 0;
    std::cout << buf << std::endl;
    
    return 0;    
}

int beys_cli_main(int argc, char *argv[]) {
    int sd;

	sd = socket(AF_INET,SOCK_STREAM,0);
	if (sd<0) {
		perror("");
	}

    while (1) {
        char *line;
        
        line = readline("> ");

        if (strcmp(line,"quit")==0) {
            break;
        }
        
	    beys_cli_do_command(sd,line);
	    
        free(line);	    
    }
	
	close(sd);
    return 0;
}
    