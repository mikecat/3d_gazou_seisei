#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "script_loader.h"
#include "lua_library.h"

const color_t colBlack={0,0,0};
world_t theWorld;

static int theParamNum;
static char** theParams;

/*
 * テクスチャのPNGファイルを読み込み、オブジェクトにデータを設定する
 * @param obj 設定されるオブジェクト
 * @param textureName 設定するテクスチャのファイル名
 * @return 成功すれば真、失敗すれば偽
 */
static int setTexture(object_t* obj,const char* textureName) {
	FILE* fp;
	png_structp pngPtr;
	png_infop pngInfoPtr;
	png_bytepp imageData;
	int width,height;
	int alphaExist;
	int okFlag=1;
	pngPtr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if(!pngPtr)return 0;
	pngInfoPtr=png_create_info_struct(pngPtr);
	if(!pngInfoPtr) {
		png_destroy_read_struct(&pngPtr,NULL,NULL);
	}
	fp=fopen(textureName,"rb");
	if(fp==NULL)return 0;
	if(setjmp(png_jmpbuf(pngPtr))) {
		png_destroy_read_struct(&pngPtr,&pngInfoPtr,NULL);
		fclose(fp);
		return 0;
	}
	png_init_io(pngPtr,fp);
	png_read_png(pngPtr,pngInfoPtr,
		PNG_TRANSFORM_PACKING | PNG_TRANSFORM_SCALE_16 |
		PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_GRAY_TO_RGB,
		NULL);
	imageData=png_get_rows(pngPtr,pngInfoPtr);
	width=png_get_image_width(pngPtr,pngInfoPtr);
	height=png_get_image_height(pngPtr,pngInfoPtr);
	alphaExist=png_get_color_type(pngPtr,pngInfoPtr);
	alphaExist=(alphaExist==PNG_COLOR_TYPE_GRAY_ALPHA ||
		alphaExist==PNG_COLOR_TYPE_RGB_ALPHA);
	if(obj==NULL) {
		okFlag=0;
	} else {
		obj->tWidth=width;
		obj->tHeight=height;
		obj->texture=malloc(sizeof(int)*width*height);
		if(obj->texture==NULL) {
			okFlag=0;
		} else {
			int x,y;
			for(y=0;y<height;y++) {
				for(x=0;x<width;x++) {
					/* テクスチャの右下を(0,0)にするため、反転 */
					int index=(height-1-y)*width+x;
					if(alphaExist) {
						obj->texture[index].r=imageData[y][x*4];
						obj->texture[index].g=imageData[y][x*4+1];
						obj->texture[index].b=imageData[y][x*4+2];
						obj->texture[index].a=imageData[y][x*4+3];
					} else {
						obj->texture[index].r=imageData[y][x*3];
						obj->texture[index].g=imageData[y][x*3+1];
						obj->texture[index].b=imageData[y][x*3+2];
						obj->texture[index].a=255;
					}
				}
			}
		}
	}
	png_destroy_read_struct(&pngPtr,&pngInfoPtr,NULL);
	fclose(fp);
	return okFlag;
}

/*
 * オブジェクトの色情報を初期化する
 * @param obj 初期化するオブジェクト
 */
static void initObjectColor(object_t* obj) {
	if(obj->texture!=NULL)free(obj->texture);
	obj->useTexture=0;
	obj->color=colBlack;
	obj->tWidth=0;
	obj->tHeight=0;
	obj->texture=NULL;
	obj->toffset[0]=0;obj->toffset[1]=0;
	obj->tdx[0]=1;obj->tdx[1]=0;
	obj->tdy[0]=0;obj->tdy[1]=1;
	obj->useTextureAlpha=0;
	obj->alpha=1;
	obj->lightAlpha=0;
}

/* 以下の型表記で、vectorは3要素のリストを表す */

/* integer getCommandParamNum() */
int getCommandParamNum(lua_State *lua) {
	checkArgumentNumber(lua,0,"getCommandParamNum");
	lua_pushinteger(lua,theParamNum);
	return 1;
}

/* string getCommandParam(integer index) */
int getCommandParam(lua_State *lua) {
	static const char* funcName="getCommandParam";
	int okFlag=1;
	int index;
	checkArgumentNumber(lua,1,funcName);
	index=lua_tointegerx(lua,1,&okFlag);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	if(index<1 || theParamNum<index) {
		raiseError(lua,ERROR_INDEX_OUT_OF_RANGE,funcName);
	}
	lua_pushstring(lua,theParams[index-1]);
	return 1;
}

