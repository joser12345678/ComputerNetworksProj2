#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssl_socket.h"

//prints help message
void print_help()
{
    printf("Usage: ./http_downloader -u HTTPS_URL -n NUM_PARTS -o OUTPUT_FILE\n");
    printf("HTTPS_URL       HTTPS URL specifying object to be saved.\n");
    printf("NUM_PARTS       Integer specifying the number of parts the file will be broken into.\n");
    printf("OUTPUT_FILE     File path for output file. Directory must exist, no subdirectory will be created.\n");
}

//parses arguments given num_ars and argv
//
//set pointer to thread_count, url, and output_file_path
//return 0 for no error
//return -1 for argument error
int parse_args(int num_args, char* argv[], 
                    int* thread_count, char* url, char* output_file_path)
{
    //loop through and collect the arguements
    for (int i = 1; i < num_args; i++)
    {
        //if the argument is -u, next item should be treated as the URL
        if(strncmp(argv[i], "-u", 2))
        {
            //TODO: implement url saving
            printf("URL DETECTED\n");
            i++;
        }

        //else if the argument is -n, next item should be treated as number of parts
        //ie thread count
        else if(strncmp(argv[i], "-n", 2))
        {
            //TODO: implement thread_count saving
            printf("NUM_PARTS DETECTED\n");
            i++;
        }

        //else if argument is -o, output file path
        else if(strncmp(argv[i], "-o", 2))
        {
            //TODO: implement output_file_path saving
            printf("OUTPUT_FILE DETECTED\n");
            i++;
        }

        //invalid argument, just return error
        else
            return -1;
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    int* thread_count;
    char* url;
    char* output_file;

    //parse, if err occurs, print help
    int err = parse_args(argc, argv, thread_count, url, output_file);
    if(err == 0)
    {
        int socket;
        int status = open_clientside_tcp_connection(&socket, "80", "www.google.com");
        if(status != 0)
        {
            exit(-1);
        }
        
        init_ssl_ctx();
    }
    else
        print_help();
}