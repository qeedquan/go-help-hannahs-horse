#ifndef _BONUS_
#define _BONUS_
#include "Ghost1.h"

using namespace std;

class Bonus : public Ghost1{
	public:
		Bonus(char *filename, int x, int y, int speed, AnimationFactory* af, char type);
		// Type is 's' for stop ghosts, 'k' for kill ghosts, 'u' for speed up ghosts, 'c' for switch controls
		void decLife();
		bool alive;
		int lifeforce;
		char type;
	//	virtual ~Food();	
};

#endif
