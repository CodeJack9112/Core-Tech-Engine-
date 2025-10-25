CoreTech Engine - Experimental Features
=======================================

This folder contains two lightweight prototype modules added for prototyping and demonstration:

1. NetManager - a minimal cross-platform TCP client/server implementation (prototype).
   - Location: include/NetManager.h, src/NetManager.cpp
   - Note: simple and minimal. For production, replace with ENet/asio/etc.

2. ScriptManager - a minimal function registration + call interface for tiny scripting.
   - Location: include/ScriptManager.h, src/ScriptManager.cpp
   - Registers C++ functions and executes a single-line call like: myfunc arg string

Build integration:
- The root CMakeLists.txt has been appended to call add_subdirectory(tools/features).
- This compiles a static library `coretech_features` which can be linked into the engine.

Limitations:
- These are prototypes for immediate testing and learning; they are not optimized and lack robust error handling.
- Networking uses platform sockets; firewall/permissions may block server sockets on some systems.
