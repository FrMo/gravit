#ifndef GRAVITON_H
#define GRAVITON_H

// #define NO_GUI
// #define GRAV_SSE
#define MAX_THREADS 1

#ifdef WIN32

	#include <windows.h>
	#include <conio.h>
	#include <stdio.h>

#else

	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <termios.h>

    #include <sys/select.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>

	#include <sys/timeb.h>

	#include <pthread.h>

#endif

#ifdef GRAV_SSE

	#include <xmmintrin.h>

#endif

#ifndef NO_GUI

	#include <SDL.h>
	#include <SDL_ttf.h>
	#include <SDL_opengl.h>
//	#include <GL/glut.h>

#else

	// sdlkeys mappings
	#include "sdlk.h"

#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#define dlog(x,y)
#define frand(min,max) ((min) + ((float)rand() / RAND_MAX) * ((max) - (min)))
#define FRAMESIZE (sizeof(particle_t)*state.particleCount)

#define getParticleCurrentFrame(i) state.particleHistory + state.particleCount * state.currentFrame + i
#define getParticleFirstFrame(i) state.particleHistory + i
#define getParticleDetail(i) state.particleDetail + i;

#define G -0.00001f

#ifdef GRAV_SSE

	#define VectorNew(a) __m128 a;

	#define VectorCopy(a,b) { b = _mm_setzero_ps(); b = _mm_add_ps(a, b); }
	#define VectorAdd(a,b,c) { c = _mm_add_ps(a,b); }
	#define VectorSub(a,b,c) { c = _mm_sub_ps(a,b); }
	#define VectorMultiply(a, b, c) { __m128 d; float poo; poo = b; d = _mm_load_ps1(&poo); c = _mm_mul_ps(a,d); }
	#define VectorDivide(a, b, c) { __m128 d; float poo; poo = b; d = _mm_load_ps1(&poo); c = _mm_div_ps(a,d); }
	#define VectorZero(x) { x = _mm_setzero_ps(); }
	#define distance2(aa,bb,cc) { __m128 dd; dd = _mm_sub_ps(aa,bb); dd = _mm_mul_ps(dd,dd); cc = dd.m128_f32[0] + dd.m128_f32[1] + dd.m128_f32[2]; }
	#define distance(a,b,c) { float duh; distance2(a,b,duh); c = (float)sqrt(duh); }

#else

	#define VectorNew(a) float a[3];

	#define VectorCopy(a,b) { b[0] = a[0]; b[1] = a[1]; b[2] = a[2]; }
	#define VectorAdd(a,b,c) { c[0] = a[0] + b[0]; c[1] = a[1] + b[1]; c[2] = a[2] + b[2]; }
	#define VectorSub(a,b,c) { c[0] = a[0] - b[0]; c[1] = a[1] - b[1]; c[2] = a[2] - b[2]; }
	#define VectorMultiply(a, b, c) { c[0] = a[0] * b; c[1] = a[1] * b; c[2] = a[2] * b; }
	#define VectorDivide(a, b, c) { c[0] = a[0] / b; c[1] = a[1] / b; c[2] = a[2] / b; }
	#define VectorZero(x) { x[0] = 0; x[1] = 0; x[2] = 0; }

	#define distance2(a,b,c) c = ((float)pow((double)a[0] - b[0], 2) + (float)pow((double)a[1] - b[1], 2) + (float)pow((double)a[2] - b[2], 2));
	#define distance(a,b,c) { float d; distance2(a,b,d); c = (float)sqrt((double)d); }

#endif

#ifndef Uint32
	#define Uint32 unsigned int
#endif

#ifndef WIN32
#define _aligned_malloc(a,b) malloc(a)
#define _aligned_free(a) free(a)
#endif

#ifndef GRAV_SSE
#define _aligned_malloc(a,b) malloc(a)
#define _aligned_free(a) free(a)
#endif

// #define MAX_HISTORY 10000
#define CONSOLE_HISTORY 10
#define CONSOLE_LENGTH 255
#define CONSOLE_BLINK_TIME 100

#define TM_STANDARD 0
#define TM_HELP 1

#define SM_PAUSED 1
#define SM_RECORD 2
#define SM_PLAY 4

// #define EM

#include "command.h"

#ifndef NO_GUI
#include "font.h"
#endif

#ifndef NO_GUI

typedef struct conf_s {

	int screenBPP;
	int screenW;
	int screenH;
	int screenFS;

	SDL_VideoInfo* gfxInfo;

} conf_t;

#endif

#ifdef WIN32

