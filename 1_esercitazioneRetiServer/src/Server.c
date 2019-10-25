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
#include <stdlib.h>
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
//rest 1 quando non è un digit
int checkNum(char *num){
	int lung = strlen(num);

	for(int i=0;i<lung;i++){
		if(!isdigit(num[i])){
			return 1;
		}
	}
	return 0;
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
		/////////////////////////////////////////////////////////////////////
		printf("Hading a client %s\n", inet_ntoa(cad.sin_addr));

		//invio messaggio di connessione avvenuta
		sendSuccess(clientSocket,"Connessione avvenuta");
		///////////////////////////////////777

		float sol=0;
		char op1[BUFFERSIZE], op2[BUFFERSIZE],result[BUFFERSIZE];
		char letter;

		//ricezione primo numero
		int totByteRcv = recv(clientSocket, op1, BUFFERSIZE - 1, 0);
		op1[totByteRcv] = '\0';


		//ricezione secondo numero
		totByteRcv = recv(clientSocket, op2, BUFFERSIZE - 1, 0);
		op2[totByteRcv] = '\0';

		//ricezione lettera
		totByteRcv = recv(clientSocket, &letter, BUFFERSIZE - 1, 0);

		//esecuzione funzionalità
		if (checkNum(op1)==0 && checkNum(op2)==0){

			switch(letter){
				case 'a' : case'A':
					sol= strtof(op1, NULL)+ strtof(op2,NULL);
					sprintf(result, "Risultato dell'addizione: %.3f", sol);
					break;

				case 's': case 'S':
					sol= strtof(op1, NULL) - strtof(op2,NULL);
					sprintf(result, "Risultato della sottrazione: %.3f", sol);
					break;

				case 'm': case 'M':
					sol= strtof(op1, NULL) * strtof(op2,NULL);
					sprintf(result, "Risultato della moltiplicazione:  %.3f", sol);
					break;

				case 'd': case 'D':
					sol= strtof(op1, NULL) / strtof(op2,NULL);
					sprintf(result, "Risultato della divisione: %.3f", sol);
					break;
				default:
					sprintf(result, "TERMINE PROCESSO CLIENT");
					break;
			}
		}
		else {
			sprintf(result, "TERMINE PROCESSO CLIENT");}
		sendSuccess(clientSocket, result);


	}

	return EXIT_SUCCESS;
}
