#include <string.h>
#include <stdio.h>
#include "gliv.h"

static const uint32_t image_data_GLIV_ERROR[] = { 0x22028020, 0x92124150, 0x21404888, 0xffa00c }; // Data for displaying error indications on the display
static const gliv_image_t GLIV_ERROR = {image_data_GLIV_ERROR, 11, 11}; // Image for indicating an error on the display

#define BITARRAY_WORD_BITS (8 * sizeof(unsigned int)) // Number of bits in one word

/**
 * \brief Setting the value of a bit array to 1
 */
static inline void set_index(unsigned int * bitarray, size_t idx)
{
    bitarray[idx / BITARRAY_WORD_BITS] |= (1 << (idx % BITARRAY_WORD_BITS));
}

/**
 * \brief Setting the value of a bit array to 0
 */
static inline void reset_index(unsigned int * bitarray, size_t idx)
{
    bitarray[idx / BITARRAY_WORD_BITS] &= ~(1 << (idx % BITARRAY_WORD_BITS));
}

/**
 * \brief Reading a value from a bit array
 */
static inline uint8_t get_index(const unsigned int* const bitarray, size_t idx)
{
    return bitarray[idx / BITARRAY_WORD_BITS] >> (idx % BITARRAY_WORD_BITS) & 0x1;
}

void gliv_init(gliv_t* inst, uint8_t width, uint8_t height)
{
    inst->width = width;
    inst->height = height;
}

void gliv_fill(gliv_t* inst, uint8_t color)
{
    memset(inst->buffer, color ? 0xFF : 0x00, sizeof(inst->buffer));
}

void gliv_set_pixel(gliv_t* inst, uint8_t x, uint8_t y, uint8_t color)
{
    if (color)
    {
        set_index(inst->buffer, inst->width * y + x);
	}
    else
    {
		reset_index(inst->buffer, inst->width * y + x);
	}
}

uint8_t gliv_get_pixel(gliv_t* inst, uint8_t x, uint8_t y)
{
    return get_index(inst->buffer, inst->width * y + x);
}

void gliv_draw_line(gliv_t* inst, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	// checking ranges
	if (x0 >= inst->width) {
		x0 = inst->width - 1;
	}
	if (x1 >= inst->width) {
		x1 = inst->width - 1;
	}
	if (y0 >= inst->height) {
		y0 = inst->height - 1;
	}
	if (y1 >= inst->height) {
		y1 = inst->height - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0)
    {
		if (y1 < y0)
        {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0)
        {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		// vertical line
		for (i = y0; i <= y1; i++) {
			gliv_set_pixel(inst, x0, i, color);
		}
	}
	else if (dy == 0)
    {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		// horizontal line
		for (i = x0; i <= x1; i++) {
			gliv_set_pixel(inst, i, y0, color);
		}
	}
	else
    {
        while (1)
        {
            gliv_set_pixel(inst, x0, y0, color); 
            if (x0 == x1 && y0 == y1) {
                break;
            }
            e2 = err; 
            if (e2 > -dx) {
                err -= dy;
                x0 += sx;
            } 
            if (e2 < dy) {
                err += dx;
                y0 += sy;
            } 
	    }
    }
}

void gliv_draw_filled_rectangle(gliv_t* inst, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	uint8_t i;

	// Checking width and height
	if ((x + w) >= inst->width) {
		w = inst->width - x;
	}
	if ((y + h) >= inst->height) {
		h = inst->height - y;
	}
	
	// Drawing lines
	for (i = 0; i <= h; i++) {
		gliv_draw_line(inst, x, y + i, x + w, y + i, color);
	}
}

void gliv_draw_image(gliv_t* inst, uint8_t x, uint8_t y, const gliv_image_t* const image)
{
    for(int img_x = 0; img_x < image->width; img_x++)
    {
        for(int img_y = 0; img_y < image->height; img_y++)
        {
            gliv_set_pixel(inst, x + img_x, y + image->height - img_y - 1, get_index(image->data, image->width * img_y + img_x));
        }
    }
}

void gliv_draw_char(gliv_t* inst, uint8_t x, uint8_t y, const gliv_font_t* const font, char character)
{
	uint8_t shift = 0; // ASCII offset

	if(character >= '0' && character <= '9')
	{
		shift = 48;
	}
	else if (character >= 'A' && character <= 'Z')
	{
		shift = 65;
	}
	else if (character >= 'a' && character <= 'z')
	{
		shift = 97;
	}
	
	gliv_image_t tmp_image = {.data = font->data[character - shift],
								.width = font->widths[character - shift],
								.height = font->height};
	/*tmp_image.data = font->data[character - shift]; // for now only for numbers
	tmp_image.width = font->widths[character - shift];
	tmp_image.height = font->height;*/

    gliv_draw_image(inst, x, y, &tmp_image);
}

uint8_t gliv_get_char_width(const gliv_font_t* const font, char character)
{
	uint8_t shift = 0; // ASCII offset

	if(character >= '0' && character <= '9')
	{
		shift = 48;
	}
	else if (character >= 'A' && character <= 'Z')
	{
		shift = 65;
	}
	else if (character >= 'a' && character <= 'z')
	{
		shift = 97;
	}
	else
	{
		return 0; // if the symbol is invalid
	}
	
	return font->widths[character - shift];
}

void gliv_draw_uint(gliv_t* inst, uint8_t x, uint8_t y, const gliv_font_t* const font, uint8_t width, gliv_align_t align, unsigned int value)
{
    char buffer[11]; // Enough to store the maximum unsigned int (10 digits) and the terminator zero
    snprintf(buffer, sizeof(buffer), "%u", value); // Converting a number to a string

    uint8_t len = strlen(buffer); // String length
    uint8_t total_char_width = 0;

    // Сalculating the total width of the number in pixels
    for (uint8_t i = 0; i < len; i++) {
        total_char_width += gliv_get_char_width(font, buffer[i]);
    }
	
	total_char_width += len - 1; // we take into account a space of one pixel between characters

	if (total_char_width > width)
	{
		gliv_draw_image(inst, x, y, &GLIV_ERROR);
		return;
	}

    // Calculate the initial x-coordinate depending on the alignment
    uint8_t start_x = x;
    if (align == GLIV_ALIGN_CENTER)
	{
        start_x = x + (width - total_char_width) / 2;
	}
	else if(align == GLIV_ALIGN_RIGHT)
	{
        start_x = x + (width - total_char_width);
    }
    // By default (left justified), start_x is already equal to x

    // Drawing each character of a number
    uint8_t current_x = start_x;
    for (uint8_t i = 0; i < len; i++) {
        gliv_draw_char(inst, current_x, y, font, buffer[i]);
        current_x += gliv_get_char_width(font, buffer[i]);
		current_x++; // add a space between characters
    }
}