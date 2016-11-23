package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"log"
	"math/rand"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"time"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
	"github.com/qeedquan/go-media/sdl/sdlmixer"
	"github.com/qeedquan/go-media/sdl/sdlttf"
)

type Display struct {
	*sdl.Window
	*sdl.Renderer
}

const (
	pilltime = 200
)

var (
	dataDir    string
	saveDir    string
	fullscreen bool
	music      bool
	sound      bool
	godMode    bool

	screen *Display
)

var (
	hedge     *sdl.Texture
	hedge2    *sdl.Texture
	hedge3    *sdl.Texture
	hedge4    *sdl.Texture
	pill      *sdl.Texture
	powerpill *sdl.Texture
	grass     *sdl.Texture
	grass2    *sdl.Texture
	grass3    *sdl.Texture
	grass4    *sdl.Texture
	bg        *sdl.Texture
	bg2       *sdl.Texture
	gate      *sdl.Texture
	story     *sdl.Texture
	oneway    *sdl.Texture
	howto     *sdl.Texture
	border    *sdl.Texture
	cross     *sdl.Texture
	redDoor   *sdl.Texture
	redKey    *sdl.Texture
	gradient  *sdl.Texture
	banner    *sdl.Texture
	words     *sdl.Texture

	currentHedge *sdl.Texture
	currentGrass *sdl.Texture
)

var (
	font        *sdlttf.Font
	subgamefont *sdlttf.Font
	bigfont     *sdlttf.Font
	texture     *sdl.Texture
	surface     *sdl.Surface
)

var (
	eatpill  *sdlmixer.Chunk
	bgm1     *sdlmixer.Music
	eatmagic *sdlmixer.Chunk
	eatfood  *sdlmixer.Chunk
	scream   *sdlmixer.Chunk
	chomp    *sdlmixer.Chunk
	yeah     *sdlmixer.Chunk
	uhoh     *sdlmixer.Chunk
)

const (
	SIZEX = 15
	SIZEY = 15
	SIZET = 30

	SPEED = 15
)

const (
	NONE = iota
	FREEZE
	INVERT
	SLOW
	INVIS
)

var (
	nextxpos  = 50 + SIZET*7
	nextypos  = 50 + SIZET*7
	ypos      = nextypos
	xpos      = nextxpos
	direction = 5 // 1, 2, 3, 4, 5 (u, d, l, r, stat)
	nextdir   = 5
	speed     = 6

	level    = 1
	lives    = 3
	score    = 0
	numlives = 3

	gamestate    = 0
	subgamestate = 1
	pillsleft    = 0

	effect     = NONE
	bonustimer = 0

	lastticks uint32
	deltaTick uint32
	tick      uint32

	lev       Level
	maps      []Level
	levelfood []*Food
	hiscore   []rank

	af = new(AnimationFactory)

	red      Ghost
	pink     Ghost
	blue     Ghost
	ghosties []Ghost

	hannah *Player

	bonus *Bonus

	target *sdl.Texture
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	log.SetFlags(0)
	parseFlags()
	initVideo()
	initAudio()
	loadGFX()
	loadSounds()
	initGame()
	loop()
}

func newDisplay(w, h int, wflag sdl.WindowFlags) (*Display, error) {
	window, renderer, err := sdl.CreateWindowAndRenderer(w, h, wflag)
	if err != nil {
		return nil, err
	}
	return &Display{window, renderer}, nil
}

func loop() {
	subgamestate = 1
	playMusic(bgm1, -1)
	gamestate = 0

	for {
		tick = sdl.GetTicks()
		deltaTick = tick - lastticks
		lastticks = tick

		// 40*25 ms = 1 second
		if deltaTick < 32 {
			sdl.Delay(32 - deltaTick)
			tick = sdl.GetTicks()
			deltaTick += tick - lastticks
			lastticks = tick
		}

		event()
		step()
	}
}

