#include <stdio.h>
#include <winsock2.h>
#include <ws2def.h>

#define PORT 8081

int main()
{
    SOCKET so = INVALID_SOCKET;
    WSADATA wsa_data = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR)
    {
        printf("failed on WSAStartup");
        return 1;
    }
    so = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (so == INVALID_SOCKET)
    {
        printf("error on create socket");
        WSACleanup();
        return 1;
    }
    
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(PORT);
    int bind_result;

    bind_result = bind(so,(SOCKADDR *) &service, sizeof(service));
    if (bind_result == SOCKET_ERROR){
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(so);
        WSACleanup();
        return 1;
    }

    closesocket(so);
    WSACleanup();
    return 0;
}