//#define GRAYWIN
//#define TESTALPHA
//#define TESTBLITSPEED

#include "portable.h"

#ifdef CONFIG_SDL_PORT
#include <sdl/SDL_main.h>
#endif

int logo(int argc, char *argv[]);
int dune(int argc, char *argv[]);

static char *logo_argv[] = {
	"logo.exe"
};
static char *dune_argv[] = {
	"duneprg.exe",
	"ENG",
	"VGA",
	"ADL",
	"HIM"
};
static int logo_argc = sizeof(logo_argv) / sizeof(*logo_argv);
static int dune_argc = sizeof(dune_argv) / sizeof(*dune_argv);

int main(int argc, char *argv[]) {
	port_init_video();


	logo(logo_argc, logo_argv);
/*	dune(dune_argc, dune_argv); */


	port_close_video();
	return 0;
}






























/*** SAMPLES FOR SDL TESTING FROM SDL PACKAGE ***/

#ifdef GRAYWIN
/* Simple program:  Fill a colormap with gray and stripe it down the screen */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sdl/SDL.h>

#ifdef TEST_VGA16 /* Define this if you want to test VGA 16-color video modes */
#define NUM_COLORS	16
#else
#define NUM_COLORS	256
#endif

/* Draw a randomly sized and colored box centered about (X,Y) */
void DrawBox(SDL_Surface *screen, int X, int Y, int width, int height)
{
	static unsigned int seeded = 0;
	SDL_Rect area;
	Uint32 color;
        Uint32 randc;

	/* Seed the random number generator */
	if ( seeded == 0 ) {
		srand(time(NULL));
		seeded = 1;
	}

	/* Get the bounds of the rectangle */
	area.w = (rand()%width);
	area.h = (rand()%height);
	area.x = X-(area.w/2);
	area.y = Y-(area.h/2);
        randc = (rand()%NUM_COLORS);

        if (screen->format->BytesPerPixel==1)
        {
            color = randc;
        }
        else
        {
            color = SDL_MapRGB(screen->format, randc, randc, randc);
        }

	/* Do it! */
	SDL_FillRect(screen, &area, color);
	if ( screen->flags & SDL_DOUBLEBUF ) {
		SDL_Flip(screen);
	} else {
		SDL_UpdateRects(screen, 1, &area);
	}
}

void DrawBackground(SDL_Surface *screen)
{
	int i, j, k;
	Uint8  *buffer;
	Uint16 *buffer16;
        Uint16 color;
        Uint8  gradient;

	/* Set the surface pixels and refresh! */
	/* Use two loops in case the surface is double-buffered (both sides) */

	for ( j=0; j<2; ++j ) {
		if ( SDL_LockSurface(screen) < 0 ) {
			fprintf(stderr, "Couldn't lock display surface: %s\n",
								SDL_GetError());
			return;
		}
		buffer = (Uint8 *)screen->pixels;

		if (screen->format->BytesPerPixel!=2) {
			for ( i=0; i<screen->h; ++i ) {
				memset(buffer,(i*(NUM_COLORS-1))/screen->h, screen->w * screen->format->BytesPerPixel);
				buffer += screen->pitch;
			}
		}
                else
                {
			for ( i=0; i<screen->h; ++i ) {
				gradient=((i*(NUM_COLORS-1))/screen->h);
                                color = SDL_MapRGB(screen->format, gradient, gradient, gradient);
                                buffer16=(Uint16*)buffer;
                                for (k=0; k<screen->w; k++)
                                {
                                   *(buffer16+k)=color;
                                }
				buffer += screen->pitch;
			}
                }

		SDL_UnlockSurface(screen);
		if ( screen->flags & SDL_DOUBLEBUF ) {
			SDL_Flip(screen);
		} else {
			SDL_UpdateRect(screen, 0, 0, 0, 0);
                        break;
		}
	}
}

