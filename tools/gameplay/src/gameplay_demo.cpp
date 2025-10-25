#include <iostream>
#include "Entity.h"
#include "SceneLoader.h"
#include "../game_app/include/GameApp.h"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
int main(){
    std::cout << "Gameplay demo (with window) starting...\n";
    auto scene = CoreTech::SceneLoader::loadFromFile("tools/gameplay/sample.scene");
    scene->debugPrint();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_requiref(L, "coretech", luaopen_coretech, 1);
    lua_pop(L,1);

    // Run the GLFW window app which polls InputManager; camera movement is handled by gameplay camera controller
    CoreTech::runGameApp(1280,720,"CoreTech Game App");


    // Simple Lua REPL loop: type Lua commands into stdin
    std::cout << "Entering Lua REPL. Type 'exit' to quit the REPL." << std::endl;
    std::string line;
    while(true){
        std::cout << "lua> ";
        if(!std::getline(std::cin, line)) break;
        if(line == "exit") break;
        if(line.empty()) continue;
        if(luaL_dostring(L, line.c_str())){
            std::cerr << "Lua error: " << lua_tostring(L,-1) << std::endl;
            lua_pop(L,1);
        }
    }

    lua_close(L);
    std::cout << "Gameplay demo exiting\n";
    return 0;
}