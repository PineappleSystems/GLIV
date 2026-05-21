#include <string.h>
#include <stdio.h>
#include "gliv.h"

static const uint32_t gliv_error_image_data[] = { 0x22028020, 0x92124150, 0x21404888, 0xffa00c }; // Data for displaying error indications on the display
static const gliv_image_res_t gliv_error_image_res = { .data = gliv_error_image_data, .width = 11, .height = 11 }; // Image resource for indicating an error on the display

#define BITARRAY_WORD_BITS (8 * sizeof(unsigned int)) // Number of bits in one word

static void gliv_draw_char(gliv_t* inst, uint8_t x, uint8_t y, const gliv_font_t* const font, char character);
static uint8_t gliv_get_char_width(const gliv_font_t* const font, char character);
static void gliv_get_aligned_pos(uint8_t area_x, uint8_t area_y,
						   uint8_t area_w, uint8_t area_h, 
                           uint8_t obj_w, uint8_t obj_h, 
                           gliv_align_t align, 
                           uint8_t *obj_x, uint8_t *obj_y);

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

void gliv_fill(gliv_t* inst, gliv_color_t color)
{
    memset(inst->buffer, color ? 0xFF : 0x00, sizeof(inst->buffer));
}

void gliv_fill_pixel(gliv_t* inst, uint8_t x, uint8_t y, gliv_color_t color)
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

void gliv_draw_line(gliv_t* inst, gliv_line_t* line)
{
	uint8_t x0 = line->x0;
	uint8_t x1 = line->x1;
	uint8_t y0 = line->y0;
	uint8_t y1 = line->y1;
	int16_t dx, dy, sx, sy, err, e2; 
	
	// Boundary check. -1 due to zero-based indexing
	if (x0 >= inst->width)
	{
		x0 = inst->width - 1;
	}
	if (x1 >= inst->width)
	{
		x1 = inst->width - 1;
	}
	if (y0 >= inst->height)
	{
		y0 = inst->height - 1;
	}
	if (y1 >= inst->height)
	{
		y1 = inst->height - 1;
	}

	// Bresenham's line algorithm. Supports horizontal, vertical, and diagonal lines
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); // Absolute width delta
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); // Absolute height delta
	sx = (x0 < x1) ? 1 : -1; // X direction step: right (+1) or left (-1)
	sy = (y0 < y1) ? 1 : -1; // Y direction step: down (+1) or up (-1)
	err = ((dx > dy) ? dx : -dy) / 2; // Error threshold for stepping. Division by 2 centers the line

    while (1)
    {
        gliv_fill_pixel(inst, x0, y0, line->color);
        if (x0 == x1 && y0 == y1)
		{
            break;
        }
        e2 = err;
        if (e2 > -dx) // Step along the X axis if the error allows
		{
            err -= dy; // Error correction
            x0 += sx; // Move to the next horizontal pixel
        } 
        if (e2 < dy) // Step along the Y axis if the error accumulated
		{
            err += dx; // Error correction
            y0 += sy; // Move to the next vertical pixel
        } 
	}
}

void gliv_draw_rectangle(gliv_t* inst, gliv_rectangle_t* rectangle)
{
	uint8_t i;
	uint8_t x = rectangle->x;
    uint8_t y = rectangle->y;    
    uint8_t w = rectangle->width;
    uint8_t h = rectangle->height;
    gliv_color_t color = rectangle->color; 

	// Width and height validation
	if ((x + w) >= inst->width)
	{
		w = inst->width - x;
	}
	if ((y + h) >= inst->height)
	{
		h = inst->height - y;
	}
	
	// Draw the shape
	if (rectangle->filled == GLIV_FILL_SOLID)
	{
		for (i = 0; i <= h; i++)
		{
			gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y + i, .x1 = x + w, .y1 = y + i, .color = color});
		}
	}
	else // GLIV_FILL_NONE
	{
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y, .x1 = x + w, .y1 = y, .color = color}); // Bottom edge
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y, .x1 = x, .y1 = y + h, .color = color}); // Left edge
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x + w, .y0 = y, .x1 = x + w, .y1 = y + h, .color = color}); // Right edge
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y + h, .x1 = x + w, .y1 = y + h, .color = color}); // Top edge
	}
}

