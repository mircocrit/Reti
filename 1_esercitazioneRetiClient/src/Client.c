/*Progettare ed implementare un’applicazione TCP client/server conforme al seguente protocollo:
1) Il client richiede la connessione al server.
2) Stabilita la connessione, il server invia al client la stringa "connessione avvenuta".
3) Il client stampa la stringa ricevuta; dopodiché, legge due stringhe di caratteri dallo standard input e le invia al server.
4) Il server legge le stringhe inviate dal client
- le visualizza sullo standard output
- converte la prima stringa ricevuta tutta in MAIUSCOLO, la seconda tutta in minuscolo
- invia le nuova stringhe al client
- rimane in ascolto!
5) Il client legge la risposta inviata dal server.
- visualizza la risposta sullo standard output
- chiude la connessione corrente
- termina il processo
*/


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
#define PROTOPORT 27015

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

//messaggio da inviare al client: send(clientSocket, message, lungMessage, 0)
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

int main(void)
{
	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if(iResult != 0)
	{
		printf("error at WSAStartup \n");
		return -1;
	}
	#endif

	int cSocket;
	cSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(cSocket < 0)
	{
		errorHandler("socket creation failed. \n");
		closesocket(cSocket);
		ClearWinSock();
		return -1;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(5193);

	if(connect(cSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0)
	{
		errorHandler("Failed to connect. \n");
		closesocket(cSocket);
		ClearWinSock();
		return -1;
	}


	/////////////////////////////////////////////////////////////////////
	int totByteRcv = 0;
	char message[BUFFERSIZE];
	char op1[BUFFERSIZE],op2[BUFFERSIZE],res[BUFFERSIZE];
	char oper;

	///ricez conness avvenuta
	totByteRcv = recv(cSocket, message, BUFFERSIZE -1, 0);
	message[totByteRcv] = '\0';
	printf("%s", message);


	///////////////invio dati////////////////////////////////

	printf("\nPrimo numero   ---> ");
	scanf("%s",op1);
	sendSuccess(cSocket,op1);	//invio num

	printf("Secondo numero   ---> ");
	scanf("%s",op2);
	sendSuccess(cSocket,op2);	//invio num

	fflush(stdin);
	printf("Lettera  ---> ");
	oper=getchar();
	sendSuccess(cSocket,&oper);	//invio oper




	//////////////ricezione risultato/////////////////////////77
	totByteRcv = recv(cSocket, res, BUFFERSIZE -1, 0);
	res[totByteRcv] = '\0';

	if ((strcmp(res, "TERMINE PROCESSO CLIENT")==0))	printf("chiusura connessione");
	else												printf("%s\n", res);


	/////////////////////////////////////////////////////////////////////
	closesocket(cSocket);
	ClearWinSock();
	printf("\n");
	system("pause");
	return(0);

}