func event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}

		switch ev := ev.(type) {
		case sdl.QuitEvent:
			exit(0)
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_g:
				godMode = !godMode
				fmt.Println("godmode:", godMode)
			case sdl.K_RIGHT:
				if effect == INVERT {
					nextdir = 3
				} else {
					nextdir = 4
				}
			case sdl.K_LEFT:
				if effect == INVERT {
					nextdir = 4
				} else {
					nextdir = 3
				}
			case sdl.K_UP:
				nextdir = 1
			case sdl.K_DOWN:
				nextdir = 2
			case sdl.K_h:
				if gamestate == 0 {
					gamestate = 3
				}
			case sdl.K_s:
				if gamestate == 0 {
					gamestate = 4
				}
			case sdl.K_p:
				if gamestate == 1 && subgamestate == 0 {
					subgamestate = 4
				} else if gamestate == 1 && subgamestate == 4 {
					subgamestate = 0
				}
			case sdl.K_c:
				if gamestate == 0 {
					gamestate = 5
				}
			case sdl.K_ESCAPE:
				if gamestate == 0 {
					saveHiScores()
					exit(0)
				} else {
					gamestate = 0
					level = 1
				}
			case sdl.K_SPACE:
				switch gamestate {
				case 0:
					lives = numlives
					score = 0
					hannah.setAnimation("default")
					gamestate = 1
					initLevel(level)
				case 3, 4, 5:
					gamestate = 0
					level = 1
				}
			}
		}
	}
}

func step() {
	screen.SetTarget(target)
	screen.SetDrawColor(sdlcolor.Black)
	screen.Clear()

	switch gamestate {
	case 0: // title
		blit(story, 0, 0)

	case 1: // main
		blit(bg, 0, 0)
		blit(bg2, 550, 0)
		drawMap()
		drawScore()
		switch subgamestate {
		case 0:
			timer()
			checkPillCollision()
			move()
			drawBaddies()
			createBonus()

			if effect != FREEZE {
				red.move(xpos, ypos)
				pink.move(xpos, ypos)
				blue.move(xpos, ypos)
			}
			moveFood()
			moveBonus()
			checkBonus()
			ghost := checkBaddies()
			if ghost != nil && ghost.timer() == 0 && hannah.isAnimation("default") && !godMode {
				playSound(scream)
				hannah.setAnimation("die")
			} else if ghost != nil && (ghost.timer() > 0 || godMode) {
				ghost.setTimer(0)
				ghost.setAnimation("default")
				ghost.setRespawn(50)
				ghost.setX(ghost.startX())
				ghost.setY(ghost.startY())
				playSound(chomp)
				score += 400
			}

			for _, g := range ghosties {
				if g.timer() == 0 && g.isAnimation("flash") {
					g.setAnimation("default")
				}
			}

			if hannah.isAnimation("die") && hannah.animationFinished() {
				if lives--; lives == 0 {
					freeFood()
					subgamestate = 3
				} else {
					reInitLevel()
					subgamestate = 2
				}
			}
			checkFood()

		case 1:
			drawBaddies()
			drawScore()
			drawSubGameState(1)
			delay(2000)
			subgamestate = 0

		case 2:
			drawBaddies()
			drawScore()
			drawSubGameState(2)
			subgamestate = 0

		case 3:
			drawScore()
			drawSubGameState(subgamestate)
			gamestate = 0
			delay(2000)

			pos := checkHiScore(score)
			if pos > -1 {
				blit(border, 190, 100)
				setHiScore(pos)
				gamestate = 0
			}

			lives = numlives
			score = 0
			level = 1

		case 4:
			drawBaddies()
			drawScore()

		default:
			panic("unreachable")
		}

	case 3:
		blit(story, 0, 0)
		blit(howto, 190, 100)

	case 4:
		blit(story, 0, 0)
		blit(border, 190, 100)
		drawHiScores()

	case 5:
		blit(story, 0, 0)
		blit(border, 190, 100)
		drawCredits()

	case 6:
		endGame()
		gamestate = 0

	default:
		panic("unreachable")
	}

	screen.SetTarget(nil)
	screen.Copy(target, nil, nil)
	screen.Present()
}

