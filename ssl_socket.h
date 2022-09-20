//Header file for the ssl_socket implementation. Will contain functions for creating an ssl wrapped socket.
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

//SSL socket 
//This contains everything to use a tcp socket with openssl

int open_clientside_tcp_connection(int* tcp_socket, char* port_string, char* url); // use socket API -- this is mandatory!
SSL_CTX* init_ssl_ctx(); // use OpenSSL library to initialize TLS context
SSL* ssl_new(SSL_CTX* ssl_context); // new TLS session
int ssl_set_SNI(SSL* ssl_connection, char* name);  // sets the TLS SNI
void ssl_fd(int* socket, SSL* ssl_connection); // wrap the TCP socket descriptor into a TLS session


void ssl_session_free(SSL* ssl); // close TLS session
void ssl_context_free(SSL_CTX* ssl_context); // close SSL context
int ssl_socket_close(int* tcp_socket); // close TCP socket
