//Header file for the ssl_socket implementation. Will contain functions for creating an ssl wrapped socket.
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int open_clientside_tcp_connection(int* tcp_socket, char* port_string, char* url); // use socket API -- this is mandatory!
SSL_CTX* init_ssl_ctx(); // use OpenSSL library to initialize TLS context
SSL* ssl_new(SSL_CTX* ssl_context); // new TLS session
int ssl_set_SNI(SSL* ssl_connection, char* name);  // sets the TLS SNI
void ssl_fd(int* socket, SSL* ssl_connection); // wrap the TCP socket descriptor into a TLS session


// Send and receive HTTP/1.1 requests/responses here, using the TLS session
void ssl_free(); // close TLS session
void ssl_CTX_free(); // close TLS context
void ssl_close(); // close TCP socket
