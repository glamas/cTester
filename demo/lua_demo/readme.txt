windows 下到处dll，需要在对应的函数增加 __declspec(dllexport)
也可以在.h文件声明。类似于：

#ifdef _WIN32
    #define DECLSPEC __declspec(dllexport)
#else
    #define DECLSPEC
#endif

再修饰导出的luaopen_*函数。
DECLSPEC int luaopen_api(lua_State *L);
就可以被lua文件直接require了


如果不导出动态库，需要在c代码中，调用luaL_requiref 进行注册
后面在c中调用lua脚本，也可以直接require了


cmake导出的库默认带lib前缀，所以在set_target_properties里需要加PREFIX设置：
set_target_properties(${lib_name} PROPERTIES
    LINKER_LANGUAGE C
    ENABLE_EXPORTS ON
    PREFIX ""
    OUTPUT_NAME ${lib_name})
到处的动态库名称需要和luaopen_*的星号部分一致，lua脚本才会正确识别

c程序里可以通过luaL_loadfile加载lua脚本文件，或者luaL_loadstring加载脚本内容，
再通过lua_pcall执行。


可以通过lua_error，触发异常

使用lua_getglobal 来获取lua的全局函数或者全局变量，再通过栈，操作参数。
使用lua_pcall 执行函数，并判断返回值。
