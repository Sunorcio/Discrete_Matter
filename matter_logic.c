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
		if(world[i].materialid>1)
		{
			if(world[i].forcey > -127 && world[i-worldsize[0]].materialid == 0){world[i].forcey -= 2;}
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
			if(world[i].materialid > 1)
			{
				if( !(world[i].forcex<j) )
				{
					world[i].stepnbond = world[i].stepnbond | 0x10;
				}
				else if( !(world[i].forcex>-j) )
				{
					world[i].stepnbond = world[i].stepnbond | 0x40;
				}

				if( !(world[i].forcey<j) )
				{
					world[i].stepnbond = world[i].stepnbond | 0x20;
				}
				else if( !(world[i].forcey>-j) )
				{
					world[i].stepnbond = world[i].stepnbond | 0x80;
				}
			}

		}

		//collide or step
		for(int i = 0;i<worldsize[0]*worldsize[1];i++)
		{
			switch (world[i].stepnbond&0xf0) {
				case 0x10:
					if (world[i+1].materialid > 0) {
						world[i].forcex--;
						world[i+1].forcex++;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i+1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0x30:
					if (world[i+worldsize[0]+1].materialid > 0) {
						world[i].forcex--;
						world[i].forcey--;
						world[i+worldsize[0]+1].forcex++;
						world[i+worldsize[0]+1].forcey++;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i+worldsize[0]+1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0x20:
					if (world[i+worldsize[0]].materialid > 0) {
						world[i].forcey--;
						world[i+worldsize[0]].forcey++;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i+worldsize[0]] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0x60:
					if (world[i+worldsize[0]-1].materialid > 0) {
						world[i].forcex++;
						world[i].forcey--;
						world[i+worldsize[0]-1].forcex--;
						world[i+worldsize[0]-1].forcey++;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i+worldsize[0]-1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0x40:
					if (world[i-1].materialid > 0) {
						world[i].forcex++;
						world[i-1].forcex--;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i-1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0xc0:
					if (world[i-worldsize[0]-1].materialid > 0) {
						world[i].forcex++;
						world[i].forcey++;
						world[i-worldsize[0]-1].forcex--;
						world[i-worldsize[0]-1].forcey--;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i-worldsize[0]-1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0x80:
					if (world[i-worldsize[0]].materialid > 0) {
						world[i].forcey++;
						world[i-worldsize[0]].forcey--;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i-worldsize[0]] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;
				case 0x90:
					if (world[i-worldsize[0]+1].materialid > 0) {
						world[i].forcex--;
						world[i].forcey++;
						world[i-worldsize[0]+1].forcex++;
						world[i-worldsize[0]+1].forcey--;
						world[i].stepnbond &= 0x0f; 
					}else{
						*(uint64_t*)&world[i-worldsize[0]+1] = *(uint64_t*)&world[i]&0xffffffffffff0fff;
						*(uint64_t*)&world[i] = 0 ;
					}
				break;

				default:
				break;
			}
		}

	}
}





void worldTest()
{
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		*(uint64_t*)&world[i] = 0x0000000000000000;
		if( i<worldsize[0] || i > worldsize[0]*(worldsize[1]-1)-1 || !(i%worldsize[0]) || !((i+1)%worldsize[0]) )
		{ world[i].materialid = 0x01; }


		if( i > worldsize[0]*worldsize[1]/6 + worldsize[0]/10 &&  i < worldsize[0]*worldsize[1]/6 + worldsize[0]*2/5 )
		{ world[i].materialid = 0x01; }
	}
	world[worldsize[0]+worldsize[0]/2].materialid = 0x02;


	world[worldsize[0]*worldsize[1]/2+worldsize[0]/2].materialid = 0x02;

}

void worldRandom()
{
	for(int i = 0;i<worldsize[0]*worldsize[1];i++)
	{
		*(uint64_t*)&world[i] = 0x0000000000000000;
		if( i<worldsize[0] || i > worldsize[0]*(worldsize[1]-1)-1 || !(i%worldsize[0]) || !((i+1)%worldsize[0]) )
		{ world[i].materialid = 0x01; }
		else if(rand()%0x02)
		{ world[i].materialid = 2 + rand()%0x02; }
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
