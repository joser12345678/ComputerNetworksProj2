#include "http.h"



//given the initial struct and url string, initialize the connection struct
void init_connection(struct http_connection_info* info, char* url)
{
    //check to make sure https is the method and set the port
    //set start index for getting resource path and hostname
    char *start_ptr;
    char *s;
    if ((s = strstr(url, "https://")))
    {
        info->port = malloc(3);
        strncpy(info->port, "443", 3);
        start_ptr = s + 8;
    }
    else if ((s = strstr(url, "http://")))
    {
        info->port = malloc(2);
        strncpy(info->port, "80", 2);
        start_ptr = s + 7;
    }
    else
    {
        printf("Unrecognized method, terminating\n");
        exit(0);
    }

    //start looping through string, start by counting until we reach the first '/'
    //i will contain a pointer to the start of the resource path
    char* i;
    for (i = start_ptr; i != start_ptr + strlen(start_ptr); i++)
    {
        if (*i == '/')
            break;
    }

    //get the address difference between i and start of string
    size_t addrDiff = i - start_ptr;

    //set the hostname
    info->hostname = malloc(addrDiff);
    strncpy(info->hostname, start_ptr, addrDiff);
    //set the path
    size_t path_len = strlen(i);
    info->path = malloc(path_len);
    strncpy(info->path, i, path_len);

    //set pointers and length to appropriate values
    info->header_len = -1;      //-1 == default
    info->headers = NULL;
    info->method = NULL;
    info->response = NULL;

}

//creates a head request to send, the request is placed into the 
//request field of the struct
int create_head_request(struct http_connection_info* info)
{
    //allocate 1024 bytes for request body
    info->request = malloc(1024);

    sprintf(info->request, "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", info->path, info->hostname);
}

int send_request(struct http_connection_info* info, SSL* ssl)
{
    
    //if the write was unsuccessful, return -1
    if(!SSL_write(ssl, info->request, strlen(info->request)))
        return -1;

    //if write was successful, allocate response buffer and read from ssl
    //return -1 if read was unsuccessful
    info->response = malloc(1024);
    int bytes;
    if(!(bytes = SSL_read(ssl, info->response, 1024)))
        return -1;

    //make sure null terminated string
    info->response[bytes] = 0;
    return 0;
}

//frees connection struct, SET POINTER TO NULL AFTER CALL
void free_connection(struct http_connection_info* info)
{
    //all will be set
    free(info->hostname);
    free(info->path);
    free(info->port);

    if(info->headers != NULL)
        free(info->headers);
    if(info->method != NULL)
        free(info->method);
    if(info->request != NULL)
        free(info->request);
    if(info->response != NULL)
        free(info->response);

    free(info);
}