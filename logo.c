#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portable.h"


char da005a[52] = "logo";
char da008e[96] = { 0, 0, };
char da00ee[96] = { 0, 0, };
char *da0008[4] =  { da005a, da008e, da00ee, NULL };
unsigned char _extra[65536];
unsigned char _extra2[65536];
unsigned char *da0048 = _extra;
unsigned char *da004c = _extra2;
unsigned char *da0050;
unsigned short da0052;
unsigned short da0054;
unsigned char *block_p;		/* da0056 */
char lb0070[3*3*256];
unsigned short lb0cb0 = 0;
unsigned short lb0cb2 = 0;
unsigned short lb0cb4 =	0;
unsigned short lb0cb6 = 0;
unsigned short lb0cb8 = 1;
unsigned short *lb0cba = NULL;
unsigned short lb0cbc[] = {
	0x0028, 0x0100, 0x0034, 0x0089,
	0x001f, 0xff00, 0x0080, 0x0091,
	0x002d, 0x0100, 0x00c5, 0x0022,
	0x0039, 0xffc0, 0xff00, 0x0000,
	0x002c, 0xff24, 0x0000, 0xff00,
	0x0050, 0x0000, 0x0000, 0x0000,
	0xffff, 0x0000, 0x0000, 0x0000
};


/*** hsq */
__inline int logo_getbit(int *q, unsigned char **src) {
	int bit;

	if (*q == 1) {
		*q = 0x10000 | (**((unsigned short **) src));
		*src += 2;
	}
	bit = *q & 1;
	*q >>= 1;
	return bit;
}
void *logo_unpack(unsigned char *src_p) {
	int q = 1;
	int span, length;
	unsigned char *dst_p = _extra;

#ifdef _DEBUG
	fprintf(stderr, "hsq: (size = %d, size = %d, ", *((unsigned short *) src_p), *((unsigned short *) (src_p + 2)));
#endif
	src_p += 6;
	while (1) {
		if (logo_getbit(&q, &src_p)) {
			*dst_p++ = *src_p++;
		} else {
			if (logo_getbit(&q, &src_p)) {
				length = *src_p & 7;
				span = 0xffffe000 | ((*(unsigned short *) src_p) >> 3);
				src_p += 2;
				if (!length) length = *src_p++;
				if (!length) {
#ifdef _DEBUG
					fprintf(stderr, "size = %d)\n", dst_p - _extra);
#endif
					return _extra;
				}
			} else {
				length = logo_getbit(&q, &src_p) << 1;
				length |= logo_getbit(&q, &src_p);
				span = 0xffffff00 | *src_p++;
			}
			for (length += 2; length > 0; dst_p++, length--)
				*dst_p = *(dst_p + span);
		}
	}
}
/*** hsq */

