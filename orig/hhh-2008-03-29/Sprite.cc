#include "Sprite.h"
#include "Defines.h"
#include <string>

#define delay_set 3

class Sprite;

Sprite::Sprite(char* spritename, AnimationFactory* af){
	//printf(">-------Creating sprite\n");
	this->name = spritename;
	this->af = af;
	this->delay = delay_set;
	this->currentframe = 0;
	string s(spritename);
	s = s + "-default";
	//Animation* def = new Animation(spritename, "default", true, spritename);
	//animations.push_back(def);
	//printf("Adding animation to store\n");
	animations.push_back(af->getByName(s.c_str()));
//	animations.push_back(af->getLast());
	//current = def;
	current = animations[animations.size()-1];
	alive = true;
};

Sprite::~Sprite(){
	//int i;
	//printf("<-------Killing sprite\n");
	//for(i=0; i<animations.size(); i++){
	//	delete(animations[i]);
	//}
};

bool Sprite::animationFinished(){
//	if(!current->loop && current->currentframe < current->numframes){
//		return false;
//	} else if(!current->loop && current->currentframe >= current->numframes){
//		return true;
//	} else {
//		return false;
//	}
	if(current->numframes == currentframe && !current->loop){
		return true;
	} else {
		return false;
	}
};

bool Sprite::isAnimation(char* name){
	if(strcmp(name,current->name)==0){
		return true;
	} else {
		return false;
	}
};

char* Sprite::getAnimation(){
	return current->name;
};
/*
void Sprite::loadAnimation(char* animName, bool loop){
	Animation* tmp = new Animation(this->name,animName,loop);
	animations.push_back(tmp);
};*/

void Sprite::setAnimation(char* name){
	alive = true;
	string s(name);
	string nn(this->name);
	s = nn + "-" + s;
	int cur;
	//printf("Setting animation to %s\n",s.c_str());
	for(cur=0; cur<af->size(); cur++){
		//printf("Current animation is called %s\n",animations[cur]->name);
		Animation* tmp = (Animation*)(af->getByNumber(cur));
		if(strcmp(tmp->getKnown(),s.c_str()) == 0){
			current = tmp;
			currentframe = 0;
		}
	}
};

void Sprite::kill(){
	setAnimation("die");
};

SDL_Surface* Sprite::frame(){
	//setAnimation("default");
	SDL_Surface* tmp = current->getFrame(currentframe);
	delay--;
	if(delay==0){
		currentframe++;
		delay = delay_set;
	}
	if(currentframe>=current->numframes && current->loop){
		currentframe = 0;
	}
	return tmp;
};

void Sprite::move(){
	if(xpos < nextxpos){
		xpos = xpos + speed;
	} else if(xpos > nextxpos){
		xpos = xpos - speed;
	} else if(ypos < nextypos){
		ypos = ypos + speed;
	} else if(ypos > nextypos){
		ypos = ypos - speed;
	}
};

void Sprite::gridx(int x){
	gridxpos = x;
};

void Sprite::gridy(int y){
	gridypos = y;
};

void Sprite::x(int x){
	xpos = x;
};

void Sprite::y(int y){
	ypos = y;
};

int Sprite::x(){
	//xpos = (gridxpos * SIZET) + 50;
	return xpos;
};

int Sprite::y(){
	return ypos;
};
