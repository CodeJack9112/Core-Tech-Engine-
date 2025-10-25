#include <lua.hpp>
#include "tools/gameplay/include/Scene.h"
#include "tools/physics/include/PhysicsAPI.h"
#include "src/AudioManager.h"
#include <iostream>
using namespace CoreTech;
static int l_getEntity(lua_State* L){
    unsigned int id = (unsigned int)lua_tointeger(L,1);
    // push a table with basic fields (stub)
    lua_newtable(L);
    lua_pushstring(L, "id"); lua_pushinteger(L, id); lua_settable(L,-3);
    return 1;
}
static int l_raycast(lua_State* L){
    float ox = luaL_checknumber(L,1);
    float oy = luaL_checknumber(L,2);
    float oz = luaL_checknumber(L,3);
    float dx = luaL_checknumber(L,4);
    float dy = luaL_checknumber(L,5);
    float dz = luaL_checknumber(L,6);
    glm::vec3 hit; unsigned int ent=0;
    bool ok = PhysicsAPI::raycast(glm::vec3(ox,oy,oz), glm::vec3(dx,dy,dz), 100.0f, hit, ent);
    lua_pushboolean(L, ok);
    if(ok){
        lua_pushnumber(L, hit.x); lua_pushnumber(L, hit.y); lua_pushnumber(L, hit.z); lua_pushinteger(L, ent);
        return 4;
    }
    return 1;
}
static int l_playSound(lua_State* L){
    const char* path = luaL_checkstring(L,1);
    bool ok = CoreTech::playSoundFile(path);
    lua_pushboolean(L, ok); return 1;
}
extern "C" void register_lua_bindings(lua_State* L){
    lua_register(L, "getEntity", l_getEntity);
    lua_register(L, "raycast", l_raycast);
    lua_register(L, "playSound", l_playSound);
}
