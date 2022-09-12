#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN64) || defined(_WIN32)

#include <winsock2.h> 
#include <ws2tcpip.h>

#elif defined(__linux__) || defined(__linux)

#include <sys/socket.h>
#include <netdb.h>

#endif

#pragma comment(lib, "ws2_32.lib")

#define HOST_SIZE 32
#define PORT_SIZE 16
#define SENDBUF_SIZE 128
#define RECVBUF_SIZE 128

#define WM_RECEIVED (WM_USER + 0)

#define CHECK(err, string, action) \
	do { \
		if(err) { \
			printf("Error: " string " [with code %d]\n", WSAGetLastError()); \
			action \
			exit(-1); \
		} \
	} while(0)
	
DWORD main_thread_id = -1;
	
DWORD WINAPI client_recv_thread(void *pConnSocket){
	
	SOCKET connSocket = *(SOCKET*)(pConnSocket);
	
	int result = 0;
	char recvbuf[RECVBUF_SIZE];
	
	while(1) {
		
		// Receive Message
		memset(recvbuf, 0, RECVBUF_SIZE);
		result = recv(connSocket, recvbuf, RECVBUF_SIZE, 0);
		
		if(result > 0){
			
			// copy the message (don't forget the '\n' in the end!)
			char *dupbuf = (char*)malloc(sizeof(char) * (result + 1));
			strncpy(dupbuf, recvbuf, result + 1);
			
			// tell main thread to print
			PostThreadMessage(main_thread_id, WM_RECEIVED, result, (LPARAM)dupbuf);
			
		} else if (result == 0) {
			break;
		} else {
			CHECK(1, "receive failed!", {
				closesocket(connSocket);
				WSACleanup();
			});
		}
	}
}

int client_main(int argc, char** argv) {
	
	main_thread_id = GetCurrentThreadId();
	
	// For Error Detection
	int err = 0;
	
	// Initialize
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2,2), &wsaData);
	CHECK(err, "winsock failed!", {} );
	
	// Get Server Information
	char host[HOST_SIZE];
	char port[PORT_SIZE];
	
	printf("Server ipv4(INET) address: ");
	scanf("%s", host);
	printf("Service port: ");
	scanf("%s", port);
	
	// Setup Configuration
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;
	
	struct addrinfo *result_ai = NULL;
	
	// Get Server Address
	err = getaddrinfo(host, port, &hint, &result_ai);
	CHECK(err, "getaddrinfo failed!", {
		WSACleanup();
	});
	
	// Client Connect Socket
	SOCKET connSocket;
	
	// Loop & Connect
	printf("connecting...\n");
	for(struct addrinfo *pinfo = result_ai; pinfo; pinfo = pinfo->ai_next){
		
		// Setup Connect Socket
		connSocket = socket(pinfo->ai_family, pinfo->ai_socktype, pinfo->ai_protocol);
		err = (connSocket == INVALID_SOCKET);
		CHECK(err, "socket failed!", {
			WSACleanup();
		});
		
		// Connect Server
		if(connect(connSocket, pinfo->ai_addr, pinfo->ai_addrlen) == SOCKET_ERROR){
			closesocket(connSocket);
			connSocket = INVALID_SOCKET;
		} 
		
		// check if connection established
		if(connSocket != INVALID_SOCKET) break;
		
	}
	
	// no longer used
	freeaddrinfo(result_ai);
	
	err = (connSocket == INVALID_SOCKET);
	CHECK(err, "socket failed!", {
		WSACleanup();
	});
	
	/*                              */
	/*     Start Communication      */
	/*                              */
	
	int result = 0;
	char sendbuf[SENDBUF_SIZE];
	
	// Create Receiver
	HANDLE recvThread = CreateThread(NULL, 0, client_recv_thread, &connSocket, 0, NULL);
	
	// Send & Receive Loop
	while(1) {
		
		// Input Message
		printf("Input your message: ");
		scanf("\n%[^\n]", sendbuf);
		
		// Disconnect Request
		if(strcmp(sendbuf, "exit") == 0) break;
		
		// Send
		result = send(connSocket, sendbuf, strlen(sendbuf), 0);
		err = (result == SOCKET_ERROR);
		CHECK(err, "send failed!", {
			closesocket(connSocket);
			WSACleanup();
		});
		printf("%d Bytes Sent.\n", result);
		
		// Wait & Print Response (from child thread)
		MSG msg;
		GetMessage(&msg, (HWND)-1, 0, 0);
		if(WM_RECEIVED == msg.message){
			char *recvbuf = (char*)msg.lParam;
			printf("<SERVER MESSAGE (%d Btyes)> %s\n", msg.wParam, recvbuf);
			free(recvbuf);
		}
	}
	
	// Stop Sending
	err = shutdown(connSocket, SD_SEND);
	CHECK(err == SOCKET_ERROR, "shutdown failed!", {
		closesocket(connSocket);
		WSACleanup();
	});
	printf("Disconnect Successfully.");
	
	// Wait for Child Thread
	WaitForSingleObject(recvThread, 5000);
	
	// Cleanup
	closesocket(connSocket);
	WSACleanup();
	
	return 0;
}