/* integer getMaxObjectNum() */
int getMaxObjectNum(lua_State *lua) {
	checkArgumentNumber(lua,0,"getMaxObjectNum");
	lua_pushinteger(lua,OBJECT_MAX);
	return 1;
}

/* void setCamera(vector pos,vector dir,vector dx,vector dy) */
int setCamera(lua_State *lua) {
	static const char* funcName="setCamera";
	int okFlag=1;
	checkArgumentNumber(lua,4,funcName);
	okFlag=okFlag && getVector(theWorld.camera.pos,lua,1);
	okFlag=okFlag && getVector(theWorld.camera.dir,lua,2);
	okFlag=okFlag && getVector(theWorld.camera.dx,lua,3);
	okFlag=okFlag && getVector(theWorld.camera.dy,lua,4);
	if(!okFlag) {
		raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	}
	return 0;
}

/* void setLight(vector pos,vector color) */
int setLight(lua_State *lua) {
	static const char* funcName="setLight";
	int okFlag=1;
	checkArgumentNumber(lua,2,funcName);
	okFlag=okFlag && getVector(theWorld.light.pos,lua,1);
	okFlag=okFlag && getColor(&theWorld.light.color,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	return 0;
}

/* void setBackgroundColor(vector backGroundColor) */
int setBackgroundColor(lua_State *lua) {
	static const char* funcName="setBackgroundColor";
	int okFlag=1;
	checkArgumentNumber(lua,1,funcName);
	okFlag=okFlag && getColor(&theWorld.backGroundColor,lua,1);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	return 0;
}

/* void setTtl(integer ttl) */
int setTtl(lua_State *lua) {
	static const char* funcName="setTtl";
	int okFlag=0;
	checkArgumentNumber(lua,1,funcName);
	theWorld.ttl=lua_tointegerx(lua,1,&okFlag);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	if(theWorld.ttl<0)raiseError(lua,ERROR_INVALID_PARAMETERS,funcName);
	return 0;
}

/* void putSphere(vector center,number r) */
int putSphere(lua_State *lua) {
	static const char* funcName="putSphere";
	int okFlag=1;
	checkArgumentNumber(lua,2,funcName);
	if(theWorld.objectNum>=OBJECT_MAX) {
		raiseError(lua,ERROR_TOO_MANY_OBJECTS,funcName);
	}
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.s.c,lua,1);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum].pos.s.r,lua,2);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	if(theWorld.objects[theWorld.objectNum].pos.s.r<=0) {
		raiseError(lua,ERROR_INVALID_PARAMETERS,funcName);
	}
	theWorld.objects[theWorld.objectNum].objType=OBJTYPE_SPHERE;
	initObjectColor(&theWorld.objects[theWorld.objectNum]);
	theWorld.objectNum++;
	return 0;
}

/* void putPlane(vector pos,vector d1,vector d2,number min1,number max1,number min2,number max2) */
int putPlane(lua_State *lua) {
	static const char* funcName="putPlane";
	int okFlag=1;
	int min1e=0,max1e=0,min2e=0,max2e=0;
	checkArgumentNumber(lua,7,funcName);
	if(theWorld.objectNum>=OBJECT_MAX) {
		raiseError(lua,ERROR_TOO_MANY_OBJECTS,funcName);
	}
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.p.p,lua,1);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.p.d1,lua,2);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.p.d2,lua,3);
	okFlag=okFlag && getNumberOrNil(
		&min1e,&theWorld.objects[theWorld.objectNum].pos.p.min1,lua,4);
	okFlag=okFlag && getNumberOrNil(
		&max1e,&theWorld.objects[theWorld.objectNum].pos.p.max1,lua,5);
	okFlag=okFlag && getNumberOrNil(
		&min2e,&theWorld.objects[theWorld.objectNum].pos.p.min2,lua,6);
	okFlag=okFlag && getNumberOrNil(
		&max2e,&theWorld.objects[theWorld.objectNum].pos.p.max2,lua,7);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	theWorld.objects[theWorld.objectNum].pos.p.min1e=min1e;
	theWorld.objects[theWorld.objectNum].pos.p.max1e=max1e;
	theWorld.objects[theWorld.objectNum].pos.p.min2e=min2e;
	theWorld.objects[theWorld.objectNum].pos.p.max2e=max2e;
	theWorld.objects[theWorld.objectNum].objType=OBJTYPE_PLANE;
	initObjectColor(&theWorld.objects[theWorld.objectNum]);
	theWorld.objectNum++;
	return 0;
}

