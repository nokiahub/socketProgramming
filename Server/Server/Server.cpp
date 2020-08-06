#include "stdafx.h"
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define RLT_SIZE 4
#define OPSZ 4
#define SERVER_PORT        9001

typedef struct ListNode {
	char clientID[MAX_BUFFER];
	struct ListNode *link;
} ListNode;

typedef struct {
	ListNode *head;     // 헤드 포인터
	int length;      // 노드의 개수
} ListType;

DWORD WINAPI makeThread(void *data);
int calculate(int opnum, int opnds[], char op);
void init(ListType *list);
void insert_node(ListNode **phead, ListNode *new_node);
void remove_node(ListNode **phead, char *removed);
void display(ListNode *head);



ListType *clientList = (ListType*)malloc(sizeof(ListType));


int _tmain(int argc, _TCHAR* argv[])
{

	init(clientList);

	// Winsock Start - windock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
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
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail bind\n");
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return 1;
	}

	// 3. 수신대기열생성
	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		printf("Error - Fail listen\n");
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return 1;
	}

	// 연결대기 정보변수 선언
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;


	// thread Handle 선언
	HANDLE hThread;

	while (1)
	{
		// 4. 연결대기            
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
		hThread = CreateThread(NULL, 0, makeThread, (void *)clientSocket, 0, NULL);
		CloseHandle(hThread);
	}

	// 6-2. 리슨 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}

int calculate(int opnum, int opnds[], char op) {
	int result = opnds[0], i;
	switch (op)
	{
	case'+':
		for (i = 1; i < opnum; i++) result += opnds[i];
		break;
	case'-':
		for (i = 1; i < opnum; i++) result -= opnds[i];
		break;
	case'*':
		for (i = 1; i < opnum; i++) result *= opnds[i];
		break;
	default:
		result = -100000;
		break;
	}
	return result;
}

DWORD WINAPI makeThread(void *data)
{
	SOCKET socket = (SOCKET)data;

	char opinfo[MAX_BUFFER];
	int result, opndCnt;
	int recvCnt, recvLen;


	char messageBuffer[MAX_BUFFER] = "";

	strcpy_s(messageBuffer, "Enter your ID: ");
	int bufferLen = strlen(messageBuffer);

	int sendMsg = send(socket, messageBuffer, bufferLen, 0);
	//
	char buffer[MAX_BUFFER] = "";
	int receiveMsg = recv(socket, buffer, MAX_BUFFER, 0);

	if (receiveMsg > 0) {

		ListNode *cl;
		cl = (ListNode*)malloc(sizeof(ListNode));

		strcpy_s(cl->clientID, buffer);
		cl->link = NULL;


		insert_node(&clientList->head, cl);


		printf("\n");

	}

	display(clientList->head);

	char copy[MAX_BUFFER] = "";

	ListNode *p = clientList->head;

	while (p != NULL) {
		char s2[MAX_BUFFER] = "";
		strcpy(s2, p->clientID);
		strcat_s(copy, s2);
		char space[2] = "\n";
		strcat_s(copy, space);
		p = p->link;
	}


	int tete = send(socket, copy, sizeof(copy), 0);


	while (1) {
		opndCnt = 0;
		int rec = recv(socket, (char*)&opndCnt, 1, 0);


		recvLen = 0;
		while ((opndCnt*OPSZ + 1) > recvLen) {
			recvCnt = recv(socket, &opinfo[recvLen], MAX_BUFFER - 1, 0);
			recvLen += recvCnt;
		}

		result = calculate(opndCnt, (int*)opinfo, opinfo[recvLen - 1]);
		if (result == -100000 && opinfo[0] == 0)
			break;


		send(socket, (char*)&result, sizeof(result), 0);

	}

	closesocket(socket);
	printf("%s님이 퇴장하셨습니다.\n\n", buffer);
	remove_node(&clientList->head, buffer);
	display(clientList->head);
	return 0;


}


void init(ListType *list)
{
	if (list == NULL) return;
	list->length = 0;
	list->head = NULL;
}


void insert_node(ListNode **phead, ListNode *new_node)
{
	ListNode *before = (ListNode*)malloc(sizeof(ListNode));
	before->link = *phead;

	while (before->link) {
		before = before->link;
	}

	if (*phead == NULL) {
		new_node->link = NULL;
		*phead = new_node;
	}
	else {
		before->link = new_node;
	}

}

void remove_node(ListNode **phead, char* removed)
{

	char *temp = removed;

	ListNode *before = (ListNode*)malloc(sizeof(ListNode));
	ListNode *beforebefore = (ListNode*)malloc(sizeof(ListNode));
	beforebefore->link = NULL;

	before = *phead;

	while (1) {
		if (strcmp(before->clientID, temp) == 0)
			break;
		beforebefore = before;
		before = before->link;
	}

	if (beforebefore->link == NULL) //즉,첫번째노드삭제를의미
		*phead = (*phead)->link;
	else
		beforebefore->link = before->link;
	free(before);
}

void display(ListNode *head)
{
	printf("<현재 접속자 명단>\n");
	ListNode *p = head;
	while (p != NULL) {
		printf("%s->", p->clientID);
		p = p->link;
	}
	printf("\n");

}