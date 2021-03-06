#include "script_engine.h"

#include <iostream>

namespace {
bool scriptRunWasValid(const sol::protected_function_result &result)
{
    if (result.valid()) {
        std::cout << "Script string ran sucessfully." << '\n';
    }
    else {
        sol::error err = result;
        std::cerr << "Lua script string invalid. Error: " << err.what() << '\n';
    }
    return result.valid();
}
} // namespace

ScriptEngine::ScriptEngine()
    : gameTable(lua["game"].get_or_create<sol::table>())
{
    lua.open_libraries(sol::lib::base, sol::lib::math);
}

bool ScriptEngine::runLuaString(const std::string &script)
{
    auto result = lua.script(script, [](auto L, auto res) {
        (void)L;
        return res;
    });

    return scriptRunWasValid(result);
}

bool ScriptEngine::runLuaFile(const std::string &path)
{
    auto script = lua.load_file(path);
    if (script.valid()) {
        auto result = script();
        return scriptRunWasValid(result);
    }
    else {
        sol::error err = script;
        std::cerr << "Lua script file invalid: " << path
                  << "Error: " << err.what() << '\n';
        return false;
    }
}

sol::function ScriptEngine::getLuaFunction(const char *functionName)
{
    return gameTable[functionName];
}
