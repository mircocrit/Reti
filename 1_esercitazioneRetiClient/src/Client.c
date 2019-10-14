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

	/*
	char *inputString = "prova";
	int stringLen = strlen(inputString);

	if(send(cSocket, inputString, stringLen, 0) != stringLen)
	{
		errorHandler("send() sent a different number of bytes than expected.\n");
		closesocket(cSocket);
		ClearWinSock();
		return -1;
	}
	*/

	int totByteRcv = 0;
	char buf[BUFFERSIZE];
	totByteRcv = recv(cSocket, buf, BUFFERSIZE -1, 0);
	buf[totByteRcv] = '\0';
	printf("%s", buf);

	printf("\nPrima parola   ---> ");
	scanf("%s",buf);
	sendSuccess(cSocket,buf);	//invio parola

	memset(buf,0,sizeof(buf));	//reset memoria
	printf("Seconda parola   ---> ");
	scanf("%s",buf);
	sendSuccess(cSocket,buf);	//invio parola
	memset(buf,0,sizeof(buf));
	//ricezione parole
	totByteRcv = recv(cSocket, buf, BUFFERSIZE -1, 0);
	buf[totByteRcv] = '\0';
	printf("%s\n", buf);

	memset(buf,0,sizeof(buf));
	totByteRcv = recv(cSocket, buf, BUFFERSIZE -1, 0);
	buf[totByteRcv] = '\0';
	printf("%s\n", buf);

	memset(buf,0,sizeof(buf));
	/*
	while(totByteRcv < stringLen)
	{
		if((byteReceived = recv(cSocket, buf, BUFFERSIZE -1, 0 )) <= 0)
		{
			errorHandler("recv() failed or connection closed prematurely\n");
			closesocket(cSocket);
			ClearWinSock();
			return -1;
		}
		totByteRcv += byteReceived;
		buf[byteReceived] = '\0';
		printf("%s", buf);
	}
	*/

	closesocket(cSocket);
	ClearWinSock();
	printf("\n");
	system("pause");
	return(0);

}
