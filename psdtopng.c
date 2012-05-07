/*  -----------------------------------------------
 *  © 2012 Created by Swyter <swyterzone@gmail.com>
 *  Licensed under MIT-like terms <http://opensource.org/licenses/mit-license.php>
 
 *  Uses stb_image, by Sean Barrett and contributors. :)
 */


#include <stdio.h>
#include <stdlib.h>

//#include "../scoUtils/sco.h"

#define STBI_NO_HDR
#define STBI_NO_FAILURE_STRINGS
#include "stb_image.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("PSD, GIF, JPG, TGA, PIC -> PNG -- By Swyter\n");
        printf("Usage: %s <input filename> <optional output filename>\n", argv[0]);
        system("pause");
        return EXIT_FAILURE;
    }
    else if(argc == 2)
    {
        static char * ext = ".png";
        
        argv[2] = malloc(strlen(argv[1]) + strlen(ext));
        strcpy(argv[2], argv[1]);
        strcat(argv[2], ext);
    }
    //printf("%s\n%s\n%s\n",argv[0],argv[1],argv[2]);
    printf("%s -> %s\n",argv[1],argv[2]);
    
    int x, y, comp;
    unsigned char *foo;
    
        foo =      stbi_load(argv[1], &x, &y, &comp, 0);
    int img = stbi_write_png(argv[2],  x,  y,  comp, foo, 0); 
    
    //sprintf(cVal,"%f",fVal); 
    
    if(img!=0){
        printf("[!] image creation successful\n");
    }else{
        printf("[!] meh, doesn't works!\n");
    }
    
    stbi_image_free(foo);
    
    return img;
}