#ifndef LUA_LIBRARY_H_GUARD
#define LUA_LIBRARY_H_GUARD

#define ERROR_INVALID_NUMBER_OF_ARGUMENTS	0
#define ERROR_INVALID_ARGUMENTS				1
#define ERROR_INVALID_PARAMETERS			2
#define ERROR_TOO_MANY_OBJECTS				3
#define ERROR_INDEX_OUT_OF_RANGE			4
#define ERROR_COLOR_WITHOUT_ANY_OBJECTS		5
#define ERROR_TEXTURE_WITHOUT_ANY_OBJECTS	6
#define ERROR_TEXTURE_LOAD_ERROR			7
#define ERROR_MAPPING_WITHOUT_TEXTURE		8

struct lua_State;
struct color_t;

void raiseError(lua_State *lua,int errorType,const char* funcName);
void checkArgumentNumber(lua_State *lua,int expected,const char* funcName);
int getVector(double vec[3],lua_State *lua,int index);
int getColor(color_t* col,lua_State* lua,int index);
int getNumberOrNil(int* isNumber,double* number,lua_State* lua,int index);
int getNumber(double* number,lua_State* lua,int index);

int scalarMultiple(lua_State *lua);
int vectorAdd(lua_State *lua);
int vectorSub(lua_State *lua);
int innerProduct(lua_State *lua);
int exteriorProduct(lua_State *lua);
int vectorNormalize(lua_State *lua);
int vectorLength(lua_State *lua);
int vectorRotateXY(lua_State *lua);
int vectorRotateYZ(lua_State *lua);
int vectorRotateZX(lua_State *lua);
int makeColor(lua_State *lua);
int makeColorFromInt(lua_State *lua);

#endif