func parseFlags() {
	dataDir = filepath.Join(sdl.GetBasePath(), "assets")
	saveDir = sdl.GetPrefPath("", "hhh")
	flag.StringVar(&dataDir, "data", dataDir, "data directory")
	flag.StringVar(&saveDir, "save", saveDir, "save directory")
	flag.BoolVar(&fullscreen, "fullscreen", false, "fullscreen")
	flag.BoolVar(&music, "music", true, "enable music")
	flag.BoolVar(&sound, "sound", true, "enable sound")
	flag.BoolVar(&godMode, "god", false, "be a god")
	flag.IntVar(&level, "level", level, "start on level")
	flag.IntVar(&numlives, "lives", numlives, "number of lives")
	flag.Parse()

	if level <= 0 || level > 15 {
		fatal("flag: invalid level %v", level)
	}
}

func initVideo() {
	err := sdl.Init(sdl.INIT_EVERYTHING &^ sdl.INIT_AUDIO)
	if err != nil {
		fatal("sdl: %v", err)
	}

	err = sdlttf.Init()
	if err != nil {
		fatal("sdl: %v", err)
	}

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")

	w, h := 800, 600
	wflag := sdl.WINDOW_RESIZABLE
	if fullscreen {
		wflag |= sdl.WINDOW_FULLSCREEN_DESKTOP
	}
	screen, err = newDisplay(w, h, wflag)
	if err != nil {
		fatal("sdl: %v", err)
	}

	screen.SetLogicalSize(w, h)
	screen.SetTitle("Help Hannah's Horse")
	screen.SetDrawColor(sdlcolor.Black)
	screen.Clear()
	screen.Present()

	target, err = screen.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_TARGET, 800, 600)
	if err != nil {
		fatal("sdl: %v", err)
	}
	target.SetBlendMode(sdl.BLENDMODE_BLEND)

	sdl.ShowCursor(0)
	sdl.StartTextInput()

	texture, err = screen.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	if err != nil {
		fatal("sdl: %v", err)
	}

	surface, err = sdl.CreateRGBSurfaceWithFormat(sdl.SWSURFACE, w, h, 32, sdl.PIXELFORMAT_ABGR8888)
	if err != nil {
		fatal("sdl: %v", err)
	}
}

func initAudio() {
	err := sdlmixer.OpenAudio(22050, sdlmixer.DEFAULT_FORMAT, 2, 1024)
	if err != nil {
		warn("sdl: %v", err)
	}
}

func loadGFX() {
	hedge = loadTexture("images/hedge.png")
	hedge2 = loadTexture("images/hedge2.png")
	hedge3 = loadTexture("images/hedge3.png")
	hedge4 = loadTexture("images/hedge4.png")

	pink := sdl.Color{0x0, 0xff, 0xff, 0xff}
	pill = loadTexture("images/pill.png", colorKey(pink))
	powerpill = loadTexture("images/powerpill.png", colorKey(pink))
	grass = loadTexture("images/grass.png")
	grass2 = loadTexture("images/grass2.png")
	grass3 = loadTexture("images/grass3.png")
	grass4 = loadTexture("images/grass4.png")

	bg = loadTexture("images/bg_left.png")
	bg2 = loadTexture("images/bg_right2_1.png")
	story = loadTexture("images/story.png", colorKey(pink))
	gate = loadTexture("images/gate.png", colorKey(pink))
	oneway = loadTexture("images/oneway.png")
	howto = loadTexture("images/howtoplay.png")
	border = loadTexture("images/border.png")
	cross = loadTexture("images/cross.png")
	redDoor = loadTexture("images/red_door.png")
	redKey = loadTexture("images/red_key.png")
	gradient = loadTexture("images/gradient.png")
	banner = loadTexture("images/end_banner.png")
	words = loadTexture("images/congrats.png")

	font = loadFont("font.ttf", 36)
	bigfont = loadFont("subgamefont.ttf", 108)
	subgamefont = loadFont("subgamefont.ttf", 24)
}

