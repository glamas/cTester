// filename: api.h

#ifndef _API_H_
#define _API_H_

#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef _WIN32
    #define DECLSPEC __declspec(dllexport)
#else
    #define DECLSPEC
#endif

DECLSPEC int luaopen_api(lua_State *L);
void luareg_api(lua_State *L);

#endif
