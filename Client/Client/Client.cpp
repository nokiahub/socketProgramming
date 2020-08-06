#include "stdafx.h"
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT        9001
#define RLT_SIZE 4
#define OPSZ 4

int _tmain(int argc, _TCHAR* argv[])
{
	// Winsock Start - winsock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	// 1. 소켓생성
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");
		return 1;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);



	// 2. 연결요청
	if (connect(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return 1;
	}


	char buffer[MAX_BUFFER] = "";

	int receiveMsg = recv(listenSocket, buffer, MAX_BUFFER, 0);
	printf("%s", buffer);

	char messageBuffer[MAX_BUFFER] = "";
	scanf("%s", messageBuffer);

	int bufferLen = strlen(messageBuffer);

	int sendMsg = send(listenSocket, messageBuffer, bufferLen, 0);

	printf("\n<현재 접속자 명단입니다>\n");

	int tet = recv(listenSocket, buffer, MAX_BUFFER, 0);

	printf("%s\n", buffer);
	while (1)
	{
		char opmsg[MAX_BUFFER] = "";
		int result, opndCnt, i;

		fputs("Operand count (종료하려면 0을 누르세요): ", stdout); //피연산자의 갯수 입력 예) 1+3이면 2 입력 1+2+3이면 3 입력
		scanf("%d", &opndCnt);
		opmsg[0] = (char)opndCnt;

		if (opmsg[0] == 0) {
			printf("임의의 키(엔터 제외)를 한번 더 누르면 소켓이 종료됩니다.");
			scanf("%d", (int*)&opmsg[1]);
			send(listenSocket, opmsg, 2, 0);
			break;
		}
		for (i = 0; i < opndCnt; i++) {
			printf("Operand %d: ", i + 1);
			scanf("%d", (int*)&opmsg[i*OPSZ + 1]);
		}

		fgetc(stdin);
		fputs("Operator: ", stdout);
		scanf("%c", &opmsg[opndCnt*OPSZ + 1]);
		send(listenSocket, opmsg, opndCnt*OPSZ + 2, 0);
		if (opmsg[0] == 0) break;
		recv(listenSocket, (char*)&result, RLT_SIZE, 0);

		if (result == -100000)
			printf("잘못된 연산자입니다\n");
		else
			printf("Operation result: %d\n", result);
	}

	// 4. 소켓종료
	printf("소켓을 종료합니다.");
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}