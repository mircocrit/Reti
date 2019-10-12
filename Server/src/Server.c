#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h> 			// for atoi()
#define BUFFERSIZE 512
#define PROTOPORT 5193 		// default protocol port number
#define QLEN 6 			// Massimo n di richeste
#include "work.h"
////////////////////////////////////////////

int main (int argc, char *argv[]) {
	int port;
	if (argc > 1) {					//riceve n di porta eventualmente
		port = atoi (argv[1]); 			// if argument specified convert argument to binary
	}
	else
		port = PROTOPORT; 			// use default port number
	if (port < 0) {
			printf("bad port number %s \n", argv[1]);
			return 0;
		}

	#if defined WIN32			 // initialize Winsock
	WSADATA wsaData;			//setup di oggetto per windows, da pulire
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		ErrorHandler("Error at WSAStartup()\n");
		return 0;
	}
	#endif

	//////////CREAZIONE DELLA SOCKET/////////////////////
	int MySocket;
	MySocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MySocket < 0) {
		ErrorHandler("socket creation failed.\n");
		ClearWinSock();
		return -1;
	}

	////////////ASSEGNAZIONE DI UN INDIRIZZO ALLA SOCKET////////////////////////
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad)); 							// ensures that extra bytes contain 0
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr =	inet_addr("127.0.0.1");		//localhost
	sad.sin_port = htons(port); 			/* converts values between the host and network byte order. Specifically, htons() converts 16-bit quantities from host byte order to network byte order. */
						//assegno n di porta

	if (bind(MySocket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		ErrorHandler("bind() failed.\n");
		closesocket(MySocket);
		ClearWinSock();
		return -1;
	}



	///////////////SETTAGGIO DELLA SOCKET ALL'ASCOLTO///////////////////////////////
	if (listen (MySocket, QLEN) < 0) {
		ErrorHandler("listen() failed.\n");
		closesocket(MySocket);
		ClearWinSock();
		return -1;
	}

	//////////////////ACCETTARE UNA NUOVA CONNESSIONE/////////////////////////
	struct sockaddr_in 	cad; 			// structure for the client address
	int clientSocket; 				// socket descriptor for the client
	int clientLen;					 // the size of the client address
	printf("Waiting for a client to connect");

	while (1) {					/* oppure for (;;) */
		clientLen = sizeof(cad); 				// set the size of the client address
		printf(".");
		///////////
		send (clientSocket, "connessione accettata!", strlen("connessione accettata!"), 0);	//<--invio messaggio mio
		//////

		if ((clientSocket = accept (MySocket, (struct sockaddr *) & cad, &clientLen)) < 0) {
			ErrorHandler("accept() failed.\n");

			// CHIUSURA DELLA CONNESSIONE
			closesocket( MySocket);
			ClearWinSock();
			return 0;

		}
	printf("Handling client %s\n", inet_ntoa(cad.sin_addr));
	}


	// INVIARE DATI AL CLIENT

	/*if (send (Csocket, inputString, stringLen, 0) != stringLen) {		//invio al server
		ErrorHandler("send() sent a different number of bytes than expected");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}*/


	/////////////////RICEVERE DATI DAL CLIENT/////////////////////
	int stringLen= 10;
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[BUFFERSIZE]; 			// buffer for data from the server
	printf("Received: "); 			// Setup to print the echoed string

	while (totalBytesRcvd < stringLen) {						//flusso di byte, socketstream lettura byte per… anziche fare receive(csocket, *socket, buffersize…)
		if ((bytesRcvd = recv(clientSocket, buf, BUFFERSIZE - 1, 0)) <= 0) {
			ErrorHandler("recv() failed or connection closed prematurely");
			closesocket(clientSocket);
			ClearWinSock();
			return -1;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		buf[bytesRcvd] = '\0'; // Add \0 so printf knows where to stop
		printf("%s", buf); // Print the echo buffer
		printf("Fine");
	}


}


