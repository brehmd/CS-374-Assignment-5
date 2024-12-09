#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

static char cryptArray[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

int find_char_index(char target) {
    for (int i = 0; cryptArray[i] != '\0'; i++) {
        if (cryptArray[i] == target) {
            return i; // Found the character at index i
        }
    }
    return -1; // Character not found
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname){
 
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname); 
    if (hostInfo == NULL) { 
        fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
        exit(0); 
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, 
            hostInfo->h_addr_list[0],
            hostInfo->h_length);
}

int main(int argc, char *argv[]) {

    int socketFD, portNumber, charsWritten, charsRead;
    charsWritten = 0;
    charsRead = 0;
    struct sockaddr_in serverAddress;
    char buffer[4096];
    // Check usage & args
    if (argc != 4) { 
        fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
        exit(0);
    }


    // get text from file
    FILE *plaintext, *key;
    char text_buffer[2048], key_buffer[2048];

    plaintext = fopen(argv[1], "r");
    if (plaintext == NULL) {
        perror("Error opening plaintext file.");
        exit(1);
    }

    fgets(text_buffer, sizeof(text_buffer), plaintext);
    text_buffer[strcspn(text_buffer, "\n")] = 0;
    fclose(plaintext);

    key = fopen(argv[2], "r");
    if (key == NULL) {
        perror("Error opening key file.");
        exit(1);
    }

    fgets(key_buffer, sizeof(key_buffer), key);
    key_buffer[strcspn(key_buffer, "\n")] = 0;
    fclose(key);

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0){
        perror("CLIENT: ERROR opening socket");
        exit(2);
    }

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        perror("CLIENT: ERROR connecting");
        exit(0);
    }

    // Check that key is at least as long as plaintext/ciphertext
    // Check for bad characters in text

    int textlen = strlen(text_buffer);
    int keylen = strlen(key_buffer);
    
    if (textlen > keylen){
        perror("ERROR: key is too short");
        exit(1);
    }

    for(int i = 0; i < textlen; i++){
        if (find_char_index(text_buffer[i]) == -1){
            perror("ERROR: plaintext contains invalid characters");
            exit(1);
        }
        if (find_char_index(key_buffer[i]) == -1){
            perror("ERROR: key contains invalid characters");
            exit(1);
        }
    }

    sprintf(buffer, "enc_client!%s!%s#", text_buffer, key_buffer);

    // Send message to server
    // Write to the server
    while(charsWritten < strlen(buffer)){
        charsWritten += send(socketFD, buffer + charsWritten, strlen(buffer) - charsWritten, 0);
        if (charsWritten < 0){
            perror("CLIENT: ERROR writing to socket");
            exit(2);
        }
    }


    // Get return message from server
    // Clear out the buffer again for reuse
    memset(buffer, '\0', 4096);
    // Read data from the socket, leaving \0 at end


    while (strstr(buffer, "#") == NULL){
        charsRead += recv(socketFD, buffer+charsRead, 1000, 0);

        if (charsRead == 0){
            break;
        }

        if (charsRead < 0){
            perror("CLIENT: ERROR reading from socket");
            exit(2);
        }
    }

    buffer[strcspn(buffer, "#")] = 0;

    // client terminates if connecting to wrong server (if message gives error)
    if (strcmp(buffer, "wrong client access") == 0){
        fprintf(stderr, "ERROR: could not contact enc_server on port %s\n", argv[3]);
        close(socketFD);
        exit(2);
    }

    printf("%s\n", buffer);

    // Close the socket
    close(socketFD);
    return 0;
}