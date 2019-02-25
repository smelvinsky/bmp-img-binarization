/**
 *  @file   bmp_utils.c
 *  @author Damian Smela <damian.a.smela@gmail.com>
 *  @date   22.02.2019
 *  @brief  BMP image file utilities
 */

/********************************* INCLUDES ***********************************/

#include <assert.h>
#include "bmp_utils.h"

/***************************** STATIC FUNCTIONS *******************************/

/**
  * @brief  Prints out the BMP file info 
  * @param  bmp_handle - Handle to the BMP image
  * @retval None
  */
static void print_bmp_info(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);

    printf("File info:\n");

    printf("\tName:\t\t\t\t%s\n",               bmp_handle->filename);
    printf("\tFile size:\t\t\t%d bytes\n",      bmp_handle->header.file_size);
    printf("\tRes:\t\t\t\t%d x %d\n",           bmp_handle->info.img_width, 
                                                bmp_handle->info.img_height);
    printf("\tColor planes:\t\t\t%d\n",         bmp_handle->info.planes);
    printf("\tBits per pixel:\t\t\t%d\n",       bmp_handle->info.bit_cnt);
    printf("\tCompression:\t\t\t%d\n",          bmp_handle->info.compression);
    printf("\tImg size:\t\t\t%d bytes\n",       bmp_handle->info.img_size);
    printf("\tNum of colors:\t\t\t%d\n",        bmp_handle->info.colors_num);
    printf("\tNum of important colors:\t%d\n",  bmp_handle->info.colors_num);
}


/**
  * @brief  Check if the file is a supported 24-bit BMP.
  * @param  bmp_handle - Handle to the BMP image
  * @retval true if the format is the one supported, 
  *         otherwise returns false
  */
static bool check_format(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);

    if (bmp_handle->info.bit_cnt != 24 ||
        bmp_handle->info.planes != 1 ||
        bmp_handle->info.compression != 0 )
    {
            printf("This kind of BMP file is not supported :(\n");
            return false;
    }

    return true;
}

/***************************** PUBLIC FUNCTIONS *******************************/

/**
  * @brief  Opens the BMP file and updates the fields in handler structure
  * @param  bmp_handle - Handle to the BMP image
  * @retval true if opening file succeded, otherwise returns false
  */
bool bmp_open(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);

    /* Open a file in binary mode for read only */
    bmp_handle->file_ptr = fopen(bmp_handle->filename, "rb");
    if (bmp_handle->file_ptr == NULL)
    {
        printf("Couldn't open the \"%s\" file!\n", bmp_handle->filename);
        return false;
    }

    return true;
}


/**
  * @brief  Verifies the BMP file and updates the fields in handler structure
  * @param  bmp_handle - Handle to the BMP image
  * @retval true if BMP verification succeded, otherwise returns false
  */
bool bmp_verify(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);

    /* Allocate memory for header buffer */
    uint8_t *buff = (uint8_t *) malloc(BMP_HEADER_SIZE);

    /* Read the BMP header */
    size_t bytes_read = fread(buff, 
                              sizeof(uint8_t),
                              BMP_HEADER_SIZE,
                              bmp_handle->file_ptr);

    /* Check if data was succesfully read */
    if (bytes_read < BMP_HEADER_SIZE)
    {
        printf("Couldn't read the header of the BMP!\n");
        return false;
    }

    /* Copy all the data to header structure */
    bmp_handle->header.magic_id     = *((uint16_t *) (buff + BMP_HEADER_MAGIC_ID_OFF));
    bmp_handle->header.file_size    = *((uint32_t *) (buff + BMP_HEADER_FILE_SIZE_OFF));
    bmp_handle->header.data_offset  = *((uint32_t *) (buff + BMP_HEADER_DATA_OFFSET_OFF));

    /* Check if the file is a BMP */
    if (bmp_handle->header.magic_id != BMP_MAGIC_ID)
    {
        printf("File is not a BMP...\n");
        return false;
    }

    /* Read the size of info header */
    if (1 != fread(&bmp_handle->info.info_size,
                   sizeof(uint32_t),
                   1,
                   bmp_handle->file_ptr))
    {
        printf("Couldn't read the info header size!\n");
        return false;
    }

    /* Reallocate the memory for info header buffer */
    buff = (uint8_t *) realloc(buff, bmp_handle->info.info_size);
    if (NULL == buff)
    {
        printf("Couldn't reallocate the buffer memory!\n");
        return false;
    }

    /* Move the file pointer to the first byte of info header */
    fseek(bmp_handle->file_ptr, -4, 1);

    /* Read the BMP info */
    bytes_read = fread(buff,
                       sizeof(uint8_t),
                       bmp_handle->info.info_size,
                       bmp_handle->file_ptr);

    /* Check if data was succesfully read */
    if (bytes_read < bmp_handle->info.info_size)
    {
        printf("Couldn't read the info of the BMP!\n");
        return false;
    }

    /* Copy info data to the structure */
    bmp_handle->info.img_width      = *((uint32_t *) (buff + BMP_INFO_IMG_WIDTH_OFF));
    bmp_handle->info.img_height     = *((uint32_t *) (buff + BMP_INFO_IMG_HEIGHT_OFF));
    bmp_handle->info.planes         = *((uint16_t *) (buff + BMP_INFO_PLANES_OFF));
    bmp_handle->info.bit_cnt        = *((uint16_t *) (buff + BMP_INFO_BIT_CNT_OFF));
    bmp_handle->info.compression    = *((uint32_t *) (buff + BMP_INFO_COMPRESSION_OFF));
    bmp_handle->info.img_size       = *((uint32_t *) (buff + BMP_INFO_IMG_SIZE_OFF));
    bmp_handle->info.x_res          = *((uint32_t *) (buff + BMP_INFO_XRES_OFF));
    bmp_handle->info.y_res          = *((uint32_t *) (buff + BMP_INFO_YRES_OFF));
    bmp_handle->info.colors_num     = *((uint32_t *) (buff + BMP_INFO_COLOR_NUM_OFF));
    bmp_handle->info.imp_colors_num = *((uint32_t *) (buff + BMP_INFO_IMP_COLOR_NUM_OFF));

    /* Free the buffer */
    free(buff);

    /* Print out the image info */
    print_bmp_info(bmp_handle);

    /* Check if the BMP format is supported */
    return check_format(bmp_handle);
}


