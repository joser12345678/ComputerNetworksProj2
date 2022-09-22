#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "ssl_socket.h"
#include "http.h"
#include "pthread.h"

//used in protecting file list writes
pthread_mutex_t file_mut;

struct thread_args
{
    size_t low_b;
    size_t high_b;
    char* url;
    char* unit;
    size_t file_num;
    char** file_name;
};

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

//send the initial head request and get the initial HEAD info
void initial_head_request(struct http_connection_info* c1, char* url)
{
    //initialize tcp clientside connection socket
    int socket;
    int status = open_clientside_tcp_connection(&socket, c1->port, c1->hostname);
    if(status != 0)
    {
        printf("TCP socket connection failed.\n");
        exit(-1);
    }

    //set up ssl connection
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
    //if the initial head request was successful and there is an accept ranges header, we can continue
    create_head_request(c1);
    int requestRes = send_request(c1, ssl);
    if(requestRes== -1)
    {
        printf("Response from server wasn't 200 OK or ranges not accepted. HTTP RESPONSE:\n");
        printf("%s\n", c1->response);
        exit(-1);
    }
    //check for a cookie and set it for the connection object
    else if(requestRes == 403)
    {
        get_and_set_cookie(c1);
        create_head_request(c1);
        open_clientside_tcp_connection(&socket, c1->port, c1->hostname);
        SSL_CTX* ssl_context = init_ssl_ctx();
        SSL* ssl = ssl_new(ssl_context);
        status = ssl_set_SNI(ssl, c1->hostname);
        ssl_fd(&socket, ssl);
        SSL_connect(ssl);
        //printf("%s\n", c1->request);
        send_request(c1, ssl);
        //printf("%s\n", c1->response);
    }

    //parse request to get the content length
    if(get_content_length(c1) == -1)
    {
        printf("Error: content length not found");
        exit(-1);
    }

    //test the frees and closes
    ssl_session_free(ssl);
    ssl_socket_close(&socket);
    ssl_context_free(ssl_context);
}

//create a sub request and fulfill it using the bounds specified
//in the parameters
void* sub_req(void* args)
{
    struct thread_args* args1 = (struct thread_args*) args;
    //printf("%ld - %ld\n", args1->low_b, args1->high_b);
    struct http_connection_info conn;
    init_connection(&conn, args1->url);
    conn.low_range = args1->low_b;
    conn.high_range = args1->high_b;
    conn.content_unit = args1->unit;

    //initialize tcp clientside connection socket
    int socket;
    int status = open_clientside_tcp_connection(&socket, conn.port, conn.hostname);
    if(status != 0)
    {
        printf("TCP socket connection failed.\n");
        exit(-1);
    }

    //set up ssl connection
    SSL_CTX* ssl_context = init_ssl_ctx();
    SSL* ssl = ssl_new(ssl_context);
    status = ssl_set_SNI(ssl, conn.hostname);
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

    //create get request and send it
    create_get_request(&conn);
    if(send_request(&conn, ssl) == -1)
    {
        printf("Response from server wasn't 200 OK. HTTP RESPONSE:\n");
        printf("%s\n", conn.response);
        exit(-1);
    }
    
    char* s = strstr(conn.response, "\r\n\r\n");
    pthread_mutex_lock(&file_mut);
    *args1->file_name = malloc(11);
    sprintf(*args1->file_name, "./part_%ld", args1->file_num);
    FILE* file = fopen(*args1->file_name,"wb");
    pthread_mutex_unlock(&file_mut);
    fwrite(s + 4, conn.content_length, 1, file);
    fclose(file);

    //free ssl and connection pointers
    ssl_session_free(ssl);
    ssl_socket_close(&socket);
    ssl_context_free(ssl_context);
    //this is done to prevent premature freeing of info from 
    //the head connection
    free(conn.request);
    free(conn.response);
    free(conn.path);
    free(conn.hostname);
    free(conn.port);
    pthread_exit(NULL);

}

//writes all input files to output file
void write_to_output(char** file_list, char* output_file, size_t list_length)
{
    FILE* output = fopen(output_file,"wb");
    for (size_t i = 0; i < list_length; i++)
    {
        FILE* file = fopen(file_list[i],"rb");
        struct stat sb;
        if (stat(file_list[i], &sb) == -1) 
        {
            perror("stat");
            exit(-1);
        }

        char* buf = malloc(sb.st_size);
        fread(buf, sb.st_size, 1, file);
        fwrite(buf, sb.st_size, 1, output);
        fclose(file);
        free(buf);
        free(file_list[i]);
    }
    fclose(output);
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
        //init mutex
        pthread_mutex_init(&file_mut, NULL);

        struct http_connection_info* c1;
        //init info in memory
        c1 = malloc(sizeof(struct http_connection_info));
        init_connection(c1, url);
        
        //send the initial head request and get the total content length size
        initial_head_request(c1, url);
        //printf("%s\n", c1->response);
        size_t total_content_size = c1->high_range;

        char* filenames[thread_count];
        pthread_t threads[thread_count];
        struct thread_args t1[thread_count];

        //calculate the bounds for each sub request
        size_t thread_content_size = total_content_size / thread_count;
        size_t start_bound = 0;
        for(size_t i = 1; i < thread_count + 1; i++)
        {
            t1[i - 1].file_name = &filenames[i - 1];
            t1[i - 1].file_num = i;
            t1[i - 1].url = url;
            t1[i - 1].unit = c1->content_unit;
            if( i == thread_count)
            {
                //sub_req(start_bound, total_content_size, url, c1->content_unit, i, &filenames[i - 1]);
                t1[i - 1].low_b = start_bound;
                t1[i - 1].high_b = total_content_size;
            }
            else
            {
                //sub_req(start_bound, thread_content_size * i, url, c1->content_unit, i, &filenames[i - 1]);
                t1[i - 1].low_b = start_bound;
                t1[i - 1].high_b = thread_content_size * i;
                start_bound = (thread_content_size * i);
            }
            pthread_create(&threads[i - 1], NULL, sub_req, &t1[i - 1]);
            //sub_req(&t1);
            
        }

        for (size_t i = 0; i < thread_count; i++)
            pthread_join(threads[i], NULL);
        

        write_to_output(filenames, output_file, thread_count);

        free(url);
        free(output_file);
        free_connection(c1);
        pthread_mutex_destroy(&file_mut);
    }
    else
        print_help();

}