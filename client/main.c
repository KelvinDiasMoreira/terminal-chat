#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2def.h>
#include <windows.h>

#define PORT 8081
#define BUFFER_SIZE 1024

static SOCKET socketCreated = SOCKET_ERROR;

void encrypt(char buffer[], int key)
{
    for (int i = 0 ; i < strlen(buffer); i++)
    {
        buffer[i] = buffer[i] - key;
    }
}

void decrypt(char buffer[], int key)
{
    for (int i = 0 ; i < strlen(buffer); i++)
    {
        buffer[i] = buffer[i] + key;
    }
}

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

DWORD WINAPI see_user_input(LPVOID lpParam)
{
    int bytes_sended;
    while (1)
    {
        char buffer[BUFFER_SIZE];
        if (fgets(buffer, 51, stdin) == NULL)
        {
            printf("failed to read\n");
            exit(1);
        }
        buffer[strcspn(buffer, "\n")] = '\0';
        if (socketCreated != SOCKET_ERROR)
        {
            encrypt(buffer, 0XAED);
            bytes_sended = send(socketCreated, buffer, (int)strlen(buffer), 0);
            if (bytes_sended == SOCKET_ERROR)
            {
                printf("send failed: %d\n", WSAGetLastError());
            }
        }
    }

    return 0;
}

HANDLE thread_user_input()
{

    HANDLE t_result;
    t_result = CreateThread(NULL, 0, see_user_input, NULL, 0, 0);
    if (t_result == 0)
    {
        printf("failed on create thread -> %lu\n", GetLastError());
        closesocket(socketCreated);
        WSACleanup();
        exit(1);
    }
    return t_result;
}

void socket_connect()
{
    int func_result;
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    // service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_addr.s_addr = inet_addr("192.168.1.93");
    service.sin_port = htons(PORT);

    func_result = connect(socketCreated, (SOCKADDR *)&service, sizeof(service));
    if (func_result == SOCKET_ERROR)
    {
        printf("connect failed with error %d\n", WSAGetLastError());
        closesocket(socketCreated);
        WSACleanup();
        exit(1);
    }
    printf("connected to the room\n");
}

void listen_socket()
{
    char r_buffer[BUFFER_SIZE];
    int r_result;
    while (1)
    {
        do
        {
            r_result = recv(socketCreated, r_buffer, BUFFER_SIZE, 0);
            if (r_result > 0)
            {
                decrypt(r_buffer, 0XAED);
                r_buffer[r_result] = '\0';
                for (size_t i = 0; i < r_result; i++)
                {
                    printf("%c", r_buffer[i]);
                }
                printf("\n");
            }
        } while (r_result > 0);
    }
}

int main()
{
    // char *client_name = who_is_there();
    char send_buffer[BUFFER_SIZE];
    int func_result;
    HANDLE t_result;
    WSADATA wsa_data = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }

    socketCreated = createSocket();
    socket_connect();
    t_result = thread_user_input();
    listen_socket();

    CloseHandle(t_result);
    closesocket(socketCreated);
    WSACleanup();
    return 0;
}
