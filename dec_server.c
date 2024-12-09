#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static char cryptArray[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

int find_char_index(char target) {
    for (int i = 0; cryptArray[i] != '\0'; i++) {
        if (cryptArray[i] == target) {
            return i; // Found the character at index i
        }
    }
    return -1; // Character not found
}

char* decipher_text(char* ciphertext, char* key){
    // create a plaintext buffer equal in length to the ciphertext
    int text_length = strlen(ciphertext);
    char* plaintext = malloc(sizeof(char) * (text_length + 1));

    /*
    for each character in ciphertext,
        subtract the int values of the corresponding key character from the ciphertect character
        generate ciphertext using this encrypted int as the index
    */ 
    for(int i = 0; i < text_length; i++){
        int decrypt_int = find_char_index(ciphertext[i]) - find_char_index(key[i]) + 27;

        plaintext[i] = cryptArray[decrypt_int % 27];
    }
    plaintext[text_length] = '#';

    // // write plaintext\n to stdout
    // printf("%s\n", plaintext);

    // // free allocated memory
    // free(plaintext);

    return plaintext;
}

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a localhost client to connect to this server
  address->sin_addr.s_addr = inet_addr("127.0.0.1");
}

int main(int argc, char* argv[]){
    int connectionSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc < 2) { 
        fprintf(stderr,"USAGE: %s port\n", argv[0]); 
        exit(1);
    }

    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    // Associate the socket to the port
    if (bind(listenSocket, 
            (struct sockaddr *)&serverAddress, 
            sizeof(serverAddress)) < 0){
        perror("ERROR on binding");
        exit(1);
    }

    // Start listening for connetions. Allow up to 5 connections to queue up
    listen(listenSocket, 5); 

    // Accept a connection, blocking if one is not available until one connects
    while(1){
        // Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket, 
                    (struct sockaddr *)&clientAddress, 
                    &sizeOfClientInfo); 
        if (connectionSocket < 0){
            perror("ERROR on accept");
            exit(1);
        }


        // This should be a background process....
        // - use a child process to do encryption and decryption
        int childStatus;
        pid_t spawnPid = fork();

        // manage parent and child processes 
        switch(spawnPid){
            case -1:
                perror("failure to fork()\n");
                exit(1);
                break;

            case 0:
                // child
                ;
                char buffer[4096];
                int charsRead = 0, charsWritten = 0;

                // Get the message from the client and display it
                // memset(buffer, '\0', 1024);
                // Read the client's message from the socket
                while(strstr(buffer, "#") == NULL){
                    charsRead += recv(connectionSocket, buffer+charsRead, 1000, 0); 
                    // printf("received\n");

                    if (charsRead == 0){
                        // printf("broken\n");
                        break;
                    }
                    if (charsRead < 0){
                        perror("ERROR reading from socket");
                    }
                }

                // printf("buffer: %s", buffer);

                char* token, *client_name, *ciphertext, *key, *plaintext;
                token = strtok(buffer, "!");
                client_name = token;
                
                if(strcmp(client_name, "dec_client")){
                    charsRead += send(connectionSocket, "wrong client access#", 20, 0); 
                    if (charsRead < 0){
                        perror("ERROR writing to socket");
                    }
                    close(connectionSocket);
                    continue;
                }

                token = strtok(NULL, "!");
                ciphertext = token;
                token = strtok(NULL, "!");
                key = token;


                plaintext = decipher_text(ciphertext, key);

                // Send a Success message back to the client
                while(charsWritten < strlen(plaintext)){
                    charsWritten += send(connectionSocket, plaintext + charsWritten, strlen(plaintext) - charsWritten, 0);
                    if (charsWritten < 0){
                        perror("ERROR writing to socket");
                    }
                }
                // Close the connection socket for this client
                close(connectionSocket);
                free(plaintext);
                exit(0);
                break;

            default:
                // parent
                continue;
                break;
        }

    }

    return 0;
}