/* void putCylinder(vector pos,vector d,number min,number max,number r) */
int putCylinder(lua_State *lua) {
	static const char* funcName="putCylinder";
	int okFlag=1;
	int mine=0,maxe=0;
	checkArgumentNumber(lua,5,funcName);
	if(theWorld.objectNum>=OBJECT_MAX) {
		raiseError(lua,ERROR_TOO_MANY_OBJECTS,funcName);
	}
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.c.p,lua,1);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.c.d,lua,2);
	okFlag=okFlag && getNumberOrNil(
		&mine,&theWorld.objects[theWorld.objectNum].pos.c.min,lua,3);
	okFlag=okFlag && getNumberOrNil(
		&maxe,&theWorld.objects[theWorld.objectNum].pos.c.max,lua,4);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum].pos.c.r,lua,5);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	if(theWorld.objects[theWorld.objectNum].pos.c.r<=0) {
		raiseError(lua,ERROR_INVALID_PARAMETERS,funcName);
	}

	theWorld.objects[theWorld.objectNum].pos.c.mine=mine;
	theWorld.objects[theWorld.objectNum].pos.c.maxe=maxe;
	theWorld.objects[theWorld.objectNum].objType=OBJTYPE_CYLINDER;
	initObjectColor(&theWorld.objects[theWorld.objectNum]);
	theWorld.objectNum++;
	return 0;
}

/* void putDisk(vector pos,vector d1,vector d2) */
int putDisk(lua_State *lua) {
	static const char* funcName="putDisk";
	int okFlag=1;
	checkArgumentNumber(lua,3,funcName);
	if(theWorld.objectNum>=OBJECT_MAX) {
		raiseError(lua,ERROR_TOO_MANY_OBJECTS,funcName);
	}
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.d.p,lua,1);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.d.d1,lua,2);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.d.d2,lua,3);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	theWorld.objects[theWorld.objectNum].objType=OBJTYPE_DISK;
	initObjectColor(&theWorld.objects[theWorld.objectNum]);
	theWorld.objectNum++;
	return 0;
}

/* void putTriangle(vector pos,vector d1,vector d2,number max1,number max2) */
int putTriangle(lua_State *lua) {
	static const char* funcName="putTriangle";
	int okFlag=1;
	checkArgumentNumber(lua,5,funcName);
	if(theWorld.objectNum>=OBJECT_MAX) {
		raiseError(lua,ERROR_TOO_MANY_OBJECTS,funcName);
	}
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.t.p,lua,1);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.t.d1,lua,2);
	okFlag=okFlag && getVector(theWorld.objects[theWorld.objectNum].pos.t.d2,lua,3);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum].pos.t.max1,lua,4);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum].pos.t.max2,lua,5);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	if(theWorld.objects[theWorld.objectNum].pos.t.max1<=0 ||
	theWorld.objects[theWorld.objectNum].pos.t.max2<=0) {
		raiseError(lua,ERROR_INVALID_PARAMETERS,funcName);
	}

	theWorld.objects[theWorld.objectNum].objType=OBJTYPE_TRIANGLE;
	initObjectColor(&theWorld.objects[theWorld.objectNum]);
	theWorld.objectNum++;
	return 0;
}

