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
	#define STD_DEF 1
	#include <string.h>
	#include <stdlib.h>
	#include <time.h>
	#include <stdint.h>
	#include <math.h>
#endif
#ifndef USR_DEF
	#define USR_DEF 1
	#include "lib/sunostd/frand.h"
	#include "lib/sdlgl/sdlgl.h"
	#include <cglm/cglm.h>
#endif

#ifndef MATTER
	#define MATTER
	#include "matter_renderer.c"
	#include "matter_logic.c"
#endif

// main() controls general state and transition. side inlined loops control specific logic states


void loop()
{
	SDL_TimerID worldclock = SDL_GetTicks64();
	SDL_Event ev = {0};
	const uint8_t* state = SDL_GetKeyboardState(0);
	uint8_t run = 1;
	while(run){
		SDL_Delay(1);
		while (SDL_PollEvent(&ev)) {
			if(ev.type == SDL_QUIT){run = 0;}
			if(ev.type == SDL_KEYDOWN)
			{
				switch (ev.key.keysym.sym) {
					case SDLK_SPACE:
						run = 0;
					break;
					case SDLK_r:
						worldRandom();
					break;
					case SDLK_t:
						worldTest();
				}
			}
		}

		if(SDL_GetTicks64()>worldclock+1000/30)
		{
			worldclock = SDL_GetTicks64();

			if(state[SDL_SCANCODE_A])
			{
				for(int i = 0;i<worldsize[0]*worldsize[1];i++)
				{
					if(world[i].materialid==3 && world[i].forcex > -126)
						{world[i].forcex -= 2;}
				}
			}
			if(state[SDL_SCANCODE_D])
			{
				for(int i = 0;i<worldsize[0]*worldsize[1];i++)
				{
					if(world[i].materialid==3 && world[i].forcex < 126)
						{world[i].forcex += 2;}
				}
			}
			if(state[SDL_SCANCODE_W])
			{
				for(int i = 0;i<worldsize[0]*worldsize[1];i++)
				{
					if(world[i].materialid==3 && world[i].forcey < 126)
						{world[i].forcey += 2;}
				}
			}
			if(state[SDL_SCANCODE_S])
			{
				for(int i = 0;i<worldsize[0]*worldsize[1];i++)
				{
					if(world[i].materialid==3 && world[i].forcey > -126)
						{world[i].forcey -= 2;}
				}
			}


			worldLogic();
			worldBufferUpdate(world);

			glUseProgram(compClear);
			glDispatchCompute(worldsize[1],worldsize[0],1);
			glUseProgram(compBuffer);
			glDispatchCompute(worldsize[1],worldsize[0],1);


			glUseProgram(renderProgram);
			glDrawElements(GL_TRIANGLES, sizeof(eleid), GL_UNSIGNED_BYTE, 0);
			SDL_GL_SwapWindow(sdlglWindow);
		}
	}
}


int main(int argc, char* argv[])
{
	srand(time(0));
	sdlglInit();
	
	glClearColor(0.8,0.6,0.8,1.);
	setFullscreenWindow();
	world = worldBufferCreate();
	renderCreate();



	loop();




	renderDestroy();
	worldBufferDestroy(&world);

	catchGLError();
	int wawa;
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &wawa);
	SDL_Log("%d",wawa);

	sdlglQuit();
}



