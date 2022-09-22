#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ssl_socket.h"

//HTTP headerfile

//contains functions to properly format and send http messages

struct http_connection_info
{
    char* hostname;     //pointer to the host string
    char* path;         //resource path string
    char* port;         //pointer to the port string
    char* cookie;

    size_t low_range;    //content length range low bound
    size_t high_range;   //content length range high bound
    size_t content_length;
    char* content;
    char* content_unit;  //unit of content length, usually bytes but here for completeness
    char* request;       //pointer to the request string
    
    size_t read_length;  //amount of characters to read from response
    char* response;      //pointer to the string containing the response
};

//parse input info and create initial connection info
void init_connection(struct http_connection_info* info, char* url);

//frees the connection struct
void free_connection(struct http_connection_info* info);

//parses response to get content length
int get_content_length(struct http_connection_info* info);

//send a head request and place the info in response
int create_head_request(struct http_connection_info* info);

//sends request and recieves reply
int send_request(struct http_connection_info* info, SSL* ssl);

//create get request and place info in the response
int create_get_request(struct http_connection_info* info);

//save response to file
int save_response(struct http_connection_info* info, char* file);

//gets and sets cookie from response
int get_and_set_cookie(struct http_connection_info* info);