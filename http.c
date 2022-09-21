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
    info->request = NULL;
    info->response = NULL;
    info->high_range = 0;
    info->low_range = 0;

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

    //check the http return code
    char* s;
    if ((s = strstr(info->response, "200 OK")))
        return 0;
    //else there was an error, print the error code and exit
    else
        return -1;
    
}

//get the content length from the response section of info
int get_content_length(struct http_connection_info* info)
{
    //get pointer to the content length header
    char* s = strstr(info->response, "Content-Length");
    char* end_of_line = strstr(s, "\r\n");

    //if the content length header and end of line wasn't found, we fail
    if(s == NULL || end_of_line == NULL)
        return -1;

    s = s + 16;
    char num_ptr[(end_of_line - s)];
    strncpy(num_ptr, s, (end_of_line - s));
    info->high_range = atoi(num_ptr);

    return 0;
}

//frees connection struct, SET POINTER TO NULL AFTER CALL
void free_connection(struct http_connection_info* info)
{
    //all will be set
    free(info->hostname);
    free(info->path);
    free(info->port);

    if(info->request != NULL)
        free(info->request);
    if(info->response != NULL)
        free(info->response);

    free(info);
}