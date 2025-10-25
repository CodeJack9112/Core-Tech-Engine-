#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "Entity.h"
#include "SceneLoader.h"
#include <memory>
#include <string>
#include <iostream>

using namespace CoreTech;

static std::shared_ptr<Scene> globalScene;

static int l_load_scene(lua_State* L){
    const char* path = luaL_checkstring(L,1);
    globalScene = SceneLoader::loadFromFile(path);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_get_entities(lua_State* L){
    if(!globalScene){ lua_pushinteger(L, 0); return 1; }
    auto ents = globalScene->getAllEntities();
    lua_newtable(L);
    int idx = 1;
    for(auto &e: ents){
        lua_newtable(L);
        lua_pushstring(L, "id"); lua_pushinteger(L, e->getId()); lua_settable(L,-3);
        lua_pushstring(L, "name"); lua_pushstring(L, e->getName().c_str()); lua_settable(L,-3);
        // transform
        auto t = e->getComponent<TransformComponent>();
        if(t){
            lua_pushstring(L,"x"); lua_pushnumber(L,t->x); lua_settable(L,-3);
            lua_pushstring(L,"y"); lua_pushnumber(L,t->y); lua_settable(L,-3);
            lua_pushstring(L,"z"); lua_pushnumber(L,t->z); lua_settable(L,-3);
        }
        lua_rawseti(L, -2, idx++);
    }
    return 1;
}

static int l_set_key(lua_State* L){
    const char* key = luaL_checkstring(L,1);
    int pressed = lua_toboolean(L,2);
    CoreTech::InputManager::setKeyState(key, pressed!=0);
    lua_pushboolean(L,1);
    return 1;
}

extern "C" int luaopen_coretech(lua_State* L){
    lua_newtable(L);
    lua_pushcfunction(L, l_load_scene); lua_setfield(L, -2, "load_scene");
    lua_pushcfunction(L, l_get_entities); lua_setfield(L, -2, "get_entities");
    lua_pushcfunction(L, l_set_key); lua_setfield(L, -2, "set_key");
    return 1;
}
