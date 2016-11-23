//#include "SDL.h"
//#include "SDL_image.h"
//#include "Defines.h"
#ifndef _GHOST1_
#define _GHOST1_
#include "Ghost.h"
//#include <fstream>
//#include <vector>

using namespace std;

class Ghost1 : public Ghost{
	public:
		Ghost1(char* filename,int x, int y, int speed, AnimationFactory* af);	// Load all sprites in directory
//		virtual ~Ghost1();
		//SDL_Surface* frame();
		//void x(int x);	// Set X
		//void y(int y);
		//int getfreedirections();
		//bool testdirection(int dir);
		//int x();	// Get X
		//int y();
		virtual void move(int x, int y);
		//int nextxpos;
		//int nextypos;
		//int direction; // 1 = up, 2 = right, 3 = down, 4 = left, 5 = stationary
		//int numframes;
		//int nextdir;

	protected:
		//vector<SDL_Surface*> frames;
		//int xpos;
		//int ypos;
		//int currentframe;
		//int speed;
		//gridcell map[SIZEX][SIZEY];
};

#endif
