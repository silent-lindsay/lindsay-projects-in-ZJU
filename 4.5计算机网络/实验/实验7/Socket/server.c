#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h> //pthread_t , pthread_attr_t and so on.
#include <stdio.h>
#include <netinet/in.h> //structure sockaddr_in
#include <arpa/inet.h>	//Func : htonl; htons; ntohl; ntohs
#include <assert.h>		//Func :assert
#include <string.h>		//Func :memset
#include <unistd.h>		//Func :close,write,read
#include <time.h>
#include <sys/syscall.h>

#define PORT 5609
#define MAXCONNECTIONS 10
#define BUFFERSIZE 4096
#define TIMEOUT 20
/*
    struct in_addr{
    in_addr_t s_addr ;
    }
        
    struct sockaddr_in {
    sa_family_t sin_family ;
    uint16_t sin_port ;
    struct in_addr sin_addr ;
    char sin_zero[8] ;
    } ;
 */
struct client
{
	int fd;
	char ip[20];
	int port;
	pthread_t tid;
	int number;
};
struct client client_list[MAXCONNECTIONS];
int connection_number = 0;

int RecordConnection(pthread_t tid, int fd)  ;
static void ConnectionRun(void *client ) ;

int main(){
    /* Initialization */
    int ser_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP) ;
    struct sockaddr_in ser_add ;
    memset(&ser_add,0,sizeof(ser_add)) ;
    ser_add.sin_family = AF_INET ;
    ser_add.sin_addr.s_addr = inet_addr("127.0.0.1") ;//self ip
    ser_add.sin_port = htons(PORT) ;

    /* bind and listen */
    // bind(ser_socket , (struct sockaddr*)&ser_add , sizeof(ser_add) ) ;
    // listen ( ser_socket , TIMEOUT ) ;
    while (  bind(ser_socket , (struct sockaddr*)&ser_add , sizeof(ser_add) ) == -1 ) 
        printf("Binding Error! Bind again...\n") ;
        ;
    while ( listen ( ser_socket , TIMEOUT ) == -1 ) 
        printf( "Listening Error! Listen again...\n") ;
        ;

    struct sockaddr_in client_add ;
    socklen_t client_addr_size = sizeof ( client_add ) ;
    /* connect */
    while ( 1 ) {
        printf("Waiting for client's connection...\n") ;
        int cli_socket = accept(ser_socket,(struct sockaddr*)&client_add,&client_addr_size ) ;
        if ( cli_socket == -1 ) 
            continue ;
        pthread_t pth_id ;
        /* create a new thread for a new connection */
        if ( pthread_create(&pth_id, NULL, (void *)(&ConnectionRun), (void *)(&cli_socket)) == -1 ) {
            printf("Thread creating error!\n") ;
            break ;
        }        
        printf("A new connection is created!\n") ;
    }

    /* clear */
	assert( shutdown(ser_socket, SHUT_WR) != -1 ) ; //shut down all connections
	printf("Server shuts down\n");
	return 0;
}

static void ConnectionRun(void *client ) {
    int id_sock = *((int*)client) ;
    char *Reciving = (char*)malloc(sizeof(char)*BUFFERSIZE) , 
            *Sending = (char*)malloc(sizeof(char)*BUFFERSIZE) ;
    memset(Sending,0,sizeof(Sending)) ;
    memset(Reciving,0,sizeof(Reciving)) ;

    /* record */
    int current = RecordConnection(pthread_self(), id_sock ) ;
    
    /* response request */
    while ( 1 ) {
        memset(Reciving , 0 , BUFFERSIZE ) ;
        memset(Sending , 0 , BUFFERSIZE ) ;
        printf("Waiting for request...\n" ) ;
        if ( recv(id_sock, Reciving, BUFFERSIZE, 0) == -1 ) 
            break ;
        printf("Request from client %d: %s\n" ,current , Reciving) ;
        if ( strcmp(Reciving , "quit" ) == 0 ) {
            sprintf(Sending , ">>Connection shut") ;
            send(id_sock,  Sending , strlen(Sending) ,0 ) ;
            break ;
        }
        if ( strcmp(Reciving,"get-time" ) == 0 ) {
            time_t now;
            struct tm *tm_now;
            time(&now);
            tm_now = localtime(&now);
            sprintf(Sending, ">>Date&time: %d-%d-%d %d:%d:%d\n",
                tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        } else if ( strcmp(Reciving,"get-name" ) == 0 ) {
            char host_name[1024] = {0};
            gethostname(host_name, 1024);
            sprintf(Sending, ">>Host server's name: %s\n", host_name);
        } else if ( strcmp(Reciving,"get-list" ) == 0 ) {
            sprintf(Sending, "tid             ip              port     number\n");
            for (int i = 0; i < connection_number; i++) {
                char temp[50] = {0};
                sprintf(temp, "%u\t%s\t%d\t%d\n", client_list[i].tid, client_list[i].ip, client_list[i].port, client_list[i].number);
                sprintf(Sending,"%s%s",Sending, temp);
            }
            sprintf(Sending,"%s>>And your client's number is: %d\n",Sending ,current) ;
        } else {
            int target = Reciving[0]-'0' ;
            char *message = (char*) malloc(sizeof(char)*BUFFERSIZE) ;
            sprintf(message, "#Client %d: %s\n", current, Reciving+2);
			if (send(client_list[target].fd, message, strlen(message), 0) == -1)
				sprintf(Sending, ">>Sending error!\n") ;
			else
				sprintf(Sending, ">>Sending success...\n") ;

            printf("Client %d sends a message to client %d: %s\n" , current, target, Reciving+2 ) ;
        }

        if ( send( id_sock , Sending, strlen(Sending) , 0) == -1 ) {
            printf("Sending error!\n") ;
            break ;
        }
        printf("Message to %d:\n%s",current,Sending) ;
    }

	/* close */
    printf("Client %d's connection terminated\n",current) ;
	close(id_sock);			//close a file descriptor.
	pthread_exit(NULL); //terminate calling thread!
     free(Sending) ;
     free(RecordConnection) ;
}

int RecordConnection(pthread_t tid, int fd) {
	client_list[connection_number].fd = fd;
	client_list[connection_number].tid = tid;
	client_list[connection_number].number = connection_number;

	struct sockaddr local_addr;
	struct sockaddr addr;
	socklen_t len = sizeof(addr);
	if (getsockname(fd, &local_addr, &len) == 0) {
		struct sockaddr_in *sin = (struct sockaddr_in *)(&local_addr);

		char addr_buffer[INET_ADDRSTRLEN];
		void *tmp = &(sin->sin_addr);
		if (inet_ntop(AF_INET, tmp, addr_buffer, INET_ADDRSTRLEN) != NULL) {
			strcpy(client_list[connection_number].ip, addr_buffer);
			client_list[connection_number].port = sin->sin_port;
		}
	}
    int ret = connection_number ;
	connection_number++;
    return ret ;
}