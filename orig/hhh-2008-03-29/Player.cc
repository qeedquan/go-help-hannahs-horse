#include "Player.h"


Player::Player(char *name, AnimationFactory* af) : Sprite(name, af){

};

//Player::~Player(){
//	int i;
//	printf("Killing sprite\n");
//	for(i=0; i<animations.size(); i++){
//		free(animations[i]);
//	}
//};

void Player::move(){
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

void Player::respawn(){
	setAnimation("default");
};
