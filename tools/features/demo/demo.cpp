#include <iostream>
#include "NetManager.h"
#include "ScriptManager.h"
#include <thread>
#include <chrono>

using namespace CoreTech;

int main(){
    std::cout << "CoreTech features demo\n";

    ScriptManager sm;
    sm.registerFunction("echo", [](const std::string& arg){ return std::string("ECHO: ") + arg; });
    std::string r = sm.execute("echo hello world");
    std::cout << "Script result: " << r << "\n";

    NetManager nm;
    nm.setMessageHandler([](const std::string& msg){
        std::cout << "Net message received: " << msg << std::endl;
    });

    // Start a server in background on port 3333
    if(nm.startServer(3333)){
        std::cout << "Server started on port 3333 (connect with: `telnet localhost 3333`)\n";
    }else{
        std::cout << "Failed to start server (maybe port in use)\n";
    }

    // keep alive for 20 seconds for demo purposes
    std::this_thread::sleep_for(std::chrono::seconds(20));
    nm.stop();
    std::cout << "Demo exiting\n";
    return 0;
}
