#include "portable.h"

#ifdef CONFIG_SDL_PORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <sdl/SDL.h>
#include <sdl/SDL_main.h>

static unsigned char screen_buf[320 * 200];
unsigned char *screen_p = screen_buf;

static SDL_Color palette[256];
static SDL_Surface *surface_p;

static void port_dump_screen(void) {
#ifdef CONFIG_DUMP_BMP
	static int counter = 0;
	static Uint32 ticks;
	char fname[32];

	if (counter == 0)
		 ticks = SDL_GetTicks();
	sprintf(fname, "dump/screen-%03d-ticks-%d.bmp", counter++, (int) SDL_GetTicks() - ticks);
	SDL_SaveBMP(surface_p, fname);
#endif
}

void port_exit(int retc) {
	SDL_Quit();
	exit(retc);
}

void port_init_video(void) {
	int width = 320;
	int height = 200;
	int bpp = 8;
	Uint32 video_flags;

	memset(screen_buf, 0, 320 * 200);
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		port_exit(1);
	}
	video_flags = SDL_SWSURFACE;
	if ((surface_p = SDL_SetVideoMode(width, height, bpp, video_flags)) == NULL) {
		fprintf(stderr, "Couldn't set display mode: %s\n", SDL_GetError());
		port_exit(2);
	}
}

void port_close_video(void) {
	int done;
	SDL_Event event;

	for (done = 0; done == 0; ) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
				case SDL_KEYDOWN:
				case SDL_QUIT:
					done = 1;
					port_exit(0);
				default:
					break;
			}
		}
	}
	SDL_Quit();
}

void port_blit_to_screen(void) {
	int debug = 0;
	int done;
	SDL_Event event;

	unsigned char *buffer_p;

	if (SDL_LockSurface(surface_p) < 0) {
		fprintf(stderr, "Couldn't lock the display surface: %s\n", SDL_GetError());
		port_exit(3);
	}
	buffer_p = surface_p->pixels;
	memcpy(buffer_p, screen_p, 320 * 200);
	SDL_UnlockSurface(surface_p);
	SDL_UpdateRect(surface_p, 0, 0, 0, 0);
	port_dump_screen();

	if (debug) {
		for (done = 0; done == 0; ) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_MOUSEBUTTONDOWN:
					case SDL_KEYDOWN:
					case SDL_QUIT:
						done = 1;
						break;
					default:
						break;
				}
			}
		}
	} else {
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
				case SDL_KEYDOWN:
				case SDL_QUIT:
					port_exit(0);
					break;
			}
		}
	}
}

void port_set_palette(int first, int length, unsigned char *src_p) {
	int i;

	for (i = first; i < first + length; i++) {
		palette[i].r = (Uint8) ((int) (*src_p++) * 255 / 63);
		palette[i].g = (Uint8) ((int) (*src_p++) * 255 / 63);
		palette[i].b = (Uint8) ((int) (*src_p++) * 255 / 63);
	}
	SDL_SetColors(surface_p, palette, 0, 256);
	port_blit_to_screen();
}

void port_delay(int msec) {
	SDL_Delay((Uint32) msec);
}

/* END OF SDL PORT */












/*** SDL_main.c FROM SDL PACKAGE ***/
/* many compilers (including gcc) don't like when program entry is in library */

/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    The WinMain function -- calls your program's main() function
*/

#ifdef _WIN32_WCE
# define DIR_SEPERATOR TEXT("\\")
# undef _getcwd
# define _getcwd(str,len)	wcscpy(str,TEXT(""))
# define setbuf(f,b)
# define setvbuf(w,x,y,z)
# define fopen		_wfopen
# define freopen	_wfreopen
# define remove(x)	DeleteFile(x)
#else
# define DIR_SEPERATOR TEXT("/")
# include <direct.h>
#endif

/* Include the SDL main definition header */

#ifdef main
# ifndef _WIN32_WCE_EMULATION
#  undef main
# endif /* _WIN32_WCE_EMULATION */
#endif /* main */

/* The standard output files */
#define STDOUT_FILE	TEXT("stdout.txt")
#define STDERR_FILE	TEXT("stderr.txt")

/* Set a variable to tell if the stdio redirect has been enabled. */
static int stdioRedirectEnabled = 0;

#ifdef _WIN32_WCE
  static wchar_t stdoutPath[MAX_PATH];
  static wchar_t stderrPath[MAX_PATH];
#else
  static char stdoutPath[MAX_PATH];
  static char stderrPath[MAX_PATH];
#endif

#if defined(_WIN32_WCE) && _WIN32_WCE < 300
/* seems to be undefined in Win CE although in online help */
#define isspace(a) (((CHAR)a == ' ') || ((CHAR)a == '\t'))
#endif /* _WIN32_WCE < 300 */

