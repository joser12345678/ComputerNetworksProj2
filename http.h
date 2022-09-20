#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ssl_socket.h"

//HTTP headerfile

//contains functions to properly format and send http messages

struct http_connection_info
{
    char* hostname;      //pointer to the host string
    char* path;     //resource path string
    char* port;     //pointer to the port string

    char* method;    //method for the http method

    char** headers;  //array of strings that are headers and their values
    int header_len;  //length of the header array
    char* request;   //pointer to the request string
    char* response; //pointer to the string containing the response
};

//parse input info and create initial connection info
void init_connection(struct http_connection_info* info, char* url);

//frees the connection struct
void free_connection(struct http_connection_info* info);

//send a head request and place the info in response
int create_head_request(struct http_connection_info* info);

int send_request(struct http_connection_info* info, SSL* ssl);