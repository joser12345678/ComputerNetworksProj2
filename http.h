

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
    char* response; //pointer to the string containing the response
};

//parse input info and create initial connection info
void init_connection(struct http_connection_info* info, char* url);

//send a head request and place the info in response
int send_head(struct http_connection_info* info);