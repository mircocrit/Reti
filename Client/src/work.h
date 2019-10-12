#ifndef WORK_H_
#define WORK_H_

/////////////////////Di lavoro///////////////////////////////////
void ErrorHandler(char *errorMessage) {
	printf("%s",errorMessage);
}
void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

#endif /* WORK_H_ */
