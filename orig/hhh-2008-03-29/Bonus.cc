#include "Bonus.h"

class Bonus;

Bonus::Bonus(char *filename, int x, int y, int speed,AnimationFactory* af, char type):
	Ghost1(filename, x, y, speed, af){
	this->lifeforce = 300;
	this->alive = true;
	this->type = type;
};

void Bonus::decLife(){
	if(lifeforce>0) lifeforce--;
};
//Food::~Food(){
//	int i;
//	for(i=0; i<frames.size(); i++){
//		SDL_FreeSurface(frames[i]);
//	}
//};
