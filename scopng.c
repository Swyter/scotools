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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		//get the app name
		char *exec = strrchr(argv[0], '\\');
		if(!exec) exec=argv[0]; else exec+=1;
		
		printf("\n | SceneOBJ -> PNG Heightmap -- By Swyter\n | Based on the work of Disgrntld and cmpxchg8b.\n\n");
		printf( ">>   Usage: %s <input SCO filename> <optional output filename in PNG, TGA or BMP>\n\n", exec);
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
	
	
	
	if( strcmp( strupr( (char *)get_filename_ext(argv[1]) ) ,"SCO")!=0){
		printf("\n | This only works with Mount&Blade SceneObj files... :)\n\n");
		system("pause");
		return EXIT_FAILURE;
	}

	FILE *in  = fopen(argv[1], "rb");
	char *out = argv[2];

	if(!in || !out)
	{
		printf("ERROR: file %s or %s not found\n", argv[1], argv[2]);
		return EXIT_FAILURE;
	}

	printf("Reading %s\n", argv[1]);
	sco_file_t sco_file;
	
	               read_sco_file( in, &sco_file);
	return save_heightmap_as_png(out, &sco_file);
}

int save_heightmap_as_png(char *out, sco_file_t *src)
{
	printf("\n | Saving heightmap as: %s...\n", out);
	printf(  " |    Number of layers: %d\n",   src->ground_paint->num_layers);
	printf(  " |              Size X: %d\n",   src->ground_paint->size_x);
	printf(  " |              Size Y: %d\n\n", src->ground_paint->size_y);
	
	//printf("test: %s\n", src->ground_paint->layers[1].cells[660]);
	//unsigned char foo[512*512*3]={255,192,1,0,0,0,255,255,255};
	
	int x = src->ground_paint->size_x;
	int y = src->ground_paint->size_y; /* terrible hack of death!  >:( */
	int img;
	unsigned char *foo = malloc( x * y * sizeof(char) * 3 );
	   const char *ext = strupr( (char *)get_filename_ext(out) );
	
	
	
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
	for(gol = 0; gol <= src->ground_paint->num_layers; ++gol)
	{
		printf("\n\nlayer %d\n spec_id[%s] continuity_count[%d]\n",gol,src->ground_paint->layers[gol].ground_spec_id,src->ground_paint->layers[gol].continuity_count);
		if( src->ground_paint->layers[gol].ground_spec_no == GROUND_PAINT_ELEVATION_MAGIC/*GROUND_PAINT_LEVELING_MAGIC*/  && src->ground_paint->layers[gol].cells){
		if( src->ground_paint->layers[gol].continuity_count > 0){
			
			int goy; //--> I got this one bad the first time, first goes the horizontal seek (x) then jumps to the next line (y), silly transcription error that kept me busy.
			for(goy = 0; goy < x; ++goy)
			{
				int gox;
				for(gox = 0; gox < x; ++gox)
				{
					//if(count > *src->ground_paint->layers[gol].continuity_count){
					//	break; //avoid getting in trouble, probably unneeded
					//}
					////printf("%f,", src->ground_paint->layers[gol].cells[gox * y + goy] /** 255.0f*/ );//[gox*goy+y]);
					// 16 to 8 bits // precission downgrade :(
					////foo[count]=(float)src->ground_paint->layers[gol].cells[gox * goy + x] + 16.0f;
					foo[count]=(float)src->ground_paint->layers[gol].cells[gox * y + goy] + 16.0f; //! I think that leaving that margin for negative terrain is enough, there's going to be a precission lost anyway.
					//printf("%d,",gox * y + goy);
					
					/*             	 gox * y + goy
					1 2 3 4 5 6 7 8 -> 8 * 8 + 1  ¿?
					2 3 4 5 6 7 8 1
					3 4 5 6 7 8 1 2 Later edit: Okay, I just got this one wrong
					4 5 6 7 8 1 2 3             I started by itinerating per column
					5 6 7 8 1 2 3 4             instead of per row.
					6 7 8 1 2 3 4 5
					7 8 1 2 3 4 5 6             That was the mistake with the diagonal garbling.
					8 1 2 3 4 5 6 7             Now it's fine.
					*/
					
					count++;
				}
				//printf("\n");
			}
		}else{
			break;
		}
		}
	}
	//system("pause");
	if(count==0){
		return 1;
	}
	
	printf(" | Output format is %s\n\n", ext);
	
	if      ( strcmp(ext,"TGA")==0 ){
		img = stbi_write_tga(out, x, y, 1, foo);
	}
	else if ( strcmp(ext,"BMP")==0 ){
		img = stbi_write_bmp(out, x, y, 1, foo);
	}
	else{
		img = stbi_write_png(out, x, y, 1, foo, 0);
	}
	
	//sprintf(cVal,"%f",fVal);
	
	if(img!=0){
		printf(" | >> Done! :)\n");
		return EXIT_SUCCESS;
	}else{
		printf(" | >> Meh, doesn't works! :(\n");
		return EXIT_FAILURE;
	}
}