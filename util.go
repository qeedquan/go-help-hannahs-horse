package main

import (
	"bufio"
	"fmt"
	"image"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage"
	"github.com/qeedquan/go-media/sdl/sdlmixer"
	"github.com/qeedquan/go-media/sdl/sdlttf"
)

func dpath(path ...string) string {
	return filepath.Join(dataDir, filepath.Join(path...))
}

func spath(path ...string) string {
	return filepath.Join(saveDir, filepath.Join(path...))
}

func fatal(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
	exit(1)
}

func warn(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format+"\n", args...)
}

func loadTexture(name string, filters ...func(image.Image) image.Image) *sdl.Texture {
	f, err := os.Open(dpath(name))
	if err != nil {
		fatal("texture: %v", err)
	}
	defer f.Close()

	m, _, err := image.Decode(f)
	if err != nil {
		fatal("texture: %v: %v", name, err)
	}
	for _, filter := range filters {
		m = filter(m)
	}

	texture, err := sdlimage.LoadTextureImage(screen.Renderer, m)
	if err != nil {
		fatal("texture: %v: %v", name, err)
	}

	return texture
}

func colorKey(color sdl.Color) func(image.Image) image.Image {
	return func(m image.Image) image.Image {
		return imageutil.ColorKey(m, color)
	}
}

func loadSound(name string) *sdlmixer.Chunk {
	chunk, err := sdlmixer.LoadWAV(dpath(name))
	if err != nil {
		warn("sound: %v: %v", dpath(name), err)
		return nil
	}
	return chunk
}

func loadMusic(name string) *sdlmixer.Music {
	music, err := sdlmixer.LoadMUS(dpath(name))
	if err != nil {
		warn("music: %v: %v", dpath(name), err)
		return nil
	}
	return music
}

func loadFont(name string, ptSize int) *sdlttf.Font {
	font, err := sdlttf.OpenFont(dpath(name), ptSize)
	if err != nil {
		fatal("font: %v: %v", dpath(name), err)
	}
	return font
}

func playMusic(mus *sdlmixer.Music, loops int) {
	if !music || mus == nil {
		return
	}

	mus.Play(loops)
}

func playSound(chunk *sdlmixer.Chunk) {
	if !sound || chunk == nil {
		return
	}

	chunk.PlayChannel(-1, 0)
}

func delay(ms uint32) {
	screen.SetTarget(nil)
	ticker := time.NewTicker(time.Duration(ms) * time.Millisecond)
	for {
		select {
		case <-ticker.C:
			return
		default:
			for {
				ev := sdl.PollEvent()
				if ev == nil {
					break
				}
				switch ev.(type) {
				case sdl.QuitEvent:
					exit(0)
				}
			}
		}
		screen.Copy(target, nil, nil)
		screen.Present()
	}

	ticker.Stop()
}

func exit(status int) {
	os.Exit(status)
}

type scanner struct {
	l       *bufio.Scanner
	f       *os.File
	name    string
	linePos int
	atEOF   bool
}

func newScanner(name string) *scanner {
	f, err := os.Open(dpath(name))
	if err != nil {
		fatal("scanner: %v", err)
	}
	s := &scanner{
		l: bufio.NewScanner(f),
		f: f,
	}
	s.next()
	return s
}

func (s *scanner) next() {
	if !s.l.Scan() {
		err := s.l.Err()
		if err != nil {
			fatal("scanner: %v:%v: %v", s.name, s.linePos, err)
		}
		s.atEOF = true
	}
}

func (s *scanner) eof() bool {
	return s.atEOF
}

func (s *scanner) line() string {
	for !s.eof() {
		text := s.l.Text()
		s.next()

		if strings.TrimSpace(text) == "" {
			continue
		}
		return text
	}

	return ""
}

func (s *scanner) scan(args ...interface{}) {
	line := s.line()
	n, err := fmt.Sscan(line, args...)
	if err != nil {
		fatal("scanner: %v:%v: %v", s.name, s.linePos, err)
	}
	if n != len(args) {
		fatal("scanner: %v:%v: failed to read neccessary input, got %v but expected %v", s.name, s.linePos, n, len(args))
	}
}

func (s *scanner) close() {
	s.f.Close()
}
