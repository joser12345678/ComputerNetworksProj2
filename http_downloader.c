#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ssl_socket.h"
#include "http.h"

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
                    int* thread_count, char** url, char** output_file_path)
{
    int paramsSet = 0;

    //loop through and collect the arguements
    for (int i = 1; i < num_args; i++)
    {
        //if the argument is -u, next item should be treated as the URL
        if(!strncmp(argv[i], "-u", 2))
        {
            size_t url_size = strlen(argv[i+1]);
            *(url) = malloc(url_size + 1);
            strncpy(*url, argv[i+1], url_size);
            paramsSet++;
            i++;
        }

        //else if the argument is -n, next item should be treated as number of parts
        //ie thread count
        else if(!strncmp(argv[i], "-n", 2))
        {
            if(i + 1 >= num_args)
                return -1;

            size_t thread_count_size = strlen(argv[i+1]);

            //check if the parameter is a number
            for (int j = 0; j < thread_count_size; j++)
                if (!isdigit(argv[i+1][j]))
                    return -1;

            //convert
            *thread_count = atoi(argv[i+1]);
            paramsSet++;
            i++;
        }

        //else if argument is -o, output file path
        else if(!strncmp(argv[i], "-o", 2))
        {
            if(i + 1 >= num_args)
                return -1;

            size_t file_length = strlen(argv[i+1]);
            *(output_file_path) = malloc(file_length + 1);
            strncpy(*output_file_path, argv[i+1], file_length);
            paramsSet++;
            i++;
        }

        //invalid argument, just return error
        else
            return -1;
    }

    if (paramsSet == 3)
        return 0;
    else
        return -1;
}

int main(int argc, char* argv[])
{
    int thread_count;
    char* url;
    char* output_file;

    //parse, if err occurs, print help
    int err = parse_args(argc, argv, &thread_count, &url, &output_file);
    //printf("%s\n", url);
    if(err == 0)
    {
        struct http_connection_info* c1;
        //init info in memory
        c1 = malloc(sizeof(struct http_connection_info));
        init_connection(c1, url);
        int socket;
        int status = open_clientside_tcp_connection(&socket, c1->port, c1->hostname);
        if(status != 0)
        {
            printf("TCP socket connection failed.\n");
            exit(-1);
        }

        SSL_CTX* ssl_context = init_ssl_ctx();
        SSL* ssl = ssl_new(ssl_context);
        status = ssl_set_SNI(ssl, c1->hostname);
        if (status !=0)
        {
            printf("SSL SNI setting failed\n");
            exit(-1);
        }
        ssl_fd(&socket, ssl);

        if(SSL_connect(ssl) == -1)
        {
            printf("SSL connection failed\n");
            exit(-1);
        }

        //create head and send it
        create_head_request(c1);
        send_request(c1, ssl);
        printf("%s\n", c1->response);
        free_connection(c1);

        //test the frees and closes
        ssl_session_free(ssl);
        ssl_socket_close(&socket);
        ssl_context_free(ssl_context);
        free(url);
        free(output_file);
    }
    else
        print_help();

}