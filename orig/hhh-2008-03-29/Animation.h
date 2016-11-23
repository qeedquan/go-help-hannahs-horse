// Loads and stores an animation sequence
//
//

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

//#define WIN32

#include "SDL.h"
#include "SDL_image.h"
#include <vector>

using namespace std;

class Animation{
	public:
		Animation(char* spritename, char *filename, bool loop, char* knownAs);
		virtual ~Animation();
		SDL_Surface* getFrame();
		SDL_Surface* getFrame(int fnum);
		char* getKnown();
		void reset();
		bool loop;
		bool finished;
		char* name;
		char* knownAs;
	//private:
		int numframes;
		int currentframe;
		vector<SDL_Surface*> frames;
		int delay;
};

#endif
