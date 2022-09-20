#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
                    int* thread_count, char** url, char* output_file_path)
{
    if(num_args < 2)
        return -1;

    //loop through and collect the arguements
    for (int i = 1; i < num_args; i++)
    {
        //if the argument is -u, next item should be treated as the URL
        if(!strncmp(argv[i], "-u", 2))
        {
            //TODO: implement url saving
            printf("URL DETECTED\n");
            size_t url_size = strlen(argv[i+1]);
            *(url) = malloc(url_size + 1);
            strncpy(*url, argv[i+1], url_size);
            //*(url)[url_size] = '\0';
            i++;
        }

        //else if the argument is -n, next item should be treated as number of parts
        //ie thread count
        else if(!strncmp(argv[i], "-n", 2))
        {
            //TODO: implement thread_count saving
            printf("NUM_PARTS DETECTED\n");
            i++;
        }

        //else if argument is -o, output file path
        else if(!strncmp(argv[i], "-o", 2))
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
    int err = parse_args(argc, argv, thread_count, &url, output_file);
    //printf("%s\n", url);
    struct http_connection_info* c1;
    init_connection(c1, url);
    if(err == 0)
    {
        int socket;
        int status = open_clientside_tcp_connection(&socket, "443", url);
        if(status != 0)
        {
            printf("TCP socket connection failed.\n");
            exit(-1);
        }

        SSL_CTX* ssl_context = init_ssl_ctx();
        SSL* ssl = ssl_new(ssl_context);
        status = ssl_set_SNI(ssl, url);
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

        X509* cert = SSL_get_peer_certificate(ssl);
        char* line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);

        //test the frees and closes
        ssl_session_free(ssl);
        ssl_socket_close(&socket);
        ssl_context_free(ssl_context);
    }
    else
        print_help();
}