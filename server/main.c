#include <stdio.h>
#include <winsock2.h>
#include <ws2def.h>

#define PORT 8081
#define BUFFER_SIZE 1024
#define MAX_CONNECTION 10

typedef struct
{
    SOCKET data[MAX_CONNECTION];
    int size;
    int capacity;
} SOCKET_CONNECTIONS;

SOCKET_CONNECTIONS connections = {.capacity = MAX_CONNECTION};

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
    // service.sin_addr.s_addr = inet_addr("192.168.1.93");
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

int add_connection(SOCKET_CONNECTIONS *connections, SOCKET socket)
{
    if (connections->size == connections->capacity)
        return 1;
    for (int i = 0; i < connections->capacity; i++)
    {
        if (connections->data[i] == 0)
        {
            connections->data[i] = socket;
            connections->size++;
            break;
        }
    }
    return 0;
}

void print_connections(SOCKET_CONNECTIONS connections)
{
    printf("[");
    for (int i = 0; i < connections.capacity; i++)
    {
        if (connections.capacity - 1 == i)
        {
            printf("%llu", connections.data[i]);
        }
        else
        {
            printf("%llu,", connections.data[i]);
        }
    }
    printf("]\n");
}

void remove_connection(SOCKET_CONNECTIONS *connections, SOCKET socket)
{
    for (int i = 0; i < connections->capacity; i++)
    {
        if (connections->data[i] == socket)
        {
            connections->data[i] = 0;
            connections->size--;
            break;
        }
    }
}

DWORD WINAPI handle_socket_connection(LPVOID lpParam)
{
    SOCKET socket_accepted = (SOCKET)lpParam;
    printf("client connected -> %llu\n", socket_accepted);
    char buffer[BUFFER_SIZE];
    int int_result;

    if (add_connection(&connections, socket_accepted) == 1)
    {
        printf("socket limit reached\n");
        goto exit;
    }
    print_connections(connections);
    while (1)
    {
        do
        {
            int_result = recv(socket_accepted, buffer, BUFFER_SIZE, 0);
            if (int_result > 0 && int_result < BUFFER_SIZE)
            {
                printf("bytes received: %d from socket -> %llu\n", int_result, socket_accepted);
                buffer[int_result] = '\0';
                for (size_t i = 0; i < connections.size; i++)
                {
                    /**
                     * we don't emit to the emitter
                     */
                    if (connections.data[i] != socket_accepted)
                    {
                        /**
                         * we don't care if the client received the message
                         */
                        send(connections.data[i], buffer, int_result, 0);
                    }
                }
            }
            else if (int_result == 0)
            {
                printf("client closed connection\n");
                goto exit;
            }
            else
            {
                printf("recv failed: %d\n", WSAGetLastError());
                printf("closing connection socket -> %llu\n", socket_accepted);
                goto exit;
            }
        } while (int_result > 0);
    }

exit:
    closesocket(socket_accepted);
    remove_connection(&connections, socket_accepted);
    print_connections(connections);
    return 0;
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
    socket_set_bind(&socketCreated);
    socket_set_listen(&socketCreated);

    while (1)
    {
        SOCKET accepted_socket = SOCKET_ERROR;

        while (accepted_socket == SOCKET_ERROR)
        {
            accepted_socket = accept(socketCreated, 0, 0);
        }

        DWORD threadId;
        CreateThread(NULL, 0, handle_socket_connection, (LPVOID)accepted_socket, 0, &threadId);
    }

    closesocket(socketCreated);
    WSACleanup();
    return 0;
}
