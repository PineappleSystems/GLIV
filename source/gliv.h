#ifndef _GLIV_H_
#define _GLIV_H_

#include <stdint.h>

// maximum screen sizes for static memory allocation
#define GLIV_MAX_WIDTH 128
#define GLIV_MAX_HEIGHT 64

/**
 * \brief Align
 */
typedef enum gliv_align_e
{
    GLIV_ALIGN_LEFT = 0,
    GLIV_ALIGN_CENTER,
    GLIV_ALIGN_RIGHT
} gliv_align_t;

/**
 * \brief Image type
 */
typedef struct gliv_image_s
{
    const unsigned int* data; // pointer to a bit field with image data
    const uint8_t width;
    const uint8_t height;
} gliv_image_t;

/**
 * \brief Font type
 */
typedef struct gliv_font_s
{
    const unsigned int** data;    // pointer to an array of font chars
    const uint8_t* widths;        // pointer to character widths
    const uint8_t height;
} gliv_font_t;

/**
 * \brief Main library structure
 */
typedef struct gliv_s
{
    unsigned int buffer[GLIV_MAX_WIDTH * GLIV_MAX_HEIGHT / sizeof(unsigned int)];
    uint8_t width;
    uint8_t height;
} gliv_t;

/**
 * \brief Library initialization
 * \param inst Pointer to the library object
 * \param width Width
 * \param height Height
 */
void gliv_init(gliv_t* inst, uint8_t width, uint8_t height);

/**
 * \brief Fill the entire display
 * \param inst Pointer to the library object
 * \param color 0 - black, otherwise white
 */
void gliv_fill(gliv_t* inst, uint8_t color);

void gliv_set_pixel(gliv_t* inst, uint8_t x, uint8_t y, uint8_t color);

uint8_t gliv_get_pixel(gliv_t* inst, uint8_t x, uint8_t y);

void gliv_draw_line(gliv_t* inst, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);

void gliv_draw_filled_rectangle(gliv_t* inst, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

void gliv_draw_image(gliv_t* inst, uint8_t x, uint8_t y, const gliv_image_t* const image);

void gliv_draw_uint(gliv_t* inst, uint8_t x, uint8_t y, const gliv_font_t* const font, uint8_t width, gliv_align_t align, unsigned int value);

#endif /* _GLIV_H_ */
