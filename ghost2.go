package main

type Ghost2 struct {
	*ghost
}

func newGhost2(filename string, x, y, speed int, af *AnimationFactory) *Ghost2 {
	g := &Ghost2{}
	g.ghost = newGhost(filename, x, y, speed, af)
	g.speed = speed
	g.startx = x
	g.starty = y
	g.direction = 2
	g.xpos = 50 + x*SIZET
	g.ypos = 50 + y*SIZET
	g.nextxpos = g.ypos
	g.nextypos = g.xpos

	for y := 0; y < SIZEY; y++ {
		for x := 0; x < SIZEX; x++ {
			g.lev.map_[x][y] = lev.map_[x][y]
		}
	}
	return g
}

func (g *Ghost2) move(x, y int) {
	moveGhost123(g.ghost, x, y, 2)
}
