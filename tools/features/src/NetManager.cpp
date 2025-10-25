#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include "NetManager.h"
#include <iostream>
#include <vector>
#include <mutex>

namespace CoreTech {

struct NetManager::Impl {
#if defined(_WIN32)
    SOCKET sock = INVALID_SOCKET;
#else
    int sock = -1;
#endif
    std::thread worker;
    std::atomic<bool> running{false};
    MessageHandler handler = nullptr;
    std::mutex send_mutex;
};

NetManager::NetManager() : p(new Impl()) {
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}

NetManager::~NetManager(){
    stop();
    delete p;
#if defined(_WIN32)
    WSACleanup();
#endif
}

bool NetManager::startServer(int port){
    if(p->running) return false;
#if defined(_WIN32)
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSock == INVALID_SOCKET) return false;
#else
    int listenSock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(listenSock < 0) return false;
#endif
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((unsigned short)port);

    int opt = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    if(bind(listenSock, (sockaddr*)&addr, sizeof(addr)) < 0){
#if defined(_WIN32)
        closesocket(listenSock);
#else
        close(listenSock);
#endif
        return false;
    }
    if(listen(listenSock, 1) < 0){
#if defined(_WIN32)
        closesocket(listenSock);
#else
        close(listenSock);
#endif
        return false;
    }

    p->running = true;
    p->worker = std::thread([this, listenSock](){
        while(p->running){
            sockaddr_in clientAddr;
            socklen_t len = sizeof(clientAddr);
#if defined(_WIN32)
            SOCKET client = accept(listenSock, (sockaddr*)&clientAddr, &len);
            if(client == INVALID_SOCKET) continue;
#else
            int client = accept(listenSock, (sockaddr*)&clientAddr, &len);
            if(client < 0) continue;
#endif
            // handle one client connection in a new thread (simple)
            std::thread([this, client](){
                char buf[1024];
                while(p->running){
#if defined(_WIN32)
                    int ret = recv(client, buf, (int)sizeof(buf)-1, 0);
#else
                    int ret = ::recv(client, buf, (int)sizeof(buf)-1, 0);
#endif
                    if(ret <= 0) break;
                    buf[ret]=0;
                    std::string s(buf, ret);
                    if(p->handler) p->handler(s);
                }
#if defined(_WIN32)
                closesocket(client);
#else
                close(client);
#endif
            }).detach();
        }
#if defined(_WIN32)
        closesocket(listenSock);
#else
        close(listenSock);
#endif
    });
    return true;
}

bool NetManager::startClient(const std::string& host, int port){
    if(p->running) return false;
    // resolve host
    struct addrinfo hints{}, *res=nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    std::string portstr = std::to_string(port);
    if(getaddrinfo(host.c_str(), portstr.c_str(), &hints, &res) != 0) return false;
#if defined(_WIN32)
    SOCKET s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(s == INVALID_SOCKET){ freeaddrinfo(res); return false; }
#else
    int s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(s < 0){ freeaddrinfo(res); return false; }
#endif
    if(connect(s, res->ai_addr, (int)res->ai_addrlen) != 0){
#if defined(_WIN32)
        closesocket(s);
#else
        close(s);
#endif
        freeaddrinfo(res);
        return false;
    }
    freeaddrinfo(res);

    p->sock = s;
    p->running = true;
    p->worker = std::thread([this](){
        char buf[1024];
        while(p->running){
#if defined(_WIN32)
            int ret = recv(p->sock, buf, (int)sizeof(buf)-1, 0);
#else
            int ret = ::recv(p->sock, buf, (int)sizeof(buf)-1, 0);
#endif
            if(ret <= 0) break;
            buf[ret]=0;
            std::string s(buf, ret);
            if(p->handler) p->handler(s);
        }
    });
    return true;
}

bool NetManager::send(const std::string& msg){
    std::lock_guard<std::mutex> lk(p->send_mutex);
#if defined(_WIN32)
    if(p->sock == INVALID_SOCKET) return false;
    int sent = ::send(p->sock, msg.c_str(), (int)msg.size(), 0);
    return sent == (int)msg.size();
#else
    if(p->sock < 0) return false;
    ssize_t sent = ::send(p->sock, msg.c_str(), msg.size(), 0);
    return sent == (ssize_t)msg.size();
#endif
}

void NetManager::stop(){
    p->running = false;
#if defined(_WIN32)
    if(p->sock != INVALID_SOCKET) { shutdown(p->sock, SD_BOTH); closesocket(p->sock); p->sock = INVALID_SOCKET; }
#else
    if(p->sock >= 0) { ::shutdown(p->sock, SHUT_RDWR); close(p->sock); p->sock = -1; }
#endif
    if(p->worker.joinable()) p->worker.join();
}

void NetManager::setMessageHandler(MessageHandler h){
    p->handler = h;
}

} // namespace CoreTech
