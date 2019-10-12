#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 512
#define PROTOPORT 27015			 // Numero di porta di default	MIA PORTA
#include "work.h"
///////////////////////////////////////////////

int main(void) {
	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return -1;
	}
	#endif

	//////// CREAZIONE DELLA SOCKET CLIENT/////////////
	int Csocket;
	Csocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Csocket < 0) {
		ErrorHandler("socket creation failed.\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}
	/////////////////////////////////////////

	////////COSTRUZIONE DELL’INDIRIZZO DEL SERVER//////////
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));				//indirizzo l server, devo chiedere di connettermi alla mia socket
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1"); 		// IP del server
	sad.sin_port = htons(5193); 					//porta Server


	////////////CONNESSIONE AL SERVER///////////////////////
	if (connect (Csocket, (struct sockaddr *)&sad, sizeof(sad)) < 0){		//sockaddr lego al server
		ErrorHandler( "Failed to connect.\n" );
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	char* inputString = "Mirco Sipone"; 			// Stringa da inviare
	int stringLen = strlen(inputString); 	// Determina la lunghezza
	////////////////////////////////////////////////////////////////////////////////////////77


	// INVIARE DATI AL SERVER (OK)
	//
	if (send (Csocket, inputString, stringLen, 0) != stringLen) {		//invio al server
		ErrorHandler("send() sent a different number of bytes than expected");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}


	// RICEVERE DATI DAL SERVER (OK)
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[BUFFERSIZE]; 			// buffer for data from the server
	printf("Received from server: "); 			// Setup to print the echoed string

	while (totalBytesRcvd < stringLen) {						//flusso di byte, socketstream lettura byte per… anziche fare receive(csocket, *socket, buffersize…)
		if ((bytesRcvd = recv(Csocket, buf, BUFFERSIZE-1, 0)) <= 0) {
			ErrorHandler("recv() failed or connection closed prematurely");
			closesocket(Csocket);
			ClearWinSock();
			return -1;
		}
		totalBytesRcvd += bytesRcvd; // Keep tally of total bytes
		buf[bytesRcvd] = '\0'; // Add \0 so printf knows where to stop
		printf("...........%s........", buf); // Print the echo buffer
	}


	send (Csocket, "buongiorno!", strlen("buongiorno!"), 0);

	// CHIUSURA DELLA CONNESSIONE
	closesocket(Csocket);
	ClearWinSock();
	printf("\n"); // Print a final linefeed

	system("pause");
	//return(0);
}