SDL_Surface *CreateScreen(Uint16 w, Uint16 h, Uint8 bpp, Uint32 flags)
{
	SDL_Surface *screen;
	int i;
	SDL_Color palette[NUM_COLORS];

	/* Set the video mode */
	screen = SDL_SetVideoMode(w, h, bpp, flags);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set display mode: %s\n",
							SDL_GetError());
		return(NULL);
	}
	fprintf(stderr, "Screen is in %s mode\n",
		(screen->flags & SDL_FULLSCREEN) ? "fullscreen" : "windowed");

	if (bpp==8) {
		/* Set a gray colormap, reverse order from white to black */
		for ( i=0; i<NUM_COLORS; ++i ) {
			palette[i].r = (NUM_COLORS-1)-i * (256 / NUM_COLORS);
			palette[i].g = (NUM_COLORS-1)-i * (256 / NUM_COLORS);
			palette[i].b = (NUM_COLORS-1)-i * (256 / NUM_COLORS);
		}
		SDL_SetColors(screen, palette, 0, NUM_COLORS);
	}

	return(screen);
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	Uint32 videoflags;
	int    done;
	SDL_Event event;
	int width, height, bpp;

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}

	/* See if we try to get a hardware colormap */
	width = 640;
	height = 480;
	bpp = 8;
	videoflags = SDL_SWSURFACE;
	while ( argc > 1 ) {
		--argc;
		if ( argv[argc-1] && (strcmp(argv[argc-1], "-width") == 0) ) {
			width = atoi(argv[argc]);
			--argc;
		} else
		if ( argv[argc-1] && (strcmp(argv[argc-1], "-height") == 0) ) {
			height = atoi(argv[argc]);
			--argc;
		} else
		if ( argv[argc-1] && (strcmp(argv[argc-1], "-bpp") == 0) ) {
			bpp = atoi(argv[argc]);
			--argc;
		} else
		if ( argv[argc] && (strcmp(argv[argc], "-hw") == 0) ) {
			videoflags |= SDL_HWSURFACE;
		} else
		if ( argv[argc] && (strcmp(argv[argc], "-hwpalette") == 0) ) {
			videoflags |= SDL_HWPALETTE;
		} else
		if ( argv[argc] && (strcmp(argv[argc], "-flip") == 0) ) {
			videoflags |= SDL_DOUBLEBUF;
		} else
		if ( argv[argc] && (strcmp(argv[argc], "-noframe") == 0) ) {
			videoflags |= SDL_NOFRAME;
		} else
		if ( argv[argc] && (strcmp(argv[argc], "-resize") == 0) ) {
			videoflags |= SDL_RESIZABLE;
		} else
		if ( argv[argc] && (strcmp(argv[argc], "-fullscreen") == 0) ) {
			videoflags |= SDL_FULLSCREEN;
		} else {
			fprintf(stderr, "Usage: %s [-width] [-height] [-bpp] [-hw] [-hwpalette] [-flip] [-noframe] [-fullscreen] [-resize]\n",
								argv[0]);
			exit(1);
		}
	}

	/* Set a video mode */
	screen = CreateScreen(width, height, bpp, videoflags);
	if ( screen == NULL ) {
		exit(2);
	}
        
        DrawBackground(screen);
		
	/* Wait for a keystroke */
	done = 0;
	while ( !done && SDL_WaitEvent(&event) ) {
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				DrawBox(screen, event.button.x, event.button.y, width, height);
				break;
			case SDL_KEYDOWN:
				/* Ignore ALT-TAB for windows */
				if ( (event.key.keysym.sym == SDLK_LALT) ||
				     (event.key.keysym.sym == SDLK_TAB) ) {
					break;
				}
				/* Center the mouse on <SPACE> */
				if ( event.key.keysym.sym == SDLK_SPACE ) {
					SDL_WarpMouse(width/2, height/2);
					break;
				}
				/* Toggle fullscreen mode on <RETURN> */
				if ( event.key.keysym.sym == SDLK_RETURN ) {
					videoflags ^= SDL_FULLSCREEN;
					screen = CreateScreen(
						screen->w, screen->h,
						screen->format->BitsPerPixel,
								videoflags);
					if ( screen == NULL ) {
						fprintf(stderr,
					"Couldn't toggle fullscreen mode\n");
						done = 1;
					}
                                        DrawBackground(screen);
					break;
				}
				/* Any other key quits the application... */
			case SDL_QUIT:
				done = 1;
				break;
			case SDL_VIDEOEXPOSE:
				DrawBackground(screen);
				break;
			case SDL_VIDEORESIZE:
					screen = CreateScreen(
						event.resize.w, event.resize.h,
						screen->format->BitsPerPixel,
								videoflags);
					if ( screen == NULL ) {
						fprintf(stderr,
					"Couldn't resize video mode\n");
						done = 1;
					}
					DrawBackground(screen);
				break;
			default:
				break;
		}
	}
	SDL_Quit();
	return(0);
}
#endif

#ifdef TESTALPHA


/* Simple program:  Fill a colormap with gray and stripe it down the screen,
		    Then move an alpha valued sprite around the screen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sdl/SDL.h>

#define FRAME_TICKS	(1000/30)		/* 30 frames/second */

/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
static void quit(int rc)
{
	SDL_Quit();
	exit(rc);
}

