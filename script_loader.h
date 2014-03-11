#ifndef SCRIPT_LOADER_H_GUARD
#define SCRIPT_LOADER_H_GUARD

enum {
	OBJTYPE_SPHERE,
	OBJTYPE_PLANE,
	OBJTYPE_CYLINDER,
	OBJTYPE_DISK,
	OBJTYPE_TRIANGLE
};

typedef struct {
	double r,g,b;
} color_t;

typedef struct {
	unsigned char r,g,b,a;
} intcolor_t;

typedef struct {
	double c[3];
	double r;
} sphere_t;

typedef struct {
	double p[3];
	double d1[3];
	double d2[3];
	char min1e,max1e,min2e,max2e; /* if false, no min/max value set */
	double min1,max1,min2,max2;
} plane_t;

typedef struct {
	double p[3];
	double d[3];
	short mine,maxe; /* if false, no min/max value set */
	double min,max;
	double r;
} cylinder_t;

typedef struct {
	double p[3];
	double d1[3];
	double d2[3];
} disk_t;

typedef struct {
	double p[3];
	double d1[3];
	double d2[3];
	double max1,max2;
} triangle_t;

typedef struct {
	int objType;
	union {
		sphere_t s;
		plane_t p;
		cylinder_t c;
		disk_t d;
		triangle_t t;
	} pos;
	int useTexture;
	color_t color; /* use if useTexture is false */
	int tWidth,tHeight;
	intcolor_t* texture;
	double toffset[2];
	double tdx[2];
	double tdy[2];
	int useTextureAlpha;
	double alpha; /* [0,1] less -> more reflection */
	double lightAlpha; /* [0,1] more -> more effect from the light */
	double lightPower;
} object_t;

typedef struct {
	double pos[3];
	double dir[3];
	double dx[3];
	double dy[3];
} camera_t;

typedef struct {
	double pos[3];
	color_t color;
} light_t;

#define OBJECT_MAX 100000
typedef struct {
	camera_t camera;
	light_t light;
	color_t backGroundColor;
	int ttl;
	int objectNum;
	object_t objects[OBJECT_MAX];
} world_t;

extern const color_t colBlack;
extern world_t theWorld;

int loadScript(const char* fileName,int paramNum,char* params[]);
void freeTheWorld(void);

#endif
