#include "frame.h"

int CommonFrame::IndexMetamethod(lua_State* L) {
	if (lua_tointeger(L, lua_upvalueindex(UV_TYPE)) != TYPE_STATIC)
		return 0;

	const char* key = lua_tostring(L, 2);

	int ret = StandardIndex(L, CommonFrameRead, CommonFrameWrite);
	if (ret > -1) return ret;

	return 0;
}

int CommonFrame::NewIndexMetamethod(lua_State* L) {
	if (lua_tonumber(L, lua_upvalueindex(UV_TYPE)) != TYPE_STATIC)
		return 0;

	const char* key = lua_tostring(L, 2);

	int ret = StandardNewIndex(L, CommonFrameRead, CommonFrameWrite);
	if (ret > -1) return ret;

	return 0;
}

int CommonFrame::MemoClosure(lua_State* L, const char* key, lua_CFunction lfunc) {
	return ::MemoClosure(L, key, lfunc, 2);
}

int CommonFrame::NewCommonFrame(lua_State* L) {
	LPRH rh = xlua_get_run_header(L);

	int base = lua_gettop(L);								// +0
	if (base <= 0)
		return 0;

	lua_checkstack(L, base + 10);

	int (*index_method)(lua_State * L) = CommonFrame::IndexMetamethod;
	int (*newindex_method)(lua_State * L) = CommonFrame::NewIndexMetamethod;

	// Create object table
	lua_createtable(L, 0, 0);								// +1 = Object Table

	// Create object metatable
	lua_createtable(L, 0, 2);								// +2 = Object Metatable

	// Add __index
	lua_pushnumber(L, TYPE_STATIC);							// +3
	lua_pushlightuserdata(L, (void*)rh);					// +4
	lua_pushcclosure(L, index_method, 2);					// +3
	lua_setfield(L, -2, "__index");							// +2

	// Add __newindex
	lua_pushnumber(L, TYPE_OBJECT);							// +3
	lua_pushlightuserdata(L, (void*)rh);					// +4
	lua_pushcclosure(L, newindex_method, 2);				// +3
	lua_setfield(L, -2, "__newindex");						// +2

	// Add __metatable
	lua_pushstring(L, "Access violation");					// +3
	lua_setfield(L, -2, "__metatable");						// +2

	// Bind metatable
	lua_setmetatable(L, -2);								// +1

	return 1;
}

// -----

int CommonFrame::StartingFrameId(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	CRunApp* app = GetParentApp(rh);
	lua_pushinteger(L, app->m_startFrame);
	return 1;
}

int CommonFrame::NextFrameId(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	CRunApp* app = GetParentApp(rh);
	lua_pushinteger(L, app->m_nextFrame);
	return 1;
}

int CommonFrame::CurrentFrameId(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	CRunApp* app = GetParentApp(rh);
	lua_pushinteger(L, app->m_nCurrentFrame);
	return 1;
}

int CommonFrame::CurrentFrameName(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh || !rh->rhFrame || !rh->rhFrame->m_name) return 0;

	lua_pushstring(L, rh->rhFrame->m_name);
	return 1;
}

// ----

int CommonFrame::NextFrame(lua_State* L) {
	return CommonFrame::MemoClosure(L, lua_tostring(L, 2), CommonFrame::NextFrameFunc);
}

int CommonFrame::NextFrameFunc(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	rh->rhQuit = 1;
	return 0;
}

int CommonFrame::PreviousFrame(lua_State* L) {
	return CommonFrame::MemoClosure(L, lua_tostring(L, 2), CommonFrame::PreviousFrameFunc);
}

int CommonFrame::PreviousFrameFunc(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	rh->rhQuit = 2;
	return 0;
}

int CommonFrame::JumpToFrame(lua_State* L) {
	return CommonFrame::MemoClosure(L, lua_tostring(L, 2), CommonFrame::JumpToFrameFunc);
}

int CommonFrame::JumpToFrameFunc(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	if (lua_gettop(L) < 1)
		return 0;

	int frame_id = lua_tointeger(L, 1);
	rh->rhQuit = 3;
	rh->rhQuitParam = frame_id;
	return 0;
}

int CommonFrame::RestartFrame(lua_State* L) {
	return CommonFrame::MemoClosure(L, lua_tostring(L, 2), CommonFrame::RestartFrameFunc);
}

int CommonFrame::RestartFrameFunc(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	rh->rhQuit = 101;
	return 0;
}

