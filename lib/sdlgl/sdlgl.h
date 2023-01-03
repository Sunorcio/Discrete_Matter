#pragma once


// Init parameters, edit these
//             ||
//             \/

#define	wWIDTH 1024
#define wHEIGHT 768
#define VSYNC 1 // 1^0
#define BLENDING 0 // 1^0
#define DEPTH 0 // 1^0
#define STENCIL 0 // 1^0
#define DITHERING 0 // 1^0
#define ANTIALIASING 0 // 1^0
#define SHADERCHARMAX 10000
// #define SHADEROBJMAX 5 // Max shaders in SDLGLprogram
#define MAJOR_VERSION 4
#define MINOR_VERSION 6
#define PROFILE SDL_GL_CONTEXT_PROFILE_CORE // CORE^COMPATIBILITY



#define _sdlglClearTerminal() SDL_Log("\e[H\e[2J")

#ifdef DBG
 #define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
 #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
 #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
 #define GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
 #define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B
 //#define VBO_FREE_MEMORY_ATI 0x87FB // for ATI(AMD)
 //#define TEXTURE_FREE_MEMORY_ATI 0x87FC
 //#define RENDERBUFFER_FREE_MEMORY_ATI 0x87FD

 // Quickly check for vram memory leaks (nvidia only)
 #define _sdlglMemUsageLog												\
  int avalmem,totmem;													\
  glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &avalmem);	\
  glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &totmem);		\
  SDL_Log("%g%%",100.-100.*(double)avalmem/(double)totmem);
#else
 #define _sdlglMemUsageLog
#endif

#ifdef DBG
 // Wrap gl functions with this then declare debug build with -DDBG to print gl errors and their locations in code
 #define _sdlgldbg(x)	catchGLError(); \
						x; \
						if(catchGLError()){SDL_Log("in line : %d, function : %s",__LINE__,__FUNCTION__);}
#else
 #define _sdlgldbg(x) x;
#endif

#include "sdlgl.c"


extern SDL_Window* sdlglWindow; // Swap this window by default
extern SDL_GLContext* sdlglContext;
extern char* sdlglSource;
extern void sdlglInit();
// initialises SDL window and GL context based on above defines, use instead of SDL_Init / SDL_Quit
static void sdlglQuit();

extern char catchSDLError(int functionreturn);
// feed a negative literal to just print SDL error, else load with a SDL function call to detect and print error
extern char catchGLError();
// if no gl error return 0 else print and clear all errors



extern unsigned int sdlglShaderCompileFile(const char* srcfile,unsigned int shaderType);
// Compile source file into returned shader object

extern char* sdlglShaderLoadSource(const char* srcfile);
// Loads sourcefile into heap
extern unsigned char sdlglShaderSourceCompare(char** src,const char* srcfile);
// If sourcefile differs from loaded source update it and return !0


/*
typedef SDLGLprogram SDLGLprogram;
// contains program id and their attached shader ids
extern SDLGLprogram* sdlglCreateProgram();
extern void sdlglDestroyProgram(SDLGLprogram* program);
*/

