#include <stdio.h>
#include <winsock2.h>
#include <ws2def.h>

#define PORT 8081
#define BUFFER_SIZE 1024

SOCKET createSocket()
{
    SOCKET socketCreated = INVALID_SOCKET;
    socketCreated = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
    char buffer[BUFFER_SIZE];
    int recvResult;
    WSADATA wsa_data = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }

    SOCKET socketCreated = createSocket();

    struct sockaddr_in service;
    int bind_result;
    int accept_result;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(PORT);

    bind_result = bind(socketCreated, (SOCKADDR *)&service, sizeof(service));
    if (bind_result == SOCKET_ERROR)
    {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    printf("binded\n");
    if (listen(socketCreated, 10) == SOCKET_ERROR)
    {
        wprintf(L"listen failed with error %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    printf("waiting connections...\n");
    accept_result = accept(socketCreated, 0, 0);
    if (accept_result == SOCKET_ERROR)
    {
        wprintf(L"accept failed with error %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }

    /**
     * empty buffer
     */
    for (size_t i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = '-';
    }
    recvResult = recv(socketCreated, buffer, BUFFER_SIZE, MSG_PEEK);
    if (recvResult == SOCKET_ERROR)
    {
        printf("failed on recv: %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    printf("%d", recvResult);
    // while (1)
    // {
    //     recvResult = recv(socketCreated, buffer, BUFFER_SIZE, MSG_PEEK);
    //     if (recvResult != 1)
    //     {
    //         printf("%d", recvResult);
    //         // for (size_t i = 0; i < BUFFER_SIZE; i++)
    //         // {
    //         //     printf("%c", buffer[i]);
    //         // }
    //     }
    // }

    closesocket(socketCreated);
    WSACleanup();
    return 0;
}