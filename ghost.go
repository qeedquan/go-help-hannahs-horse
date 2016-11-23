package main

import (
	"fmt"
	"math/rand"

	"github.com/qeedquan/go-media/sdl"
)

type Ghost interface {
	move(x, y int)
	frame() *sdl.Texture
	setRespawn(time int)
	setLevel(lev Level, startx, starty int)
	setAnimation(name string)
	setTimer(timerleft int)
	setX(x int)
	setY(y int)
	timer() int
	x() int
	y() int
	startX() int
	startY() int
	isAnimation(name string) bool
}

type ghost struct {
	*Sprite
	nextxpos    int
	nextypos    int
	startx      int
	starty      int
	speed       int
	timeleft    int
	respawntime int
	effect      int
	lev         Level
}

func newGhost(filename string, x, y, speed int, af *AnimationFactory) *ghost {
	return &ghost{
		Sprite: newSprite(filename, af),
	}
}

func (g *ghost) setTimer(timeleft int) {
	g.timeleft = timeleft
}

func (g *ghost) timer() int {
	return g.timeleft
}

func (g *ghost) setLevel(lev Level, startx, starty int) {
	g.lev = lev
	g.startx = startx
	g.starty = starty
}

func (g *ghost) setRespawn(time int) {
	g.respawntime = time
}

func (g *ghost) startX() int {
	return g.startx
}

func (g *ghost) startY() int {
	return g.starty
}

func (g *ghost) setX(x int) {
	g.xpos = 50 + x*SIZET
	g.nextxpos = g.xpos
}

func (g *ghost) setY(y int) {
	g.ypos = 50 + y*SIZET
	g.nextypos = g.ypos
}

func (g *ghost) testDirection(dir int) bool {
	ax := g.xpos/SIZET - 1
	ay := g.ypos/SIZET - 1
	switch dir {
	case 1:
		switch g.lev.map_[ax][ay-1].typ {
		case '#', 'x':
			return false
		}
	case 3:
		switch g.lev.map_[ax][ay+1].typ {
		case '#', 'x', '-':
			return false
		}
	case 4:
		switch g.lev.map_[ax-1][ay].typ {
		case '#', 'x', '-':
			return false
		}
	case 2:
		switch g.lev.map_[ax+1][ay].typ {
		case '#', 'x', '-':
			return false
		}
	default:
		panic(fmt.Sprint("unreachable direction: ", dir))
	}
	return true
}

func moveGhost123(g *ghost, x, y int, typ int) {
	if g.timeleft > 0 {
		g.timeleft--
	} else {
		g.effect = ' '
	}

	if g.respawntime > 0 {
		g.respawntime--
		return
	}

	pacx := x/SIZET - 1
	pacy := y/SIZET - 1

	ax := g.xpos/SIZET - 1
	ay := g.ypos/SIZET - 1

	infront := false
	left := false
	switch g.direction {
	case 1:
		if pacy < ay {
			infront = true
		}
		if pacx < ax {
			left = true
		}
	case 2:
		if pacy < ay {
			left = true
		}
		if !(pacx < ax) {
			infront = true
		}
	case 3:
		if !(pacy < ay) {
			infront = true
		}
		if !(pacx < ax) {
			left = true
		}
	case 4:
		if !(pacy < ay) {
			left = true
		}
		if pacx < ax {
			infront = true
		}
	default:
		panic(fmt.Sprint("unreachable direction: ", g.direction))
	}

	if g.nextxpos == g.xpos && g.nextypos == g.ypos {
		lft := g.direction - 1
		if lft == 0 {
			lft = 4
		}
		rit := g.direction + 1
		if rit == 5 {
			rit = 1
		}
		back := g.direction - 2
		if back < 1 {
			back = -back + 2*g.direction
		}

		newdirection := 0
		if g.testDirection(lft) && !g.testDirection(rit) {
			// left is ok, right is blocked
			newdirection = lft
		} else if !g.testDirection(lft) && g.testDirection(rit) {
			// right is ok, left is blocked
			newdirection = rit
		} else if !g.testDirection(lft) && !g.testDirection(rit) {
			// left and right are blocked
			// ...go backwards
			newdirection = back
		} else if g.testDirection(lft) && g.testDirection(rit) {
			switch typ {
			case 1:
				// left / right both ok. pick one at random.
				rn := rand.Intn(2)
				// random between 0 and 1 inclusive
				if rn == 1 {
					newdirection = lft
				} else {
					newdirection = rit
				}

			case 2:
				rn := rand.Intn(8)
				biaseddirection := 0
				unbiaseddirection := 0
				if left {
					biaseddirection = lft
					unbiaseddirection = rit
				} else {
					unbiaseddirection = rit
					biaseddirection = lft
				}

				if rn == 0 {
					newdirection = unbiaseddirection
				} else {
					newdirection = biaseddirection
				}

			case 3:
				biaseddirection := 0
				if left {
					biaseddirection = lft
				} else {
					biaseddirection = rit
				}
				newdirection = biaseddirection

			default:
				panic(fmt.Sprint("unreachable ghost type: ", typ))
			}
		} else {
			panic("unreachable")
		}

		if !g.testDirection(g.direction) {
			g.direction = newdirection
		} else if g.testDirection(g.direction) && !g.testDirection(rit) && !g.testDirection(lft) {
			// no-op go the same way
		} else {
			switch typ {
			case 1:
				// 50% move forwards, 50% turn
				rn := rand.Intn(2)
				if rn == 1 {
					g.direction = newdirection
				}

			case 2:
				rn := rand.Intn(8)
				biaseddirection := 0
				unbiaseddirection := 0
				if infront {
					biaseddirection = g.direction
					unbiaseddirection = newdirection
				} else {
					unbiaseddirection = g.direction
					biaseddirection = newdirection
				}

				if rn == 0 {
					g.direction = unbiaseddirection
				} else {
					g.direction = biaseddirection
				}

			case 3:
				biaseddirection := 0
				if infront {
					biaseddirection = g.direction
				} else {
					biaseddirection = newdirection
				}
				g.direction = biaseddirection

			default:
				panic(fmt.Sprint("unreachable ghost type: ", typ))
			}
		}

		ax := (g.xpos / SIZET) - 1
		ay := (g.ypos / SIZET) - 1

		switch g.direction {
		case 1:
			g.nextypos = lev.map_[ax][ay-1].t
		case 2:
			g.nextxpos = lev.map_[ax+1][ay].l
		case 3:
			g.nextypos = lev.map_[ax][ay+1].t
		case 4:
			g.nextxpos = lev.map_[ax-1][ay].l
		default:
			panic(fmt.Sprint("unreachable direction: ", g.direction))
		}

	} else if g.nextxpos != g.xpos {
		switch g.direction {
		case 4:
			g.xpos -= speed
		case 2:
			g.xpos += speed
		}
	} else if g.nextypos != g.ypos {
		switch g.direction {
		case 1:
			g.ypos -= speed
		case 3:
			g.ypos += speed
		}
	}
}