#ifdef GRAV_SSE
typedef struct __declspec(align(16)) particle_s {
#else
typedef struct particle_s {
#endif
#else
typedef struct particle_s {
#endif

// things that change per frame
#ifdef GRAV_SSE

	__m128 pos;
	__m128 vel;

#else

	float pos[3];
	float vel[3];

#endif

// things that change less often
	// float size;
	// float mass;
	// float col[3];
	// int active;
	// char padding[4];


} particle_t;

typedef struct particleDetail_s {

	// things that change less often
	int frame;
	float size;
	float mass;
	float col[4];
	int active;
//	struct particleDetail_t *next;

} particleDetail_t;

typedef struct state_s {

	particle_t *particleHistory;
	particleDetail_t *particleDetail;

	int particleCount;
	int frame;
	int totalFrames;
	int currentFrame;
	int historyFrames;
	int historyNFrame;

	int frameCompression;

	float center[3];
	int mode;
	float massRange[2];

	int processFrameThreads;

	int particlesToSpawn;

	unsigned int memoryAllocated;

	int incSave;	// save last frame + info after every frame
	int autoSave;	// auto save every n frames. 0 for off.
	char *fileName; // if null dont autosave or incsave.

} state_t;

typedef struct saveInfo_s {

	int particleCount;
	int historyFrames;
	int totalFrames;
	int frame;
	int historyNFrame;

} saveInfo_t;

typedef struct view_s {

	Uint32 dt;
	Uint32 ft;
	int quit;

	float rot[3];
	float zoom;

	float mat1[16];
	float mat2[16];

#ifndef NO_GUI
	int keys[SDLK_LAST];
#endif

	float pos[3];
	float face[3];

	float tailWidth;
	int tailLength;
	float tailOpacity;
	int tailFaded;
	int tailSkip;

	int drawAxis;

	int textMode;
	int consoleMode;

	int drawTree;
	int frameSkip;

	float fps;
	int verticies;

	int particleSize;

#define CM_MASS 1
#define CM_VEL 2

	int particleColorMode;

	int verboseMode;

} view_t;

typedef struct spawnVars_s {

	int minGalCount;
	int maxGalCount;
	float minGalMass;
	float maxGalMass;
	float minGalSize;
	float maxGalSize;
	float minGalVel;
	float maxGalVel;
	float minSpawnRange;
	float maxSpawnRange;

} spawnVars_t;

typedef struct col_s {

	float r;
	float g;
	float b;

} col_t;

typedef struct con_s {

	col_t c;
	char s[255];

} con_t;

// for processframeot
typedef struct otinfo_s {

	particle_t *p;
	particleDetail_t *pd;
	struct node_s *n;

} otinfo_t;

// for otComputeParticleToTreeRecursive
typedef struct pttr_s {
	
	particle_t *p;
	particleDetail_t *pd;
	struct node_s *n;

} pttr_t;

// main.c
#ifdef WIN32

#ifndef NO_GUI
extern conf_t conf;
#endif
extern state_t state;
extern view_t view;

#else

#ifndef NO_GUI
conf_t conf;
#endif
state_t state;
view_t view;

#endif

void cleanMemory();

// tool.c
char * va( char *format, ... );
int gfxPowerOfTwo(int input);
void gfxNormalToRGB(float *c, float n);
int LoadMemoryDump(char *fileName, unsigned char *d, unsigned int size);
int SaveMemoryDump(char *FileName, unsigned char *d, unsigned int total);
Uint32 getMS();

// spawn.c
extern spawnVars_t spawnVars;
void pickPositions();
void spawnDefaults();

// console.c
extern con_t con[CONSOLE_HISTORY];
void conAdd(int mode, char *f, ... );
void conInit();
void conDraw();
void conInput(char c);

#ifndef NO_GUI

// osd.c
void drawOSD();

// input.c
int processKeys();
void processMouse();

// gfx.c
void drawAll();
void drawFrame();
void drawFrameSet2D();
void drawFrameSet3D();
int gfxInit();

// color.c
void setColors();
void setColorsByMass();

#else

int processKeys();

#endif

// frame.c
int initFrame();
void processFrame();
void forceToCenter();
void processCollisions();

// frame-pp.c
void processFramePP(int s, int n);

// frame-ot.c
#ifdef WIN32
#ifdef GRAV_SSE
typedef struct __declspec(align(16)) node_s {
#else
typedef struct node_s {
#endif
#else
typedef struct node_s {
#endif

#ifdef GRAV_SSE

	__m128 min;
	__m128 max;
	__m128 c;
	__m128 cm;

#else

	float min[3];
	float max[3];
	float c[3];
	float cm[3];

#endif

	particle_t *p;
    struct node_t *b[8];
	float mass;
	float length;

} node_t;

void otDrawTree();
void otFreeTree();
void processFrameOT(int,int);

void frDoGravity(particle_t *p, node_t *n, float d);

#endif
