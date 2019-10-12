#ifndef WORK_H_
#define WORK_H_

/////////////////////di lavoro///////////////
void ErrorHandler(char *errorMessage) {
	printf ("%s", errorMessage);
}
void ClearWinSock() {
	#if defined WIN32
	WSACleanup();		//disturgge un goggetto
	#endif
}

#endif /* WORK_H_ */
