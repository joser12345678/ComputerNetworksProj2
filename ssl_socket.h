//Header file for the ssl_socket implementation. Will contain functions for creating an ssl wrapped socket.
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int open_clientside_tcp_connection(); // use socket API -- this is mandatory!
void init_ssl_ctx(); // use OpenSSL library to initialize TLS context
void ssl_new(); // new TLS session
void ssl_set_tlsext_host_name();  // sets the TLS SNI
void ssl_set_fd(); // wrap the TCP socket descriptor into a TLS session


// Send and receive HTTP/1.1 requests/responses here, using the TLS session
void ssl_free(); // close TLS session
void ssl_CTX_free(); // close TLS context
void ssl_close(); // close TCP socket