void gliv_draw_image(gliv_t* inst, gliv_image_t* image)
{
	uint8_t x = image->x;
	uint8_t y = image->y;

    for(int img_x = 0; img_x < image->res->width; img_x++)
    {
        for(int img_y = 0; img_y < image->res->height; img_y++)
        {
            gliv_fill_pixel(inst, x + img_x, y + image->res->height - img_y - 1, (gliv_color_t)get_index(image->res->data, image->res->width * img_y + img_x));
        }
    }
}

void gliv_draw_label(gliv_t* inst, gliv_label_t* label)
{
    uint8_t len = strlen(label->text); // String length
    uint8_t total_char_width = 0;

    // Calculate total text width in pixels
    for (uint8_t i = 0; i < len; i++)
	{
        total_char_width += gliv_get_char_width(label->font, label->text[i]);
    }
	
	total_char_width += len - 1; // account for a 1-pixel gap between characters

	if (total_char_width > label->width)
	{
		gliv_draw_image(inst, &(gliv_image_t){.x = label->x, .y = label->y, .res = &gliv_error_image_res});
		return;
	}

    // Calculate initial x and y coordinates based on alignment
	uint8_t a_x = 0, a_y = 0;
	gliv_get_aligned_pos(label->x, label->y, label->width, label->height, total_char_width, label->font->height, label->align, &a_x, &a_y);

    // Render each character of the text string
    uint8_t current_x = a_x;
    for (uint8_t i = 0; i < len; i++)
	{
        gliv_draw_char(inst, current_x, a_y, label->font, label->text[i]);
        current_x += gliv_get_char_width(label->font, label->text[i]);
		current_x++; // add space between characters
    }
}

static void gliv_draw_char(gliv_t* inst, uint8_t x, uint8_t y, const gliv_font_t* const font, char character)
{
	uint8_t shift = 0; // ASCII table offset

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

	gliv_image_t tmp_image = {
		.x   = x,
		.y   = y,
		.res = &(const gliv_image_res_t){
			.data   = font->data[character - shift],
			.width  = font->widths[character - shift],
			.height = font->height
		}
	};

    gliv_draw_image(inst, &tmp_image);
}

static uint8_t gliv_get_char_width(const gliv_font_t* const font, char character)
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

static void gliv_get_aligned_pos(uint8_t area_x, uint8_t area_y,
						   uint8_t area_w, uint8_t area_h, 
                           uint8_t obj_w, uint8_t obj_h, 
                           gliv_align_t align, 
                           uint8_t *obj_x, uint8_t *obj_y)
{
	// Horizontal alignment
    switch (align)
	{
        case GLIV_ALIGN_TOP_LEFT:
        case GLIV_ALIGN_MIDDLE_LEFT:
        case GLIV_ALIGN_BOTTOM_LEFT:
            *obj_x = area_x;
            break;
        case GLIV_ALIGN_TOP_CENTER:
        case GLIV_ALIGN_MIDDLE_CENTER:
        case GLIV_ALIGN_BOTTOM_CENTER:
            *obj_x = area_x + (area_w - obj_w) / 2;
            break;
        default: // RIGHT
            *obj_x = area_x + (area_w - obj_w);
            break;
    }

    // Vertical alignment
    switch (align)
	{
        case GLIV_ALIGN_BOTTOM_LEFT:
        case GLIV_ALIGN_BOTTOM_CENTER:
        case GLIV_ALIGN_BOTTOM_RIGHT:
            *obj_y = area_y;
            break;
        case GLIV_ALIGN_MIDDLE_LEFT:
        case GLIV_ALIGN_MIDDLE_CENTER:
        case GLIV_ALIGN_MIDDLE_RIGHT:
            *obj_y = area_y + (area_h - obj_h) / 2;
            break;
        default: // TOP
            *obj_y = area_y + (area_h - obj_h);
            break;
    }
}
