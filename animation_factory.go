package main

type AnimationFactory struct {
	store []*Animation
}

func (af *AnimationFactory) getByName(name string) *Animation {
	for _, a := range af.store {
		if a.getKnown() == name {
			return a
		}
	}
	return nil
}

func (af *AnimationFactory) getByNumber(n int) *Animation {
	if n < len(af.store) && n >= 0 {
		return af.store[n]
	}
	return nil
}

func (af *AnimationFactory) loadAnimation(spriteName, path string, loop bool, knownAs string) {
	a := newAnimation(spriteName, path, loop, knownAs)
	af.store = append(af.store, a)
}

func (af *AnimationFactory) size() int {
	return len(af.store)
}