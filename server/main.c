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
        printf("failed on create socket\n");
        WSACleanup();
        exit(1);
    }
    printf("socket created\n");
    return socketCreated;
}

void socket_set_bind(SOCKET *socket)
{
    SOCKET deref_socket = (SOCKET)*socket;
    struct sockaddr_in service;
    int intResult;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(PORT);
    intResult = bind(deref_socket, (SOCKADDR *)&service, sizeof(service));
    if (intResult == SOCKET_ERROR)
    {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        closesocket(deref_socket);
        WSACleanup();
        exit(1);
    }
    printf("success on bind...\n");
}

void socket_set_listen(SOCKET *socket)
{
    SOCKET deref_socket = (SOCKET)*socket;
    if (listen(deref_socket, 10) == SOCKET_ERROR)
    {
        wprintf(L"listen failed with error %d\n", WSAGetLastError());
        closesocket(deref_socket);
        WSACleanup();
        exit(1);
    }
}

void socket_set_accept(SOCKET *socket)
{
    SOCKET deref_socket = (SOCKET)*socket;
    printf("waiting connections...\n");
    if (accept(deref_socket, 0, 0) == SOCKET_ERROR)
    {
        wprintf(L"accept failed with error %d\n", WSAGetLastError());
        closesocket(deref_socket);
        WSACleanup();
        exit(1);
    }
}

int main()
{
    char buffer[BUFFER_SIZE];
    int intResult;
    WSADATA wsa_data = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }
    SOCKET socketCreated = createSocket();
    socket_set_bind(&socketCreated);
    socket_set_listen(&socketCreated);
    socket_set_accept(&socketCreated);
    /**
     * Now wtf is happen... i don't know
     */
    intResult = recv(socketCreated, buffer, BUFFER_SIZE, 0);
    if (intResult == SOCKET_ERROR)
    {
        printf("failed on recv: %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    printf("%d", intResult);
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