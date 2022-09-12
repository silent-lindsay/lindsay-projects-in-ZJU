#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h> //pthread_t , pthread_attr_t and so on.
#include <stdio.h>
#include <netinet/in.h> //structure sockaddr_in
#include <arpa/inet.h>  //Func : htonl; htons; ntohl; ntohs
#include <assert.h>     //Func :assert
#include <string.h>     //Func :memset
#include <unistd.h>     //Func :close,write,read

#define SERVERPORT 5609
#define BUFFERSIZE 4096


static void Data_Receive(void *sock_fd); 
void Command() ;

int main() {

    char *data_send = (char*)malloc(sizeof(char)*BUFFERSIZE) ;
    memset(data_send, 0, BUFFERSIZE);

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //ipv4,TCP
    struct sockaddr_in s_addr_in ;

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, (void *)(&Data_Receive), (void *)(&sockfd)) == -1) {
        fprintf(stderr, "pthread_create error\n");
        printf("Cannot communicate with other clients.\n");
        exit(1);
    }

    // initial attrs of structure sockaddr.
    memset(&s_addr_in, 0, sizeof(s_addr_in));
    s_addr_in.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip of self computer
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_port = htons(SERVERPORT);

    while ( connect(sockfd, (struct sockaddr *)(&s_addr_in), sizeof(s_addr_in) ) == -1 ) 
        printf( "Connection error! \n");
    printf("Connection success!! Server's port: %d\n",SERVERPORT);

    while (1) {
        sleep(1) ; 
        Command() ; // print introductions
        int choice = 0 ;
        scanf("%d", &choice);
        if (choice == 3) {
            printf("Bye bye!\n") ; 
            sprintf(data_send,"quit") ;
            if ( send(sockfd, data_send, strlen(data_send), 0) == -1 ) 
                fprintf(stderr, "write error\n");
            break ;
        } else if (choice == 2) {
            int client_number;
            printf("Input the client's number you want to send message:\n>" );
            scanf("%d", &client_number );
            printf("Input something you want to send to client %d:\n>",client_number );
            getchar();
            char *temp = (char*)malloc( sizeof(char)*(BUFFERSIZE - 2) ) ;
            memset(temp,0,sizeof(temp)) ;
            gets(temp);
            sprintf(data_send, "%d-%s", client_number, temp);
            free(temp) ;
        } else {
            int temp_choice;
            printf("Which information of the server do you want?\n");
            printf("1 for server time, 2 for server name, 3 for client list\n");
            printf("Input your choice: >");
            scanf("%d", &temp_choice);

            if (temp_choice == 1)
                sprintf(data_send, "get-time");
            else if (temp_choice == 2)
                sprintf(data_send, "get-name");
            else
                sprintf(data_send, "get-list");
        }
        printf("Sending: %s\n",data_send) ;
        if ( send(sockfd, data_send, strlen(data_send), 0) == -1 ) {
            fprintf(stderr, "write error\n");
            break ;
        }
        memset(data_send, 0, sizeof(data_send)) ;
    }
    printf("Connection shut!\n") ;
    free(data_send) ;
    assert(shutdown(sockfd, SHUT_WR) != -1);
    
    return 0;
}

static void Data_Receive(void *sock_fd) {
    char *data_recv = (char*)malloc(sizeof(char)*BUFFERSIZE );
    int i_recvBytes, fd = *((int *)sock_fd);

    while (1) {
        memset(data_recv, 0, BUFFERSIZE) ;
        i_recvBytes = recv(fd, data_recv, sizeof(data_recv), 0) ;
        if ( strcmp(data_recv, ">>Connection shut") == 0 || i_recvBytes == -1 )
            break;
        else if (data_recv[0] == '#')
            printf("\n%s", data_recv);
        else
            printf("%s", data_recv);
    }
    free(data_recv) ;
    printf("Connection shut!\n") ;
    assert( shutdown(sock_fd, SHUT_WR) != -1 );
    exit(0);
}

void Command() {
    printf("Here are the choices you have:\n");
    printf("1)\tGet information about the server.\n");
    printf("2)\tSend information about the server.\n");
    printf("3)\tQuit the client program.\n");
    printf("Now please input the number of command: \n>");
}
