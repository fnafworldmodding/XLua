#include "LuaDispatcher.h"
#include "Common.h"
#include "Surface.h"
#include "Globals.h"


LuaDispatcher::LuaDispatcher(XLuaState* lua_state) : state(lua_state) {
    _events = std::unordered_map<std::string, std::vector<int>>();
}

LuaDispatcher::LuaDispatcher() : state(nullptr) {
    _events = std::unordered_map<std::string, std::vector<int>>();
}

void LuaDispatcher::SetState(XLuaState* L) { state = L; }

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
    auto newInstanceLambda = [](lua_State* L) -> int {
        void* userdata = lua_newuserdata(L, sizeof(LuaDispatcher));
        new (userdata) LuaDispatcher(globalXLua->state);

        luaL_getmetatable(L, LUA_DISPATCHER_METATABLE);
        lua_setmetatable(L, -2);
        return 1;
        };
    lua_register(L, "Dispatcher", newInstanceLambda);
}

void LuaDispatcher::Subscribe(const std::string& eventName, int funcRef) {
    _events[eventName].push_back(funcRef);
}

bool LuaDispatcher::Unsubscribe(const std::string& eventName, int funcToUnsubscribeIdx) {
    auto it = _events.find(eventName);
    if (it == _events.end()) return false;
    lua_State* L = state->state;

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
    lua_State* L = state->state;

    std::vector<int> funcRefsCopy = it->second;
    for (int funcRef : funcRefsCopy) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, funcRef);
        for (int i = 0; i < argCount; ++i) {
            lua_pushvalue(L, 3 + i);
        }
        if (lua_pcall(L, argCount, 0, 0) != 0) { // if not okay
            //state->RaiseError("Error dispatching event '" + eventName + "': " + lua_tostring(L, -1));
            std::string errorMsg = "Error dispatching event '" + eventName + "': " + lua_tostring(L, -1);
            std::cout << errorMsg << std::endl;
            lua_pop(L, 1); // pop error message
        }
    }
}

int LuaDispatcher::NewInstance(XLuaState* xstate, LuaDispatcher*& dispatcher) {
    void* userdata = lua_newuserdata(xstate->state, sizeof(LuaDispatcher));
    new (userdata) LuaDispatcher(xstate);
    dispatcher = static_cast<LuaDispatcher*>(userdata);
    luaL_getmetatable(xstate->state, LUA_DISPATCHER_METATABLE);
    lua_setmetatable(xstate->state, -2);
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

void LuaDispatcher::Cleanup(lua_State* L) {
    if (this->_events.empty()) return;
    lua_State* state = L != nullptr ? L : this->state->state;

    for (const auto& pair : this->_events) {
        for (int ref : pair.second) {
            luaL_unref(state, LUA_REGISTRYINDEX, ref);
        }
    }
}

int LuaDispatcher::GC(lua_State* L) {
    LuaDispatcher* dispatcher = CheckInstance(L);
    dispatcher->Cleanup(L);
    return 0;
}
