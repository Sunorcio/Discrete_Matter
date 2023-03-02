#if(0) // lspgarbage
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_rwops.h>
#endif // ignore


#ifndef GL_DEF
 #define GL_DEF 1
 #include <GL/glew.h>
#endif
#ifndef SDL2_DEF
 #define SDL2_DEF 1
 #include <SDL2/SDL.h>
#endif
#ifndef STD_DEF
 #include <string.h>
 #include <stdint.h>
#endif
 #include <stdio.h>



// Configuration variables are defined in header file, edit them there, ignore compiler warnings.

static char catchSDLError(int functionreturn){
	if(functionreturn<0){ SDL_Log("%s",SDL_GetError());}
	if(SDL_GetError() == 0){return -1;}
	return 0;
}
static char catchGLError(){
	GLenum error = glGetError();
	if (error == GL_NO_ERROR){ return 0; }

	do{SDL_Log("GLerror : %s", gluErrorString(error));}
	while(( error = glGetError() ));
	return -1;
}


static SDL_Window* sdlglWindow;
static SDL_GLContext* sdlglContext;
static char* sdlglSource;
static void sdlglInit()
{
	sdlglSource = calloc(SHADERCHARMAX+1, sizeof(char));
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, PROFILE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	sdlglWindow 
	  =SDL_CreateWindow(0,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,wWIDTH,wHEIGHT,SDL_WINDOW_OPENGL);
	sdlglContext = SDL_GL_CreateContext(sdlglWindow);
	glewExperimental = GL_TRUE;
	glewInit();

	SDL_GL_SetSwapInterval(VSYNC);

	#if(!DITHERING)
	glDisable(GL_DITHER);
	#endif

	#if(!ANTIALIASING)
	glDisable(GL_MULTISAMPLE);
	#endif

	#if(BLENDING)
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	#endif

	#if(DEPTH)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	//glDisable(GL_CULL_FACE);
	#endif
	
	#if(STENCIL)
	glEnable(GL_STENCIL_TEST);
	glStencilFuncSeparate(GL_FRONT_AND_BACK,GL_EQUAL,0x00,0xFF);
	glStencilOpSeparate(GL_FRONT_AND_BACK,GL_KEEP,GL_KEEP,GL_KEEP);
	#endif

	glClearColor(0.f,0.25f,0.25f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(sdlglWindow);
}
static void sdlglQuit()
{
	free(sdlglSource);
	SDL_GL_DeleteContext(sdlglContext);
	SDL_DestroyWindow(sdlglWindow);
	SDL_Quit();
}



static unsigned int sdlglShaderCompileFile(const char* srcfile,unsigned int shaderType)
{
	FILE* f = fopen(srcfile, "a+");
	fseek(f, 0, SEEK_END);
	int l = ftell(f);
	if(!l){SDL_Log("Shader file missing or empty");fclose(f);return 0;}
	if(l>SHADERCHARMAX){SDL_Log("Shader exceeds character limit(defined in sdlgl.h)");fclose(f);return 0;}
	memset(sdlglSource, 0, SHADERCHARMAX);
	fseek(f, 0, SEEK_SET);
	fread(sdlglSource, 1, l, f);
	fclose(f);

	unsigned int shaderObject = glCreateShader(shaderType);
	glShaderSource(shaderObject,1,(const char**)&sdlglSource,0);
	glCompileShader(shaderObject);

#ifdef DBG
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &l);
	if(!l)
	{
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &l);
		char log[l];
		glGetShaderInfoLog(shaderObject, l, &l, &log[0]);
		SDL_Log("Compilation failed - %s",log);
		return 0;
	}
#endif
	return shaderObject;
}

static char* sdlglShaderLoadSource(const char* srcfile)
{
	char* srcpointer = calloc(SHADERCHARMAX+1, sizeof(char));
	FILE* f = fopen(srcfile, "a+");
	fseek(f, 0, SEEK_END);
	int l = ftell(f);
	if(!l){SDL_Log("Shader file missing or empty");fclose(f);return 0;}
	if(l>SHADERCHARMAX){SDL_Log("Shader exceeds character limit(defined in sdlgl.h)");fclose(f);return 0;}
	memset(srcpointer, 0, SHADERCHARMAX);
	fseek(f, 0, SEEK_SET);
	fread(srcpointer, 1, l, f);
	fclose(f);
	return srcpointer;
}
static unsigned char sdlglShaderSourceCompare(char** loadedsrc,const char* srcfile)
{
	FILE* f = fopen(srcfile, "a+");
	fseek(f, 0, SEEK_END);
	int l = ftell(f);
	if(!l){SDL_Log("Shader file missing or empty");fclose(f);return 0;}
	if(l>SHADERCHARMAX){SDL_Log("Shader exceeds character limit(defined in sdlgl.h)");fclose(f);return 0;}
	memset(sdlglSource, 0, SHADERCHARMAX);
	fseek(f, 0, SEEK_SET);
	fread(sdlglSource, 1, l, f);
	fclose(f);
	
	if(strcmp(*loadedsrc, sdlglSource))
	{
		memset(*loadedsrc, 0, SHADERCHARMAX);
		memcpy(*loadedsrc, sdlglSource, l);
		return 1;
	}
	return 0;
}




/*
typedef struct
{	
	unsigned int id;
	unsigned int shadobj[SHADEROBJMAX];
}SDLGLprogram;
static SDLGLprogram* sdlglCreateProgram()
{
	SDLGLprogram* program = calloc(1, sizeof(SDLGLprogram));
	program->id = glCreateProgram();
	return program;
}
static void sdlglDestroyProgram(SDLGLprogram* program)
{
	for(int i=0;i<SHADEROBJMAX;i++)
	{
		if (program->shadobj[i]) {
			glDetachShader(program->id,program->shadobj[i]);
			glDeleteShader(program->shadobj[i]);
		}
	}
	glUseProgram(0);
	glDeleteProgram(program->id);
	free(program);
}
*/