func loadSounds() {
	eatpill = loadSound("sounds/DripC.wav")
	eatmagic = loadSound("sounds/gulp.wav")
	eatfood = loadSound("sounds/crunch.wav")
	scream = loadSound("sounds/scream.wav")
	// for some reason chomp doesn't decode and you get random noise
	// use another bite sound
	// chomp = loadSound("sounds/chomp.wav")
	chomp = loadSound("sounds/bite.wav")
	uhoh = loadSound("sounds/uh-oh.wav")
	yeah = loadSound("sounds/yeah.wav")
	bgm1 = loadMusic("bgm1.ogg")
}

func initGame() {
	for i := 1; i <= 15; i++ {
		readMap(fmt.Sprintf("maps/map%v.dat", i))
	}

	af.loadAnimation("hannah", "die", false, "hannah-die")
	af.loadAnimation("hannah", "default", true, "hannah-default")
	af.loadAnimation("hannah", "invis", true, "hannah-invis")

	af.loadAnimation("red", "default", true, "red-default")
	af.loadAnimation("pink", "default", true, "pink-default")
	af.loadAnimation("blue", "default", true, "blue-default")

	af.loadAnimation("red", "flash", true, "red-flash")
	af.loadAnimation("pink", "flash", true, "pink-flash")
	af.loadAnimation("blue", "flash", true, "blue-flash")

	af.loadAnimation("carrot", "default", true, "carrot-default")

	af.loadAnimation("freeze", "default", true, "freeze-default")
	af.loadAnimation("invis", "default", true, "invis-default")
	af.loadAnimation("invert", "default", true, "invert-default")

	red = newGhost1("red", 0, 0, 5, af)
	pink = newGhost2("pink", 0, 0, 5, af)
	blue = newGhost3("blue", 0, 0, 5, af)
	ghosties = []Ghost{red, pink, blue}

	hannah = newPlayer("hannah", af)

	loadHiScores()
}

func checkNext() bool {
	return checkCollide(direction)
}

func checkCollisions() bool {
	return checkCollide(nextdir)
}

func checkCollide(dir int) bool {
	ax := xpos/SIZET - 1
	ay := ypos/SIZET - 1
	switch dir {
	case 1:
		switch lev.map_[ax][ay-1].typ {
		case '#', '~', '$':
			return true
		}
	case 2:
		switch lev.map_[ax][ay+1].typ {
		case '#', '-', '~', '$':
			return true
		}
	case 3:
		switch lev.map_[ax-1][ay].typ {
		case '#', '$':
			return true
		}
	case 4:
		switch lev.map_[ax+1][ay].typ {
		case '#', '$':
			return true
		}
	}

	return false
}

func checkPillCollision() {
	ax := xpos/SIZET - 1
	ay := ypos/SIZET - 1
	m := &lev.map_[ax][ay]
	switch m.typ {
	case '.':
		m.typ = '_'
		score += 10
		playSound(eatpill)
		if pillsleft--; pillsleft == 0 {
			level++
			freeFood()
			if level > len(maps) {
				endGame()
			} else {
				subgamestate = 1
				initLevel(level)
				initHannah(7, 7)
			}
		}
	case 'p':
		m.typ = '_'
		score += 100
		playSound(eatmagic)
		red.setTimer(pilltime)
		blue.setTimer(pilltime)
		pink.setTimer(pilltime)
		red.setAnimation("flash")
		blue.setAnimation("flash")
		pink.setAnimation("flash")
	case 'r':
		m.typ = '_'
		openRedDoor()
	}
}

func blit(texture *sdl.Texture, x, y int) {
	_, _, w, h, err := texture.Query()
	if err != nil {
		fatal("texture: %v", err)
	}
	screen.Copy(texture, nil, &sdl.Rect{int32(x), int32(y), int32(w), int32(h)})
}