/**
  * @brief  Load the BMP data to memory
  * @param  bmp_handle - Handle to the BMP image
  * @retval true if BMP loading succeded, otherwise returns false
  */
bool bmp_load_img(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);

    uint8_t pixel[3];
    size_t num_of_pixels = bmp_handle->info.img_width * 
                           bmp_handle->info.img_height;

    /* Allocate the memory for all the pixel data */
    bmp_handle->data = malloc(sizeof(bmp_pixel_t) * num_of_pixels);

    /* Move the file pointer to the begining of img data */
    fseek(bmp_handle->file_ptr, bmp_handle->header.data_offset, 0);

    /* Load img data */
    for (int i = 0; i < num_of_pixels; i++)
    {
        /* Read the next pixel */
        if (3 != fread(pixel,
                       sizeof(uint8_t),
                       3,
                       bmp_handle->file_ptr))
        {
            printf("Couldn't read the next pixel!\n");
            return false;
        }

        /* Copy data */
        bmp_handle->data[i].blue    = pixel[0];
        bmp_handle->data[i].green   = pixel[1];
        bmp_handle->data[i].red     = pixel[2];
    }

    return true;
}


/**
  * @brief  Create and write to BMP file
  * @param  bmp_handle - Handle to the BMP image
  * @retval true if BMP write succeded, otherwise returns false
  */
bool bmp_write_img(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);

    char *out_filename = "out.bmp";
    FILE *out_file;
    size_t bytes_written;
    size_t bytes_read;
    uint8_t *buff;
    uint8_t pixel[3];
    size_t num_of_pixels = bmp_handle->info.img_width * 
                           bmp_handle->info.img_height;

    /* Open a file in binary mode for write only */
    out_file = fopen(out_filename, "wb");
    if (out_file == NULL)
    {
        printf("Couldn't open/create the \"%s\" file!\n", out_filename);
        return false;
    }

    /* Copy the header from loaded file */
    buff = malloc(bmp_handle->header.data_offset);

    fseek(bmp_handle->file_ptr, 0, 0);
    bytes_read = fread(buff,
                       sizeof(uint8_t),
                       bmp_handle->header.data_offset,
                       bmp_handle->file_ptr);

    bytes_written = fwrite(buff, 
                           sizeof(uint8_t),
                           bmp_handle->header.data_offset,
                           out_file);

    /* Check if the header was succesfully written */
    if (bytes_written != bmp_handle->header.data_offset ||
        bytes_read != bytes_written)
    {
        printf("Couldn't copy the header of the BMP!\n");
        return false;
    }

    /* Write img data */
    for (int i = 0; i < num_of_pixels; i++)
    {
        /* Copy data */
        pixel[0] = bmp_handle->data[i].blue;
        pixel[1] = bmp_handle->data[i].green;
        pixel[2] = bmp_handle->data[i].red; 

        /* Read the next pixel */
        if (3 != fwrite(pixel,
                        sizeof(uint8_t),
                        3,
                        out_file))
        {
            printf("Couldn't write the next pixel!\n");
            return false;
        }
    }

    /* Free the buffer */
    free(buff);

    return true;
}


/**
  * @brief  Free the memory 
  * @param  bmp_handle - Handle to the BMP image
  * @retval None
  */
void bmp_cleanup(bmp_file_handle_t *bmp_handle)
{
    assert(bmp_handle);
    free(bmp_handle->data);
}

/************************************ EOF *************************************/