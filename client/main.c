#include <stdio.h>
#include <winsock2.h>
#include <ws2def.h>

#define PORT 8081

SOCKET createSocket()
{
    SOCKET socketCreated = INVALID_SOCKET;
    socketCreated = socket(AF_INET, SOCK_STREAM, 0);
    if (socketCreated == INVALID_SOCKET)
    {
        WSACleanup();
        exit(1);
    }
    printf("socket created\n");
    return socketCreated;
}

int main()
{
    WSADATA wsa_data = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }

    SOCKET socketCreated = createSocket();

    struct sockaddr_in service;
    int connect_result;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(PORT);

    connect_result = connect(socketCreated, (SOCKADDR *)&service, sizeof(service));
    if (connect_result == SOCKET_ERROR)
    {
        wprintf(L"connect failed with error %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    
    char *bufferToSend = "Teste";
    int sendResult;
    sendResult = send(socketCreated, bufferToSend, strlen(bufferToSend), 0);
    if (sendResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }

    closesocket(socketCreated);
    WSACleanup();
    return 0;
}