static void UnEscapeQuotes( char *arg )
{
	char *last = NULL;

	while( *arg ) {
		if( *arg == '"' && *last == '\\' ) {
			char *c_curr = arg;
			char *c_last = last;

			while( *c_curr ) {
				*c_last = *c_curr;
				c_last = c_curr;
				c_curr++;
			}
			*c_last = '\0';
		}
		last = arg;
		arg++;
	}
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	char *lastp = NULL;
	int argc, last_argc;

	argc = last_argc = 0;
	for ( bufp = cmdline; *bufp; ) {
		/* Skip leading whitespace */
		while ( isspace(*bufp) ) {
			++bufp;
		}
		/* Skip over argument */
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ( *bufp != '"' || *lastp == '\\' ) ) {
				lastp = bufp;
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}

		/* Strip out \ from \" sequences */
		if( argv && last_argc != argc ) {
			UnEscapeQuotes( argv[last_argc] );	
		}
		last_argc = argc;	
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return(argc);
}

/* Show an error message */
static void ShowError(const char *title, const char *message)
{
/* If USE_MESSAGEBOX is defined, you need to link with user32.lib */
#ifdef USE_MESSAGEBOX
	MessageBox(NULL, message, title, MB_ICONEXCLAMATION|MB_OK);
#else
	fprintf(stderr, "%s: %s\n", title, message);
#endif
}

/* Pop up an out of memory message, returns to Windows */
static BOOL OutOfMemory(void)
{
	ShowError("Fatal Error", "Out of memory - aborting");
	return FALSE;
}

/* SDL_Quit() shouldn't be used with atexit() directly because
   calling conventions may differ... */
static void cleanup(void)
{
	SDL_Quit();
}

/* Remove the output files if there was no output written */
static void cleanup_output(void) {
	FILE *file;
	int empty;

	/* Flush the output in case anything is queued */
	fclose(stdout);
	fclose(stderr);

	/* Without redirection we're done */
	if (!stdioRedirectEnabled) {
		return;
	}

	/* See if the files have any output in them */
	if ( stdoutPath[0] ) {
		file = fopen(stdoutPath, TEXT("rb"));
		if ( file ) {
			empty = (fgetc(file) == EOF) ? 1 : 0;
			fclose(file);
			if ( empty ) {
				remove(stdoutPath);
			}
		}
	}
	if ( stderrPath[0] ) {
		file = fopen(stderrPath, TEXT("rb"));
		if ( file ) {
			empty = (fgetc(file) == EOF) ? 1 : 0;
			fclose(file);
			if ( empty ) {
				remove(stderrPath);
			}
		}
	}
}

/* Redirect the output (stdout and stderr) to a file */
static void redirect_output(void)
{
	DWORD pathlen;
#ifdef _WIN32_WCE
	wchar_t path[MAX_PATH];
#else
	char path[MAX_PATH];
#endif
	FILE *newfp;

	pathlen = GetModuleFileName(NULL, path, SDL_arraysize(path));
	while ( pathlen > 0 && path[pathlen] != '\\' ) {
		--pathlen;
	}
	path[pathlen] = '\0';

#ifdef _WIN32_WCE
	wcsncpy( stdoutPath, path, SDL_arraysize(stdoutPath) );
	wcsncat( stdoutPath, DIR_SEPERATOR STDOUT_FILE, SDL_arraysize(stdoutPath) );
#else
	SDL_strlcpy( stdoutPath, path, SDL_arraysize(stdoutPath) );
	SDL_strlcat( stdoutPath, DIR_SEPERATOR STDOUT_FILE, SDL_arraysize(stdoutPath) );
#endif
    
	/* Redirect standard input and standard output */
	newfp = freopen(stdoutPath, TEXT("w"), stdout);

#ifndef _WIN32_WCE
	if ( newfp == NULL ) {	/* This happens on NT */
#if !defined(stdout)
		stdout = fopen(stdoutPath, TEXT("w"));
#else
		newfp = fopen(stdoutPath, TEXT("w"));
		if ( newfp ) {
			*stdout = *newfp;
		}
#endif
	}
#endif /* _WIN32_WCE */

#ifdef _WIN32_WCE
	wcsncpy( stderrPath, path, SDL_arraysize(stdoutPath) );
	wcsncat( stderrPath, DIR_SEPERATOR STDOUT_FILE, SDL_arraysize(stdoutPath) );
#else
	SDL_strlcpy( stderrPath, path, SDL_arraysize(stderrPath) );
	SDL_strlcat( stderrPath, DIR_SEPERATOR STDERR_FILE, SDL_arraysize(stderrPath) );
#endif

	newfp = freopen(stderrPath, TEXT("w"), stderr);
#ifndef _WIN32_WCE
	if ( newfp == NULL ) {	/* This happens on NT */
#if !defined(stderr)
		stderr = fopen(stderrPath, TEXT("w"));
#else
		newfp = fopen(stderrPath, TEXT("w"));
		if ( newfp ) {
			*stderr = *newfp;
		}
#endif
	}
#endif /* _WIN32_WCE */

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);	/* Line buffered */
	setbuf(stderr, NULL);			/* No buffering */
	stdioRedirectEnabled = 1;
}

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
/* The VC++ compiler needs main defined */
#define console_main main
#endif

