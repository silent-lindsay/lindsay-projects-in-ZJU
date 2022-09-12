#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN64) || defined(_WIN32)

#include <winsock2.h> 
#include <ws2tcpip.h>

#elif defined(__linux__) || defined(__linux)

#include <sys/socket.h>
#include <netdb.h>

#endif

#pragma comment(lib, "ws2_32.lib")

#define SENDBUF_SIZE 128
#define RECVBUF_SIZE 128
#define MAXNR_CLIENT 128

#define CHECK(err, string, function) \
	do { \
		if(err) { \
			printf("Error: " string " [with code %d]", WSAGetLastError()); \
			function \
			exit(-1); \
		} \
	} while(0)
	
DWORD WINAPI server_response_thread(void *pClientSocket){
	
	SOCKET clientSocket = *(SOCKET*)(pClientSocket);
	
	int err = 0;
	int result = 0, result2 = 0;
	char sendbuf[SENDBUF_SIZE];
	char recvbuf[RECVBUF_SIZE];
	
	// Receive & Echo Loop
	while(1) {
		memset(recvbuf, 0, RECVBUF_SIZE);
		result = recv(clientSocket, recvbuf, RECVBUF_SIZE, 0);
		
		// Check Message
		if(result > 0){
			
			printf("<CLIENT MESSAGE (%d Btyes)> %s\n", result, recvbuf);
			
			// Echo
			sprintf(sendbuf, "Message Received. %d Btyes in Total.", result);
			result2 = send(clientSocket, sendbuf, strlen(sendbuf), 0);
			err = (result2 == SOCKET_ERROR);
			CHECK(err, "echo failed!", {
				closesocket(clientSocket);
				WSACleanup();
			});
			
		} else if (result == 0){
			break;
		} else {
			CHECK(1, "receive failed!", {
				closesocket(clientSocket);
				WSACleanup();
			});
		}
	};
	
	// Stop Sending
	err = shutdown(clientSocket, SD_SEND);
	CHECK(err == SOCKET_ERROR, "shutdown failed!", {
		closesocket(clientSocket);
		WSACleanup();
	});
	
	// Cleanup
	closesocket(clientSocket);
	return 0;
}

int server_main(int argc, char** argv) {
	
	// For Error Detection
	int err = 0;
	
	// Initialize
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2,2), &wsaData);
	CHECK(err, "winsock failed!", {} );
	
	// Service Information
	char port[] = "2191";
	
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;
	hint.ai_flags = AI_PASSIVE;
	
	struct addrinfo *result_ai = NULL;
	
	// Get Service Address
	err = getaddrinfo(NULL, port, &hint, &result_ai);
	CHECK(err, "getaddrinfo failed!", {
		WSACleanup();
	});
	
	// Setup Listen Socket
	SOCKET listenSocket = socket(result_ai->ai_family, result_ai->ai_socktype, result_ai->ai_protocol);
	SOCKET clientSocket = INVALID_SOCKET;
	
	err = (listenSocket == INVALID_SOCKET);
	CHECK(err, "socket failed!", {
		freeaddrinfo(result_ai);
		WSACleanup();
	});
	
	// Bind Listen Socket
	err = bind(listenSocket, result_ai->ai_addr, result_ai->ai_addrlen);
	freeaddrinfo(result_ai);
	CHECK(err == SOCKET_ERROR, "bind failed!", {
		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;
		WSACleanup();
	});
	
	// Start Listening
	err = listen(listenSocket, SOMAXCONN);
	CHECK(err, "listen failed!", {
		closesocket(listenSocket);
		WSACleanup();
	});
	
	printf("Server Launched.\n");
	
	// work for multithreads waiting
	HANDLE respThreads[MAXNR_CLIENT];
	int nr_client = 0;
	
	// Wait & Accept Client Conncetion
	while(1){
		clientSocket = accept(listenSocket, NULL, NULL);
		err = (clientSocket == INVALID_SOCKET);
		CHECK(err, "accept failed!", {
			closesocket(listenSocket);
			WSACleanup();
		});
		
		// Create Response Thread
		HANDLE respThread = CreateThread(NULL, 0, server_response_thread, &clientSocket, 0, NULL);
		respThreads[nr_client++] = respThread;
	}
	
	WaitForMultipleObjects(nr_client, respThreads, 1, 5000);
	
	closesocket(listenSocket);
	WSACleanup();
	
	return 0;
}
