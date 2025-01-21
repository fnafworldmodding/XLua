#include "runtime.h"
#include "frame.h"
#include "helper.h"

int Runtime::IndexMetamethod (lua_State* L) {
	if (lua_tointeger(L, lua_upvalueindex(UV_TYPE)) != TYPE_STATIC)
		return 0;

	const char* key = lua_tostring(L, 2);

	int ret = StandardIndex(L, RuntimeRead, RuntimeWrite);
	if (ret > -1) return ret;

	int ret = StandardNewIndex(L, CommonFrameRead, CommonFrameWrite);
	if (ret > -1) return ret;

	ret = Globals::GlobalsIndex(L);
	if (ret > -1) return ret;

	return 0;
}

int Runtime::NewIndexMetamethod (lua_State* L) {
	if (lua_tonumber(L, lua_upvalueindex(UV_TYPE)) != TYPE_STATIC)
		return 0;

	const char* key = lua_tostring(L, 2);

	int ret = StandardNewIndex(L, RuntimeRead, RuntimeWrite);
	if (ret > -1) return ret;

	ret = Globals::GlobalsNewIndex(L);
	if (ret > -1) return ret;

	return 0;
}

int Runtime::MemoClosure (lua_State* L, const char* key, lua_CFunction lfunc) {
	return ::MemoClosure(L, key, lfunc, 2);
}

int Runtime::NewRuntime (lua_State* L) {
	LPRH rh = xlua_get_run_header(L);

	int base = lua_gettop(L);								// +0
	if (base <= 0)
		return 0;

	lua_checkstack(L, base + 10);

	int (*index_method)(lua_State*L) = Runtime::IndexMetamethod;
	int (*newindex_method)(lua_State*L) = Runtime::NewIndexMetamethod;

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

int Runtime::FrameRate (lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	lua_pushnumber(L, rh->rhApp->m_hdr.gaFrameRate);
	return 1;
}

// -----

int Runtime::PlayerScore (lua_State* L) {
	return Runtime::MemoClosure(L, lua_tostring(L, 2), Runtime::PlayerScoreFunc);
}

int Runtime::PlayerScoreFunc (lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	if (lua_gettop(L) < 1)
		return 0;

	int player = lua_tointeger(L, 1);
	if (player < 1 || player > 4)
		return 0;

	CRunApp* app = GetParentApp(rh);

	lua_pushinteger(L, ~app->m_pScores[player - 1]);
	return 1;
}

int Runtime::PlayerLives (lua_State* L) {
	return Runtime::MemoClosure(L, lua_tostring(L, 2), Runtime::PlayerLivesFunc);
}

int Runtime::PlayerLivesFunc (lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	if (lua_gettop(L) < 1)
		return 0;

	int player = lua_tointeger(L, 1);
	if (player < 1 || player > 4)
		return 0;

	CRunApp* app = GetParentApp(rh);

	lua_pushinteger(L, ~app->m_pLives[player - 1]);
	return 1;
}

// -----

int Runtime::SetPlayerScore (lua_State* L) {
	return Runtime::MemoClosure(L, lua_tostring(L, 2), Runtime::SetPlayerScoreFunc);
}


int Runtime::SetPlayerScoreFunc (lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	if (lua_gettop(L) < 2)
		return 0;

	int player = lua_tointeger(L, 1);
	if (player < 1 || player > 4)
		return 0;

	int score = lua_tointeger(L, 2);

	CRunApp* app = GetParentApp(rh);

	app->m_pScores[player - 1] = ~score;
	return 0;
}

int Runtime::SetPlayerLives (lua_State* L) {
	return Runtime::MemoClosure(L, lua_tostring(L, 2), Runtime::SetPlayerLivesFunc);
}


int Runtime::SetPlayerLivesFunc (lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;

	if (lua_gettop(L) < 2)
		return 0;

	int player = lua_tointeger(L, 1);
	if (player < 1 || player > 4)
		return 0;

	int lives = lua_tointeger(L, 2);

	CRunApp* app = GetParentApp(rh);

	app->m_pLives[player - 1] = ~lives;
	return 0;
}

int Runtime::FramesCount(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh || !rh->rhFrame || !rh->rhFrame->m_name) return 0;

	CRunApp* app = GetParentApp(rh);

	lua_pushinteger(L, app->m_frameMaxIndex);
	return 1;
}

// ----

int Runtime::RestartGame(lua_State* L) {
    LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
    if (!rh) return 0;

    rh->rhQuit = 4;
    return 0;
}

int Runtime::PauseGame(lua_State* L) {
    LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
    if (!rh) return 0;

    rh->rhQuit = 5;
    return 0;
}

int Runtime::CloseGame(lua_State* L) {
    LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
    if (!rh) return 0;

    rh->rhQuit = 6;
    return 0;
}

/////////////////////////////////////////

int Runtime::LoadImageFromPath(lua_State* L) {
	LPRH rh = (LPRH)lua_touserdata(L, lua_upvalueindex(UV_STATIC_RH));
	if (!rh) return 0;
	mv* p_mv = rh->rh4.rh4Mv;

	// Get the path from the Lua stack
    const char* path = lua_tostring(L, 1);
    if (!path) {
        luaL_error(L, "path is required");
        return 0;
    }

	// Get optional arguments from the Lua stack or use default values
	int hotSpotX = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : 0;
	int hotSpotY = lua_isnumber(L, 3) ? lua_tointeger(L, 3) : 0;
	int actionPointX = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;
	int actionPointY = lua_isnumber(L, 5) ? lua_tointeger(L, 5) : 0;
	int loadflags = lua_isnumber(L, 6) ? lua_tointeger(L, 6) : LI_NONE;

	// Load the image using the helper function with mv pointer
	lua_pushnumber(L, CreateImageFromFile(p_mv, path, hotSpotX, hotSpotY, actionPointX, actionPointY, (LIFlags)loadflags));

	return 1; // Number of return values
}