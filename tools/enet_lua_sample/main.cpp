#include <iostream>

// Try to include enet and lua headers if available
#if __has_include("enet/enet.h")
  #include <enet/enet.h>
  #define HAVE_ENET 1
#else
  #define HAVE_ENET 0
#endif

#if __has_include("lua.h")
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
  #define HAVE_LUA 1
#else
  #define HAVE_LUA 0
#endif

int main(int argc, char** argv){
    std::cout << "ENet+Lua sample starting...\n";

#if HAVE_ENET
    if(enet_initialize() != 0){
        std::cerr << "ENet failed to initialize\n";
    } else {
        std::cout << "ENet initialized successfully\n";
        enet_deinitialize();
    }
#else
    std::cout << "ENet not available - compiled without ENet\n";
#endif

#if HAVE_LUA
    lua_State* L = luaL_newstate();
    if(L){
        luaL_openlibs(L);
        const char* script = "return 'hello from lua'";
        if(luaL_loadstring(L, script) || lua_pcall(L, 0, 1, 0)){
            std::cerr << "Lua error: " << lua_tostring(L, -1) << "\n";
        } else {
            if(lua_isstring(L, -1)){
                std::cout << "Lua returned: " << lua_tostring(L, -1) << "\n";
            }
            lua_pop(L,1);
        }
        lua_close(L);
    } else {
        std::cerr << "Failed to create Lua state\n";
    }
#else
    std::cout << "Lua not available - compiled without Lua\n";
#endif

    std::cout << "Sample exiting\n";
    return 0;
}
