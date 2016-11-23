package main

import "github.com/qeedquan/go-media/sdl"

type Sprite struct {
	nextxpos           int
	nextypos           int
	direction          int // 1 = up, 2 = down, 3 = left, 4 = right, 5 = stationary
	alive              bool
	animations         []*Animation
	current            *Animation
	af                 *AnimationFactory
	name               string
	xpos, ypos         int
	gridxpos, gridypos int
	currentFrame       int
	speed              int
	delay              int
}

func newSprite(spriteName string, af *AnimationFactory) *Sprite {
	animations := []*Animation{af.getByName(spriteName + "-default")}
	return &Sprite{
		name:       spriteName,
		af:         af,
		delay:      delaySet,
		alive:      true,
		animations: animations,
		current:    animations[0],
	}
}

func (s *Sprite) x() int {
	return s.xpos
}

func (s *Sprite) y() int {
	return s.ypos
}

func (s *Sprite) setX(x int) {
	s.xpos = x
}

func (s *Sprite) setY(y int) {
	s.ypos = y
}

func (s *Sprite) frame() *sdl.Texture {
	frame := s.current.getFrameN(s.currentFrame)
	if s.delay--; s.delay == 0 {
		s.currentFrame++
		s.delay = delaySet
	}
	if s.currentFrame >= len(s.current.frames) && s.current.loop {
		s.currentFrame = 0
	}
	return frame
}

func (s *Sprite) isAnimation(name string) bool {
	return s.current.name == name
}

func (s *Sprite) setAnimation(name string) {
	s.alive = true
	for i := 0; i < af.size(); i++ {
		a := af.getByNumber(i)
		if a.getKnown() == s.name+"-"+name {
			s.current = a
			s.currentFrame = 0
			return
		}
	}
	panic("unreachable")
}

func (s *Sprite) animationFinished() bool {
	if s.currentFrame == len(s.current.frames) && !s.current.loop {
		return true
	}
	return false
}
