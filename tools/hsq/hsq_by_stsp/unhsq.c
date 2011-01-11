#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short word;

struct HSQ {
    word orig_size;
    byte magic1;
    word size;
    byte magic2;
    byte *data;
} __attribute__((packed));

static int loadfile(const char *filename, struct HSQ *hsq)
{
    int ret = 1;
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
	printf("ERROR: Can't open file %s!\n", filename);
	return 0;
    } else {
	printf("Reading file %s...\n", filename);
    }

    fread(hsq, 6, 1, f);
    hsq->data = (byte*)malloc(hsq->size - 6);
    if (fread(hsq->data, 1, hsq->size - 6, f) != hsq->size - 6) {
	printf("ERROR: Can't read %i bytes!\n", hsq->size - 6);
	ret = 0;
    }

    fclose(f);
    return ret;
}

static int check_magic(byte *magic)
{
    return (((magic[0] + magic[1] + magic[2] + magic[3] + magic[4] + magic[5])
	    & 0xff) == 0xab);
}


/* The data is organized in a chunks, 18 and more bytes each.
 * Every chunk starts with the 2-bytes "header" bitmap, followed
 * by 16 or more data bytes.
 * The bits in the bitmap specify how to interpret the respective
 * data bytes of a chunk. If the subsequent bit in the bitmap is
 * set (1), then the corresponding data byte is just copied to an
 * output. If the bitmap bit is zero, then the subsequent bitmap
 * bits and data bytes are used to locate the sequence in a
 * previously extracted data and duplicate it. */
static int unpack2(byte * src, byte * dst)
{
#define NEXT_BIT { \
    csize--; \
    if (!csize) { \
	cmask = *(word *)&src[sidx]; \
	sidx += 2; \
	csize = 16; \
    } \
    cur_bit = (cmask >> (16 - csize)) & 1; \
}

    byte csize, cur_bit;
    word cmask;
    long sidx = 0, didx = 0;

    csize = 1;
    NEXT_BIT;

    while (1) {
	if (cur_bit) {
	    /* 1 - just copy one byte */
	    dst[didx++] = src[sidx++];
	} else {
	    long count, offset;

	    NEXT_BIT;
	    if (cur_bit) {
		/* 10 - copy up to 7 bytes of previously extracted
		 * data, from no more than 0x2000 bytes behind the
		 * current extract position. */
		count = src[sidx] & 7;
		offset = ((*(word *)&src[sidx]) >> 3) - 0x2000;
		sidx += 2;
		if (!count) {
		    /* can copy up to 255 bytes */
		    count = src[sidx++];
		}
		if (!count)
		    return 1;
	    } else {
		/* 00 - copy up to 3 bytes from the position not
		 * further than 0x100 bytes behind. */
		NEXT_BIT;
		count = cur_bit << 1;
		NEXT_BIT;
		count |= cur_bit;
		offset = src[sidx++] - 0x100;
	    }

	    /* make sure we copy from the already extracted data */
	    if (offset > 0 || didx + offset < 0)
		return 0;

	    /* um, I lied in the above comments - copy 2 bytes more */
	    count += 2;
	    memcpy(dst + didx, dst + didx + offset, count);
	    didx += count;
	}

	NEXT_BIT;
    }
}

int main(int argc, char *argv[])
{
    struct HSQ hsq;
    byte *dst;
    FILE *f1;

    printf("UnHSQ Copyright (C) Roman Dolejsi\n\n");

    if (argc != 3) {
	printf("usage: unhsq <input file.hsq> <output file>\n");
	return 1;
    }

    if (!loadfile(argv[1], &hsq))
	return 1;
    if (!check_magic((byte *)&hsq)) {
	printf("ERROR: Not a HSQ archive!\n");
	return 1;
    }

    dst = (byte*)malloc(hsq.orig_size);

    printf("Unpacking...\n");

    if (unpack2(hsq.data, dst)) {
	f1 = fopen(argv[2], "wb");
	if (!f1) {
	    printf("ERROR: Can't create file %s!\n", argv[2]);
	} else {
	    printf("Writing file %s...\n", argv[2]);
    	    fwrite(dst, hsq.orig_size, 1, f1);
	    fclose(f1);
	    printf("done\n");
	}
    } else {
	printf("failed\n");
    }

    free(hsq.data);
    free(dst);

    return 0;
}
