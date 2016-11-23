package main

type gridcell struct {
	typ        int
	l, r, t, b int
}

type Level struct {
	map_      [SIZEX][SIZEY]gridcell
	px, py    int
	rx, ry    int
	bx, by    int
	pillsleft int
	foods     []foodinfo
}
