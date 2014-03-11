#include <math.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "script_loader.h"
#include "lua_library.h"

/*
 * �G���[�𓊂���
 * @param lua Lua���C�u�����̕ϐ�
 * @param errorType �G���[�̎��
 * @param funcName �֐���
 */
void raiseError(lua_State *lua,int errorType,const char* funcName) {
	static const char* message[]={
		"invalid number of arguments",
		"invalid arguments",
		"invalid parameter(s)",
		"too many objects",
		"index out of range",
		"color without any objects",
		"texture without any objects",
		"texture load error",
		"mapping without texture"
	};
	luaL_error(lua,"%s in %s",message[errorType],funcName);
}

/*
 * �����̐����`�F�b�N����
 * @param lua Lua���C�u�����̕ϐ�
 * @param expected �z�肷������̐�
 * @param funcName �֐���
 */
void checkArgumentNumber(lua_State *lua,int expected,const char* funcName) {
	if(lua_gettop(lua)!=expected) {
		raiseError(lua,ERROR_INVALID_NUMBER_OF_ARGUMENTS,funcName);
	}
}

/*
 * Lua�̃��X�g���x�N�g���Ƃ��Ď擾����B
 * @param vec �o�͂̃x�N�g��
 * @param lua Lua���C�u�����̕ϐ�
 * @param index �擾���̃��X�g�̃X�^�b�N��̈ʒu
 * @return ����Ɏ擾�ł�����1�A�擾�ł��Ȃ����0
 */
int getVector(double vec[3],lua_State *lua,int index) {
	int i;
	/* �w�肳�ꂽ���̂��e�[�u���łȂ���΃G���[ */
	if(!lua_istable(lua,index))return 0;
	/* �e�[�u���̃T�C�Y��3�łȂ���΃G���[ */
	if(luaL_len(lua,index)!=3)return 0;
	/* �l���擾����B�擾�ł��Ȃ���΃G���[ */
	for(i=0;i<3;i++) {
		int okFlag=0;
		lua_pushinteger(lua,i+1);				/* �C���f�b�N�X���w�� */
		lua_gettable(lua,index);				/* �l���擾 */
		vec[i]=lua_tonumberx(lua,-1,&okFlag);	/* �����ɕϊ� */
		lua_pop(lua,1);							/* �o�b�t�@������ */
		if(!okFlag)return 0;					/* �擾�ł������`�F�b�N */
	}
	return 1;
}

/*
 * Lua�̃��X�g��F�Ƃ��Ď擾����B
 * @param vec �o�͂̐F
 * @param lua Lua���C�u�����̕ϐ�
 * @param index �擾���̃��X�g�̃X�^�b�N��̈ʒu
 * @return ����Ɏ擾�ł�����1�A�擾�ł��Ȃ����0
 */
int getColor(color_t* col,lua_State* lua,int index) {
	int i;
	double vec[3];
	if(!getVector(vec,lua,index))return 0;
	for(i=0;i<3;i++) {
		if(vec[i]<=0)vec[i]=0;
		if(vec[i]>1)vec[i]=1;
	}
	col->r=vec[0];
	col->g=vec[1];
	col->b=vec[2];
	return 1;
}

/*
 * Lua�̎����܂���nil�ł���l���擾����
 * @param isNumber �l�������Ȃ�1�Anil�Ȃ�0������|�C���^
 * @param number �擾��������������|�C���^
 * @param lua Lua���C�u�����̕ϐ�
 * @param index Lua�̃X�^�b�N��̈ʒu
 * @return �擾�ł�����1�A�擾�ł��Ȃ����0
 */
int getNumberOrNil(int* isNumber,double* number,lua_State* lua,int index) {
	int isNumberInternal=0;
	if(lua_isnil(lua,index)) {
		*isNumber=0;
		return 1;
	}
	*number=lua_tonumberx(lua,index,&isNumberInternal);
	if(!isNumberInternal)return 0;
	*isNumber=1;
	return 1;
}

/*
 * Lua�̎����l���擾����
 * @param number �擾��������������|�C���^
 * @param lua Lua���C�u�����̕ϐ�
 * @param index Lua�̃X�^�b�N��̈ʒu
 * @return �擾�ł�����1�A�擾�ł��Ȃ����0
 */
int getNumber(double* number,lua_State* lua,int index) {
	int isNumber=0;
	*number=lua_tonumberx(lua,index,&isNumber);
	if(!isNumber)return 0;
	return 1;
}

/* vector scalarMultiple(number a,vector b) */
int scalarMultiple(lua_State *lua) {
	static const char* funcName="scalarMultiple";
	double a;
	double b[3];
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,funcName);
	okFlag=okFlag && getNumber(&a,lua,1);
	okFlag=okFlag && getVector(b,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,a*b[i]);
		lua_settable(lua,3);
	}
	return 1;
}

/* vector vectorAdd(vector a,vector b) */
int vectorAdd(lua_State *lua) {
	static const char* funcName="vectorAdd";
	double a[3];
	double b[3];
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,funcName);
	okFlag=okFlag && getVector(a,lua,1);
	okFlag=okFlag && getVector(b,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,a[i]+b[i]);
		lua_settable(lua,3);
	}
	return 1;
}

