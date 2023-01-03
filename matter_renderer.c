#if(0) // lspgarbage 
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_rwops.h>
#endif // ignore
	
	   
#ifndef STD_DEF
	#include <stdint.h>
#endif
#ifndef USR_DEF
	#include "lib/sdlgl/sdlgl.h"
#endif


typedef struct particle
{
	uint8_t materialid;
	uint8_t stepnbond;	// [0xf0] = [ d , l , u , r ] = (step potential) , [0x0f] = [ ur? , u? , ul? , r? ] = (bonds)
	int8_t forcey;		
	int8_t forcex;
	uint16_t state; // ?
	uint16_t clamp; // clamped mesh memory?
}node;

//		640	360	(16:9)	^	480 360 (4:3)         (720/2 == 360, min target 1280 720)
uint16_t worldsize[2] = {48*4,36*4}; // represented grid (x,y)

void setFullscreenWindow()
{
	SDL_DisplayMode display;
	SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(sdlglWindow), &display);
	SDL_SetWindowSize(sdlglWindow, display.w,display.h);
	SDL_SetWindowFullscreen(sdlglWindow, SDL_WINDOW_FULLSCREEN);
	if(worldsize[0]/worldsize[1]>(float)display.w/display.h)
		{glViewport(0,(display.h-display.w*worldsize[1]/worldsize[0])/2,display.w, display.w*worldsize[1]/worldsize[0]);}
	else{glViewport((display.w-display.h*worldsize[0]/worldsize[1])/2,0,display.h*worldsize[0]/worldsize[1], display.h);}
}

unsigned int bo;
unsigned int texb;
unsigned int texo;
unsigned int compBuffer;
unsigned int compClear;
node* worldBufferCreate()
{
	node* world = calloc(worldsize[0]*worldsize[1],sizeof(node));

	compBuffer = glCreateProgram();
	unsigned int cs = sdlglShaderCompileFile("bufferstate.comp",GL_COMPUTE_SHADER);
	glAttachShader(compBuffer,cs);
	glLinkProgram(compBuffer);
	glDetachShader(compBuffer,cs);
	glDeleteShader(cs);

	compClear = glCreateProgram();
	cs = sdlglShaderCompileFile("clearbuffer.comp",GL_COMPUTE_SHADER);
	glAttachShader(compClear,cs);
	glLinkProgram(compClear);
	glDetachShader(compClear,cs);
	glDeleteShader(cs);


	glGenBuffers(1,&bo);
	glBindBuffer(GL_TEXTURE_BUFFER,bo);
	glBufferData(GL_TEXTURE_BUFFER,sizeof(node)*worldsize[1]*worldsize[0],world,GL_DYNAMIC_DRAW);
	glGenTextures(1, &texb);
	glBindTexture(GL_TEXTURE_BUFFER, texb);
	glTexBuffer(GL_TEXTURE_BUFFER,GL_RGBA16UI,bo);

	glGenTextures(1, &texo);
	glBindTexture(GL_TEXTURE_2D, texo);
	glTextureParameteri(texo,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTextureParameteri(texo,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTextureParameteri(texo,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTextureParameteri(texo,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,worldsize[1]*2,worldsize[0]*2,0,GL_RGBA,GL_HALF_FLOAT,0);
	glClearTexImage(texo,0,GL_RGBA,GL_HALF_FLOAT,0);
	glBindImageTexture(0,texo,0,GL_FALSE,0,GL_WRITE_ONLY,GL_RGBA16F);

	return world;
}
void worldBufferUpdate(node* world)
{
	glBufferData(GL_TEXTURE_BUFFER,sizeof(node)*worldsize[1]*worldsize[0],world,GL_DYNAMIC_DRAW);
}
void worldBufferDestroy(node** world)
{
	glDeleteProgram(compBuffer);
	glDeleteProgram(compClear);
	free(*world);
	glDeleteTextures(1, &texo);
	glDeleteTextures(1, &texb);
	glDeleteBuffers(1,&bo);
}



unsigned int vao;
unsigned int vb;
unsigned int eb;
unsigned int renderProgram;
float position[] =
{
	-1.	,-1.,		0.	,0.,
	 1.	,-1.,		0.	,1.,
	 1.	, 1.,		1.	,1.,
	-1.	, 1.,		1.	,0.,
};
uint8_t eleid[] = {
	0,1,2,
	0,2,3,
};
void renderCreate()
{

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	glGenBuffers(1,&vb);
	glBindBuffer(GL_ARRAY_BUFFER,vb);
	glBufferData(GL_ARRAY_BUFFER,sizeof(position),position,GL_STATIC_DRAW);

	glGenBuffers(1,&eb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(eleid),eleid,GL_STATIC_DRAW);

	renderProgram = glCreateProgram();
	unsigned int vs = sdlglShaderCompileFile("render.vert",GL_VERTEX_SHADER);
	unsigned int fs = sdlglShaderCompileFile("render.frag",GL_FRAGMENT_SHADER);
	glAttachShader(renderProgram,vs);
	glAttachShader(renderProgram,fs);
	glLinkProgram(renderProgram);
	glDetachShader(renderProgram,vs);
	glDetachShader(renderProgram,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	int locpos = glGetAttribLocation(renderProgram, "pos");
	if(locpos == -1){SDL_Log("pos not found");}
	glEnableVertexAttribArray(locpos);
	glVertexAttribPointer(locpos,2,GL_FLOAT,GL_FALSE,sizeof(position[0])*4,0);

	int locuv = glGetAttribLocation(renderProgram, "uv");
	if(locuv == -1){SDL_Log("uv not found");}
	glEnableVertexAttribArray(locuv);
	glVertexAttribPointer(locuv,2,GL_FLOAT,GL_FALSE,sizeof(position[0])*4,(void*)(sizeof(float)*2));

	int loctex = glGetUniformLocation(renderProgram, "tex");
	if(loctex == -1){SDL_Log("tex not found");}

	glActiveTexture(GL_TEXTURE0);

}
void renderDestroy()
{
	glUseProgram(0);
	glDeleteProgram(renderProgram);
	glDeleteBuffers(1,&vb);
	glDeleteBuffers(1,&eb);
	glDeleteVertexArrays(1,&vao);
}