/* Fill the screen with a gradient */
static void FillBackground(SDL_Surface *screen)
{
	Uint8 *buffer;
	Uint16 *buffer16;
        Uint16 color;
        Uint8  gradient;
	int    i, k;

	/* Set the surface pixels and refresh! */
	if ( SDL_LockSurface(screen) < 0 ) {
		fprintf(stderr, "Couldn't lock the display surface: %s\n",
							SDL_GetError());
		quit(2);
	}
	buffer=(Uint8 *)screen->pixels;
	if (screen->format->BytesPerPixel!=2) {
		for ( i=0; i<screen->h; ++i ) {
			memset(buffer,(i*255)/screen->h, screen->w*screen->format->BytesPerPixel);
			buffer += screen->pitch;
		}
	}
        else
        {
		for ( i=0; i<screen->h; ++i ) {
			gradient=((i*255)/screen->h);
                        color = (Uint16)SDL_MapRGB(screen->format, gradient, gradient, gradient);
                        buffer16=(Uint16*)buffer;
                        for (k=0; k<screen->w; k++)
                        {
                            *(buffer16+k)=color;
                        }
			buffer += screen->pitch;
		}
        }

	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/* Create a "light" -- a yellowish surface with variable alpha */
SDL_Surface *CreateLight(int radius)
{
	Uint8  trans, alphamask;
	int    range, addition;
	int    xdist, ydist;
	Uint16 x, y;
	Uint16 skip;
	Uint32 pixel;
	SDL_Surface *light;

#ifdef LIGHT_16BIT
	Uint16 *buf;

	/* Create a 16 (4/4/4/4) bpp square with a full 4-bit alpha channel */
	/* Note: this isn't any faster than a 32 bit alpha surface */
	alphamask = 0x0000000F;
	light = SDL_CreateRGBSurface(SDL_SWSURFACE, 2*radius, 2*radius, 16,
			0x0000F000, 0x00000F00, 0x000000F0, alphamask);
#else
	Uint32 *buf;

	/* Create a 32 (8/8/8/8) bpp square with a full 8-bit alpha channel */
	alphamask = 0x000000FF;
	light = SDL_CreateRGBSurface(SDL_SWSURFACE, 2*radius, 2*radius, 32,
			0xFF000000, 0x00FF0000, 0x0000FF00, alphamask);
	if ( light == NULL ) {
		fprintf(stderr, "Couldn't create light: %s\n", SDL_GetError());
		return(NULL);
	}
#endif

	/* Fill with a light yellow-orange color */
	skip = light->pitch-(light->w*light->format->BytesPerPixel);
#ifdef LIGHT_16BIT
	buf = (Uint16 *)light->pixels;
#else
	buf = (Uint32 *)light->pixels;
#endif
        /* Get a tranparent pixel value - we'll add alpha later */
	pixel = SDL_MapRGBA(light->format, 0xFF, 0xDD, 0x88, 0);
	for ( y=0; y<light->h; ++y ) {
		for ( x=0; x<light->w; ++x ) {
			*buf++ = pixel;
		}
		buf += skip;	/* Almost always 0, but just in case... */
	}

	/* Calculate alpha values for the surface. */
#ifdef LIGHT_16BIT
	buf = (Uint16 *)light->pixels;
#else
	buf = (Uint32 *)light->pixels;
#endif
	for ( y=0; y<light->h; ++y ) {
		for ( x=0; x<light->w; ++x ) {
			/* Slow distance formula (from center of light) */
			xdist = x-(light->w/2);
			ydist = y-(light->h/2);
			range = (int)sqrt(xdist*xdist+ydist*ydist);

			/* Scale distance to range of transparency (0-255) */
			if ( range > radius ) {
				trans = alphamask;
			} else {
				/* Increasing transparency with distance */
				trans = (Uint8)((range*alphamask)/radius);

				/* Lights are very transparent */
				addition = (alphamask+1)/8;
				if ( (int)trans+addition > alphamask ) {
					trans = alphamask;
				} else {
					trans += addition;
				}
			}
			/* We set the alpha component as the right N bits */
			*buf++ |= (255-trans);
		}
		buf += skip;	/* Almost always 0, but just in case... */
	}
	/* Enable RLE acceleration of this alpha surface */
	SDL_SetAlpha(light, SDL_SRCALPHA|SDL_RLEACCEL, 0);

	/* We're done! */
	return(light);
}

static Uint32 flashes = 0;
static Uint32 flashtime = 0;

void FlashLight(SDL_Surface *screen, SDL_Surface *light, int x, int y)
{
	SDL_Rect position;
	Uint32   ticks1;
	Uint32   ticks2;

	/* Easy, center light */
	position.x = x-(light->w/2);
	position.y = y-(light->h/2);
	position.w = light->w;
	position.h = light->h;
	ticks1 = SDL_GetTicks();
	SDL_BlitSurface(light, NULL, screen, &position);
	ticks2 = SDL_GetTicks();
	SDL_UpdateRects(screen, 1, &position);
	++flashes;

	/* Update time spend doing alpha blitting */
	flashtime += (ticks2-ticks1);
}

static int sprite_visible = 0;
static SDL_Surface *sprite;
static SDL_Surface *backing;
static SDL_Rect    position;
static int         x_vel, y_vel;
static int	   alpha_vel;

int LoadSprite(SDL_Surface *screen, char *file)
{
	SDL_Surface *converted;

	/* Load the sprite image */
	sprite = SDL_LoadBMP(file);
	if ( sprite == NULL ) {
		fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError());
		return(-1);
	}

	/* Set transparent pixel as the pixel at (0,0) */
	if ( sprite->format->palette ) {
		SDL_SetColorKey(sprite, SDL_SRCCOLORKEY,
						*(Uint8 *)sprite->pixels);
	}

	/* Convert sprite to video format */
	converted = SDL_DisplayFormat(sprite);
	SDL_FreeSurface(sprite);
	if ( converted == NULL ) {
		fprintf(stderr, "Couldn't convert background: %s\n",
							SDL_GetError());
		return(-1);
	}
	sprite = converted;

	/* Create the background */
	backing = SDL_CreateRGBSurface(SDL_SWSURFACE, sprite->w, sprite->h, 8,
								0, 0, 0, 0);
	if ( backing == NULL ) {
		fprintf(stderr, "Couldn't create background: %s\n",
							SDL_GetError());
		SDL_FreeSurface(sprite);
		return(-1);
	}

	/* Convert background to video format */
	converted = SDL_DisplayFormat(backing);
	SDL_FreeSurface(backing);
	if ( converted == NULL ) {
		fprintf(stderr, "Couldn't convert background: %s\n",
							SDL_GetError());
		SDL_FreeSurface(sprite);
		return(-1);
	}
	backing = converted;

	/* Set the initial position of the sprite */
	position.x = (screen->w-sprite->w)/2;
	position.y = (screen->h-sprite->h)/2;
	position.w = sprite->w;
	position.h = sprite->h;
	x_vel = 0; y_vel = 0;
	alpha_vel = 1;

	/* We're ready to roll. :) */
	return(0);
}

