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
	// Winsock Start - winsock.dll �ε�
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	// 1. ���ϻ���
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");
		return 1;
	}

	// �������� ��ü����
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);



	// 2. �����û
	if (connect(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. ��������
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

	printf("\n<���� ������ ����Դϴ�>\n");

	int tet = recv(listenSocket, buffer, MAX_BUFFER, 0);

	printf("%s\n", buffer);
	while (1)
	{
		char opmsg[MAX_BUFFER] = "";
		int result, opndCnt, i;

		fputs("Operand count (�����Ϸ��� 0�� ��������): ", stdout); //�ǿ������� ���� �Է� ��) 1+3�̸� 2 �Է� 1+2+3�̸� 3 �Է�
		scanf("%d", &opndCnt);
		opmsg[0] = (char)opndCnt;

		if (opmsg[0] == 0) {
			printf("������ Ű(���� ����)�� �ѹ� �� ������ ������ ����˴ϴ�.");
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
			printf("�߸��� �������Դϴ�\n");
		else
			printf("Operation result: %d\n", result);
	}

	// 4. ��������
	printf("������ �����մϴ�.");
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}