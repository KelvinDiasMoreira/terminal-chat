#include <stdio.h>
#include <winsock2.h>

int main()
{
    SOCKET so;
    WSADATA wsaData = {0};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("error");
        return 1;
    }
    if (so = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == INVALID_SOCKET)
    {
        printf("error");
        return 1;
    }
    
    printf("socket created...\n");
    return 0;
}