/*** bliters */
void logo_blit_clear(void) {
	memset(screen_p, 0, 320 * 200);
	port_blit_to_screen();
}
void logo_blit_adjust(void) {
	int i;
	unsigned char c;
	unsigned char *src_p = screen_p;
	unsigned char *dst_p = screen_p;

	for (i = 320 * 200; i; i--) {
		if ((c = *src_p++) < 160)
			c = 0;
		*dst_p++ = c;
	}
}
void logo_blit_mirror(void) {
	unsigned int *src_p;
	unsigned int *dst_p;
	int cx, cy;

	src_p = (unsigned int *) screen_p;
	dst_p = (unsigned int *) &screen_p[320];
	for (cy = 200 / 2; cy > 0; cy--) {
		for (cx = 320 / 2 / 4; cx > 0; cx--) {
			unsigned int quad_pixel = *src_p++;

			quad_pixel =
				((quad_pixel >> 24) & 0x000000ff) | ((quad_pixel >> 8) & 0x0000ff00) |
				((quad_pixel << 8) & 0x00ff0000) | ((quad_pixel << 24) & 0xff000000);
			*(--dst_p) = quad_pixel;
		}
		src_p += 320 / 2 / 4;
		dst_p += 3 * 320 / 2 / 4;
	}
	src_p = (unsigned int *) screen_p;
	dst_p = (unsigned int *) &screen_p[320 * 199];
	for (cy = 200 / 2; cy > 0; cy--) {
		for (cx = 320 / 4; cx > 0; cx--)
			*dst_p++ = *src_p++;
		dst_p -= 320 / 2;
	}
	port_blit_to_screen();
}
__inline void logo_blit_rle8_key_left(unsigned char *dst_p, unsigned char *src_p, int width, int height, int advance) {
	unsigned char c;
	int i, count, bp;
	
	for (; height; height--) {
		bp = width;
		while (bp > 0) {
			if ((c = *src_p++) & 0x80) {
				count = 257 - c;
				if ((c = *src_p++) != 0)
					memset(dst_p, c, count);
				dst_p += count;
			} else {
				count = 1 + c;
				for (i = count; i; i--) {
					if ((c = *src_p++) != 0)
						*dst_p = c;
					dst_p++;
				}
			}
			bp -= count;
		}
		dst_p -= width;
		dst_p += advance;
	}
}
__inline void logo_blit_rle8_key_right(unsigned char *dst_p, unsigned char *src_p, int width, int height, int advance) {
	unsigned char c;
	int i, count, bp;
	
	for (; height; height--) {
		bp = width;
		while (bp > 0) {
			if ((c = *src_p++) == 0) {
				count = 257 - c;
				if ((c = *src_p++) != 0) {
					for (i = count; i; i--)
						*dst_p-- = c;
				} else {
					dst_p -= count;
				}
			} else {
				count = 1 + c;
				for (i = count; i; i--) {
					if ((c = *src_p++) != 0)
						*dst_p-- = c;
					else
						dst_p--;
				}
			}
			bp -= count;
		}
		dst_p += width;
		dst_p += advance;
	}
}
__inline void logo_blit_rle8_nokey_left(unsigned char *dst_p, unsigned char *src_p, int width, int height, int advance) {
	unsigned char c;
	int count, bp;
	
	for (; height; height--) {
		bp = width;
		while (bp > 0) {
			if ((c = *src_p++) == 0) {
				count = 257 - c;
				c = *src_p++;
				memset(dst_p, c, count);
			} else {
				count = 1 + c;
				memcpy(dst_p, src_p, count);
				src_p += count;
			}
			dst_p += count;
			bp -= count;
		}
		dst_p -= width;
		dst_p += advance;
	}
}
__inline void logo_blit_rle8_nokey_right(unsigned char *dst_p, unsigned char *src_p, int width, int height, int advance) {
	unsigned char c;
	int i, count, bp;
	
	for (; height; height--) {
		bp = width;
		while (bp > 0) {
			if ((c = *src_p++) == 0) {
				count = 257 - c;
				c = *src_p++;
				for (i = count; i; i--)
					*dst_p-- = c;
			} else {
				count = 1 + c;
				for (i = count; i; i--)
					*dst_p-- = *src_p++;
			}
			bp -= count;
		}
		dst_p += width;
		dst_p += advance;
	}
}
void logo_blit_sprite(unsigned char *src_p, int pos_x/*dx*/, int pos_y/*bx*/, int width/*di*/, int height/*cx*/) {
	int cx;
	int cy;
	unsigned char *dst_p;

#ifdef _DEBUG
	fprintf(stderr, "sprite: (x = %d[%02X], y = %d[%02X], width = %d[%02X], height = %d[%02X], wflags = %02X, hflags = %02X)\n",
		pos_x, pos_x, pos_y, pos_y, width & 0x1ff, width & 0x1ff, height & 0xff, height & 0xff, (width >> 8) & 0xfe, height >> 8);
#endif
	if ((height >> 8) < 0xfe)
		return;
	if ((width & 0x8000) == 0) {
		if (pos_y >= 200)
			pos_y = 199;
		if ((height >> 8) == 0xff) {
			dst_p = screen_p + 320 * pos_y + pos_x;
			cx = width & 0x1ff;
			cy = height & 0xff;
			
			for (; cy > 0; cy--) {
				memcpy(dst_p, src_p, cx);
				dst_p += 320;
				src_p += cx;
			}
		} else {
			unsigned char *dst2_p = screen_p + 320 * pos_y + pos_x;

			for (cy = height & 0xff; cy > 0; cy--) {
				dst_p = dst2_p;
				for (cx = width & 0x1ff; cx > 0; cx--) {
					unsigned char pix;
					
					if ((pix = *src_p++) != 0)
						*dst_p = pix;
					dst_p++;
				}
				dst2_p += 320;
			}
		}
	} else {
		if (pos_y >= 200)
			pos_y = 199;
		if (width & 0x4000) {
			int step;
			
			dst_p = screen_p + 320 * pos_y + pos_x;
			cx = width & 0x1ff;
			cy = height & 0xff;
			if (width & 0x2000) {
				step = -320;
				dst_p += 320 * (cy - 1);
			} else {
				step = +320;
			}
			dst_p += cx - 1;
			if ((height >> 8) == 0xff)
				logo_blit_rle8_key_right(dst_p, src_p, cx, cy, step);
			else
				logo_blit_rle8_nokey_right(dst_p, src_p, cx, cy, step);
		} else {
			int step;
			
			dst_p = screen_p + 320 * pos_y + pos_x;
			cx = width & 0x1ff;
			cy = height & 0xff;
			if (width & 0x2000) {
				dst_p += 320 * (cy - 1);
				step = -320;
			} else {
				step = +320;
			}
			if ((height >> 8) == 0xff)
				logo_blit_rle8_key_left(dst_p, src_p, cx, cy, step);
			else
				logo_blit_rle8_nokey_left(dst_p, src_p, cx, cy, step);
		}
	}
	port_blit_to_screen();
}
void logo_blit(void) {
	int pos_x, pos_y, width, height;
	unsigned short *src_p = (unsigned short *) block_p;

#ifdef _DEBUG
	fprintf(stderr, "sprite: (block_size = %d)\n", *((unsigned short *) src_p));
#endif
	src_p++;
	width = *src_p++;
	height = *src_p++;
	if ((height & 0xff)	== 0)
		return;
	if (width & 0x200) {
		width &= 0xfdff;
		src_p = logo_unpack((unsigned char *) src_p);
	}
	pos_x = *src_p++;
	pos_y = *src_p++;
	logo_blit_sprite((unsigned char *) src_p, pos_x, pos_y, width, height);
}
/*** blit */

