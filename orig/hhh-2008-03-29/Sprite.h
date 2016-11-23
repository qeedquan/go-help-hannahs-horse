#ifndef _SPRITE_H_
#define _SPRITE_H_

//#include "Animation.h"
#include "AnimationFactory.h"

using namespace std;

class Sprite{
	public:
		Sprite(char* filename, AnimationFactory* af);	// Load all sprites in directory
		virtual ~Sprite();
		SDL_Surface* frame();
		void loadAnimation(char *animName, bool loop);
		bool isAnimation(char* name);
		char* getAnimation();
		void setAnimation(char *name);
		bool animationFinished();
		void kill();
		void x(int x);	// Set X
		void y(int y);
		int x();	// Get X
		int y();
		void gridx(int x);
		void gridy(int y);
		void move();
		int nextxpos;
		int nextypos;
		int direction; // 1 = up, 2 = down, 3 = left, 4 = right, 5 = stationary
		bool alive;

	protected:
		vector<Animation*> animations;
		Animation* current;
		AnimationFactory* af;
		char* name;
		int xpos;
		int ypos;
		int gridxpos;
		int gridypos;
		int currentframe;
		int speed;
		int delay;
};

#endif
