#include "AnimationFactory.h"
#include <iostream>

using namespace std;

AnimationFactory::AnimationFactory(){
};

AnimationFactory::~AnimationFactory(){
	unsigned int i;
	for(i=0; i<store.size(); i++){
		Animation* tmp = (Animation*)(store[i]);
		//cout << "Deleting Animation" << i << "\n";
		delete tmp;
	}
};

void AnimationFactory::list(){
	unsigned int i;
	for(i=0; i<store.size(); i++){
		printf("> %s\n",((Animation*)store[i])->getKnown());
	}
};

Animation* AnimationFactory::getLast(){
	return (Animation*)store[store.size()-1];
};

Animation* AnimationFactory::getByNumber(unsigned int n){
	if(n<store.size() && n>=0){
		return (Animation*)store[n];
	} else {
		return NULL;
	}
};

int AnimationFactory::size(){
	return store.size();
};

Animation* AnimationFactory::getByName(const char* name){
	//printf("Getting %s by name from store of %d items\n",name,store.size());
	unsigned int i;
	for(i=0; i<store.size(); i++){
		//printf("Item %d is %s\n",i,((Animation*)store[i])->getKnown());
		Animation* tmp = ((Animation*)(store[i]));
		//printf("Comparing %s with %s\n",tmp->getKnown(), name);
		if(strcmp(tmp->getKnown(), name) == 0){
			//printf("Match Found\n");
			return tmp;
		}
	}
	return NULL;
};

void AnimationFactory::loadAnimation(char* spriteName, char* path, bool loop, char* knownAs){
	Animation* a = new Animation(spriteName, path, loop, knownAs);
	store.push_back(a);
	//printf("Added. Store now stands at %d item\n",store.size());
};
