//the 3 steps of tcp non-blocking connect :
//1. connect (with socket fd has been set non-blocking).
//2. check fd writable
//3. get connect result with  getsockopt


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <fcntl.h> // fctl
#include <arpa/inet.h> //struct sockaddr_in, IPPROTO_TCP
#include <errno.h> // errno

int SetSocketNoBlock(int nSocket)
{
    int nRetCode = -1;
    int nOption  = 0;

    nOption = fcntl(nSocket, F_GETFL, 0);
    nRetCode = fcntl(nSocket, F_SETFL, nOption | O_NONBLOCK);
    if (nRetCode != 0)
    {
        perror("fcntl");
        return false;
    }
    return  true;
}

int SetSocketRST(int nSocket)
{
    int             nRetCode        = -1;
    struct linger   lingerStruct;

    lingerStruct.l_onoff  = 1;
    lingerStruct.l_linger = 0;
    //set RST
    nRetCode = setsockopt(nSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));
    return nRetCode;
}

void CloseSocket(int nSocket)
{
    if (nSocket <= 0)
        return;

    if(SetSocketRST(nSocket) < 0)
        perror("set RST");

    close(nSocket);
}

int Connect(char szIP[], int nPort, int* nRetSocktFd)
{
    int                 nResult   = false;
    int                 nSocket   = -1;
    int                 nRetCode  = 0;
    struct sockaddr_in  ServerAddr;

    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket == -1)
    {
        perror("fcntl");
        return false;
    }

    nRetCode =  SetSocketNoBlock(nSocket);
    if (!nRetCode)
    {
        perror("SetSocketNoBlock");
        return false;
    }

    ServerAddr.sin_family       = AF_INET;
    ServerAddr.sin_addr.s_addr  = inet_addr(szIP);
    ServerAddr.sin_port         = (u_short)htons(nPort);
    memset(ServerAddr.sin_zero, 0, 8);

    errno = 0;
    nRetCode = connect(nSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));

    if (nRetCode != -1 && errno != EINPROGRESS) //EINPROGRESS connecting.
    {
        perror("connect");
        return false;
    }

    *nRetSocktFd = nSocket;

    return true;
}

int main(int argc, char const *argv[])
{
    int         nSocket         = -1;
    int         nRetCode        = -1;
    int         nResult         = -1;
    socklen_t   nResultLen      = 0;
    fd_set      WriteFdSet;

    int         nPort           = 80;
    char        szIP[]          = "220.181.57.216";

    nRetCode = Connect(szIP, nPort, &nSocket);
    if (!nRetCode)
    {
        CloseSocket(nSocket);
        perror("Connect");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&WriteFdSet);
    FD_SET(nSocket, &WriteFdSet);

    while (1)
    {
        nRetCode = select(nSocket + 1, NULL, &WriteFdSet, NULL, NULL);
        if (nRetCode < 0)
        {
          perror("select");
          CloseSocket(nSocket);
          exit(EXIT_FAILURE);
        }
        if (nRetCode >0) // nSocket writable
           break;
    }

    nResultLen = sizeof(nResult);
    nRetCode   = getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nResult, &nResultLen);
    if (nRetCode< 0 || nResult != 0) {
        perror("getsockopt");
        CloseSocket(nSocket);
        exit(EXIT_FAILURE);
    }

    printf("connect success.\n");

    CloseSocket(nSocket);
    return 0;
}
