package main

type Player struct {
	*Sprite
}

func newPlayer(name string, af *AnimationFactory) *Player {
	return &Player{
		Sprite: newSprite(name, af),
	}
}

func (p *Player) respawn() {
	p.setAnimation("default")
}
