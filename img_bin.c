
/**
 *  @file   img_bin.c
 *  @author Damian Smela <damian.a.smela@gmail.com>
 *  @date   22.02.2019
 *  @brief  BMP image binarization program
 */

/********************************* INCLUDES ***********************************/

#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include "bmp_utils.h"

/********************************* DEFINES ************************************/

#define R_WEIGHT    (0.2989f)
#define G_WEIGHT    (0.5870f)
#define B_WEIGHT    (0.1140f)

/***************************** STATIC FUNCTIONS *******************************/

/* Converts ASCII char to corresponding number */
static int ascii_to_num(char ascii_char)
{
    return ((int) ascii_char) - 48;
}


/* Checks if given string is a integer number */
static bool is_number(char num_str[])
{
    char *str_ptr = &num_str[0];

    while(*str_ptr != '\0')
    {
     	if (ascii_to_num(*str_ptr) < 0 || ascii_to_num(*str_ptr) > 9)
        {
            return false;
        }
	    str_ptr++;
    }
    return true;
}

/* Colorimetric (perceptual luminance-preserving) conversion to grayscale
 *
 * Matlab / GIMP formula:
 * Y = 0.2989 * R + 0.5870 * G + 0.1140 * B 
 * 
 */
static uint8_t conv_rgb2grey(bmp_pixel_t *pixel)
{
    double grey = (R_WEIGHT * (double) pixel->red) +
                  (G_WEIGHT * (double) pixel->green) +
                  (B_WEIGHT * (double) pixel->blue);
    
    return (uint8_t) grey;
}

/* Image binarization */
static void bmp_binarization(bmp_file_handle_t *bmp_handle, uint8_t treshold)
{
    assert(bmp_handle);

    size_t  num_of_pixels = bmp_handle->info.img_width * 
                            bmp_handle->info.img_height;

    for (int i = 0; i < num_of_pixels; i++)
    {
        if (conv_rgb2grey(&bmp_handle->data[i]) < treshold)
        {
            bmp_handle->data[i].blue = 0;
            bmp_handle->data[i].green = 0;
            bmp_handle->data[i].red = 0;
        }
        else
        {
            bmp_handle->data[i].blue = 255;
            bmp_handle->data[i].green = 255;
            bmp_handle->data[i].red = 255;
        }
        
    }
}

/*********************************** MAIN *************************************/

int main(int argc, char **argv)
{
    uint32_t            tresh_tmp;
    uint8_t             treshold;
    bmp_file_handle_t   bmp_handle;

    /* Check if there is exactly two arguments given */
    if (argc != 3)
    {
     	printf("Program needs exactly two arguments - "
               "filename of the BMP file & binarization treshold!\n");
        return 1;
    }

    /* Load the filename */
    memset(&bmp_handle, 0, sizeof(bmp_file_handle_t));
    bmp_handle.filename = argv[1];

    /* Check if the second parameer is a <0,255> int number */
    if (!is_number(argv[2]))
    {
     	printf("Second paramater must be a int in range <0,255> \n");
        return 2;
    }

    /* Store the treshold value */
    tresh_tmp = atoi(argv[2]);
    if (tresh_tmp > 255)
    {
        printf("Second paramater must be a int in range <0,255> \n");
        return 3;
    }

    treshold = tresh_tmp;

    /* Open the BMP file of given name */
    printf("Opening %s...\n", bmp_handle.filename);
    if (false == bmp_open(&bmp_handle))
    {
        return 4;
    }

    /* Verify the BMP format */
    printf("Verifying the file...\n");
    if (false == bmp_verify(&bmp_handle))
    {
        return 5;
    }

    /* Load the image to memory */
    printf("Loading image...\n");
    if (false == bmp_load_img(&bmp_handle))
    {
        return 6;
    }

    /* Binarize the image */
    printf("Performing image binarization (treshold=%u)...\n", treshold);
    bmp_binarization(&bmp_handle, treshold);

    /* Save the BMP file */
    printf("Saving BMP file...\n");
    if (false == bmp_write_img(&bmp_handle))
    {
        return 7;
    }

    /* Cleanup */
    bmp_cleanup(&bmp_handle);

    printf("Done.\n");
    return 0;
}

/************************************ EOF *************************************/