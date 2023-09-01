#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "api.h"

int main(void){
    char buff[256];
    int error;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luareg_api(L);

    // loadfile
    if (luaL_loadfile(L, "test.lua") || lua_pcall(L, 0, 0, 0)){
        printf("%s", lua_tostring(L, -1));
    }
    // call lua function
    lua_getglobal(L, "add");
    lua_pushnumber(L, 10);
    lua_pushnumber(L, 20);
    int result = lua_pcall(L, 2, 1, 0);
    if (result != 0) {
        printf("error %s\n", lua_tostring(L, -1));
        return -1;
    }
    int val = lua_tonumber(L, -1);
    printf("val: %d\n", val);
    lua_pop(L, -1);
    // while (fgets(buff, sizeof(buff), stdin) != NULL){
    //     error = luaL_loadbuffer(L, buff, strlen(buff), "line") ||
    //         lua_pcall(L, 0, 0, 0);
    //     if (error) {
    //         fprintf(stderr, "%s", lua_tostring(L, -1));
    //         lua_pop(L, 1);
    //     }
    // }
    lua_close(L);
    return 0;
}