void AttractSprite(Uint16 x, Uint16 y)
{
	x_vel = ((int)x-position.x)/10;
	y_vel = ((int)y-position.y)/10;
}

void MoveSprite(SDL_Surface *screen, SDL_Surface *light)
{
	SDL_Rect updates[2];
	int alpha;

	/* Erase the sprite if it was visible */
	if ( sprite_visible ) {
		updates[0] = position;
		SDL_BlitSurface(backing, NULL, screen, &updates[0]);
	} else {
		updates[0].x = 0; updates[0].y = 0;
		updates[0].w = 0; updates[0].h = 0;
		sprite_visible = 1;
	}

	/* Since the sprite is off the screen, we can do other drawing
	   without being overwritten by the saved area behind the sprite.
	 */
	if ( light != NULL ) {
		int x, y;

		SDL_GetMouseState(&x, &y);
		FlashLight(screen, light, x, y);
	}
	   
	/* Move the sprite, bounce at the wall */
	position.x += x_vel;
	if ( (position.x < 0) || (position.x >= screen->w) ) {
		x_vel = -x_vel;
		position.x += x_vel;
	}
	position.y += y_vel;
	if ( (position.y < 0) || (position.y >= screen->h) ) {
		y_vel = -y_vel;
		position.y += y_vel;
	}

	/* Update transparency (fade in and out) */
	alpha = sprite->format->alpha;
	if ( (alpha+alpha_vel) < 0 ) {
		alpha_vel = -alpha_vel;
	} else
	if ( (alpha+alpha_vel) > 255 ) {
		alpha_vel = -alpha_vel;
	}
	SDL_SetAlpha(sprite, SDL_SRCALPHA, (Uint8)(alpha+alpha_vel));

	/* Save the area behind the sprite */
	updates[1] = position;
	SDL_BlitSurface(screen, &updates[1], backing, NULL);
	
	/* Blit the sprite onto the screen */
	updates[1] = position;
	SDL_BlitSurface(sprite, NULL, screen, &updates[1]);

	/* Make it so! */
	SDL_UpdateRects(screen, 2, updates);
}

void WarpSprite(SDL_Surface *screen, int x, int y)
{
	SDL_Rect updates[2];

	/* Erase, move, Draw, update */
	updates[0] = position;
	SDL_BlitSurface(backing, NULL, screen, &updates[0]);
	position.x = x-sprite->w/2;	/* Center about X */
	position.y = y-sprite->h/2;	/* Center about Y */
	updates[1] = position;
	SDL_BlitSurface(screen, &updates[1], backing, NULL);
	updates[1] = position;
	SDL_BlitSurface(sprite, NULL, screen, &updates[1]);
	SDL_UpdateRects(screen, 2, updates);
}

int main(int argc, char *argv[])
{
	const SDL_VideoInfo *info;
	SDL_Surface *screen;
	int    w, h;
	Uint8  video_bpp;
	Uint32 videoflags;
	int    i, done;
	SDL_Event event;
	SDL_Surface *light;
	int mouse_pressed;
	Uint32 ticks, lastticks;


	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(1);
	}

	/* Alpha blending doesn't work well at 8-bit color */
#ifdef _WIN32_WCE
	/* Pocket PC */
	w = 240;
	h = 320;
#else
	w = 640;
	h = 480;
