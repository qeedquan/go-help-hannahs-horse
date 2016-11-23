#ifndef _FOOD_
#define _FOOD_

#include "Ghost1.h"

using namespace std;

class Food : public Ghost1{
	public:
		Food(char *filename, int x, int y, int speed, AnimationFactory* af);
		bool alive;
		virtual ~Food();	
};

#endif
