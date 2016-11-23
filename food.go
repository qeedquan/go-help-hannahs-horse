package main

type foodinfo struct {
	sx, sy, speed int
}

type Food struct {
	*Ghost1
	alive bool
}

func newFood(filename string, x, y, speed int, af *AnimationFactory) *Food {
	return &Food{
		Ghost1: newGhost1(filename, x, y, speed, af),
		alive:  true,
	}
}