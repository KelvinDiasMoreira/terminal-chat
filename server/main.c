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

SOCKET socket_set_accept(SOCKET *socket)
{
    SOCKET deref_socket = (SOCKET)*socket;
    SOCKET accept_socket = accept(deref_socket, 0, 0);
    if (accept_socket == INVALID_SOCKET)
    {
        wprintf(L"accept failed with error %d\n", WSAGetLastError());
        closesocket(deref_socket);
        WSACleanup();
        exit(1);
    }
    return accept_socket;
}

int main()
{
    char buffer[BUFFER_SIZE];
    int intResult;
    int sentResult;
    WSADATA wsa_data = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }
    SOCKET socketCreated = createSocket();
    socket_set_bind(&socketCreated);
    socket_set_listen(&socketCreated);

    while (1)
    {
        SOCKET connection_socket = socket_set_accept(&socketCreated);
        printf("new conection -> %p\n", connection_socket);
        do
        {
            intResult = recv(connection_socket, buffer, BUFFER_SIZE, 0);
            if (intResult > 0)
            {
                printf("bytes received: %d\n", intResult);
                buffer[intResult] = '\0';
                for (size_t i = 0; i < intResult; i++)
                {
                    printf("%c", buffer[i]);
                }
                printf("\n");
                sentResult = send(connection_socket, buffer, intResult, 0);
                if (sentResult > 0)
                {
                    sentResult = shutdown(connection_socket, SD_SEND);
                    printf("%d\n", sentResult);
                }
            }
            else if (intResult == 0)
            {
                // printf("connection closing...\n");
            }
            else
            {
                printf("recv failed: %d\n", WSAGetLastError());
                closesocket(connection_socket);
                WSACleanup();
            }
        } while (intResult > 0);
    }

    closesocket(socketCreated);
    WSACleanup();
    return 0;
}