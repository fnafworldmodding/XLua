#include "LuaDispatcher.h"
#include "lua.hpp"
#include "Common.h"
#include "Surface.h"

LuaDispatcher::LuaDispatcher(lua_State* lua_state) : L(lua_state) {}

LuaDispatcher::LuaDispatcher() : L(nullptr) {}

void LuaDispatcher::Register(XLuaState* xluas) {
    if (xluas->state == nullptr) {
        return;
    }
    lua_State* L = xluas->state;
    if (luaL_newmetatable(L, LUA_DISPATCHER_METATABLE)) {
        LuaDispatcher dispatcheri;
        lua_newtable(L);

        lua_pushcfunction(L, LuaDispatcher::SubscribeWrapper);
        lua_setfield(L, -2, "subscribe");

        lua_pushcfunction(L, LuaDispatcher::UnsubscribeWrapper);
        lua_setfield(L, -2, "unsubscribe");

        lua_pushcfunction(L, LuaDispatcher::DispatchWrapper);
        lua_setfield(L, -2, "dispatch");

        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, LuaDispatcher::GC);
        lua_setfield(L, -2, "__gc");
    }
    lua_pop(L, 1); // pop the metatable
    lua_register(L, "Dispatcher", LuaDispatcher::NewInstance);
}

void LuaDispatcher::Subscribe(const std::string& eventName, int funcRef) {
    _events[eventName].push_back(funcRef);
}

bool LuaDispatcher::Unsubscribe(const std::string& eventName, int funcToUnsubscribeIdx) {
    auto it = _events.find(eventName);
    if (it == _events.end()) return false;

    auto& funcRefs = it->second;
    for (auto ref_it = funcRefs.begin(); ref_it != funcRefs.end(); ++ref_it) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, *ref_it);
        if (lua_rawequal(L, funcToUnsubscribeIdx, -1)) {
            lua_pop(L, 1);
            luaL_unref(L, LUA_REGISTRYINDEX, *ref_it);
            funcRefs.erase(ref_it);
            if (funcRefs.empty()) _events.erase(it);
            return true;
        }
        lua_pop(L, 1);
    }
    return false;
}

void LuaDispatcher::Dispatch(const std::string& eventName, int argCount) {
    auto it = _events.find(eventName);
    if (it == _events.end()) return;

    std::vector<int> funcRefsCopy = it->second;
    for (int funcRef : funcRefsCopy) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, funcRef);
        for (int i = 0; i < argCount; ++i) {
            lua_pushvalue(L, 3 + i);
        }
        if (lua_pcall(L, argCount, 0, 0) != 0) { // if not okay
            std::cerr << "Error dispatching event '" << eventName << "': "
                << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
        }
    }
}

int LuaDispatcher::NewInstance(lua_State* L) {
    void* userdata = lua_newuserdata(L, sizeof(LuaDispatcher));
    new (userdata) LuaDispatcher(L);
    luaL_getmetatable(L, LUA_DISPATCHER_METATABLE);
    lua_setmetatable(L, -2);
    return 1;
}

LuaDispatcher* LuaDispatcher::CheckInstance(lua_State* L, int index) {
    void* userdata = luaL_checkudata(L, index, LUA_DISPATCHER_METATABLE);
    return static_cast<LuaDispatcher*>(userdata);
}

int LuaDispatcher::SubscribeWrapper(lua_State* L) {
    LuaDispatcher* dispatcher = CheckInstance(L);
    const char* eventName = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    int funcRef = luaL_ref(L, LUA_REGISTRYINDEX);
    dispatcher->Subscribe(eventName, funcRef);
    return 0;
}

int LuaDispatcher::UnsubscribeWrapper(lua_State* L) {
    LuaDispatcher* dispatcher = CheckInstance(L);
    const char* eventName = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    bool success = dispatcher->Unsubscribe(eventName, 3);
    lua_pushboolean(L, success);
    return 1;
}

int LuaDispatcher::DispatchWrapper(lua_State* L) {
    LuaDispatcher* dispatcher = CheckInstance(L);
    const char* eventName = luaL_checkstring(L, 2);
    int argCount = lua_gettop(L) - 2;
    dispatcher->Dispatch(eventName, argCount);
    return 0;
}

int LuaDispatcher::GC(lua_State* L) {
    LuaDispatcher* dispatcher = CheckInstance(L);

    for (const auto& pair : dispatcher->_events) {
        for (int ref : pair.second) {
            luaL_unref(L, LUA_REGISTRYINDEX, ref);
        }
    }
    return 0;
}
