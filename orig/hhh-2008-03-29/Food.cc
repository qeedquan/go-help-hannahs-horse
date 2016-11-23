#include "Food.h"

class Food;

Food::Food(char *filename, int x, int y, int speed,AnimationFactory* af):
	Ghost1(filename, x, y, speed, af){
	this->alive = true;
};

Food::~Food(){
//	int i;
//	for(i=0; i<frames.size(); i++){
//		SDL_FreeSurface(frames[i]);
//	}
};
