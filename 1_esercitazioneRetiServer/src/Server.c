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
#include <ctype.h>
#define PROTOPORT 5193
#define QLEN 6
#define BUFFERSIZE 512

void errorHandler(char *errorMessage)
{
	printf("%s", errorMessage);
}

void ClearWinSock()
{
	#if defined WIN32
		WSACleanup();
	#endif
}
//invio messaggio a client
int sendToClient(int clientSocket, const char *message)
{
	int lungMessage = strlen(message);
	if(send(clientSocket, message, lungMessage, 0) != lungMessage)
	{
		errorHandler("send() sent a different number of bytes than expected.\n");
		closesocket(clientSocket);
		ClearWinSock();
		return -1;
	}
	return 0;
}

void sendSuccess(int clientSocket, const char *message)
{
	int success;
	success = sendToClient(clientSocket, message);
}
//funzionalità da implemetare
void ToUpper(char *message)
{
	int lung = strlen(message);
	int i;
	for(i = 0; i < lung; i++)
	{
		message[i] = toupper(message[i]);
	}
}
//funzionalità da implemetare
void ToLower(char *message)
{
	int lung = strlen(message);
	int i;
	for(i = 0; i < lung; i++)
	{
		message[i] = tolower(message[i]);
	}
}
//////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int port;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}
	else
	{
		port = PROTOPORT;
		if(port < 0)
		{
			printf("bad port number %s \n", argv[1]);
			return 0;
		}
	}

	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (iResult != 0)
	{
		errorHandler("Error at WSAStartup()\n");
		return 0;
	}
	#endif

	int mySocket;
	mySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mySocket < 0)
	{
		errorHandler("socket creation failed. \n");
		ClearWinSock();
		return -1;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(port);

	if(bind(mySocket, (struct sockaddr*) &sad, sizeof(sad)) < 0)
	{
		errorHandler("bind() failed. \n");
		closesocket(mySocket);
		ClearWinSock();
		return -1;
	}

	if(listen(mySocket, QLEN) < 0)
	{
		errorHandler("listen() failed. \n");
		closesocket(mySocket);
		ClearWinSock();
		return -1;
	}

	struct sockaddr_in cad;
	int clientSocket;
	int clientLen;
	printf("Waiting for a client to connect... \n");
	while(1)
	{
		clientLen = sizeof(cad);
		if((clientSocket = accept(mySocket, (struct sockaddr *)&cad, &clientLen)) < 0)
		{
			closesocket(mySocket);
			ClearWinSock();
			return 0;
		}
		printf("Hading a client %s\n", inet_ntoa(cad.sin_addr));
		//invio messaggio di connessione avvenuta
		sendSuccess(clientSocket,"Connessione avvenuta");

		//ricezione da client parola 1
		char word1[BUFFERSIZE], word2[BUFFERSIZE];
		int totByteRcv = recv(clientSocket, word1, BUFFERSIZE - 1, 0);
		word1[totByteRcv] = '\0';
		printf("%s\n",word1);
		//ricezione da client parola 2
		totByteRcv = recv(clientSocket, word2, BUFFERSIZE - 1, 0);
		word2[totByteRcv] = '\0';
		printf("%s\n",word2);
		//esecuzione funzionalità
		ToUpper(word1);
		ToLower(word2);
		//invio parole modificate
		sendSuccess(clientSocket, word1);
		sendSuccess(clientSocket, word2);

	}

	return EXIT_SUCCESS;
}