/* This is where execution begins [console apps] */
int console_main(int argc, char *argv[])
{
	size_t n;
	char *bufp, *appname;
	int status;

	/* Get the class name from argv[0] */
	appname = argv[0];
	if ( (bufp=SDL_strrchr(argv[0], '\\')) != NULL ) {
		appname = bufp+1;
	} else
	if ( (bufp=SDL_strrchr(argv[0], '/')) != NULL ) {
		appname = bufp+1;
	}

	if ( (bufp=SDL_strrchr(appname, '.')) == NULL )
		n = SDL_strlen(appname);
	else
		n = (bufp-appname);

	bufp = SDL_stack_alloc(char, n+1);
	if ( bufp == NULL ) {
		return OutOfMemory();
	}
	SDL_strlcpy(bufp, appname, n+1);
	appname = bufp;

	/* Load SDL dynamic link library */
	if ( SDL_Init(SDL_INIT_NOPARACHUTE) < 0 ) {
		ShowError("WinMain() error", SDL_GetError());
		return(FALSE);
	}
	atexit(cleanup_output);
	atexit(cleanup);

	/* Sam:
	   We still need to pass in the application handle so that
	   DirectInput will initialize properly when SDL_RegisterApp()
	   is called later in the video initialization.
	 */
	SDL_SetModuleHandle(GetModuleHandle(NULL));

	/* Run the application main() code */
	status = SDL_main(argc, argv);

	/* Exit cleanly, calling atexit() functions */
	exit(status);

	/* Hush little compiler, don't you cry... */
	return 0;
}

/* This is where execution begins [windowed apps] */
#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR szCmdLine, int sw)
#else
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
#endif
{
	HINSTANCE handle;
	char **argv;
	int argc;
	char *cmdline;
	char *env_str;
#ifdef _WIN32_WCE
	wchar_t *bufp;
	int nLen;
#else
	char *bufp;
	size_t nLen;
#endif

	/* Start up DDHELP.EXE before opening any files, so DDHELP doesn't
	   keep them open.  This is a hack.. hopefully it will be fixed 
	   someday.  DDHELP.EXE starts up the first time DDRAW.DLL is loaded.
	 */
	handle = LoadLibrary(TEXT("DDRAW.DLL"));
	if ( handle != NULL ) {
		FreeLibrary(handle);
	}

	/* Check for stdio redirect settings and do the redirection */
	if ((env_str = SDL_getenv("SDL_STDIO_REDIRECT"))) {
		if (SDL_atoi(env_str)) {
			redirect_output();
		}
	}
#ifndef NO_STDIO_REDIRECT
	else {
		redirect_output();
	}
#endif

#ifdef _WIN32_WCE
	nLen = wcslen(szCmdLine)+128+1;
	bufp = SDL_stack_alloc(wchar_t, nLen*2);
	wcscpy (bufp, TEXT("\""));
	GetModuleFileName(NULL, bufp+1, 128-3);
	wcscpy (bufp+wcslen(bufp), TEXT("\" "));
	wcsncpy(bufp+wcslen(bufp), szCmdLine,nLen-wcslen(bufp));
	nLen = wcslen(bufp)+1;
	cmdline = SDL_stack_alloc(char, nLen);
	if ( cmdline == NULL ) {
		return OutOfMemory();
	}
	WideCharToMultiByte(CP_ACP, 0, bufp, -1, cmdline, nLen, NULL, NULL);
#else
	/* Grab the command line */
	bufp = GetCommandLine();
	nLen = SDL_strlen(bufp)+1;
	cmdline = SDL_stack_alloc(char, nLen);
	if ( cmdline == NULL ) {
		return OutOfMemory();
	}
	SDL_strlcpy(cmdline, bufp, nLen);
#endif

	/* Parse it into argv and argc */
	argc = ParseCommandLine(cmdline, NULL);
	argv = SDL_stack_alloc(char*, argc+1);
	if ( argv == NULL ) {
		return OutOfMemory();
	}
	ParseCommandLine(cmdline, argv);

	/* Run the main program (after a little SDL initialization) */
	console_main(argc, argv);

	/* Hush little compiler, don't you cry... */
	return 0;
}

#endif