func blitText(font *sdlttf.Font, x, y int, c sdl.Color, format string, args ...interface{}) {
	text := fmt.Sprintf(format, args...)
	r, err := font.RenderUTF8BlendedEx(surface, text, c)
	if err != nil {
		fatal("text: %v", err)
	}

	p, err := texture.Lock(nil)
	if err != nil {
		fatal("text: %v", err)
	}

	err = surface.Lock()
	if err != nil {
		fatal("text: %v", err)
	}
	s := surface.Pixels()
	for i := 0; i < len(p); i += 4 {
		p[i] = s[i+2]
		p[i+1] = s[i]
		p[i+2] = s[i+1]
		p[i+3] = s[i+3]
	}

	surface.Unlock()
	texture.Unlock()

	texture.SetBlendMode(sdl.BLENDMODE_BLEND)
	screen.Copy(texture, &sdl.Rect{0, 0, r.W, r.H}, &sdl.Rect{int32(x), int32(y), r.W, r.H})
}
func drawMap() {
	for mapy := 0; mapy < SIZEY; mapy++ {
		for mapx := 0; mapx < SIZEX; mapx++ {
			x := mapx*SIZET + 50
			y := mapy*SIZET + 50
			switch lev.map_[mapx][mapy].typ {
			case '#':
				blit(currentHedge, x, y)
			case '.':
				blit(currentGrass, x, y)
				blit(pill, x, y)
			case '_':
				blit(currentGrass, x, y)
			case 'p':
				blit(currentGrass, x, y)
				blit(powerpill, x, y)
			case '-':
				blit(currentGrass, x, y)
				blit(gate, x, y)
			case '~':
				blit(currentGrass, x, y)
				blit(oneway, x, y)
			case 'x':
				blit(currentGrass, x, y)
				blit(cross, x, y)
			case '$':
				blit(currentGrass, x, y)
				blit(redDoor, x, y)
			case 'r':
				blit(currentGrass, x, y)
				blit(redKey, x, y)
			}
		}
	}
	blit(hannah.frame(), xpos, ypos)
}

func readMap(filename string) {
	var levl Level

	s := newScanner(filename)
	defer s.close()

	for y := 0; y < SIZEY; y++ {
		line := s.line()
		if len(line) != SIZEX {
			fatal("map: %v: invalid width at line %d, expected %d but got %d", dpath(filename), s.line, len(line), SIZEX)
		}
		for x := 0; x < SIZEX; x++ {
			m := &levl.map_[x][y]
			m.typ = int(line[x])
			if m.typ == '.' {
				levl.pillsleft++
			}
			m.t = y*SIZET + 50
			m.b = m.t + 49
			m.l = x*SIZET + 50
			m.r = m.l + 49
		}
	}

	var line string
	s.scan(&line, &levl.px, &levl.py)
	s.scan(&line, &levl.bx, &levl.by)
	s.scan(&line, &levl.rx, &levl.ry)

	var food foodinfo
	for !s.eof() {
		s.scan(&line, &food.sx, &food.sy, &food.speed)
		levl.foods = append(levl.foods, food)
	}

	maps = append(maps, levl)
}

func drawHiScores() {
	y := 120
	for _, r := range hiscore {
		drawTextAt(fmt.Sprint(r.score), 200, y)
		drawTextAt(r.name, 400, y)
		y += 35
	}

	drawTextAt("Press SPACE", 430, 480)
}

func drawCredits() {
	drawTextAt("Help Hannah's Horse", 200, 120)
	drawTextAt("(c)2006 Steve Branley", 200, 170)
	drawTextAt("Graphics - Steve Branley", 200, 220)
	drawTextAt("Programming - Steve Branley", 200, 270)
	drawTextAt("Inspiration - Hannah Williams", 200, 320)
	drawTextAt("Encouragement - Zoe Cunningham", 200, 370)
	drawTextAt("This game is released under", 200, 420)
	drawTextAt("the terms of the GPL v2", 200, 470)
}

func drawScore() {
	blitText(font, 610, 260, sdlcolor.Black, "%v", score)
	blitText(font, 610, 350, sdlcolor.Black, "Level %v", level)
	blitText(font, 610, 440, sdlcolor.Black, "Lives %v", lives)
}

func drawBigTextAt(text string, x, y int) {
	blitText(bigfont, x, y, sdlcolor.Black, "%s", text)
}

