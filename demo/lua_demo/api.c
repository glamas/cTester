// filename: api.c
#include "api.h"

static int f_hello (lua_State *L) {
    printf("hello world\n");
    return 0;
}

static int f_sum (lua_State *L){
    int n = lua_gettop(L);
    lua_Number sum = 0.0;
    int i;
    for (i = 1; i<= n; i++) {
        if (!lua_isnumber(L, i)) {
            lua_pushliteral(L, "incorrect argument");
            lua_error(L);
        }
        sum += lua_tonumber(L, i);
    }
    lua_pushnumber(L, sum);
    lua_pushnumber(L, sum / n);
    return 2;
}

static const struct luaL_Reg lib[] = {
    {"hello", f_hello},
    {"sum", f_sum},
    {NULL, NULL}
};

// loader 函数
int luaopen_api(lua_State *L){
    luaL_checkversion(L);
    luaL_newlib(L, lib);
    return 1;
}

void luareg_api(lua_State *L) {
    luaL_checkversion(L);
    luaL_requiref(L, "api", luaopen_api, 1);
}
