#ifndef LUADISPATCHER_H_
#define LUADISPATCHER_H_

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include	"lua.hpp"

class XLuaState;

#define LUA_DISPATCHER_METATABLE "xlua.LuaDispatcher.metatable"

class LuaDispatcher {
public:
    std::unordered_map<std::string, std::vector<int>> _events{};
    XLuaState* state;

    LuaDispatcher(XLuaState* lua_state);
    LuaDispatcher();
    void SetState(XLuaState* L);

    static void Register(XLuaState* xluas);
    void Subscribe(const std::string& eventName, int funcRef);
    bool Unsubscribe(const std::string& eventName, int funcToUnsubscribeIdx);
    void Dispatch(const std::string& eventName, int argCount);
    void Cleanup(lua_State* L = nullptr);

    static int NewInstance(XLuaState* xstate, LuaDispatcher*& dispatcher);
    static LuaDispatcher* CheckInstance(lua_State* L, int index = 1);
    static int SubscribeWrapper(lua_State* L);
    static int UnsubscribeWrapper(lua_State* L);
    static int DispatchWrapper(lua_State* L);
    static int GC(lua_State* L);
};
/*
static const luaL_Reg dispatcher_methods[] = {
    {"subscribe", LuaDispatcher::SubscribeWrapper},
    {"unsubscribe", LuaDispatcher::UnsubscribeWrapper},
    {"dispatch", LuaDispatcher::DispatchWrapper},
    {NULL, NULL}
};
*/

#endif // LUADISPATCHER_H_
