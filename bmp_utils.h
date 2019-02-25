/**
 *  @file   bmp_utils.h
 *  @author Damian Smela <damian.a.smela@gmail.com>
 *  @date   22.02.2019
 *  @brief  BMP image file utilities - header file
 */

/********************************* INCLUDES ***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/********************************* DEFINES ************************************/

/* BMP HEADER */
#define BMP_HEADER_SIZE             (14U)

/* BMP HEADER OFFSETS*/
#define BMP_HEADER_MAGIC_ID_OFF     (0U)
#define BMP_HEADER_FILE_SIZE_OFF    (2U)
#define BMP_HEADER_DATA_OFFSET_OFF  (10U)

#define BMP_MAGIC_ID                (0x4D42)

/* BMP INFO HEADER OFFSETS*/
#define BMP_INFO_HEADER_SIZE_OFF    (0U)
#define BMP_INFO_IMG_WIDTH_OFF      (4U)
#define BMP_INFO_IMG_HEIGHT_OFF     (8U)
#define BMP_INFO_PLANES_OFF         (12U)
#define BMP_INFO_BIT_CNT_OFF        (14U)
#define BMP_INFO_COMPRESSION_OFF    (16U)
#define BMP_INFO_IMG_SIZE_OFF       (20U)
#define BMP_INFO_XRES_OFF           (24U)
#define BMP_INFO_YRES_OFF           (28U)
#define BMP_INFO_COLOR_NUM_OFF      (32U)
#define BMP_INFO_IMP_COLOR_NUM_OFF  (36U)

/********************************* TYPEDEFS ***********************************/

/* Header of BMP file */
typedef struct
{
    uint16_t        magic_id;              /* 'BM' in char or 0x4D42 in hex */
    uint32_t        file_size;             /* File size in bytes */
    uint32_t        reserved;              /* Reserved (unused, = 0) */
    uint32_t        data_offset;           /* Offset to image data */
} bmp_header_t;


/* Info header of BMP file */
typedef struct
{
    uint32_t        info_size;             /* Info size in bytes (= 40) */
    uint32_t        img_width;             /* Bitmap width */
    uint32_t        img_height;            /* Bitmap height */
    uint16_t        planes;                /* Number of color planes (= 1) */
    uint16_t        bit_cnt;               /* Bits per pixel */
    uint32_t        compression;           /* Compression type */
    uint32_t        img_size;              /* Image size in bytes */
    uint32_t        x_res;                 /* Horizontal resolution (pixel / meter) */
    uint32_t        y_res;                 /* Vertical resolution */
    uint32_t        colors_num;            /* Number of actually used colors */
    uint32_t        imp_colors_num;        /* Number of important colors (0 = all) */
} bmp_info_t;


/* Color table (not used yet) */
typedef struct 
{
    uint8_t         red;                    /* Red intensity */
    uint8_t         green;                  /* Green intensity */
    uint8_t         blue;                   /* Blue intensity */
    uint8_t         reserved;               /* Reserved (unused, = 0) */
} bmp_color_table_t;


/* Pixel struct */
typedef struct
{
    uint8_t         red;
    uint8_t         green;
    uint8_t         blue;
} bmp_pixel_t;


/* BMP file handle */
typedef struct
{
    FILE            *file_ptr;              /* File pointer */
    char            *filename;              /* File name */
    bmp_header_t    header;                 /* BMP header structure */
    bmp_info_t      info;                   /* BMP info structure */
    bmp_pixel_t     *data;                  /* Pointer to all the img's pixels */
} bmp_file_handle_t;

/***************************** PUBLIC FUNCTIONS *******************************/

bool bmp_open(bmp_file_handle_t *bmp_header);
bool bmp_verify(bmp_file_handle_t *bmp_handle);
bool bmp_load_img(bmp_file_handle_t *bmp_handle);
void bmp_cleanup(bmp_file_handle_t *bmp_handle);
bool bmp_write_img(bmp_file_handle_t *bmp_handle);

/************************************ EOF *************************************/