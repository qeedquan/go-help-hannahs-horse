package main

type Ghost1 struct {
	*ghost
}

func newGhost1(filename string, x, y, speed int, af *AnimationFactory) *Ghost1 {
	g := &Ghost1{}
	g.ghost = newGhost(filename, x, y, speed, af)
	g.startx = x
	g.starty = y
	g.lev = lev
	g.speed = speed
	g.direction = 2
	g.xpos = 50 + x*SIZET
	g.ypos = 50 + y*SIZET
	g.nextypos = g.ypos
	g.nextxpos = g.xpos
	return g
}

func (g *Ghost1) move(x, y int) {
	moveGhost123(g.ghost, x, y, 1)
}
