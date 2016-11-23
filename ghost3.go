package main

type Ghost3 struct {
	*ghost
}

func newGhost3(filename string, x, y, speed int, af *AnimationFactory) *Ghost3 {
	g := &Ghost3{}
	g.ghost = newGhost(filename, x, y, speed, af)
	g.startx = x
	g.starty = y
	g.direction = 2
	g.xpos = 50 + x*SIZET
	g.ypos = 50 + y*SIZET
	g.nextypos = g.ypos
	g.nextxpos = g.xpos
	g.speed = 5

	for y := 0; y < SIZEY; y++ {
		for x := 0; x < SIZEX; x++ {
			g.lev.map_[x][y] = lev.map_[x][y]
		}
	}

	return g
}

func (g *Ghost3) move(x, y int) {
	moveGhost123(g.ghost, x, y, 3)
}
