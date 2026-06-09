#ifndef GLIV_H
#define GLIV_H

#include <stdint.h>
#include <stdbool.h>

// maximum screen sizes for static memory allocation
#define GLIV_MAX_WIDTH 128
#define GLIV_MAX_HEIGHT 64

#define GLIV_BITARRAY_WORD_BITS (8 * sizeof(uint8_t)) // Number of bits in one word

#define GLIV_LABEL_TEXT_MAX_LENGTH 128

/**
 * \brief Color
 */
typedef enum gliv_color_e
{
    GLIV_COLOR_BLACK = 0,
    GLIV_COLOR_WHITE,
} gliv_color_t;

/**
 * \brief Fill mode
 */
typedef enum gliv_fill_mode_e
{
    GLIV_FILL_NONE,      // Outline only (hollow shape)
    GLIV_FILL_SOLID,     // Solid fill with line color
    //GLIV_FILL_DITHER_50, // Dithering (50% checkerboard pattern)
    //GLIV_FILL_V_LINES,   // Vertical line hatching
    //GLIV_FILL_H_LINES    // Horizontal line hatching
} gliv_fill_mode_t;

/**
 * \brief Align
 */
typedef enum gliv_align_e
{
    GLIV_ALIGN_TOP_LEFT,
    GLIV_ALIGN_TOP_CENTER,
    GLIV_ALIGN_TOP_RIGHT,
    GLIV_ALIGN_MIDDLE_LEFT,
    GLIV_ALIGN_MIDDLE_CENTER,
    GLIV_ALIGN_MIDDLE_RIGHT,
    GLIV_ALIGN_BOTTOM_LEFT,
    GLIV_ALIGN_BOTTOM_CENTER,
    GLIV_ALIGN_BOTTOM_RIGHT,
} gliv_align_t;

/**
 * \brief Line type
 */
typedef struct gliv_line_s
{
    uint8_t x0;
    uint8_t y0;    
    uint8_t x1;
    uint8_t y1;
    gliv_color_t color;    
} gliv_line_t;

/**
 * \brief Rectangle type
 */
typedef struct gliv_rectangle_s
{
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    gliv_color_t color;
    gliv_fill_mode_t fill_mode;
} gliv_rectangle_t;

/**
 * \brief Font characters block type
 */
typedef struct gliv_font_char_block_s
{
    const uint16_t first;          // first character code
    const uint16_t last;           // last character code
    const uint16_t offset;         // block start position
} gliv_font_char_block_t;

/**
 * \brief Font type
 */
typedef struct gliv_font_s
{
    const uint8_t** data;                  // pointer to an array of font chars
    const uint8_t* widths;                 // pointer to character widths
    const uint8_t height;
    const uint8_t num_of_blocks;
    const gliv_font_char_block_t *blocks;  // pointer to font characters blocks parameters
} gliv_font_t;

/**
 * \brief Label type
 */
typedef struct gliv_label_s
{
    uint8_t x;
    uint8_t y;
    char* text;
    uint8_t width;
    uint8_t height;
    bool bg_transparent; // true: transparent background, false: opaque
    gliv_color_t color; // text color (background color is inverted)
    gliv_align_t align;
    const gliv_font_t* font;
} gliv_label_t;

/**
 * \brief Image resource type
 */
typedef struct gliv_image_res_s
{
    uint8_t width;
    uint8_t height;
    const uint8_t* data; // pointer to a bit field with image data
} gliv_image_res_t;

/**
 * \brief Image type
 */
typedef struct gliv_image_s
{
    uint8_t x;
    uint8_t y;
    bool bg_transparent; // true: transparent background, false: opaque
    gliv_color_t color; // image color (background color is inverted)
    const gliv_image_res_t* res; // pointer to a image resource
} gliv_image_t;

/**
 * \brief Main library structure
 */
typedef struct gliv_s
{
    uint8_t buffer[GLIV_MAX_WIDTH * GLIV_MAX_HEIGHT / GLIV_BITARRAY_WORD_BITS];
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
void gliv_fill(gliv_t* inst, gliv_color_t color);

void gliv_draw_pixel(gliv_t* inst, uint8_t x, uint8_t y, gliv_color_t color);

gliv_color_t gliv_get_pixel(const gliv_t* inst, uint8_t x, uint8_t y);

void gliv_draw_line(gliv_t* inst, gliv_line_t* line);

void gliv_draw_rectangle(gliv_t* inst, gliv_rectangle_t* rectangle);

void gliv_draw_image(gliv_t* inst, gliv_image_t* image);

void gliv_draw_label(gliv_t* inst, gliv_label_t* label);

#endif /* GLIV_H */
