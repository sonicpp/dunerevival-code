#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

typedef unsigned char byte;
typedef unsigned short word;

byte *ori_src;
byte *ori_dst;

byte *loadfile(const char *filename)
{
       FILE *f = fopen(filename, "rb");
       if (f == NULL) {
               printf("ERROR:Can't open file %s!\n", filename);
               exit(0);
       }
       else
               printf("Reading file %s...\n", filename);

       unsigned short size = 0;
       fseek(f, 3, SEEK_SET);
       fread(&size, 2, 1, f);
       byte *data = malloc(size);
	   printf("source =%x\n",data);

       fseek(f, 0, SEEK_SET);
       fread(data, size, 1, f);

       fclose(f);
       return data;
}

int getbit(int *q, byte** src)
{
	int b;
	printf("q1=%d ",*q);
       if (*q == 1) {
				
               *q = 0x10000 | (**((word **)src));
               *src += 2;
			   printf("q2=%d ",*q);
       }
       if (*q & 1) {			// q impair
               *q >>= 1;
               return 1;
       }
       else {				// q paire
               *q >>= 1;
               return 0;
       }
}

char unpack2(byte *src, byte *dst)
{
		
       if (((src[0] + src[1] + src[2] + src[3] + src[4] + src[5]) & 0xff) != 171) return 0;
       int q = 1;

       src += 6;

       while (1) {	
				if (getbit(&q, &src)) {

					*dst++ = *src++;
				}
               else {
                       int count;
                       int offset;

                       if (getbit(&q, &src)) {
	
                               count = *src & 7;
					
                               offset = 0xffffe000 | ((*(word *) src) >> 3);

                               src += 2;

                               if (!count) count = *src++;
                               

                               if (!count)     return 1;
                       }
                       else {
                               count = getbit(&q, &src) << 1;
                               count |= getbit(&q, &src);

                               offset = 0xffffff00 | *src++;
                       }

                       count += 2;

                       byte *dm = dst + offset;

					   while (count--) {
						   *dst++ = *dm++;
					   }
               }
       }
}


void writefile(const char *filename, byte *data, unsigned short size)
{
	int i;
	char nom[256];
	strcpy(nom,filename);
	for(i=0;i<255;i++){
		if(nom[i]=='.'){
			nom[i+1]='_';
			nom[i+2]='_';
			nom[i+3]='_';
			break;
		}
	}
       FILE *f = fopen(nom, "wb");

       if (f == NULL) {
               printf("ERROR:Can't create file %s!\n", filename);
               exit(0);
       }
       else
               printf("Writing file %s...\n", filename);

       fwrite(data, size, 1, f);

       fclose(f);
}

int main(int argc, char* argv[])
{
       printf("UnHSQ Copyright (C) Roman Dolejsi\n\n");

       if (argc != 2) {
               printf("usage: unhsq <input file.hsq>\n");
               return 1;
       }

       byte *src = loadfile(argv[1]);
		ori_src = src;

       unsigned short size = *((short *) src) ;

       byte *dst = malloc(size);
		ori_dst = dst;


       printf("Unpacking...\n");

       if (unpack2(src, dst)) 
	   {

               writefile(argv[1], ori_dst, size);
               printf("done\n");
       }
       else printf("failed\n");

       free(ori_src);
       free(ori_dst);

       return 0;
}

