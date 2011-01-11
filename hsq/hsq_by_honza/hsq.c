/*************************************************************************************************/
/* hsq compression / decompression command line utility                                          */
/* - optimized for low memory footprint                                                          */
/* - data and heap memory requirements: ~20kB (designed for embedded microcontroller systems)    */
/* - speed depends on i/o speed                                                                  */
/*************************************************************************************************/
/* configurable options */
#define hsq main			/* enables hsq compression / decompression utility */
/*#define selftest main*/	/* enables self test program, you need dune data files */
/*#define DEBUG*/			/* enables debug info log */
/*************************************************************************************************/
/* base i/o headers */
/*************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <fcntl.h>
/*************************************************************************************************/
/* antidements */
/*************************************************************************************************/
#if defined (_WIN32)
/* MS Visual C 9.0+ treats standart posix libs as deprected :-o */
#pragma warning(disable: 4996)
#endif
/*************************************************************************************************/
/* workaround */
#ifdef _WIN32
/* I need 16-bit getw/putw (not 32-bit) */
/* returns EOF (-1) on error or word read / written on success */
#define getw getw16bit
#define putw putw16bit

int getw(FILE *file_p) {
	int a, b;

	if ((a = getc(file_p)) == EOF || ferror(file_p))
		return EOF;
	if ((b = getc(file_p)) == EOF || ferror(file_p))
		return EOF;
	return (a & 0x00ff) | ((b << 8) & 0xff00);
}

