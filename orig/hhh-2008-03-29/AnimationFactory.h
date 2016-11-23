#ifndef _ANIMATION_FACTORY_
#define _ANIMATION_FACTORY_

#include "Animation.h"

class AnimationFactory{
	public:
		AnimationFactory();
		virtual ~AnimationFactory();
		Animation* getByName(const char* name);
		Animation* getByNumber(unsigned int n);
		void loadAnimation(char* spriteName, char* path, bool loop, char* knownAs);
		Animation* getLast();
		int size();
		void list();
	private:
		vector<Animation*> store;
};

#endif