/* void setPrevObjectColor(vector color,number alpha,number lightAlpha,number lightPower) */
int setPrevObjectColor(lua_State *lua) {
	static const char* funcName="setPrevObjectColor";
	int okFlag=1;
	checkArgumentNumber(lua,4,funcName);
	if(theWorld.objectNum<=0) {
		raiseError(lua,ERROR_COLOR_WITHOUT_ANY_OBJECTS,funcName);
	}
	initObjectColor(&theWorld.objects[theWorld.objectNum-1]);
	okFlag=okFlag && getColor(&theWorld.objects[theWorld.objectNum-1].color,lua,1);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].alpha,lua,2);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].lightAlpha,lua,3);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].lightPower,lua,4);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	if(theWorld.objects[theWorld.objectNum-1].alpha<=0) {
		theWorld.objects[theWorld.objectNum-1].alpha=0;
	}
	if(theWorld.objects[theWorld.objectNum-1].alpha>1) {
		theWorld.objects[theWorld.objectNum-1].alpha=1;
	}
	if(theWorld.objects[theWorld.objectNum-1].lightAlpha<=0) {
		theWorld.objects[theWorld.objectNum-1].lightAlpha=0;
	}
	if(theWorld.objects[theWorld.objectNum-1].lightAlpha>1) {
		theWorld.objects[theWorld.objectNum-1].lightAlpha=1;
	}
	return 0;
}

/* void setPrevObjectTexture(string textureName,number alpha,number lightAlpha,number lightPower) */
int setPrevObjectTexture(lua_State *lua) {
	static const char* funcName="setPrevObjectTexture";
	int okFlag=1;
	int useAlpha;
	const char* textureName;
	checkArgumentNumber(lua,4,funcName);
	if(theWorld.objectNum<=0) {
		raiseError(lua,ERROR_TEXTURE_WITHOUT_ANY_OBJECTS,funcName);
	}
	initObjectColor(&theWorld.objects[theWorld.objectNum-1]);
	textureName=lua_tostring(lua,1);
	if(textureName==NULL)okFlag=0;
	theWorld.objects[theWorld.objectNum-1].alpha=0;
	okFlag=okFlag && getNumberOrNil(&useAlpha,
		&theWorld.objects[theWorld.objectNum-1].alpha,lua,2);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].lightAlpha,lua,3);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].lightPower,lua,4);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);

	theWorld.objects[theWorld.objectNum-1].useTextureAlpha=(useAlpha?0:1);
	if(theWorld.objects[theWorld.objectNum-1].alpha<=0) {
		theWorld.objects[theWorld.objectNum-1].alpha=0;
	}
	if(theWorld.objects[theWorld.objectNum-1].alpha>1) {
		theWorld.objects[theWorld.objectNum-1].alpha=1;
	}
	if(theWorld.objects[theWorld.objectNum-1].lightAlpha<=0) {
		theWorld.objects[theWorld.objectNum-1].lightAlpha=0;
	}
	if(theWorld.objects[theWorld.objectNum-1].lightAlpha>1) {
		theWorld.objects[theWorld.objectNum-1].lightAlpha=1;
	}
	if(setTexture(&theWorld.objects[theWorld.objectNum-1],textureName)) {
		theWorld.objects[theWorld.objectNum-1].useTexture=1;
	} else {
		raiseError(lua,ERROR_TEXTURE_LOAD_ERROR,funcName);
	}
	return 0;
}

/* void setPrevTextureMapping(number ox,number oy,number xx,number xy,number yx,number yy) */
int setPrevTextureMapping(lua_State* lua) {
	static const char* funcName="setPrevTextureMapping";
	int okFlag=1;
	checkArgumentNumber(lua,6,funcName);
	if(theWorld.objectNum<=0 || !theWorld.objects[theWorld.objectNum-1].useTexture) {
		raiseError(lua,ERROR_MAPPING_WITHOUT_TEXTURE,funcName);
	}
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].toffset[0],lua,1);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].toffset[1],lua,2);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].tdx[0],lua,3);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].tdx[1],lua,4);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].tdy[0],lua,5);
	okFlag=okFlag && getNumber(&theWorld.objects[theWorld.objectNum-1].tdy[1],lua,6);
	if(!okFlag)raiseError(lua,ERROR_INVALID_ARGUMENTS,funcName);
	theWorld.objects[theWorld.objectNum-1].tdx[0]-=theWorld.objects[theWorld.objectNum-1].toffset[0];
	theWorld.objects[theWorld.objectNum-1].tdx[1]-=theWorld.objects[theWorld.objectNum-1].toffset[1];
	theWorld.objects[theWorld.objectNum-1].tdy[0]-=theWorld.objects[theWorld.objectNum-1].toffset[0];
	theWorld.objects[theWorld.objectNum-1].tdy[1]-=theWorld.objects[theWorld.objectNum-1].toffset[1];
	return 0;
}