int putw(int c, FILE *file_p) {
	if (putc(c & 0xff, file_p) == EOF || ferror(file_p))
		return EOF;
	if (putc((c >> 8) & 0xff, file_p) == EOF || ferror(file_p))
		return EOF;
	return c;
}
#endif
/*************************************************************************************************/
/* bitstream structure / primitives */
/*************************************************************************************************/
typedef struct bitstream_s {
	unsigned int bits;
	int data[48];
	int count;
	FILE *ifile_p, *ofile_p;
	int ifsize, ifpos, ofsize, ofpos, bsize;
	unsigned char *start_p, *buf_p, *end_p;
} bitstream_t;
/*************************************************************************************************/
int bitstream_init(char *ifname, char *ofname, int buf_size, bitstream_t *bitstream_p) {
	long fsize;

	bitstream_p->bits = 0x00000001;
	bitstream_p->count = 0;
	bitstream_p->ifpos = bitstream_p->ofpos = 0;
	bitstream_p->ifile_p = bitstream_p->ofile_p = NULL;
	if ((bitstream_p->start_p = (unsigned char *) malloc((size_t) buf_size)) == NULL) {
		bitstream_p->buf_p = bitstream_p->end_p = NULL;
		bitstream_p->bsize = 0;
		fprintf(stderr, "error: out of memory\n");
		return 0;
	} else {
		bitstream_p->buf_p = bitstream_p->end_p = bitstream_p->start_p;
		bitstream_p->bsize = buf_size;
		if ((bitstream_p->ifile_p = fopen(ifname, "rb")) == NULL) {
			fprintf(stderr, "error: can't open file %s\n", ifname);
			return 0;
		}
		if ((bitstream_p->ofile_p = fopen(ofname, "wb")) == NULL) {
			fprintf(stderr, "error: can't open file %s\n", ofname);
			return 0;
		}
		if (fseek(bitstream_p->ifile_p, 0L, SEEK_END) != 0) {
			fprintf(stderr, "error: seek file\n");
			return 0;
		}
		if ((fsize = ftell(bitstream_p->ifile_p)) == -1L) {
			fprintf(stderr, "error: tell file pos\n");
			return 0;
		}
		bitstream_p->ifsize = (int) fsize;
		if (fseek(bitstream_p->ifile_p, 0L, SEEK_SET) != 0) {
			fprintf(stderr, "error: seek file\n");
			return 0;
		}
	}
	return 1;
}
/*************************************************************************************************/
void bitstream_deinit(bitstream_t *bitstream_p) {
	if (bitstream_p->start_p != NULL)
		free(bitstream_p->start_p);
	bitstream_p->start_p = bitstream_p->buf_p = bitstream_p->end_p = NULL;
	bitstream_p->bsize = 0;
	if (bitstream_p->ifile_p != NULL)
		if (fclose(bitstream_p->ifile_p) != 0) { }
	if (bitstream_p->ofile_p != NULL)
		if (fclose(bitstream_p->ofile_p) != 0) { }
}
/*************************************************************************************************/
int bitstream_putc(int c, bitstream_t *bitstream_p) {
	if (bitstream_p->buf_p - bitstream_p->start_p >= 2 * 8192) {
		if ((fwrite(bitstream_p->start_p, sizeof(char), 8192, bitstream_p->ofile_p)) != 8192)
			return -1;
		memmove(bitstream_p->start_p, bitstream_p->start_p + 8192, bitstream_p->buf_p - bitstream_p->start_p - 8192);
		bitstream_p->buf_p -= 8192;
		bitstream_p->ofpos += 8192;
	}
	*bitstream_p->buf_p++ = (unsigned char) (c & 0xff);
	return c;
}
/*************************************************************************************************/
int bitstream_putw(int c, bitstream_t *bitstream_p) {
	if (bitstream_putc(c & 0xff, bitstream_p) < 0)
		return -1;
	if (bitstream_putc((c >> 8) & 0xff, bitstream_p) < 0)
		return -1;
	return c;
}
/*************************************************************************************************/
int bitstream_flush(bitstream_t *bitstream_p) {
	int block_size = bitstream_p->buf_p - bitstream_p->start_p;

	if ((fwrite(bitstream_p->start_p, sizeof(char), block_size, bitstream_p->ofile_p)) != block_size)
		return 0;
	bitstream_p->ofpos += block_size;
	return 1;
}
/*************************************************************************************************/
int bitstream_getc(bitstream_t *bitstream_p) {
	int c;
	c = (int) *bitstream_p->buf_p;
	return c;
}
/*************************************************************************************************/
int bitstream_match(int *span, bitstream_t *bitstream_p) {
	unsigned char *ptr;
	int i, j, count, shift, length = 1;

	if (bitstream_p->buf_p + 257 >= bitstream_p->end_p && bitstream_p->ifpos < bitstream_p->ifsize) {
		if (bitstream_p->buf_p - bitstream_p->start_p > 8192) {
			shift = bitstream_p->buf_p - bitstream_p->start_p - 8192;
			memmove(bitstream_p->start_p, bitstream_p->start_p + shift, bitstream_p->end_p - bitstream_p->buf_p + 8192);
			bitstream_p->buf_p -= shift;
			bitstream_p->end_p -= shift;
		}
		count = bitstream_p->bsize - (bitstream_p->end_p - bitstream_p->start_p);
		if (count > bitstream_p->ifsize - bitstream_p->ifpos)
			count = bitstream_p->ifsize - bitstream_p->ifpos;
		if ((fread(bitstream_p->end_p, sizeof(char), count, bitstream_p->ifile_p)) != count)
			return -1;
		bitstream_p->end_p += count;
		bitstream_p->ifpos += count;
	}
	if (bitstream_p->buf_p >= bitstream_p->end_p)
		return 0;

	if ((j = bitstream_p->start_p - bitstream_p->buf_p) < -8192)
		j = -8192;
	while (j <= -1) {
		ptr = bitstream_p->buf_p + j;
		for (i = 0; *(ptr + i) == *(bitstream_p->buf_p + i) && i < 257 && (bitstream_p->buf_p + i) < bitstream_p->end_p - 1; i++) { }
		if (i > 2 && i >= length) {
			length = i;
			*span = j;
		}
		j++;
	}
	if (length > 2) {
		if (*span < -8192 || *span > -1 || length < 3 || length > 257)
			fprintf(stderr, "error: bug found\n");
	} else {
		if ((j = bitstream_p->start_p - bitstream_p->buf_p) < -256)
			j = -256;
		while (j <= -1) {
			ptr = bitstream_p->buf_p + j;
			for (i = 0; *(ptr + i) == *(bitstream_p->buf_p + i) && i < 5 && (bitstream_p->buf_p + i) < bitstream_p->end_p - 1; i++) { }
			if (i > 1 && i >= length) {
				length = i;
				*span = j;
			}
			j++;
		}
		if (length > 1) {
			if (*span < -256 || *span > -1 || length < 2 || length > 5)
				fprintf(stderr, "error: bug found\n");
		} else {
			length = 1;
		}
	}
	return length;
}
/*************************************************************************************************/
/* compression / decompression helpers */
/*************************************************************************************************/
int verify(int *ptr, bitstream_t *bitstream_p) {
	int i;
	unsigned char checksum = 0;

	for (i = 6; i > 0; i--) {
		if ((*ptr = getc(bitstream_p->ifile_p)) < 0 || ferror(bitstream_p->ifile_p)) {
			fprintf(stderr, "error: input stream read\n");
			return 0;
		}
		checksum += (unsigned char) (*ptr++ & 0xff);
	}
	return checksum == 0xab;
}
/*************************************************************************************************/
int getbit(bitstream_t *bitstream_p) {
	int bit;
	
	if (bitstream_p->bits == 0x00000001) {
		if ((bitstream_p->bits = getw(bitstream_p->ifile_p)) < 0 || ferror(bitstream_p->ifile_p)) {
			fprintf(stderr, "error: input stream read\n");
			bitstream_deinit(bitstream_p);
			exit(1);
		}
		bitstream_p->bits &= 0x0000ffff;	/* sanity */
		bitstream_p->bits |= 0x00010000;
	}
	bit = bitstream_p->bits & 0x00000001;
	bitstream_p->bits >>= 1;
	return bit;
}
/*************************************************************************************************/
int putbit(int bit, bitstream_t *bitstream_p) {
	int i;
	unsigned short bits;

	if (bitstream_p->bits & 0xffff0000) {
		bits = (unsigned short) (bitstream_p->bits & 0x0000ffff);
		bits = ((bits & 0xff00) >> 8) | ((bits & 0x00ff) << 8);
		bits = ((bits & 0xf0f0) >> 4) | ((bits & 0x0f0f) << 4);
		bits = ((bits & 0xcccc) >> 2) | ((bits & 0x3333) << 2);
		bits = ((bits & 0xaaaa) >> 1) | ((bits & 0x5555) << 1);
		if (putw((int) bits, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p)) {
			fprintf(stderr, "error: output stream_write\n");
			bitstream_deinit(bitstream_p);
			exit(1);
		}
		for (i = 0; i < bitstream_p->count; i++)
			if (putc(bitstream_p->data[i], bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p)) {
				fprintf(stderr, "error: output stream_write\n");
				bitstream_deinit(bitstream_p);
				exit(1);
			}
		bitstream_p->bits = 0x00000001;
		bitstream_p->count = 0;
	}
	bitstream_p->bits <<= 1;
	if (bit)
		bitstream_p->bits |= 0x00000001;
	return 1;
}
/*************************************************************************************************/
int decompress(char *ifname, char *ofname, bitstream_t *bitstream_p) {
	static int signature[6];
	int c;
	int span, length;
	int isize, osize;

	if (!bitstream_init(ifname, ofname, 2 * 8192 + 512, bitstream_p)) {
		bitstream_deinit(bitstream_p);
		return 0;
	}
	if (!verify(signature, bitstream_p)) {
		fprintf(stderr, "error: bad checksum\n");
		bitstream_deinit(bitstream_p);
		return 0;
	}
	osize = signature[0] & 0xff | ((signature[1] & 0xff) << 8) | ((signature[2] & 0xff) << 16);
	isize = signature[3] & 0xff | ((signature[4] & 0xff) << 8);
	if (isize != bitstream_p->ifsize) {
		fprintf(stderr, "error: corrupted header\n");
		bitstream_deinit(bitstream_p);
		return 0;
	}
	while (1) {
		if (getbit(bitstream_p)) {
			if ((c = getc(bitstream_p->ifile_p)) < 0 || ferror(bitstream_p->ifile_p)) {
				fprintf(stderr, "error: input stream read\n");
				bitstream_deinit(bitstream_p);
				return 0;
			}
			if (bitstream_putc(c, bitstream_p) < 0) {
				fprintf(stderr, "error: output stream write\n");
				bitstream_deinit(bitstream_p);
				return 0;
			}
#ifdef DEBUG
			fprintf(stderr, "debug: byte at %06x - bits 1 %02x\n", bitstream_p->ofpos + (bitstream_p->buf_p - bitstream_p->start_p) - 1, *(bitstream_p->buf_p - 1));
#endif
		} else {
			if (getbit(bitstream_p)) {
				if ((span = getw(bitstream_p->ifile_p)) < 0 || ferror(bitstream_p->ifile_p)) {
					fprintf(stderr, "error: input stream read\n");
					bitstream_deinit(bitstream_p);
					return 0;
				}
				length = span & 0x00000007;
				span >>= 3;
				span |= 0xffffe000;
				if (length == 0) {
					if ((length = getc(bitstream_p->ifile_p)) < 0 || ferror(bitstream_p->ifile_p)) {
						fprintf(stderr, "error: input stream read\n");
						bitstream_deinit(bitstream_p);
						return 0;
					}
#ifdef DEBUG
					if (length)
						fprintf(stderr, "debug: block match at %06x - bits 01+ length %d span %d\n", bitstream_p->ofpos + (bitstream_p->buf_p - bitstream_p->start_p), length + 2, span);
#endif
				}
#ifdef DEBUG
				else
					fprintf(stderr, "debug: block match at %06x - bits 01 length %d span %d\n", bitstream_p->ofpos + (bitstream_p->buf_p - bitstream_p->start_p), length + 2, span);
#endif
				if (length == 0) {
					bitstream_flush(bitstream_p);
					bitstream_deinit(bitstream_p);
					return 1;
				}
			} else {
				length = getbit(bitstream_p) << 1;
				length |= getbit(bitstream_p);
				if ((span = getc(bitstream_p->ifile_p)) < 0 || ferror(bitstream_p->ifile_p)) {
					fprintf(stderr, "error: input stream read\n");
					bitstream_deinit(bitstream_p);
					return 0;
				}
				span |= 0xffffff00;
#ifdef DEBUG
				fprintf(stderr, "debug: block match at %06x - bits 00 length %d span %d\n", bitstream_p->ofpos + (bitstream_p->buf_p - bitstream_p->start_p), length + 2, span);
#endif
			}
			length += 2;
			for (; length > 0; length--) {
				c = *(bitstream_p->buf_p + span);
				if (bitstream_putc(c, bitstream_p) < 0) {
					fprintf(stderr, "error: output stream write\n");
					bitstream_deinit(bitstream_p);
					return 0;
				}
			}
		}
	}
}
/*************************************************************************************************/
int compress(char *ifname, char *ofname, bitstream_t *bitstream_p) {
	unsigned char c, checksum;
	int span, length;
	long fsize;

	if (bitstream_init(ifname, ofname, 2 * 8192, bitstream_p)) {
		if (putc(0, bitstream_p->ofile_p) < 0 ||	/* don't worry with header now, rewrite it later */
			putc(0, bitstream_p->ofile_p) < 0 ||
			putc(0, bitstream_p->ofile_p) < 0 ||
			putc(0, bitstream_p->ofile_p) < 0 ||
			putc(0, bitstream_p->ofile_p) < 0 ||
			putc(0xab, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p)) {
			fprintf(stderr, "error: can't write header\n");
		} else {
			while ((length = bitstream_match(&span, bitstream_p)) > 0) {
				if (length > 1 && length < 6 && span >= -256) {
					putbit(0, bitstream_p);
					putbit(0, bitstream_p);
					putbit((length - 2) & 0x00000002, bitstream_p);
					putbit((length - 2) & 0x00000001, bitstream_p);
					bitstream_p->data[bitstream_p->count++] = span & 0xff;
#ifdef DEBUG
					fprintf(stderr, "debug: block match at %06x - bits 00 length %d span %d\n", bitstream_p->ifpos - (bitstream_p->end_p - bitstream_p->buf_p), length, span);
#endif
				} else if (length > 2 && length < 10 && span >= -8192) {
					putbit(0, bitstream_p);
					putbit(1, bitstream_p);
					bitstream_p->data[bitstream_p->count++] = ((span << 3) | ((length - 2) & 0x00000007)) & 0xff;
					bitstream_p->data[bitstream_p->count++] = (span >> 5) & 0xff;
#ifdef DEBUG
					fprintf(stderr, "debug: block match at %06x - bits 01 length %d span %d\n", bitstream_p->ifpos - (bitstream_p->end_p - bitstream_p->buf_p), length, span);
#endif
				} else if (length >= 10 && length <= 257 && span >= -8192) {
					putbit(0, bitstream_p);
					putbit(1, bitstream_p);
					bitstream_p->data[bitstream_p->count++] = (span << 3) & 0xff;
					bitstream_p->data[bitstream_p->count++] = (span >> 5) & 0xff;
					bitstream_p->data[bitstream_p->count++] = (length - 2) & 0xff;
#ifdef DEBUG
					fprintf(stderr, "debug: block match at %06x - bits 01+ length %d span %d\n", bitstream_p->ifpos - (bitstream_p->end_p - bitstream_p->buf_p), length, span);
#endif
				} else {
					putbit(1, bitstream_p);
					bitstream_p->data[bitstream_p->count++] = bitstream_getc(bitstream_p);
#ifdef DEBUG
					fprintf(stderr, "debug: byte at %06x - bits 1 %02x\n", bitstream_p->ifpos - (bitstream_p->end_p - bitstream_p->buf_p), *bitstream_p->buf_p);
#endif
				}
				bitstream_p->buf_p += length;
			}
			if (length == 0) {
				/* flush remaing bits */
				putbit(0, bitstream_p);
				putbit(1, bitstream_p);
				bitstream_p->data[bitstream_p->count++] = 0;
				bitstream_p->data[bitstream_p->count++] = 0;
				bitstream_p->data[bitstream_p->count++] = 0;
				while (!(bitstream_p->bits & 0xffff0000))
					bitstream_p->bits <<= 1;
				putbit(0, bitstream_p);
				/* get output file size */
				if ((fsize = ftell(bitstream_p->ofile_p)) == -1L) {
					fprintf(stderr, "error: tell file pos\n");
					bitstream_deinit(bitstream_p);
					return 0;
				}
				bitstream_p->ofsize = (int) fsize;
				if (fseek(bitstream_p->ofile_p, 0L, SEEK_SET) != 0) {
					fprintf(stderr, "error: seek file\n");
					bitstream_deinit(bitstream_p);
					return 0;
				}
				/* rewrite header */
				checksum = 0xab;
				checksum -= (c = (unsigned char) ((bitstream_p->ifsize >> 0) & 0xff));
				if (putc((int) c, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p))
					fprintf(stderr, "error: can't write header\n");
				checksum -= (c = (unsigned char) ((bitstream_p->ifsize >> 8) & 0xff));
				if (putc((int) c, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p))
					fprintf(stderr, "error: can't write header\n");
				checksum -= (c = (unsigned char) ((bitstream_p->ifsize >> 16) & 0xff));
				if (putc((int) c, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p))
					fprintf(stderr, "error: can't write header\n");
				checksum -= (c = (unsigned char) ((bitstream_p->ofsize >> 0) & 0xff));
				if (putc((int) c, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p))
					fprintf(stderr, "error: can't write header\n");
				checksum -= (c = (unsigned char) ((bitstream_p->ofsize >> 8) & 0xff));
				if (putc((int) c, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p))
					fprintf(stderr, "error: can't write header\n");
				if (putc((int) checksum, bitstream_p->ofile_p) < 0 || ferror(bitstream_p->ofile_p))
					fprintf(stderr, "error: can't write header\n");
				bitstream_deinit(bitstream_p);
				return 1;
			} else
				fprintf(stderr, "error: pattern match\n");
		}
		bitstream_deinit(bitstream_p);
	}
	return 0;
}
/*************************************************************************************************/
void usage(void) {
	printf(
		"usage:\n"
		"hsq infile <outfile> ... compression\n"
		"hsq -x infile <outfile> ... decompression\n"
	);
	exit(0);
}
/*************************************************************************************************/
int hsq(int argc, char *argv[]) {
	static char iname[256], oname[256];
	static bitstream_t bitstream;
	bitstream_t *bitstream_p = &bitstream;
	int flag;
	size_t i;

	if (sizeof(int) != 4 || sizeof(short) != 2) {
		printf("sorry, can't run this machine\n");
		exit(0);
	}
	if (argc == 2) {
		flag = 0;
		strncpy(iname, argv[1], 255); iname[255] = 0;
		oname[0] = 0;
	} else if (argc == 3) {
		if (strcmp(argv[1], "-x") == 0) {
			flag = 1;
			strncpy(iname, argv[2], 255); iname[255] = 0;
			oname[0] = 0;
		} else {
			flag = 0;
			strncpy(iname, argv[1], 255); iname[255] = 0;
			strncpy(oname, argv[2], 255); oname[255] = 0;
		}
	} else if (argc == 4) {
		if (strcmp(argv[1], "-x") == 0) {
			flag = 1;
			strncpy(iname, argv[2], 255); iname[255] = 0;
			strncpy(oname, argv[3], 255); oname[255] = 0;
		} else {
			usage();
		}
	} else {
		usage();
	}
	if (flag) {
		if (oname[0] == 0) {
			strcpy(oname, iname);
			for (i = 0; i < strlen(oname); i++) {
				if (oname[i] == '.') {
					oname[i] = 0; break;
				}
			}
		}
		if (strrchr(iname, '.') == NULL && strlen(iname) < 252)
			strcat(iname, ".hsq");
		if (strcmp(iname, oname) == 0)
			exit(1);
		printf("extraction %s ... ", iname);
		if (decompress(iname, oname, bitstream_p))
			printf("ok\n");
		else
			printf("failed\n");
	} else {
		if (oname[0] == 0) {
			strcpy(oname, iname);
			for (i = 0; i < strlen(oname); i++) {
				if (oname[i] == '.') {
					oname[i] = 0; break;
				}
			}
		}
		if (strrchr(oname, '.') == NULL && strlen(oname) < 252)
			strcat(oname, ".hsq");
		printf("compression %s ... ", iname);
		if (compress(iname, oname, bitstream_p))
			printf("%d%%\n", (bitstream_p->ifsize != 0)? bitstream_p->ofsize * 100 / bitstream_p->ifsize: 0);
		else
			printf("failed\n");
	}
	return 0;
}
/*************************************************************************************************/
char *dunefiles[] = {
	"arrakis.agd",
	"arrakis.hsq",
	"arrakis.m32",
	"attack.hsq",
	"back.hsq",
	"bagdad.agd",
	"bagdad.hsq",
	"bagdad.m32",
	"balcon.hsq",
	"baro.hsq",
	"book.hsq",
	"bota.hsq",
	"bunk.hsq",
	"comm.hsq",
	"command1.hsq",
	"command2.hsq",
	"command3.hsq",
	"condit.hsq",
	"corr.hsq",
	"credits.hsq",
	"cryo.hsq",
	"death1.hsq",
	"death2.hsq",
	"death3.hsq",
	"dialogue.hsq",
	"dune386.hsq",
	"duneadl.hsq",
	"duneagd.hsq",
	"dunechar.hsq",
	"dunemid.hsq",
	"dunepcs.hsq",
	"dunes2.hsq",
	"dunes3.hsq",
	"dunes.hsq",
	"dunesdb.hsq",
	"dunevga.hsq",
	"empr.hsq",
	"equi.hsq",
	"feyd.hsq",
	"final.hsq",
	"fort.hsq",
	"fresk.hsq",
	"frm1.hsq",
	"frm2.hsq",
	"frm3.hsq",
	"generic.hsq",
	"globdata.hsq",
	"gurn.hsq",
	"hara.hsq",
	"hark.sal",
	"hawa.hsq",
	"chan.hsq",
	"chankiss.hsq",
	"icones.hsq",
	"idah.hsq",
	"intds.hsq",
	"jess.hsq",
	"kyne.hsq",
	"leto.hsq",
	"logo.hnm",
	"map2.hsq",
	"map.hsq",
	"mirror.hsq",
	"mois.hsq",
	"morning.agd",
	"morning.hsq",
	"morning.m32",
	"onmap.hsq",
	"orny.hsq",
	"ornycab.hsq",
	"ornypan.hsq",
	"ornytk.hsq",
	"palace.sal",
	"palplan.hsq",
	"paul.hsq",
	"pers.hsq",
	"phrase11.hsq",
	"phrase12.hsq",
	"phrase21.hsq",
	"phrase22.hsq",
	"phrase31.hsq",
	"phrase32.hsq",
	"por.hsq",
	"progue.hsq",
	"prt.hnm",
	"sd1.hsq",
	"sd2.hsq",
	"sd3.hsq",
	"sd4.hsq",
	"sd5.hsq",
	"sd6.hsq",
	"sd7.hsq",
	"sd8.hsq",
	"sd9.hsq",
	"sda.hsq",
	"sdb.hsq",
	"sekence.agd",
	"sekence.hsq",
	"sekence.m32",
	"serre.hsq",
	"shai2.hsq",
	"shai.hsq",
	"siet0.hsq",
	"siet1.hsq",
	"siet.sal",
	"sietchm.agd",
	"sietchm.hsq",
	"sietchm.m32",
	"sky.hsq",
	"smug.hsq",
	"stars.hsq",
	"stil.hsq",
	"sun.hsq",
	"sunrs.hsq",
	"tablat.bin",
	"ver.hsq",
	"verbin.hsq",
	"vilg.hsq",
	"vilg.sal",
	"vis.hsq",
	"warsong.agd",
	"warsong.hsq",
	"warsong.m32",
	"water.agd",
	"water.hsq",
	"water.m32",
	"wormintr.agd",
	"wormintr.hsq",
	"wormintr.m32",
	"wormsuit.agd",
	"wormsuit.hsq",
	"wormsuit.m32"
};
/*************************************************************************************************/
int fcompare(char *name1, char *name2) {
	int rval = 1;
	FILE *file1_p, *file2_p;

	if ((file1_p = fopen(name1, "rb")) != NULL) {
		if ((file2_p = fopen(name2, "rb")) != NULL) {
			while (1) {
				if (ferror(file1_p) || ferror(file2_p)) { rval = 0; break; }
				if (feof(file1_p) || feof(file2_p)) {
					if (feof(file1_p) && !feof(file2_p)) rval = 0;
					if (feof(file2_p) && !feof(file1_p)) rval = 0;
					break;
				}
				if (fgetc(file1_p) != fgetc(file2_p)) { rval = 0; break; }
			}
			fclose(file2_p);
		} else
			rval = 0;
		fclose(file1_p);
	} else
		rval = 0;
	return rval;
}
/*************************************************************************************************/
int selftest(void) {
	static char name1[256], name2[256], name3[256];
	static bitstream_t bitstream;
	size_t i;
	bitstream_t *bitstream_p = &bitstream;

	if (sizeof(int) != 4 || sizeof(short) != 2) {
		printf("sorry, can't run this machine\n");
		exit(0);
	}
	printf("pass #1\n");
	for (i = 0; i < sizeof(dunefiles) / sizeof(*dunefiles); i++) {
		printf("%s ... ", dunefiles[i]);
		strcpy(name2, dunefiles[i]);
		strcpy(name1, dunefiles[i]);
		strcat(name1, ".new");
		strcat(name2, ".bin");
		if (decompress(dunefiles[i], name2, bitstream_p)) {
			if (compress(name2, name1, bitstream_p)) {
				if (fcompare(dunefiles[i], name1))
					printf("same\n");
				else
					printf("different\n");
			} else
				printf("compression failed\n");
		} else
			printf("decompression failed\n");
	}
	printf("pass #2\n");
	for (i = 0; i < sizeof(dunefiles) / sizeof(*dunefiles); i++) {
		printf("%s ... ", dunefiles[i]);
		strcpy(name1, dunefiles[i]);
		strcpy(name2, dunefiles[i]);
		strcpy(name3, dunefiles[i]);
		strcat(name1, ".new");
		strcat(name2, ".bin2");
		strcat(name3, ".bin");
		if (decompress(name1, name2, bitstream_p)) {
			if (fcompare(name2, name3))
				printf("ok\n");
			else
				printf("failed\n");
		} else
			printf("decompression failed\n");
	}
	printf("done\n");
	return 0;
}
/*************************************************************************************************/
