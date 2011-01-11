#include "portable.h"

#ifdef CONFIG_DUMMY_PORT

#include <string.h>

static unsigned char screen_buf[320 * 200];

unsigned char *screen_p = screen_buf;

void port_exit(int retc) {
	/* TODO: use instead exit() to close graphic and exit before port_close_video() call */
	exit(retc);
}

void port_init_video(void) {
	/* TODO: setup 320x200x256 graphic mode, paletized */
}

void port_close_video(void) {
	/* TODO: close graphic */
}

void port_blit_to_screen(void) {
	/* TODO: copy screen_buf content to display */
}

void port_set_palette(int first, int length, unsigned char *src_p) {
	/* TODO: set block of palette registers; change must take effect immediately; order is (r,g,b); r, g, b range is <0..63> */
}

void port_delay(int msec) {
	/* TODO: sleep for given milisecond interval */
}

#endif
