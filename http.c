#include "http.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//given the initial struct and url string, initialize the connection struct
void init_connection(struct http_connection_info* info, char* url)
{
    //init info in memory
    info = malloc(sizeof(struct http_connection_info));

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


}

