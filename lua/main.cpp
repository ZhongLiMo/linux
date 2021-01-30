#include "LuaScript.h"
#include <cstdio>
#include "check.h"
#include "json.h"
#include <iostream> 
#include <fstream> 

using namespace std;

int main()
{
	CLuaScript* luaScript = CLuaScript::LuaScript();

	CHECKF(luaScript->Init());
	
	printf("%d\n", luaScript->jiafa(10, 11));
	luaScript->dayin(10);

	return 0;
}