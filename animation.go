package main

import (
	"path/filepath"

	"github.com/qeedquan/go-media/sdl"
)

const delaySet = 3

type Animation struct {
	loop         bool
	finished     bool
	name         string
	knownAs      string
	currentFrame int
	frames       []*sdl.Texture
	delay        int
}

func newAnimation(spriteName, filename string, loop bool, knownAs string) *Animation {
	name := filepath.Join(spriteName, filename, "sprite.dat")
	s := newScanner(name)
	defer s.close()

	var frames []*sdl.Texture
	var numFrames int

	s.scan(&numFrames)
	for i := 0; i < numFrames; i++ {
		var str string

		s.scan(&str)
		textureFile := filepath.Join(spriteName, filename, str)
		frames = append(frames, loadTexture(textureFile))
	}

	return &Animation{
		name:    filename,
		loop:    loop,
		delay:   delaySet,
		knownAs: knownAs,
		frames:  frames,
	}
}

func (a *Animation) getKnown() string {
	return a.knownAs
}

func (a *Animation) reset() {
	a.currentFrame = 0
	a.finished = false
}

func (a *Animation) getFrameN(fnum int) *sdl.Texture {
	if fnum >= 0 && fnum < len(a.frames) {
		return a.frames[fnum]
	}
	return a.frames[0]
}

func (a *Animation) getFrame() *sdl.Texture {
	if a.currentFrame == len(a.frames)-1 && a.loop {
		a.currentFrame = 0
	}
	if a.currentFrame == len(a.frames)-1 && !a.loop {
		a.finished = true
	}
	if a.currentFrame < len(a.frames)-1 {
		a.delay--
		if a.delay == 0 {
			a.currentFrame++
			a.delay = delaySet
		}
	}
	return a.frames[a.currentFrame]
}
