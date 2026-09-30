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

// char *who_is_there()
void who_is_there()
{
    char name[50];
    printf("Qual o seu nome: ");
    scanf("%s", &name);
    printf("%s", name);
    // return name;
}

int main()
{
    // char *client_name = who_is_there();
    who_is_there();

    char buffer[BUFFER_SIZE];
    int func_result;
    WSADATA wsa_data = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }

    SOCKET socketCreated = createSocket();

    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(PORT);

    func_result = connect(socketCreated, (SOCKADDR *)&service, sizeof(service));
    if (func_result == SOCKET_ERROR)
    {
        wprintf(L"connect failed with error %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    printf("connected to the socket: %d\n", func_result);
    char *bufferToSend = "teste";
    func_result = send(socketCreated, bufferToSend, (int)strlen(bufferToSend), 0);
    if (func_result == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }
    printf("bytes sended %d\n", func_result);
    func_result = shutdown(socketCreated, SD_SEND);
    if (func_result == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        return 1;
    }

    while (1)
    {
        func_result = recv(socketCreated, buffer, BUFFER_SIZE, 0);
        if (func_result > 0)
        {
            printf("bytes received: %d\n", func_result);
            buffer[func_result] = '\0';
            for (size_t i = 0; i < func_result; i++)
            {
                printf("%c", buffer[i]);
            }
            printf("\n");
        }
    }

    closesocket(socketCreated);
    WSACleanup();
    return 0;
}