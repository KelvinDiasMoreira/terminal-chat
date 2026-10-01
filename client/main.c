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

char *who_is_there()
{
    /*we need free this*/
    char *name = (char *)malloc(sizeof(char) * 51);
    if (name == NULL)
    {
        printf("failed see who is there....\n");
        exit(1);
    }
    printf("Qual o seu nome: ");
    if (fgets(name, 51, stdin) == NULL)
    {
        printf("failed see who is there....\n");
        free(name);
        exit(1);
    }
    name[strcspn(name, "\n")] = '\0';
    return name;
}

void send_message(SOCKET socket)
{
    char buffer[50];
    printf("message: ");
    scanf("%s\n", buffer);
}

int main()
{
    char *client_name = who_is_there();
    char buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];
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
    printf("connected to the socket:");
    func_result = send(socketCreated, client_name, (int)strlen(client_name), 0);
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
        do
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
        } while (func_result > 0);
        send_message(socketCreated);
    }

    closesocket(socketCreated);
    WSACleanup();
    return 0;
}