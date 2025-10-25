#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>

// A minimal cross-platform TCP networking manager (simple, for prototyping).
// Note: This is a lightweight implementation intended to be included in this repo
// as a starting point. For production use, replace with ENet/asio/raknet/etc.

namespace CoreTech {

class NetManager {
public:
    using MessageHandler = std::function<void(const std::string&)>;

    NetManager();
    ~NetManager();

    // Start a TCP server on the given port. Returns true on success.
    bool startServer(int port);

    // Start a TCP client and connect to host:port. Returns true on success.
    bool startClient(const std::string& host, int port);

    // Send a message (client or server-side accepted socket will be used).
    bool send(const std::string& msg);

    // Stop networking and join threads.
    void stop();

    // Set a handler to be called when a message is received.
    void setMessageHandler(MessageHandler h);

private:
    struct Impl;
    Impl* p;
};

} // namespace CoreTech