func drawTextAt(text string, x, y int) {
	blitText(subgamefont, x, y, sdlcolor.Black, "%s", text)

}

func drawBaddies() {
	blit(red.frame(), red.x(), red.y())
	blit(pink.frame(), pink.x(), pink.y())
	blit(blue.frame(), blue.x(), blue.y())
	drawFood()
}

func drawFood() {
	for _, f := range levelfood {
		if f.alive {
			blit(f.frame(), f.x(), f.y())
		}
	}
}

func createBonus() {
	if !(bonus == nil && effect == NONE) {
		return
	}
	if sdl.GetTicks()%200 != 0 {
		return
	}
	switch rand.Intn(3) + 1 {
	case 1:
		bonus = newBonus("freeze", 7, 7, 1, af, 'f')
	case 2:
		bonus = newBonus("invis", 7, 7, 1, af, 'i')
	case 3:
		bonus = newBonus("invert", 7, 7, 1, af, 's')
	}
	bonus.setLevel(lev, 7, 7)
}

func move() {
	ax := xpos/SIZET - 1
	ay := ypos/SIZET - 1
	collision := checkCollisions()
	if !collision && xpos == nextxpos && ypos == nextypos {
		direction = nextdir
		setNextPos(direction, ax, ay)
	} else if collision && xpos == nextxpos && ypos == nextypos {
		switch {
		case nextdir == direction:
			direction = 5
			nextdir = 5
		case nextdir != direction && checkNext():
			direction = 5
			nextdir = 5
		default:
			setNextPos(direction, ax, ay)
		}
	}

	if nextxpos != xpos {
		switch direction {
		case 3:
			xpos -= speed
		case 4:
			xpos += speed
		}
	} else if nextypos != ypos {
		switch direction {
		case 1:
			ypos -= speed
		case 2:
			ypos += speed
		}
	}
}

func initFood(levl Level) {
	lev.foods = lev.foods[:0]
	for _, f := range levl.foods {
		food := newFood("carrot", f.sx, f.sy, f.speed, af)
		food.setLevel(levl, f.sx, f.sy)
		levelfood = append(levelfood, food)
		pillsleft++
	}
}

func moveFood() {
	for _, f := range levelfood {
		f.move(xpos, ypos)
	}
}

func moveBonus() {
	if bonus == nil {
		return
	}

	bonus.move(xpos, ypos)
	blit(bonus.frame(), bonus.x(), bonus.y())
	bonus.decLife()
	if bonus.lifeforce == 0 {
		bonus = nil
	}
}

func checkBonus() {
	if bonus == nil {
		return
	}
	if bonus.x() < xpos+30 && bonus.x()+30 > xpos && bonus.y()+30 > ypos && bonus.y() < ypos+30 {
		switch bonus.typ {
		case 'f':
			setTimer(200, FREEZE)
			playSound(yeah)
		case 's':
			if !godMode {
				setTimer(200, INVERT)
				playSound(uhoh)
			}
		case 'i':
			setTimer(200, INVIS)
			playSound(yeah)
		}
		bonus = nil
	}
}

func setNextPos(dir, ax, ay int) {
	switch dir {
	case 1:
		nextypos = lev.map_[ax][ay-1].t
	case 2:
		nextypos = lev.map_[ax][ay+1].t
	case 3:
		nextxpos = lev.map_[ax-1][ay].l
	case 4:
		nextxpos = lev.map_[ax+1][ay].l
	}
}

func openRedDoor() {
	for x := 0; x < SIZEX; x++ {
		for y := 0; y < SIZEY; y++ {
			if lev.map_[x][y].typ == '$' {
				lev.map_[x][y].typ = '_'
			}
		}
	}
}

