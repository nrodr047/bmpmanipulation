/*
Student Name: Natalie Rodriguez
Panther ID: 2919853

Program description: Program manipulates a bmp image from choices on the commandline.
User can rotate, flip horizontally/vertically, and enlarge an image and manually assign
the image to a new output file.

Affirmation of Originality: I affirm that I completed this project by myself and without help from outside resources.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "bmplib.h"


//message error
//to indicate usage
void usage()
{
	fprintf(stderr, "usage: bmptool [-s scale | -r degree | -f] [-o output_file] [input_file]\n");
	exit(1);
}


//method horizontally flips a 24-bit, uncompressed bmp file
int flip (PIXEL *original, PIXEL **new, int rows, int cols)
{
	int row, col;

	if((rows <=0) || (cols <= 0)) return -1; //if there are no rows or columns

	//allocates memory for new PIXEL
	*new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

	//for loop scrolls through first row
	//when pixels in row are changed, for loop continues next column
	for(row = 0; row < rows; row++)
		for(col=0; col < cols; col++)
		{
			PIXEL* o = original + row*cols + col;  
			PIXEL* n = (*new) + row*cols + (cols-1-col); //get pointer to pixel
			*n = *o; //copy pixel from original to flipped
		}
	return 0;
}

//method flips bmp image vertically
int verticleflip(PIXEL *original, PIXEL **new, int rows, int cols)
{
	int row, col;	
	*new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));
	
	//each row of the image
	for(row = 0; row < rows; row++)
	{
		//each byte on a row
		for(col = 0; col < cols; col++)
		{
			PIXEL* o = original + row*cols + col;
			PIXEL* n = (*new) + (rows-1-row)*cols + col;
			*n = *o;
		}
	}
	return 0;
}


//function takes original pixels and their rows and columns
//enlarges the originals by the given scale to create new pixels 
int enlarge(PIXEL* original, int rows, int cols, int scale, PIXEL** new, int* newrows, int* newcols)
{
	int row, col;

	*newrows = scale * rows; //multiplied to scale for the rows
	*newcols = scale * cols; //multiplied to scale for the columns
	*new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL)); //array containing enlarged PIXELS with allocated memory
	
	//error message for memory allocation	
	if(!*new)
	{
		free(*new);
		printf("Error: Failed allocating memory.\n");
		exit(1);
	}
	


	//for loops scale a single row at a time
	for(row = 0; row < (*newrows); row++)
	{
		for(col = 0; col <(*newcols); col++)
		{
			(*new)[row * (*newcols) + col] = original[(row * rows / (*newrows)) * cols + col * cols / (*newcols)]; 
		}
	}
	return 0;
}


//Method rotates 24-bit, uncompressed bmp file.
int rotate(PIXEL* original, int rows, int cols, int rotation, PIXEL** new, int* newrows, int* newcols)
{
	int row, col;


	*new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));


	if(!*new)
	{
		free(*new);
		printf("Error: Failed allocating memory");
		exit(1);
	}
	//condition if rotation is +360 or -360  degrees
	switch(rotation)
	{
		case 360:
		case -360:
			*newrows = rows;
			*newcols = cols;
			//for loop rotates each column in the row
			//before continuing to the next row		
			for(row = 0; row < rows; row++)
			{
				for(col = 0; col < cols; col++)
				{
					PIXEL* o = original + row * cols + col;
					PIXEL* n = (*new) + row * cols + col;
					*n = *o;
				}
			}
			break;
		case 270:
		case -90:
			*newcols = rows;
			*newrows = cols;
			//angle +270 and -90 are visually the same
			//condition after or checks for the negative in 90 degrees
			//condition if rotation is +270 or -90  degrees
			for(row = 0; row < rows; row++)
			{
				for(col = 0; col < cols; col++)
				{
					PIXEL* o = original + row * cols + col;
					PIXEL* n = (*new) +(cols-col-1)*rows + row;
					*n = *o;
				};

			}
			break;
		case 90:
		case -270:
			*newcols = rows;
			*newrows = cols;
			//angle +90 and -270 are visually the same
			//same concept as previous if statement but reversed
			for(row = 0; row < rows; row++)
			{
				for(col = 0; col < cols; col++)
				{
					PIXEL* o = original + row * cols + col;
					PIXEL* n = (*new)+  col * rows + (rows-row-1);
					*n = *o;
				}
			}
			break;
		case 180:
		case -180:
			*newcols = cols;
			*newrows = rows;
			//condition for angels +180 and -180
			for(row = 0; row < rows; row++)
			{
				for(col = 0; col < cols; col++)
				{
					PIXEL *o = original + row*cols + col;
					PIXEL *n = (*new) + (rows - row) * cols - (col + 1);
					*n = *o;
				}
			}
			break;
		default:
			//error message if rotation is not a valid integer.
			printf("Error: rotation input is not valid.\n");
			printf("Integer must be a multiple of 90 between -360 and +360.\n");	
			exit(1);
	}
	return 0;
}


//Main

int main(int argc, char *argv[])
{
	extern char *optarg; //user's value for option
	extern int optind; //number of arguments needed
	PIXEL *b, *nb; //orginal pixel and new pixel
	int command; //stores user getopt
	int scale; //stores scale value
	int r_val; //stores degree rotation value
	char *inputFile;
	inputFile = NULL;
	char *outputFile;
	outputFile = NULL;
	int newrows;
	int row;
	int column;
	int newcolumns;
	int s_flag = 0, r_flag = 0, o_flag = 0, f_flag = 0, v_flag = 0; //option flags
	
	//parse  user's getopt choice from the commandline
	//if input is invalid, output an error message


	if(argc < 1)
	{
		printf("Error: Too few arguments.");
		exit(1);
	}

	while((command = getopt(argc, argv, "s:r:fvo:")) != -1)
	{
		//finds user option
		//sets flag if found
		//else, outputs error
		switch(command)
		{
			case 's':
				if(s_flag)
				{
					printf("Error: duplicate 's' found\n");
					usage();
				}
				s_flag = 1;
				scale =atoi(optarg);
				break;
			case 'r':
				if(r_flag)
				{
					printf("Error: duplicate 'r' found\n");
					usage();
				}
				r_val = atoi(optarg);
				break;
			case 'f':
				if(f_flag)
				{
					printf("Error: duplicate 'f' found.\n");
					usage();
				}
				f_flag = 1;
				break;
			case 'v':
				if(v_flag)
				{
					printf("Error: duplicate 'v' found.\n");
					usage();
				}
				v_flag = 1;
				break;
			case 'o':
				if(o_flag)
				{
					printf("Error: duplicate 'o' found\n");
					usage();
				}
				o_flag = 1;
				outputFile = optarg;
				break;
			case '?':
				printf("Error: unkown argument.\n");
				usage();
				break;
		} //switch statement
	}// while loop.

	
	//checks for input file name
	if(optind < argc)
	{
		inputFile = argv[optind];
	}

	//reads inputfile
	//passes the flag, command choice, and pixel allocated
	readFile(inputFile, &row, &column, &b);

	//enlarge
	if(s_flag)
	{
		enlarge(b, row, column, scale, &nb,&newrows, &newcolumns);
		row = newrows;
		column = newcolumns;
	}
	
	//rotate
	if(r_flag)
	{
		//enlarge and rotate
		if(s_flag)
		{
			b = nb; //updates image
		}
		rotate(b, row, column, r_val, &nb, &newrows, &newcolumns);
		row = newrows;
		column = newcolumns;
	}

	//flip
	if(f_flag)
	{
		//enlarge or rotate
		if(s_flag || r_flag)
		{
			b = nb;
		}
		flip(b, &nb, row, column);
	}

	//verticle flip
	if(v_flag)
	{
		if(s_flag || r_flag)
		{
			b = nb;
		}
		verticleflip(b, &nb, row, column);
	}


	//write file
	writeFile(outputFile,row,column,nb);

	//frees allocated memory
	free(b);
	free(nb);

	return 0;
}

