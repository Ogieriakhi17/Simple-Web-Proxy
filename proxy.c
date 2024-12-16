#include "csapp.h"

// Function declarations
void process_client_request(int client_socket);
void dissect_uri(char *uri, char *server_host, char *server_port, char *server_path);

static const char *custom_user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

int main(int argc, char **argv) {
    int server_socket, client_socket;
    char client_host[MAXLINE], client_service[MAXLINE];
    socklen_t client_addr_length;
    struct sockaddr_storage client_address;

    // Validate the command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Set up the listening socket
    server_socket = Open_listenfd(argv[1]);

    while (1) {
        client_addr_length = sizeof(client_address);
        client_socket = Accept(server_socket, (SA *)&client_address, &client_addr_length);
        printf("New connection established.\n");

        // Handle incoming client requests
        process_client_request(client_socket);

        // Close the connection with the client
        Close(client_socket);
    }

    return 0;
}

void process_client_request(int client_socket) {
    rio_t client_buffer, server_buffer;
    char request_line[MAXLINE], method[MAXLINE], uri[MAXLINE], http_version[MAXLINE];
    char server_host[MAXLINE], server_port[10], server_path[MAXLINE];
    char request_headers[MAXLINE];
    int backend_socket;

    // Initialize buffered I/O for the client
    Rio_readinitb(&client_buffer, client_socket);

    // Read and parse the request line from the client
    if (!Rio_readlineb(&client_buffer, request_line, MAXLINE)) {
        return;
    }
    printf("Received request line: %s", request_line);

    sscanf(request_line, "%s %s %s", method, uri, http_version);

    // Only support GET requests
    if (strcasecmp(method, "GET")) {
        printf("Only GET requests are supported.\n");
        return;
    }

    // Extract server details from the URI
    dissect_uri(uri, server_host, server_port, server_path);

    // Open a connection to the target server
    backend_socket = Open_clientfd(server_host, server_port);
    if (backend_socket < 0) {
        printf("Unable to connect to the server: %s:%s\n", server_host, server_port);
        return;
    }

    // Initialize buffered I/O for the server
    Rio_readinitb(&server_buffer, backend_socket);

    // Construct and forward the HTTP request to the server
    sprintf(request_headers, "GET %s HTTP/1.0\r\n", server_path);
    Rio_writen(backend_socket, request_headers, strlen(request_headers));
    sprintf(request_headers, "Host: %s\r\n", server_host);
    Rio_writen(backend_socket, request_headers, strlen(request_headers));
    Rio_writen(backend_socket, custom_user_agent, strlen(custom_user_agent));
    sprintf(request_headers, "Connection: close\r\n");
    Rio_writen(backend_socket, request_headers, strlen(request_headers));
    sprintf(request_headers, "Proxy-Connection: close\r\n\r\n");
    Rio_writen(backend_socket, request_headers, strlen(request_headers));

    // Relay the server's response back to the client
    size_t bytes_read;
    while ((bytes_read = Rio_readlineb(&server_buffer, request_headers, MAXLINE)) != 0) {
        Rio_writen(client_socket, request_headers, bytes_read);
    }

    // Close the connection to the backend server
    Close(backend_socket);
}

void dissect_uri(char *uri, char *server_host, char *server_port, char *server_path) {
    char *separator;

    // Skip the "http://" prefix if present
    if (strstr(uri, "http://")) {
        uri += 7;
    }

    // Separate the host from the path
    separator = strchr(uri, '/');
    if (separator) {
        *separator = '\0';
        strcpy(server_host, uri);
        *separator = '/';
        strcpy(server_path, separator);
    } else {
        strcpy(server_host, uri);
        strcpy(server_path, "/");
    }

    // Extract the port if specified, otherwise default to 80
    separator = strchr(server_host, ':');
    if (separator) {
        *separator = '\0';
        strcpy(server_port, separator + 1);
    } else {
        strcpy(server_port, "80");
    }
}
