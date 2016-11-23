#ifndef _GHOST_
#define _GHOST_

#include "SDL.h"
#include "SDL_image.h"
#include "Defines.h"
#include "Level.h"
#include "Sprite.h"
#include <fstream>
#include <vector>

using namespace std;

class Ghost : public Sprite{
	public:
		Ghost(char* filename, int x, int y, int speed, AnimationFactory* af);	// Load all sprites in directory
		virtual ~Ghost();
//		SDL_Surface* frame();
		void x(int x);	// Set X
		void y(int y);
		void setLevel(Level lev, int startx, int starty);
		int getfreedirections();
		bool testdirection(int dir);
		void setRespawn(long time);
		void decTimer();
		int x();	// Get X
		int y();
		virtual void move(int x, int y);
		int nextxpos;
		int nextypos;
		int direction; // 1 = up, 2 = right, 3 = down, 4 = left, 5 = stationary
		int numframes;
		int nextdir;
		long timeleft;
		char effect; // Used for timeleft, effects are 'flash', 'stop', 'fast', 'slow' etc...
		long respawntime;
		long timer();
		void timer(long timeleft);

	//protected:
//		vector<SDL_Surface*> frames;
		int startx;
		int starty;
//		int xpos;
//		int ypos;
//		int currentframe;
//		int speed;
		Level lev;
};

#endif
