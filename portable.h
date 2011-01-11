#ifndef _PORTABLE_H_
#define _PORTABLE_H_

#pragma warning(disable: 4996)

/* choose platform */
#undef CONFIG_DUMMY_PORT
#define CONFIG_SDL_PORT

/* enable bitmap dumper */
#define CONFIG_DUMP_BMP


extern unsigned char *screen_p;

void port_init_video(void);
void port_close_video(void);
void port_blit_to_screen(void);
void port_set_palette(int first, int length, unsigned char *src_p);
void port_delay(int msec);
void port_exit(int retc);

#endif