/*
 * スクリプト(世界やカメラの情報が入ったファイル)を読み込む
 * @param fileName スクリプトのファイル名
 * @param paramNum 追加パラメータの数
 * @param params 追加パラメータの配列
 * @return 成功すれば1、失敗すれば偽
 */
int loadScript(const char* fileName,int paramNum,char* params[]) {
	int okFlag=0;
	lua_State *lua;
	theParamNum=paramNum;
	theParams=params;
	/* 初期値の設定 */
	theWorld.camera.pos[0]=0;theWorld.camera.pos[1]=0;theWorld.camera.pos[2]=0;
	theWorld.camera.dir[0]=0;theWorld.camera.dir[1]=0;theWorld.camera.dir[2]=1;
	theWorld.camera.dx[0]=1;theWorld.camera.dx[1]=0;theWorld.camera.dx[2]=0;
	theWorld.camera.dy[0]=0;theWorld.camera.dy[1]=1;theWorld.camera.dy[2]=0;
	theWorld.light.pos[0]=0;theWorld.light.pos[1]=0;theWorld.light.pos[2]=0;
	theWorld.light.color=colBlack;
	theWorld.backGroundColor=colBlack;
	theWorld.ttl=16;
	theWorld.objectNum=0;
	/* Luaスクリプトの実行準備 */
	lua=luaL_newstate();
	luaL_openlibs(lua);
	/* 世界を操作するための関数を用意 */
	lua_register(lua,"getCommandParamNum",getCommandParamNum);
	lua_register(lua,"getCommandParam",getCommandParam);
	lua_register(lua,"getMaxObjectNum",getMaxObjectNum);
	lua_register(lua,"setCamera",setCamera);
	lua_register(lua,"setLight",setLight);
	lua_register(lua,"setBackgroundColor",setBackgroundColor);
	lua_register(lua,"setTtl",setTtl);
	lua_register(lua,"putSphere",putSphere);
	lua_register(lua,"putPlane",putPlane);
	lua_register(lua,"putCylinder",putCylinder);
	lua_register(lua,"putDisk",putDisk);
	lua_register(lua,"putTriangle",putTriangle);
	lua_register(lua,"setPrevObjectColor",setPrevObjectColor);
	lua_register(lua,"setPrevObjectTexture",setPrevObjectTexture);
	lua_register(lua,"setPrevTextureMapping",setPrevTextureMapping);
	/* 世界と直接は関係ないライブラリ関数を用意 */
	lua_register(lua,"scalarMultiple",scalarMultiple);
	lua_register(lua,"vectorAdd",vectorAdd);
	lua_register(lua,"vectorSub",vectorSub);
	lua_register(lua,"innerProduct",innerProduct);
	lua_register(lua,"dotProduct",innerProduct); /* alias */
	lua_register(lua,"exteriorProduct",exteriorProduct);
	lua_register(lua,"crossProduct",exteriorProduct); /* alias */
	lua_register(lua,"outerProduct",exteriorProduct); /* alias */
	lua_register(lua,"vectorNormalize",vectorNormalize);
	lua_register(lua,"vectorLength",vectorLength);
	lua_register(lua,"vectorRotateXY",vectorRotateXY);
	lua_register(lua,"vectorRotateYZ",vectorRotateYZ);
	lua_register(lua,"vectorRotateZX",vectorRotateZX);
	lua_register(lua,"vectorRotateZaxis",vectorRotateXY); /* alias */
	lua_register(lua,"vectorRotateXaxis",vectorRotateYZ); /* alias */
	lua_register(lua,"vectorRotateYaxis",vectorRotateZX); /* alias */
	lua_register(lua,"makeColor",makeColor);
	lua_register(lua,"getColor",makeColor); /* alias */
	lua_register(lua,"makeColorFromInt",makeColorFromInt);
	lua_register(lua,"getColorFromInt",makeColorFromInt); /* alias */
	/* Luaスクリプトの実行 */
	if(luaL_dofile(lua,fileName)) {
		okFlag=0;
		printf("error: %s\n",lua_tostring(lua,-1));
	} else {
		okFlag=1;
	}

	return okFlag;
}

/*
 * 確保したテクスチャを解放する
 */
void freeTheWorld(void) {
	int i;
	for(i=0;i<theWorld.objectNum;i++) {
		if(theWorld.objects[i].texture)free(theWorld.objects[i].texture);
	}
}
