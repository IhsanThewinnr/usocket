#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    constexpr unsigned short kPort = 9001;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[usocket] WSAStartup failed\n";
        return 1;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "[usocket] socket() failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(kPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[usocket] bind() failed on port " << kPort << "\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[usocket] listen() failed\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    std::cout << "[usocket] server listening on 0.0.0.0:" << kPort << "\n";

    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);

        if (clientSock == INVALID_SOCKET) {
            std::cerr << "[usocket] accept() failed\n";
            continue;
        }

        char ipBuf[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipBuf, sizeof(ipBuf));
        std::cout << "[usocket] client connected from " << ipBuf << ":" << ntohs(clientAddr.sin_port) << "\n";

        const std::string payload = "hello from usocket server\n";
        int sent = send(clientSock, payload.c_str(), static_cast<int>(payload.size()), 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "[usocket] send() failed\n";
        } else {
            std::cout << "[usocket] sent " << sent << " bytes\n";
        }

        closesocket(clientSock);
        std::cout << "[usocket] client disconnected\n";
    }

    closesocket(listenSock);
    WSACleanup();
    return 0;
}