/*** palette */
void logo_load_palette(void) {
	char *src_p = block_p;
	unsigned short flags;
	int first, length;

	src_p += 2;
	while (1) {
		flags = *((unsigned short *) src_p);
		src_p += 2;
		if ((flags & 0xff) == 0xff)
			break;
		first = flags & 0xff;
		length = flags >> 8;
		port_set_palette(first, length, src_p);
		memcpy(&lb0070[3 * first], src_p, 3 * length);
		src_p += 3 * length;
#ifdef _DEBUG
		fprintf(stderr, "palette: (first = %d, length = %d)\n", first, length);
#endif
	}
}
void lb0d5f(unsigned char **dst_pp) {
	unsigned short c;
	unsigned short *src_p = lb0cba;
	unsigned char *dst_p = *dst_pp;

	if (--lb0cb6 == 0) {
		src_p += 4;
		lb0cba = src_p;
		lb0cb6 = *src_p;
	}
	c = (lb0cb0 += src_p[1]);
	if (c & 0x80) c += 256;
	*dst_p++ = (unsigned char) ((c >> 8) & 0x3f);
	c = (lb0cb2 += src_p[2]);
	if (c & 0x80) c += 256;
	*dst_p++ = (unsigned char) ((c >> 8) & 0x3f);
	c = (lb0cb4 += src_p[3]);
	if (c & 0x80) c += 256;
	*dst_p++ = (unsigned char) ((c >> 8) & 0x3f);
	*dst_pp = dst_p;
}

void lb0d22(void) {
	unsigned char *src_p, *dst_p;
	unsigned short ax, cx, dx;

	if (lb0cb6 & 0x8000)
		return;
	src_p = dst_p = &lb0070[3 * 80];
	cx = 3 * 80;
	dx = lb0cb8;
	ax = 3 * dx;
	src_p += ax;
	cx -= ax;
	memcpy(dst_p, src_p, cx);
	dst_p += cx;
	cx = dx;
	for (cx = dx; cx > 0; cx--) {
		lb0d5f(&dst_p);
	}
	port_set_palette(80, 80, &lb0070[3 * 80]);
	port_delay(20);
}
void lb0cf4(void) {
	unsigned short cx, *src_p;
	
	logo_blit_mirror();
	src_p = lb0cbc;
	lb0cb6 = *src_p;
	lb0cba = src_p;
	lb0cb0 = 0;
	lb0cb2 = 0;
	lb0cb4 = 0;
	for (cx = 251; cx > 0; cx--)
		lb0d22();
}
/*** palette */








unsigned short lb0ead(char **src_p) {
	size_t block_size;

	*src_p = block_p;
	block_size = *((unsigned short *) *src_p);
	*src_p += 2;
#ifdef _DEBUG
	fprintf(stderr, "block: (ptr = %08x, size = %d)\n", (unsigned) src_p, block_size);
#endif
	return block_size;
}
unsigned short lb0e86(void) {
	unsigned short block_size;
	char *src_p = block_p;

	src_p += *((unsigned short *) src_p);
	block_p = src_p;
	block_size = *((unsigned short *) src_p);
	src_p += 2;
#ifdef _DEBUG
	fprintf(stderr, "block: (ptr = %08x, size = %d)\n", (unsigned) src_p, block_size);
#endif
	return block_size;
}
unsigned short lb0e4c(void) {
	unsigned short block_size;
	
	if ((block_size = lb0e86()) == 0)
		da0052 = 0;
	return block_size;
}
void lb0e49(void) {
	logo_blit();
	lb0e4c();
}
void lb0e46(void) {
	lb0d22();
	lb0e49();
}










