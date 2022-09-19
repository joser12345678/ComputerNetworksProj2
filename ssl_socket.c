#include "ssl_socket.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

//opens a tcp client connection and returns an int with the result
// -1 if socket was created incorrectly
// 0 if no error occured
int open_clientside_tcp_connection(int* tcp_socket, char* port_string, char* url)
{
    //target is used to hold the target, so the actual ip and port we are targeting
    struct addrinfo* target, *addr;

    //hints to help getaddrinfo resolve hostname
    //gets an ip4 address, a tcp socket
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    //perform hostname resolution for the url
    int status;
    if ((status = getaddrinfo(url, port_string, &hints, &target)) != 0)
    {
        printf("Error resolving hostname: %s\n", gai_strerror(status));
        return -1;
    }

    //loop through ips and attempt to connect to each, if we get a connection
    //free the addr info object and return
    for (addr = target; addr != NULL; addr = addr->ai_next)
    {
        if ((*tcp_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0)
            continue;
        
        int status;
        if((status = connect(*tcp_socket, addr->ai_addr, addr->ai_addrlen)) == -1)
        {
            close(*tcp_socket);
            continue;
        }
        else
        {
            freeaddrinfo(target);
            return 0;
        }

    }
    
    //tcp socket connection failed
    return -1;

}

//initialize the ssl library, error strings, and algorithms.
//Not sure if error strings is needed but is included.
SSL_CTX* init_ssl_ctx()
{
    const SSL_METHOD *meth = TLS_client_method();
    SSL_load_error_strings();   //MAY NOT BE NECESSARY
    OpenSSL_add_all_algorithms();
    return SSL_CTX_new(meth);
}

//return new ssl connection state
SSL* ssl_new(SSL_CTX* ssl_context)
{
    return SSL_new(ssl_context);
}

//set the SNI for the TLS connection
//returns -1 if there was an error
//0 if none
int ssl_set_SNI(SSL* ssl_connection, char* name)
{
    int err = SSL_set_tlsext_host_name(ssl_connection, name);
    if(err == 0)
        return -1;
    else
        return 0;
}

//attaches socket to the ssl connection
void ssl_fd(int* socket, SSL* ssl_connection)
{
    SSL_set_fd(ssl_connection, *socket);
}