#include <allegro.h>
#include <stdio.h>

RGB ma_palette[256];
FILE* unp_fd;
unsigned short offset_A;
unsigned short offset_B[256];
unsigned short offset_C;
unsigned char sel_offset_B;
unsigned short offset_courant;
unsigned char debut_mes_couleur;
unsigned char nb_mes_couleur;

unsigned int size_x;
unsigned char size_x_read;
unsigned int size_y;
unsigned char compression;
unsigned char offset_pal;
char repetition;
unsigned char bipixel;
unsigned char bipixel2;
unsigned char inconnu1;
unsigned char inconnu2;


unsigned int word;

BITMAP* the_image;
int ligne;
int colonne;
char flag_do_transp;

int main(int argc, char* argv[]) {
    int i,j;
    char nom_fichier_out[200];
    
    flag_do_transp = 0;
    
	allegro_init();
	set_color_depth(8);
	if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) != 0) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
      return 1;
   }
   
    if ((argc != 2)&&(argc != 3)) return 1;
    if ((argc == 3) && (strcmp(argv[2],"-transp"))){
       allegro_message("Argument invalide\n");
       return 1;       
    }
    if ((argc == 3) && (!strcmp(argv[2],"-transp"))){
       flag_do_transp = 1;      
    }
    
    offset_courant=0;
    sel_offset_B=0;
    
    unp_fd =fopen(argv[1],"rb");
    if (!unp_fd){
       allegro_message("Impossible d'ouvrir le fichier\n");
       return 1;
    }
    //allegro_message("Lecture offset_A\n");
    fread(&offset_A,2,1,unp_fd); offset_courant+=2;
    
    //allegro_message("lecture couleurs\n");
    if (offset_A != 2){
        while (1) {
            fread(&debut_mes_couleur,1,1,unp_fd); 
            fread(&nb_mes_couleur,1,1,unp_fd);
            offset_courant+=2;
            
            if( (debut_mes_couleur==0xFF) && (nb_mes_couleur==0xFF)) break;
        
            for(i=0;i<nb_mes_couleur;i++){
               fread(&(ma_palette[debut_mes_couleur+i].r),1,1,unp_fd); ma_palette[debut_mes_couleur+i].r <<2;
               fread(&(ma_palette[debut_mes_couleur+i].g),1,1,unp_fd); ma_palette[debut_mes_couleur+i].g <<2;
               fread(&(ma_palette[debut_mes_couleur+i].b),1,1,unp_fd); ma_palette[debut_mes_couleur+i].b <<2;
               offset_courant+=3;
            }
        }
    }else{
          for(i=0;i<256;i++){
                ma_palette[i].r = i;
                ma_palette[i].g = i;
                ma_palette[i].b = i;
          }
    }
    
    if (flag_do_transp) {
       ma_palette[0].r = 255;
       ma_palette[0].g = 0;
       ma_palette[0].b = 255;
    }
    
    //offset_C = offset_courant;

    //allegro_message("lecture offsets images\n");
    fseek(unp_fd,offset_A,SEEK_SET);
    fread(&offset_B[sel_offset_B],2,1,unp_fd); 
    offset_courant+=2; 
    sel_offset_B++;
    
    while (offset_courant < offset_A + offset_B[0]) {
        fread(&offset_B[sel_offset_B],2,1,unp_fd);         
        offset_courant+=2; 
        sel_offset_B++;        
    }
    
    
    //pour chaque image
    for (i=0;i<sel_offset_B;i++){
        //allegro_message("lecture image %d\n",i);
        fseek(unp_fd,offset_B[i] + offset_A,SEEK_SET);
    
        fread(&size_x_read,1,1,unp_fd);
        size_x = size_x_read;

        fread(&compression,1,1,unp_fd);
        fread(&size_y,1,1,unp_fd);

        fread(&offset_pal,1,1,unp_fd); 
        
        size_x = size_x + ((compression & 0x7F) << 8);
        if (size_x==0) break;
        compression = compression & 0x80;
        
        if ((offset_A == 2))
        {
            fread(&inconnu1,1,1,unp_fd); 
            fread(&inconnu2,1,1,unp_fd); 
        }
        
        ligne = 0;
        //colonne = size_x -1;    
        colonne = 0;        
        
        the_image = create_bitmap(size_x,size_y);
        word=0;
        
        if (compression )
        while(1){
            fread(&repetition,1,1,unp_fd);  
                      
            //allegro_message(" %d, ligne=%d , col=%d", repetition,ligne,colonne);            
            // si compteur repetition negatif alors je reproduit x fois l'octet suivant (2pixels)
            
            if(repetition < 0) {
               fread(&bipixel,1,1,unp_fd);
               
               //fread(&bipixel2,1,1,unp_fd);
  
               
               for (j=0;j<(-repetition)+1;j++){ 
                                   
                    //if(colonne>=0)
                    if (colonne<size_x)
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel & 0x0F))?0:offset_pal) + (bipixel & 0x0F) );
                    colonne++;
                    word++;
                    //if(colonne<0) { colonne = size_x -1; ligne++; }            
       
                    //if(colonne>=0) 
                    if (colonne<size_x)           
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel>>4))?0:offset_pal) + (bipixel>>4) );  
                    colonne++;  
                    word++;                                     
                    //if(colonne<0) { colonne = size_x -1; ligne++; }
                    
                    //if(colonne>=0)
                    //putpixel(the_image,colonne,ligne, offset_pal + (bipixel2 & 0x0F));
                    //colonne--;
                    //if(colonne<0) { colonne = size_x -1; ligne++; }            
       
                    //if(colonne>=0)            
                    //putpixel(the_image,colonne,ligne, offset_pal + (bipixel2>>4));  
                    //colonne--;  
                    //if(colonne<0) { colonne = size_x -1; ligne++; }
                     
               }
               if(colonne>=size_x) { colonne = 0; ligne++;
                     //allegro_message(" word=%d, word mod 4=%d , lecture=%d", word,word%4,(word%4)?4-(word%4):0);  
                     fread(&bipixel,(word%4)?4-(word%4):0,1,unp_fd); word=0; }  
            }else {
               for (j=0;j<repetition+1;j++){ 
                    fread(&bipixel,1,1,unp_fd);
                    
                    //fread(&bipixel2,1,1,unp_fd);
                    
                    if (colonne<size_x)
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel & 0x0F))?0:offset_pal) + (bipixel & 0x0F) );
                    colonne++;
                    word++;
                    //if(colonne<0) { colonne = size_x -1; ligne++; }
                    
                    if (colonne<size_x)
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel>>4))?0:offset_pal) + (bipixel>>4) );  
                    colonne++;
                    word++;
                    //if(colonne<0) { colonne = size_x -1; ligne++; }   
                    
                    //if(colonne>=0)
                    //putpixel(the_image,colonne,ligne, offset_pal + (bipixel2 & 0x0F));
                    //colonne--;
                    //if(colonne<0) { colonne = size_x -1; ligne++; }
                    
                    //if(colonne>=0)
                    //putpixel(the_image,colonne,ligne, offset_pal + (bipixel2>>4));  
                    //colonne--;
                    //if(colonne<0) { colonne = size_x -1; ligne++; }  
                                    
               }
               if(colonne>=size_x) { colonne = 0; ligne++;
               //allegro_message(" word=%d, word mod 4=%d , lecture=%d", word,word%4,(word%4)?4-(word%4):0);  
                fread(&bipixel,(word%4)?4-(word%4):0,1,unp_fd); word=0;}  
            }
            
            if (ligne>=size_y ) break;
        }
        else
        if (!compression )
        while(1){

               fread(&bipixel,1,1,unp_fd);
               fread(&bipixel2,1,1,unp_fd);

                    if (colonne<size_x)
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel & 0x0F))?0:offset_pal) + (bipixel & 0x0F) );
                    colonne++;
                    //word++;
              //if(colonne<0) { colonne = size_x -1; ligne++; }   
    
                    if (colonne<size_x)           
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel>>4))?0:offset_pal) + (bipixel>>4) );  
                    colonne++;  
                    //word++;                                     
               //if(colonne<0) {  colonne = size_x -1; ligne++; }  
               
                    if (colonne<size_x)
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel2 & 0x0F))?0:offset_pal) + (bipixel2 & 0x0F) );
                    colonne++;
              //if(colonne<0) { colonne = size_x -1; ligne++; }   
    
                    if (colonne<size_x)            
                    putpixel(the_image,colonne,ligne, ((flag_do_transp&& !(bipixel2>>4))?0:offset_pal) + (bipixel2>>4) );  
                    colonne++;     
                                                      
               if(colonne>=size_x) {  colonne = 0; ligne++; 
               //fread(&bipixel,(word%4)?4-(word%4):0,1,unp_fd); word=0;
               } 
            
            //allegro_message(" ligne = %d , size_y=%d", ligne,size_y);
            if (ligne>=size_y ) break;
        }
        sprintf(nom_fichier_out,"%s%d.bmp",argv[1],i);
        save_bitmap(nom_fichier_out, the_image, ma_palette);
        destroy_bitmap(the_image);
    }
    

	return 0;
}
END_OF_MAIN()