size_t lb109a(const char *dst_p, FILE *file_p) {
	long fsize;
	size_t nread, count;
	char *buffer_p = (char *) dst_p;

	if (fseek(file_p, 0L, SEEK_END) != 0)
		return 0;
	if ((fsize = ftell(file_p)) <= 0)
		return 0;
	if (fseek(file_p, 0L, SEEK_SET) != 0)
		return 0;
	count = (size_t) fsize;
	while (count > 0) {
		size_t block_size = (count > 8192)? 8192: count;

		nread = fread(buffer_p, sizeof(char), block_size, file_p);
		buffer_p += nread;
		count -= nread;
	}
	fclose(file_p);
	return (size_t) (fsize - count);
}
int lb0dbc(FILE *file_p) {
	size_t length;

	block_p = da004c;
	if ((length = lb109a(block_p, file_p)) > 0) {
		*((unsigned short *) &block_p[length]) = 0;
		return 1;
	} else {
		return 0;
	}
}
/*
void lb0fea(unsigned short ax, void (*func)(void)) {
	unsigned short bios_ticks, msec;

	func();
	bios_ticks = (ax >> 3) - (ax >> 5);
	msec = 55 * bios_ticks;
	port_delay(msec);
}
*/
void lb0fea(unsigned short ax) {
	unsigned short bios_ticks, msec;
	bios_ticks = (ax >> 3) - (ax >> 5);
	msec = 55 * bios_ticks;
	port_delay(msec);
}
int lb0dde(unsigned short ax, FILE *file_p) {
	char *src_p;

	da0054 = ax;
	if (!lb0dbc(file_p)) return 0;


	logo_blit_clear();
	da0052 = da0054;

	block_p = da004c;
	if (lb0ead(&src_p) <= 0) return 0;
	logo_load_palette();
	if (lb0e4c() <= 0) return 0;
	lb0e49();
	lb0cf4();

	do {
		lb0e46();
		lb0fea(da0052);
	} while (da0052 > 0);

	return 1;
}




void lb10f4(char *par_tab[], int argc, char *argv[]) {
	int i;
	if (argc > 1) {
		for (i = 1; i < min(argc, 4); i++) {
			if (par_tab[i-1] == NULL) break;
			strcpy(par_tab[i-1], argv[i]);
		}
	}
}
void lb112d(char *str) {
	/* parse parameters convert string to number rewrite string with converted value */
	int c, i;
	char *src;
	unsigned long value = 0, temp;

	for (src = str; (c = *src) <= ' '; src++)
		if (c >= 'a') *src &= 0xdf;		/* convert to uppercase */
	for (src = str, i = 10; i > 0 && *src != 0; src++, i--)	/* scan string for 'H' */
		if (*src == 'H') break;
	if (*src == 'H') {	/* present -> convert string to hex number */
		src = str;
		for (i = 8; i; i--) {
			c = *src++;
			if (c >= 'A' && c <= 'F') c -= 7;
			c -= '0';
			if (c < 0 || c >= 16) break;
			value <<= 4;
			value |= c;
		}	
	} else {			/* !present -> convert string to decimal number */
		src = str;
		for (i = 8; i; i--) {
			c = *src++;
			c -= '0';
			if (c < 0 || c >= 10) break;
			value <<= 1;
			temp = value;
			temp <<= 2;
			value += temp;	/* value=value*10=value*(8+2)=(value<<3)+(value<<1) no multiplication needed */
			value += c;
		}
	}
	*((unsigned long *) str) = value;
}




int logo(int argc, char *argv[]) {
	char *str_p;
	FILE *file_p;
	unsigned short ax;
/*	char *pc;
	FILE *handle;  */

	/* runtime check */
	if (sizeof(char) != 1 || sizeof(short) != 2 || sizeof(int) != 4)
		return 1;	/* logo run only 32-bit platforms */

	lb10f4(da0008, argc, argv);
	ax = 0x000c;
	if (*da008e != 0) {
		lb112d(da008e);
		ax = *((unsigned short *) da008e);
	}
	da0052 = ax;	
	if ((str_p = strchr(da005a, '.')) == NULL)
		strcat(da005a, ".hnm");
	if ((file_p = fopen(da005a, "rb")) != NULL) {
		lb0dde(da0052, file_p);
		logo_blit_adjust();
	} else {
		fprintf(stderr, "Error: HNM file not found\n");
	}
	return 0;
}