#endif
	info = SDL_GetVideoInfo();
	if ( info->vfmt->BitsPerPixel > 8 ) {
		video_bpp = info->vfmt->BitsPerPixel;
	} else {
		video_bpp = 16;
                fprintf(stderr, "forced 16 bpp mode\n");
	}
	videoflags = SDL_SWSURFACE;
	for ( i = 1; argv[i]; ++i ) {
		if ( strcmp(argv[i], "-bpp") == 0 ) {
			video_bpp = atoi(argv[++i]);
                        if (video_bpp<=8) {
                            video_bpp=16;
                            fprintf(stderr, "forced 16 bpp mode\n");
                        }
		} else
		if ( strcmp(argv[i], "-hw") == 0 ) {
			videoflags |= SDL_HWSURFACE;
		} else
		if ( strcmp(argv[i], "-warp") == 0 ) {
			videoflags |= SDL_HWPALETTE;
		} else
		if ( strcmp(argv[i], "-width") == 0 && argv[i+1] ) {
			w = atoi(argv[++i]);
		} else
		if ( strcmp(argv[i], "-height") == 0 && argv[i+1] ) {
			h = atoi(argv[++i]);
		} else
		if ( strcmp(argv[i], "-resize") == 0 ) {
			videoflags |= SDL_RESIZABLE;
		} else
		if ( strcmp(argv[i], "-noframe") == 0 ) {
			videoflags |= SDL_NOFRAME;
		} else
		if ( strcmp(argv[i], "-fullscreen") == 0 ) {
			videoflags |= SDL_FULLSCREEN;
		} else {
			fprintf(stderr, 
			"Usage: %s [-width N] [-height N] [-bpp N] [-warp] [-hw] [-fullscreen]\n",
								argv[0]);
			quit(1);
		}
	}

	/* Set video mode */
	if ( (screen=SDL_SetVideoMode(w,h,video_bpp,videoflags)) == NULL ) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
						w, h, video_bpp, SDL_GetError());
		quit(2);
	}
	FillBackground(screen);

	/* Create the light */
	light = CreateLight(82);
	if ( light == NULL ) {
		quit(1);
	}

	/* Load the sprite */
	if ( LoadSprite(screen, "icon.bmp") < 0 ) {
		SDL_FreeSurface(light);
		quit(1);
	}

	/* Print out information about our surfaces */
	printf("Screen is at %d bits per pixel\n",screen->format->BitsPerPixel);
	if ( (screen->flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
		printf("Screen is in video memory\n");
	} else {
		printf("Screen is in system memory\n");
	}
	if ( (screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF ) {
		printf("Screen has double-buffering enabled\n");
	}
	if ( (sprite->flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
		printf("Sprite is in video memory\n");
	} else {
		printf("Sprite is in system memory\n");
	}

	/* Run a sample blit to trigger blit acceleration */
	MoveSprite(screen, NULL);
	if ( (sprite->flags & SDL_HWACCEL) == SDL_HWACCEL ) {
		printf("Sprite blit uses hardware alpha acceleration\n");
	} else {
		printf("Sprite blit dosn't uses hardware alpha acceleration\n");
	}

	/* Set a clipping rectangle to clip the outside edge of the screen */
	{ SDL_Rect clip;
		clip.x = 32;
		clip.y = 32;
		clip.w = screen->w-(2*32);
		clip.h = screen->h-(2*32);
		SDL_SetClipRect(screen, &clip);
	}

	/* Wait for a keystroke */
	lastticks = SDL_GetTicks();
	done = 0;
	mouse_pressed = 0;
	while ( !done ) {
		/* Update the frame -- move the sprite */
		if ( mouse_pressed ) {
			MoveSprite(screen, light);
			mouse_pressed = 0;
		} else {
			MoveSprite(screen, NULL);
		}

		/* Slow down the loop to 30 frames/second */
		ticks = SDL_GetTicks();
		if ( (ticks-lastticks) < FRAME_TICKS ) {
#ifdef CHECK_SLEEP_GRANULARITY
fprintf(stderr, "Sleeping %d ticks\n", FRAME_TICKS-(ticks-lastticks));
#endif
			SDL_Delay(FRAME_TICKS-(ticks-lastticks));
#ifdef CHECK_SLEEP_GRANULARITY
fprintf(stderr, "Slept %d ticks\n", (SDL_GetTicks()-ticks));
#endif
		}
		lastticks = ticks;

		/* Check for events */
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, video_bpp, videoflags);
					if ( screen ) {
						FillBackground(screen);
					}
					break;
				/* Attract sprite while mouse is held down */
				case SDL_MOUSEMOTION:
					if (event.motion.state != 0) {
						AttractSprite(event.motion.x,
								event.motion.y);
						mouse_pressed = 1;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if ( event.button.button == 1 ) {
						AttractSprite(event.button.x,
						              event.button.y);
						mouse_pressed = 1;
					} else {
						SDL_Rect area;

						area.x = event.button.x-16;
						area.y = event.button.y-16;
						area.w = 32;
						area.h = 32;
						SDL_FillRect(screen, &area, 0);
						SDL_UpdateRects(screen,1,&area);
					}
					break;
				case SDL_KEYDOWN:
#ifndef _WIN32_WCE
					if ( event.key.keysym.sym == SDLK_ESCAPE ) {
						done = 1;
					}
#else
					// there is no ESC key at all
					done = 1;
#endif
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
	}
	SDL_FreeSurface(light);
	SDL_FreeSurface(sprite);
	SDL_FreeSurface(backing);

	/* Print out some timing information */
	if ( flashes > 0 ) {
		printf("%d alpha blits, ~%4.4f ms per blit\n", 
			flashes, (float)flashtime/flashes);
	}

	SDL_Quit();
	return(0);
}

#endif

#ifdef TESTBLITSPEED

/*
 * Benchmarks surface-to-surface blits in various formats.
 *
 *  Written by Ryan C. Gordon.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sdl/SDL.h>

static SDL_Surface *dest = NULL;
static SDL_Surface *src = NULL;
static int testSeconds = 10;


static int percent(int val, int total)
{
    return((int) ((((float) val) / ((float) total)) * 100.0f));
}

static int randRange(int lo, int hi)
{
    return(lo + (int) (((double) hi)*rand()/(RAND_MAX+1.0)));
}

static void copy_trunc_str(char *str, size_t strsize, const char *flagstr)
{
    if ( (strlen(str) + strlen(flagstr)) >= (strsize - 1) )
        strcpy(str + (strsize - 5), " ...");
    else
        strcat(str, flagstr);
}

static void __append_sdl_surface_flag(SDL_Surface *_surface, char *str,
                                      size_t strsize, Uint32 flag,
                                      const char *flagstr)
{
    if (_surface->flags & flag)
        copy_trunc_str(str, strsize, flagstr);
}


#define append_sdl_surface_flag(a, b, c, fl) __append_sdl_surface_flag(a, b, c, fl, " " #fl)
#define print_tf_state(str, val) printf("%s: {%s}\n", str, (val) ? "true" : "false" )

static void output_videoinfo_details(void)
{
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    printf("SDL_GetVideoInfo():\n");
    if (info == NULL)
        printf("  (null.)\n");
    else
    {
        print_tf_state("  hardware surface available", info->hw_available);
        print_tf_state("  window manager available", info->wm_available);
        print_tf_state("  accelerated hardware->hardware blits", info->blit_hw);
        print_tf_state("  accelerated hardware->hardware colorkey blits", info->blit_hw_CC);
        print_tf_state("  accelerated hardware->hardware alpha blits", info->blit_hw_A);
        print_tf_state("  accelerated software->hardware blits", info->blit_sw);
        print_tf_state("  accelerated software->hardware colorkey blits", info->blit_sw_CC);
        print_tf_state("  accelerated software->hardware alpha blits", info->blit_sw_A);
        print_tf_state("  accelerated color fills", info->blit_fill);
        printf("  video memory: (%d)\n", info->video_mem);
    }

    printf("\n");
}

static void output_surface_details(const char *name, SDL_Surface *surface)
{
    printf("Details for %s:\n", name);

    if (surface == NULL)
    {
        printf("-WARNING- You've got a NULL surface!");
    }
    else
    {
        char f[256];
        printf("  width      : %d\n", surface->w);
        printf("  height     : %d\n", surface->h);
        printf("  depth      : %d bits per pixel\n", surface->format->BitsPerPixel);
        printf("  pitch      : %d\n", (int) surface->pitch);
        printf("  alpha      : %d\n", (int) surface->format->alpha);
        printf("  colorkey   : 0x%X\n", (unsigned int) surface->format->colorkey);

        printf("  red bits   : 0x%08X mask, %d shift, %d loss\n",
                    (int) surface->format->Rmask,
                    (int) surface->format->Rshift,
                    (int) surface->format->Rloss);
        printf("  green bits : 0x%08X mask, %d shift, %d loss\n",
                    (int) surface->format->Gmask,
                    (int) surface->format->Gshift,
                    (int) surface->format->Gloss);
        printf("  blue bits  : 0x%08X mask, %d shift, %d loss\n",
                    (int) surface->format->Bmask,
                    (int) surface->format->Bshift,
                    (int) surface->format->Bloss);
        printf("  alpha bits : 0x%08X mask, %d shift, %d loss\n",
                    (int) surface->format->Amask,
                    (int) surface->format->Ashift,
                    (int) surface->format->Aloss);

        f[0] = '\0';

        /*append_sdl_surface_flag(surface, f, sizeof (f), SDL_SWSURFACE);*/
        if ((surface->flags & SDL_HWSURFACE) == 0)
            copy_trunc_str(f, sizeof (f), " SDL_SWSURFACE");

        append_sdl_surface_flag(surface, f, sizeof (f), SDL_HWSURFACE);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_ASYNCBLIT);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_ANYFORMAT);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_HWPALETTE);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_DOUBLEBUF);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_FULLSCREEN);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_OPENGL);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_OPENGLBLIT);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_RESIZABLE);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_NOFRAME);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_HWACCEL);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_SRCCOLORKEY);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_RLEACCELOK);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_RLEACCEL);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_SRCALPHA);
        append_sdl_surface_flag(surface, f, sizeof (f), SDL_PREALLOC);

        if (f[0] == '\0')
            strcpy(f, " (none)");

        printf("  flags      :%s\n", f);
    }

    printf("\n");
}

