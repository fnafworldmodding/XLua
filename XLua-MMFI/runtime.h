#ifndef RUNTIME_H_
#define RUNTIME_H_

#include "common.h"

class Runtime {
public:

	static int	NewRuntime (lua_State* L);


	// Getters 

	static int	FrameRate (lua_State* L);
	// Frames Getters

	static int  StartingFrameId(lua_State* L);
	static int  NextFrameId(lua_State* L);
	static int  CurrentFrameId(lua_State* L);
	static int  CurrentFrameName(lua_State* L);
	static int  FramesCount(lua_State* L);

	// Queries

	static int	PlayerScore (lua_State* L);
	static int	PlayerScoreFunc (lua_State* L);

	static int	PlayerLives (lua_State* L);
	static int	PlayerLivesFunc (lua_State* L);

	// Actions

	static int	SetPlayerScore (lua_State* L);
	static int	SetPlayerScoreFunc (lua_State* L);

	static int	SetPlayerLives (lua_State* L);
	static int	SetPlayerLivesFunc (lua_State* L);

	// Frame Actions
	static int	NextFrame(lua_State* L);
	static int	PreviousFrame(lua_State* L);
	static int	JumpToFrame(lua_State* L);
	static int	RestartFrame(lua_State* L);

	// App Actions
	static int	RestartGame(lua_State* L);
	static int	PauseGame(lua_State* L);
	static int	CloseGame(lua_State* L);

public:

	static int	IndexMetamethod (lua_State* L);
	static int	NewIndexMetamethod (lua_State* L);

	static int	MemoClosure (lua_State* L, const char* key, lua_CFunction lfunc);
	//
	static inline CRunApp* Runtime::GetParentApp (LPRH rh);
};

const FunctionPair RuntimeRead[] = {
	{ "frameRate",		Runtime::FrameRate },
	{ "playerLives",	Runtime::PlayerLives },
	{ "playerScore",	Runtime::PlayerScore },
	{ "setPlayerLives",	Runtime::SetPlayerLives },
	{ "setPlayerScore",	Runtime::SetPlayerScore },
};

const FunctionPair RuntimeWrite[] = {
	{ 0, 0 },
};

class Globals {
public:
	
	static int	Value (lua_State* L);
	static int	String (lua_State* L);

	static int	SetValue (lua_State* L);
	static int	SetString (lua_State* L);

public:

	static int	GlobalsIndex (lua_State* L);
	static int	GlobalsNewIndex (lua_State* L);

	static int	SubIndexMetamethod (lua_State* L);
	static int	SubNewIndexMetamethod (lua_State* L);

	static int	ValueKeyLookup (lua_State* L);
	static int	StringKeyLookup (lua_State* L);
};

#endif
