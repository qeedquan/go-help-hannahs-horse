package main

type Bonus struct {
	*Ghost1
	lifeforce int
	typ       int
	alive     bool
}

func newBonus(filename string, x, y, speed int, af *AnimationFactory, typ int) *Bonus {
	return &Bonus{
		Ghost1:    newGhost1(filename, x, y, speed, af),
		lifeforce: 300,
		alive:     true,
		typ:       typ,
	}
}

func (b *Bonus) decLife() {
	if b.lifeforce > 0 {
		b.lifeforce--
	}
}