static void output_details(void)
{
    output_videoinfo_details();
    output_surface_details("Source Surface", src);
    output_surface_details("Destination Surface", dest);
}

static Uint32 blit(SDL_Surface *dst, SDL_Surface *src, int x, int y)
{
    Uint32 start = 0;
    SDL_Rect srcRect;
    SDL_Rect dstRect;

    srcRect.x = 0;
    srcRect.y = 0;
    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = srcRect.w = src->w;  /* SDL will clip as appropriate. */
    dstRect.h = srcRect.h = src->h;

    start = SDL_GetTicks();
    SDL_BlitSurface(src, &srcRect, dst, &dstRect);
    return(SDL_GetTicks() - start);
}

static void blitCentered(SDL_Surface *dst, SDL_Surface *src)
{
    int x = (dst->w - src->w) / 2;
    int y = (dst->h - src->h) / 2;
    blit(dst, src, x, y);
}

static int atoi_hex(const char *str)
{
    if (str == NULL)
        return 0;

    if (strlen(str) > 2)
    {
        int retval = 0;
        if ((str[0] == '0') && (str[1] == 'x'))
            sscanf(str + 2, "%X", &retval);
        return(retval);
    }

    return(atoi(str));
}


static int setup_test(int argc, char **argv)
{
    const char *dumpfile = NULL;
    SDL_Surface *bmp = NULL;
    Uint32 dstbpp = 32;
    Uint32 dstrmask = 0x00FF0000;
    Uint32 dstgmask = 0x0000FF00;
    Uint32 dstbmask = 0x000000FF;
    Uint32 dstamask = 0x00000000;
    Uint32 dstflags = 0;
    int dstw = 640;
    int dsth = 480;
    Uint32 srcbpp = 32;
    Uint32 srcrmask = 0x00FF0000;
    Uint32 srcgmask = 0x0000FF00;
    Uint32 srcbmask = 0x000000FF;
    Uint32 srcamask = 0x00000000;
    Uint32 srcflags = 0;
    int srcw = 640;
    int srch = 480;
    Uint32 origsrcalphaflags = 0;
    Uint32 origdstalphaflags = 0;
    Uint32 srcalphaflags = 0;
    Uint32 dstalphaflags = 0;
    int srcalpha = 255;
    int dstalpha = 255;
    int screenSurface = 0;
    int i = 0;

    for (i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        if (strcmp(arg, "--dstbpp") == 0)
            dstbpp = atoi(argv[++i]);
        else if (strcmp(arg, "--dstrmask") == 0)
            dstrmask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--dstgmask") == 0)
            dstgmask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--dstbmask") == 0)
            dstbmask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--dstamask") == 0)
            dstamask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--dstwidth") == 0)
            dstw = atoi(argv[++i]);
        else if (strcmp(arg, "--dstheight") == 0)
            dsth = atoi(argv[++i]);
        else if (strcmp(arg, "--dsthwsurface") == 0)
            dstflags |= SDL_HWSURFACE;
        else if (strcmp(arg, "--srcbpp") == 0)
            srcbpp = atoi(argv[++i]);
        else if (strcmp(arg, "--srcrmask") == 0)
            srcrmask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--srcgmask") == 0)
            srcgmask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--srcbmask") == 0)
            srcbmask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--srcamask") == 0)
            srcamask = atoi_hex(argv[++i]);
        else if (strcmp(arg, "--srcwidth") == 0)
            srcw = atoi(argv[++i]);
        else if (strcmp(arg, "--srcheight") == 0)
            srch = atoi(argv[++i]);
        else if (strcmp(arg, "--srchwsurface") == 0)
            srcflags |= SDL_HWSURFACE;
        else if (strcmp(arg, "--seconds") == 0)
            testSeconds = atoi(argv[++i]);
        else if (strcmp(arg, "--screen") == 0)
            screenSurface = 1;
        else if (strcmp(arg, "--dumpfile") == 0)
            dumpfile = argv[++i];
        /* !!! FIXME: set colorkey. */
        else if (0)  /* !!! FIXME: we handle some commandlines elsewhere now */
        {
            fprintf(stderr, "Unknown commandline option: %s\n", arg);
            return(0);
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return(0);
    }

    bmp = SDL_LoadBMP("sample.bmp");
    if (bmp == NULL)
    {
        fprintf(stderr, "SDL_LoadBMP failed: %s\n", SDL_GetError());
        SDL_Quit();
        return(0);
    }

    if ((dstflags & SDL_HWSURFACE) == 0) dstflags |= SDL_SWSURFACE;
    if ((srcflags & SDL_HWSURFACE) == 0) srcflags |= SDL_SWSURFACE;

    if (screenSurface)
        dest = SDL_SetVideoMode(dstw, dsth, dstbpp, dstflags);
    else
    {
        dest = SDL_CreateRGBSurface(dstflags, dstw, dsth, dstbpp,
                                    dstrmask, dstgmask, dstbmask, dstamask);
    }

    if (dest == NULL)
    {
        fprintf(stderr, "dest surface creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return(0);
    }

    src = SDL_CreateRGBSurface(srcflags, srcw, srch, srcbpp,
                               srcrmask, srcgmask, srcbmask, srcamask);
    if (src == NULL)
    {
        fprintf(stderr, "src surface creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return(0);
    }

    /* handle alpha settings... */
    srcalphaflags = (src->flags&SDL_SRCALPHA) | (src->flags&SDL_RLEACCEL);
    dstalphaflags = (dest->flags&SDL_SRCALPHA) | (dest->flags&SDL_RLEACCEL);
    origsrcalphaflags = srcalphaflags;
    origdstalphaflags = dstalphaflags;
    srcalpha = src->format->alpha;
    dstalpha = dest->format->alpha;
    for (i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        if (strcmp(arg, "--srcalpha") == 0)
            srcalpha = atoi(argv[++i]);
        else if (strcmp(arg, "--dstalpha") == 0)
            dstalpha = atoi(argv[++i]);
        else if (strcmp(arg, "--srcsrcalpha") == 0)
            srcalphaflags |= SDL_SRCALPHA;
        else if (strcmp(arg, "--srcnosrcalpha") == 0)
            srcalphaflags &= ~SDL_SRCALPHA;
        else if (strcmp(arg, "--srcrleaccel") == 0)
            srcalphaflags |= SDL_RLEACCEL;
        else if (strcmp(arg, "--srcnorleaccel") == 0)
            srcalphaflags &= ~SDL_RLEACCEL;
        else if (strcmp(arg, "--dstsrcalpha") == 0)
            dstalphaflags |= SDL_SRCALPHA;
        else if (strcmp(arg, "--dstnosrcalpha") == 0)
            dstalphaflags &= ~SDL_SRCALPHA;
        else if (strcmp(arg, "--dstrleaccel") == 0)
            dstalphaflags |= SDL_RLEACCEL;
        else if (strcmp(arg, "--dstnorleaccel") == 0)
            dstalphaflags &= ~SDL_RLEACCEL;
    }
    if ((dstalphaflags != origdstalphaflags) || (dstalpha != dest->format->alpha))
        SDL_SetAlpha(dest, dstalphaflags, (Uint8) dstalpha);
    if ((srcalphaflags != origsrcalphaflags) || (srcalpha != src->format->alpha))
        SDL_SetAlpha(src, srcalphaflags, (Uint8) srcalpha);

    /* set some sane defaults so we can see if the blit code is broken... */
    SDL_FillRect(dest, NULL, SDL_MapRGB(dest->format, 0, 0, 0));
    SDL_FillRect(src, NULL, SDL_MapRGB(src->format, 0, 0, 0));

    blitCentered(src, bmp);
    SDL_FreeSurface(bmp);

    if (dumpfile)
        SDL_SaveBMP(src, dumpfile);  /* make sure initial convert is sane. */

    output_details();

    return(1);
}


static void test_blit_speed(void)
{
    Uint32 clearColor = SDL_MapRGB(dest->format, 0, 0, 0);
    Uint32 iterations = 0;
    Uint32 elasped = 0;
    Uint32 end = 0;
    Uint32 now = 0;
    Uint32 last = 0;
    int testms = testSeconds * 1000;
    int wmax = (dest->w - src->w);
    int hmax = (dest->h - src->h);
    int isScreen = (SDL_GetVideoSurface() == dest);
    SDL_Event event;

    printf("Testing blit speed for %d seconds...\n", testSeconds);

    now = SDL_GetTicks();
    end = now + testms;

    do
    {
        /* pump the event queue occasionally to keep OS happy... */
        if (now - last > 1000)
        {
            last = now;
            while (SDL_PollEvent(&event)) { /* no-op. */ }
        }

        iterations++;
        elasped += blit(dest, src, randRange(0, wmax), randRange(0, hmax));
        if (isScreen)
        {
            SDL_Flip(dest);  /* show it! */
            SDL_FillRect(dest, NULL, clearColor); /* blank it for next time! */
        }

        now = SDL_GetTicks();
    } while (now < end);

    printf("Non-blitting crap accounted for %d percent of this run.\n",
            percent(testms - elasped, testms));

    printf("%d blits took %d ms (%d fps).\n",
            (int) iterations,
            (int) elasped,
            (int) (((float)iterations) / (((float)elasped) / 1000.0f)));
}

int main(int argc, char **argv)
{
    int initialized = setup_test(argc, argv);
    if (initialized)
    {
        test_blit_speed();
        SDL_Quit();
    }
    return(!initialized);
}

/* end of testblitspeed.c ... */

#endif