/* vector vectorSub(vector a,vector b) */
int vectorSub(lua_State *lua) {
	static const char* fileName="vectorSub";
	double a[3];
	double b[3];
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,fileName);
	okFlag=okFlag && getVector(a,lua,1);
	okFlag=okFlag && getVector(b,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,fileName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,a[i]-b[i]);
		lua_settable(lua,3);
	}
	return 1;
}

/* number innerProduct(vector a,vector b) */
int innerProduct(lua_State *lua) {
	static const char* fileName="innerProduct";
	double a[3];
	double b[3];
	int okFlag=1;
	checkArgumentNumber(lua,2,fileName);
	okFlag=okFlag && getVector(a,lua,1);
	okFlag=okFlag && getVector(b,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,fileName);
	lua_pushnumber(lua,a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
	return 1;
}

/* vector exteriorProduct(vector a,vector b) */
int exteriorProduct(lua_State *lua) {
	static const char* fileName="exteriorProduct";
	double a[3];
	double b[3];
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,fileName);
	okFlag=okFlag && getVector(a,lua,1);
	okFlag=okFlag && getVector(b,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,fileName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,a[(i+1)%3]*b[(i+2)%3]-a[(i+2)%3]*b[(i+1)%3]);
		lua_settable(lua,3);
	}
	return 1;
}

/* vector vectorNormalize(vector a) */
int vectorNormalize(lua_State *lua) {
	static const char* fileName="vectorNormalize";
	double a[3];
	double len;
	int i;
	checkArgumentNumber(lua,1,fileName);
	if(!getVector(a,lua,1))raiseError(lua,ERROR_INVALID_ARGUMENTS,fileName);
	len=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
	if(len<=0)raiseError(lua,ERROR_INVALID_PARAMETERS,fileName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,a[i]/len);
		lua_settable(lua,2);
	}
	return 1;
}

/* number vectorLength(vector a) */
int vectorLength(lua_State *lua) {
	static const char* funcName="vectorLength";
	double a[3];
	checkArgumentNumber(lua,1,funcName);
	if(!getVector(a,lua,1))raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	lua_pushnumber(lua,sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]));
	return 1;
}

/* vector vectorRotateXY(number angle,vector a) */
int vectorRotateXY(lua_State *lua) {
	static const char* funcName="vectorRotateXY";
	double angle;
	double a[3];
	double ret[3];
	double sine,cosine;
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,funcName);
	okFlag=okFlag && getNumber(&angle,lua,1);
	okFlag=okFlag && getVector(a,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	sine=sin(angle);cosine=cos(angle);
	ret[0]=cosine*a[0]-sine*a[1];
	ret[1]=sine*a[0]+cosine*a[1];
	ret[2]=a[2];

	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,ret[i]);
		lua_settable(lua,3);
	}
	return 1;
}

/* vector vectorRotateYZ(number angle,vector a) */
int vectorRotateYZ(lua_State *lua) {
	static const char* funcName="vectorRotateYZ";
	double angle;
	double a[3];
	double ret[3];
	double sine,cosine;
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,funcName);
	okFlag=okFlag && getNumber(&angle,lua,1);
	okFlag=okFlag && getVector(a,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	sine=sin(angle);cosine=cos(angle);
	ret[1]=cosine*a[1]-sine*a[2];
	ret[2]=sine*a[1]+cosine*a[2];
	ret[0]=a[0];

	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,ret[i]);
		lua_settable(lua,3);
	}
	return 1;
}

/* vector vectorRotateZX(number angle,vector a) */
int vectorRotateZX(lua_State *lua) {
	static const char* funcName="vectorRotateZX";
	double angle;
	double a[3];
	double ret[3];
	double sine,cosine;
	int okFlag=1;
	int i;
	checkArgumentNumber(lua,2,funcName);
	okFlag=okFlag && getNumber(&angle,lua,1);
	okFlag=okFlag && getVector(a,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	sine=sin(angle);cosine=cos(angle);
	ret[2]=cosine*a[2]-sine*a[0];
	ret[0]=sine*a[2]+cosine*a[0];
	ret[1]=a[1];

	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,ret[i]);
		lua_settable(lua,3);
	}
	return 1;
}

/* vector makeColor(number r,number g,number b) */
int makeColor(lua_State *lua) {
	static const char* funcName="makeColor";
	double a[3];
	int i;
	int okFlag=1;
	checkArgumentNumber(lua,3,funcName);
	for(i=0;okFlag && i<3;i++) {
		a[i]=lua_tonumberx(lua,i+1,&okFlag);
	}
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		if(a[i]<=0)a[i]=0;
		if(a[i]>1)a[i]=1;
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,a[i]);
		lua_settable(lua,4);
	}
	return 1;
}

/* vector makeColorFromInt(integer r,integer g,integer b) */
int makeColorFromInt(lua_State *lua) {
	static const char* funcName="makeColorFromInt";
	int a[3];
	int i;
	int okFlag=1;
	checkArgumentNumber(lua,3,funcName);
	for(i=0;okFlag && i<3;i++) {
		a[i]=lua_tointegerx(lua,i+1,&okFlag);
	}
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	lua_createtable(lua,3,0);
	for(i=0;i<3;i++) {
		if(a[i]<0)a[i]=0;
		if(a[i]>255)a[i]=255;
		lua_pushinteger(lua,i+1);
		lua_pushnumber(lua,(double)a[i]/255.0);
		lua_settable(lua,4);
	}
	return 1;
}
