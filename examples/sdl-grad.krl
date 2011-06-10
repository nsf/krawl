import "SDL/SDL.h"

func put_pixel(screen *SDL.SDL_Surface, x, y, r, g, b int) {
	p := screen.pixels.(*byte) +
		y * screen.pitch +
		x * screen.format.BytesPerPixel

	p[0] = r
	p[1] = g
	p[2] = b
}

func fill(screen *SDL.SDL_Surface) {
	step := 1.0 / 512
	col := 0.0

	for y := 0; y < 512; y++ {
		for x := 0; x < 512; x++ {
			icol := (col * 255).(int)
			put_pixel(screen, x, y, icol, icol, icol)
		}
		col += step
	}
}

func main(argc int, argv **byte) int {
	SDL.SDL_Init(SDL.SDL_INIT_VIDEO)
	screen := SDL.SDL_SetVideoMode(512, 512, 24, SDL.SDL_SWSURFACE)
	fill(screen)
	SDL.SDL_Flip(screen)

	var e SDL.SDL_Event
	for {
		for SDL.SDL_PollEvent(&e) != 0 {
			switch e._type {
			case SDL.SDL_QUIT:
				return 0
			case SDL.SDL_KEYDOWN:
				return 0
			}
		}
	}

	return 0
}