func endGame() {
	bx := 150
	by := -500
	wx := 150
	wy := 650
	bend := false
	wend := false

loop:
	for {
		for {
			ev := sdl.PollEvent()
			if ev == nil {
				break
			}
			switch ev.(type) {
			case sdl.QuitEvent:
				exit(0)
			case sdl.KeyDownEvent:
				if bend && wend {
					break loop
				}
			}
		}

		if by < 100 {
			by += 5
		} else {
			bend = true
		}

		if wy > 100 {
			wy -= 5
		} else {
			wend = true
		}
		blit(gradient, 0, 0)
		blit(banner, bx, by)
		blit(words, wx, wy)
		screen.Present()
	}

	subgamestate = 3
}

func setTimer(t, e int) {
	effect = e
	bonustimer = t
	if e == INVIS {
		hannah.setAnimation("invis")
	} else if hannah.isAnimation("invis") {
		hannah.setAnimation("default")
	}
}

func timer() {
	if bonustimer > 0 {
		bonustimer--
	} else {
		effect = NONE
		if hannah.isAnimation("invis") {
			hannah.setAnimation("default")
		}
	}
}

func checkBaddies() Ghost {
	if effect != INVIS || godMode {
		for _, g := range ghosties {
			if g.x() < xpos+30 && g.x()+30 > xpos && g.y()+30 > ypos && g.y() < ypos+30 {
				return g
			}
		}
	}
	return nil
}

func initLevel(levelNum int) {
	switch {
	case levelNum < 4:
		currentGrass = grass
		currentHedge = hedge
	case levelNum < 7:
		currentGrass = grass2
		currentHedge = hedge2
	case levelNum < 10:
		currentGrass = grass3
		currentHedge = hedge3
	case levelNum < 13:
		currentGrass = grass4
		currentHedge = hedge4
	default:
		currentGrass = grass
		currentHedge = hedge
	}

	ghosties = ghosties[:0]
	bonus = nil

	subgamestate = 1
	setTimer(0, NONE)
	lev = maps[levelNum-1]
	red.setLevel(lev, lev.rx, lev.ry)
	red.setX(lev.rx)
	red.setY(lev.ry)
	red.setAnimation("default")
	red.setTimer(0)

	pink.setLevel(lev, lev.px, lev.py)
	pink.setX(lev.px)
	pink.setY(lev.py)
	pink.setAnimation("default")
	pink.setTimer(0)

	blue.setLevel(lev, lev.bx, lev.by)
	blue.setX(lev.bx)
	blue.setY(lev.by)
	blue.setAnimation("default")
	blue.setTimer(0)

	pillsleft = lev.pillsleft
	initFood(lev)

	ghosties = []Ghost{red, pink, blue}

	initHannah(7, 7)
}

func initHannah(x, y int) {
	nextxpos = 50 + SIZET*x
	nextypos = 50 + SIZET*y
	xpos = nextxpos
	ypos = nextypos
	direction = 5
	nextdir = 5
}

func checkFood() bool {
	for _, f := range levelfood {
		if f.x() < xpos+30 && f.x()+30 > xpos && f.y()+30 > ypos && f.y() < ypos+30 && f.alive {
			playSound(eatfood)
			f.alive = false
			score += 50
			if pillsleft--; pillsleft == 0 {
				level++
				subgamestate = 1
				initLevel(level)
				initHannah(7, 7)
			}
			return true
		}
	}
	return false
}

func reInitLevel() {
	red.setX(lev.rx)
	red.setY(lev.ry)
	red.setRespawn(0)

	blue.setX(lev.bx)
	blue.setY(lev.by)
	blue.setRespawn(0)

	pink.setX(lev.px)
	pink.setY(lev.py)
	pink.setRespawn(0)

	red.setAnimation("default")
	pink.setAnimation("default")
	blue.setAnimation("default")
	red.setTimer(0)
	blue.setTimer(0)
	pink.setTimer(0)

	blit(bg2, 550, 0)
	initHannah(7, 7)

	setTimer(0, NONE)

	drawMap()
	move()
	red.move(xpos, ypos)
	blue.move(xpos, ypos)
	pink.move(xpos, ypos)
	drawBaddies()

	hannah.respawn()
	blit(hannah.frame(), xpos, ypos)
	bonus = nil
}

func freeFood() {
	levelfood = levelfood[:0]
}

