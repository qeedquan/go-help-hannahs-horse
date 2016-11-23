#include "Sprite.h"

class Player : public Sprite{
	public:
		Player(char* name, AnimationFactory* af);
//		virtual ~Player();
		void move();
		void respawn();
		int lives;
};
