#ifndef STD_DEF
	#include <stdint.h>
#endif
#ifndef MATTER
	#include "matter_renderer.c"
#endif

// set in main from render declaration
node* world = 0;



void gravdown()
{
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		if(world[i].materialid>0)
		{
			if(world[i].forcey > -127){world[i].forcey -= 1;}
		}
	}
}

void energyloss()
{
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		if(world[i].forcex>0){world[i].forcex--;}else if(world[i].forcex<0) {world[i].forcex++;}
		if(world[i].forcey>0){world[i].forcey--;}else if(world[i].forcey<0) {world[i].forcey++;}
	}
}



void worldLogic()
{

	for (uint8_t j = 127; j; j--) 
	{
		//flag
		for(int i = 0;i<worldsize[0]*worldsize[1];i++)
		{
			if(world[i].materialid > 0)
			{
				if( !(world[i].forcex<j) && world[i+1].materialid == 0 && (i+1)%worldsize[0])
					{ world[i].stepnbond = world[i].stepnbond | 0x10; }
				else if( !(world[i].forcex>-j) && world[i-1].materialid == 0 && i%worldsize[0])
					{ world[i].stepnbond = world[i].stepnbond | 0x40; }
				if( !(world[i].forcey<j) && world[i+worldsize[0]].materialid == 0 && !(i>worldsize[0]*(worldsize[1]-1)-1))
					{ world[i].stepnbond = world[i].stepnbond | 0x20; }
				else if( !(world[i].forcey>-j) && world[i-worldsize[0]].materialid == 0 && !(i<worldsize[0]))
					{ world[i].stepnbond = world[i].stepnbond | 0x80; }
			}
		}
		//step
		for(int i = 0;i<worldsize[0]*worldsize[1];i++)
		{
			switch (world[i].stepnbond&0xf0) {
				case 0x10:
					*(uint64_t*)&world[i+1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0x30:
					*(uint64_t*)&world[i+worldsize[0]+1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0x20:
					*(uint64_t*)&world[i+worldsize[0]] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0x60:
					*(uint64_t*)&world[i+worldsize[0]-1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0x40:
					*(uint64_t*)&world[i-1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0xc0:
					*(uint64_t*)&world[i-worldsize[0]-1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0x80:
					*(uint64_t*)&world[i-worldsize[0]] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				case 0x90:
					*(uint64_t*)&world[i-worldsize[0]+1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
					*(uint64_t*)&world[i] = 0 ;
				break;
				default:
				break;
			}
		}

	}
	energyloss();
}

void worldTest()
{
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		world[i].materialid = 0x00;
	}
	world[worldsize[0]*(worldsize[1]-1)+worldsize[0]/2].materialid = 0x03;

}

void worldRandom()
{
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		world[i].materialid = 0x00;
		if(rand()%0x02)
		{
			world[i].materialid = 0x03;
		}
	}

/*
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		world[i].bonds = 0x00;
		if(world[i].materialid != 0x00)
		{
			if( (i+1)%worldsize[0] &&
			world[i+1].materialid == world[i].materialid )
				{world[i].bonds += 0x01;}
			if( i<worldsize[0]*(worldsize[1]-1) && i%worldsize[0] && 
			world[i+worldsize[0]-1].materialid == world[i].materialid && world[i-1].bonds<0x08 )
				{world[i].bonds += 0x02;}
			if( i<worldsize[0]*(worldsize[1]-1) && 
			world[i+worldsize[0]].materialid == world[i].materialid )
				{world[i].bonds += 0x04;}
			if( i<worldsize[0]*(worldsize[1]-1) && (i+1)%worldsize[0] &&
			world[i+worldsize[0]+1].materialid == world[i].materialid )
				{world[i].bonds += 0x08;}
		}
	}
*/

}