func drawSubGameState(subgamestate int) {
	switch subgamestate {
	case 1:
		drawTextAt(fmt.Sprintf("Level %v - Get Ready!", level), 170, 232)
	case 2:
		drawTextAt("Get Ready!", 220, 232)
	case 3:
		drawTextAt("Game Over!", 220, 232)
	}
}

type rank struct {
	name  string
	score int
}

type rankSlice []rank

func (p rankSlice) Len() int           { return len(p) }
func (p rankSlice) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }
func (p rankSlice) Less(i, j int) bool { return p[i].score > p[j].score }

var defaultRanks = []rank{
	{"sab", 25600},
	{"zjc", 12800},
	{"hjw", 6400},
	{"sab", 3200},
	{"zjc", 1600},
	{"hjw", 800},
	{"sab", 400},
	{"zjc", 200},
	{"hjw", 100},
	{"sab", 50},
}

func loadHiScores() {
	defer func() {
		for i := 0; len(hiscore) < 10; i++ {
			hiscore = append(hiscore, defaultRanks[i])
		}

		hiscore = hiscore[:10]
		sort.Stable(rankSlice(hiscore))
	}()

	hiscore = hiscore[:0]
	f, err := os.Open(spath("hiscore.dat"))
	if err != nil {
		return
	}
	defer f.Close()

	var r rank
	s := bufio.NewScanner(f)
	for i := 0; i < 10; {
		if !s.Scan() {
			return
		}
		line := s.Text()
		n, err := fmt.Sscan(line, &r.name, &r.score)
		if n != 2 || err != nil {
			continue
		}

		hiscore = append(hiscore, r)
		i++
	}
}

func saveHiScores() {
	os.MkdirAll(spath(""), 0755)
	f, err := os.Create(spath("hiscore.dat"))
	if err != nil {
		warn("hiscore: %v", err)
		return
	}

	b := new(bytes.Buffer)
	for _, r := range hiscore {
		fmt.Fprintln(b, r.score, r.name)
	}
	_, err = f.Write(b.Bytes())
	if err != nil {
		warn("hiscore: %v", err)
	}

	err = f.Close()
	if err != nil {
		warn("hiscore: %v", err)
	}
}

func isLetter(r rune) bool {
	switch {
	case 'A' <= r && r <= 'Z',
		'a' <= r && r <= 'z':
		return true
	}
	return false
}

func setHiScore(pos int) {
	initial := "aaa"
	screen.SetTarget(nil)
loop:
	for {
		for {
			ev := sdl.PollEvent()
			if ev == nil {
				break
			}
			switch ev := ev.(type) {
			case sdl.QuitEvent:
				exit(0)
			case sdl.KeyDownEvent:
				switch ev.Sym {
				case sdl.K_RETURN:
					if len(initial) == 3 {
						break loop
					}

				case sdl.K_BACKSPACE:
					if len(initial) > 0 {
						initial = initial[:len(initial)-1]
					}
				}
			case sdl.TextInputEvent:
				for _, r := range string(ev.Text[:]) {
					if r == 0 {
						break
					}
					if len(initial) < 3 && isLetter(r) {
						initial += string(r)
					} else {
						break
					}
				}
			}
		}

		indx := 250
		indy := 350
		switch len(initial) {
		case 2:
			indx = 325
		case 3:
			indx = 400
		}

		screen.Copy(target, nil, nil)
		blit(border, 190, 100)
		drawTextAt("Enter your initials", 210, 150)
		drawBigTextAt(initial, 250, 250)
		blit(powerpill, indx, indy)
		screen.Present()
	}

	hiscore[pos].name = initial
}

func checkHiScore(score int) int {
	n := -1
	hiscore = append(hiscore, rank{"god", score})
	for i := len(hiscore) - 1; i > 0; i-- {
		if hiscore[i].score >= hiscore[i-1].score {
			hiscore[i], hiscore[i-1] = hiscore[i-1], hiscore[i]
		} else {
			n = i - 1
			break
		}
	}
	hiscore = hiscore[:10]
	return n
}
