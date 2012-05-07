/*  -----------------------------------------------
 *  © 2012 Created by Swyter <swyterzone@gmail.com>
 *  Licensed under MIT-like terms <http://opensource.org/licenses/mit-license.php>
 
 *  Uses stb_image_write, by Sean Barrett and contributors. :)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scoUtils/sco.h"

#define STBI_NO_HDR
#define STBI_NO_FAILURE_STRINGS
#include "stb_image.c"

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int file_exist(const char *fname)
{
    FILE *file;
    if (file = fopen(fname, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
	printf("%d",argc);

	if(argc == 1)
	{
		printf("\n | PNG Heightmap -> SceneOBJ  -- By Swyter\n | Based on the work of Disgrntld and cmpxchg8b.\n\n");
		printf( ">>   Usage: %s <input SceneOBJ> <input PNG, PSD, TGA, BMP, JPG or GIF filename> <Optional SCO output>\n     A backup is recommended... Both input files need to have the very same dimensions, otherwise there'll be distorsions\n", argv[0]);
		system("pause");
		return EXIT_FAILURE;
	
	//! Doing some logical assumptions, avoiding hassles.
    }else if(argc == 2 || argc == 3){
		//! the output is the input, same file
		
		static char * sco = "ed.sco";
		
		argv[3] = malloc(strlen(argv[1])+strlen(sco)-3);
		strcpy(argv[3], argv[1]);
		argv[3][strlen(argv[3])-3] = 0;
		strcat(argv[3], sco);
		
		printf("Assuming that the output is the input...\n<%s>\n\n",argv[3]);
	}
	if(argc == 2){
		//! we use the most probable image, the one exported.
		 static char * ext = ".png";
        
        argv[2] = malloc(strlen(argv[1]) + strlen(ext));
        strcpy(argv[2], argv[1]);
        strcat(argv[2], ext);
		
		if( file_exist(argv[2]) ) {
			// file exists
			printf("Okay, I've found a previously exported heightmap.\n<%s>\nI suppose that you want me to use that. :)\n\n",argv[2]);
		} else {
			// file doesn't exist
			printf("Please provide a heightmap file at least.\nWe aren't that smart. :(\n");
			system("pause");
			return EXIT_FAILURE;
		}
	 }
	char *imgext = strupr( (char *)get_filename_ext(argv[2]) );
	
	// Only *.sco files...
	if( strcmp( strupr( (char *)get_filename_ext(argv[1]) ) ,"SCO")!=0
	
	// ...and compatible image formats...
	|| (strcmp(imgext, "PNG")!=0 && strcmp(imgext, "PSD")!=0 && strcmp(imgext, "TGA")!=0 &&
	    strcmp(imgext, "BMP")!=0 && strcmp(imgext, "JPG")!=0 && strcmp(imgext, "GIF")!=0)){
		
		printf("\n | This only works with Mount&Blade SceneObj files and heightmap images... :)\n\n");
		system("pause");
		return EXIT_FAILURE;
	}

	FILE *in  = fopen(argv[1], "rb");
	char *img = argv[2];
	FILE *out = fopen(argv[3], "wb");

	if(!in || !out)
	{
		printf("ERROR: file %s or %s not found\n", argv[1], argv[2]);
		return EXIT_FAILURE;
	}

	printf("Reading %s\n", argv[1]);
	sco_file_t sco_file;
	
	read_sco_file( in, &sco_file);
		   
   if( save_heightmap_as_sco(img, &sco_file) ){
   
		write_sco_file(out, &sco_file);
		
		printf(" | Reimporting completed, take a look in-game...\n\n");
		return EXIT_SUCCESS;

	}else{
		// If doesn't worked as expected then don't break the scene heightfield...
		printf(" | Failed miserably.");
		return EXIT_FAILURE;
	}
}

int save_heightmap_as_sco(char *img, sco_file_t *src)
{

    int x, y, comp;
    unsigned char *foo;
    
	//Load the input heightmap...
    foo = stbi_load(img, &x, &y, &comp, 0);

	printf("\n |   Loading heightmap: %s...\n", img);
	printf(  " |    Heightmap Size X: %d\n",   x);
	printf(  " |    Heightmap Size Y: %d\n\n", y);
	printf(  " |  Heightmap channels: %d\n\n", comp);
	
	printf(  " |        Scene Size X: %d\n",   src->ground_paint->size_x);
	printf(  " |        Scene Size Y: %d\n\n", src->ground_paint->size_y);
	
	
	if(src->ground_paint->size_x != x && src->ground_paint->size_y != y){
		printf(">>  Scene and heightmap sizes aren't exactly matching... :(\n");
		return 0;
	}

	//unsigned char *foo = malloc( x * y * sizeof(char) * 3 );
	//   const char *ext = strupr( (char *)get_filename_ext(out) );
	
	
	
/* just an easy diagram for quick understanding of the structure...
   surely more readable than the original implementation.
	--swy
	
	
	->PAINT MAGIC
	->NUM_LAYERS
	->SIZ_X
	->SIZ_Y
	_ (per layer)
	 |  
	 | ->SPEC_NO
	 | ->SPEC_ID (STR)
	 V 
	   ->CONTINUITY_COUNT (SIZX * SIZY)
	 |  
	   _ (per X of every Y)
	 |  |
		| ->CONTINUITY_COUNT (X * SIZY + Y)
	 |	| ->CELLS (X * SIZY + Y)
		V
	 |   
*/
	int count = 0;
	int gol;
	for(gol = 0; gol < src->ground_paint->num_layers; ++gol)
	{
		printf("\n\nlayer %d\n spec_id[%s] continuity_count[%d]\n",gol,src->ground_paint->layers[gol].ground_spec_id,src->ground_paint->layers[gol].continuity_count);
		if( src->ground_paint->layers[gol].ground_spec_no == GROUND_PAINT_ELEVATION_MAGIC/*GROUND_PAINT_LEVELING_MAGIC*/  && src->ground_paint->layers[gol].cells){
		if( src->ground_paint->layers[gol].continuity_count > 0){

		if(comp>1){
			//we just want our very own first monochrome channel, so get rid of the rest!
			int channelcount=0;
			int everything=y*x;
			int i;
			for(i = 0; i < everything; ++i)
			{
				foo[i]=foo[channelcount];
				channelcount+=comp;
			}
		}
		
		
			//int everything=y*x;
			//int channelcount=0;
			//for(count = 0; count < everything; ++count)
			//{
				int gox; //--> I got this one bad the first time, first goes the horizontal seek (x) then jumps to the next line (y), silly transcription error that kept me busy.
				for(gox = 0; gox < x; ++gox)
				{
					int goy;
					for(goy = 0; goy < y; ++goy)
					{

						// If both heights are close each other we choose the more precise one.
						// As probably there are just minor edits done from the original.
						//char *tmp=malloc(4);
						//tmp=(float)src->ground_paint->layers[gol].cells[gox * y + goy];
					//printf("%d/%d,",(float)src->ground_paint->layers[gol].cells[count],(unsigned char)foo[goy * x + gox]);
					if((float)src->ground_paint->layers[gol].cells[count]!=foo[goy * x + gox])
					
							//src->ground_paint->layers[gol].cells[gox * y + goy]=(char)foo[count] - 16.0f; //! I think that leaving that margin for negative terrain is enough, there's going to be a precission lost anyway.
							//gox * y + goy
							src->ground_paint->layers[gol].cells[count]=(unsigned char)foo[goy * x + gox]-16.0f;
							//src->ground_paint->layers[gol].cells[count]=(unsigned char)count;
							//src->ground_paint->layers[gol].cells[count]=(unsigned char)foo[channelcount];// - 16.0f; //! I think that leaving that margin for negative terrain is enough, there's going to be a precission lost anyway.
							////printf("%d,",(unsigned char)foo[gox * y + goy]);
							//if(count%x==0){
							//	printf("\n");
							//}
							
							//channelcount+=comp;
							count++;
					//}else{
							//keep it like that...
					//}
						
						//count++;
					}
					//printf("\n");
				}
			//}else{
			//	break;
			//}
			//}
		}
		}
	}
	if(count==0){
		return 0;
	}else{
		return 1;
	}
}