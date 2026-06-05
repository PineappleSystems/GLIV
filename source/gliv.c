#include <string.h>
#include <stdio.h>
#include "gliv.h"

static const uint32_t gliv_error_image_data[] = { 0x22028020, 0x92124150, 0x21404888, 0xffa00c }; // Data for displaying error indications on the display
static const gliv_image_res_t gliv_error_image_res = { .data = gliv_error_image_data, .width = 11, .height = 11 }; // Image resource for indicating an error on the display

static void gliv_get_aligned_pos(uint8_t area_x, uint8_t area_y,
						   uint8_t area_w, uint8_t area_h, 
                           uint8_t obj_w, uint8_t obj_h, 
                           gliv_align_t align, 
                           uint8_t *obj_x, uint8_t *obj_y);
static uint16_t gliv_utf8_to_unicode(const char *text, uint8_t *utf8_bytes);

/**
 * \brief Setting the value of a bit array to 1
 */
static inline void set_index(unsigned int * bitarray, size_t idx)
{
    bitarray[idx / GLIV_BITARRAY_WORD_BITS] |= (1 << (idx % GLIV_BITARRAY_WORD_BITS));
}

/**
 * \brief Setting the value of a bit array to 0
 */
static inline void reset_index(unsigned int * bitarray, size_t idx)
{
    bitarray[idx / GLIV_BITARRAY_WORD_BITS] &= ~(1 << (idx % GLIV_BITARRAY_WORD_BITS));
}

/**
 * \brief Reading a value from a bit array
 */
static inline uint8_t get_index(const unsigned int* const bitarray, size_t idx)
{
    return bitarray[idx / GLIV_BITARRAY_WORD_BITS] >> (idx % GLIV_BITARRAY_WORD_BITS) & 0x1;
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

void gliv_draw_pixel(gliv_t* inst, uint8_t x, uint8_t y, gliv_color_t color)
{
    if (color == GLIV_COLOR_WHITE)
    {
        set_index(inst->buffer, inst->width * y + x);
	}
    else
    {
		reset_index(inst->buffer, inst->width * y + x);
	}
}

gliv_color_t gliv_get_pixel(gliv_t* inst, uint8_t x, uint8_t y)
{
    return (gliv_color_t)get_index(inst->buffer, inst->width * y + x);
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
        gliv_draw_pixel(inst, x0, y0, line->color);
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
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y, .x1 = x + w, .y1 = y, .color = color}); // Top edge
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y, .x1 = x, .y1 = y + h, .color = color}); // Left edge
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x + w, .y0 = y, .x1 = x + w, .y1 = y + h, .color = color}); // Right edge
		gliv_draw_line(inst, &(gliv_line_t){.x0 = x, .y0 = y + h, .x1 = x + w, .y1 = y + h, .color = color}); // Bottom edge
	}
}

void gliv_draw_image(gliv_t* inst, gliv_image_t* image)
{
	uint8_t x = image->x;
	uint8_t y = image->y;

	uint8_t width = image->width >= image->res->width ? image->res->width : image->width;
	uint8_t height = image->height >= image->res->height ? image->res->height : image->height;

    for(int img_x = 0; img_x < width; img_x++)
    {
        for(int img_y = 0; img_y < height; img_y++)
        {
			gliv_color_t res_color = (gliv_color_t)get_index(image->res->data, image->res->width * img_y + img_x);
			gliv_color_t pixel_color = res_color ^ (image->color != GLIV_COLOR_WHITE);
			
			if (pixel_color != image->color && image->bg_transparent) // skip transparent background pixels
			{
				continue;
			}

            gliv_draw_pixel(inst, x + img_x, y + img_y, pixel_color);
        }
    }
}

void gliv_draw_label(gliv_t* inst, gliv_label_t* label)
{
    uint8_t text_len = strlen(label->text); // String length in bytes

    // Calculate total text width in pixels and get characters offset
	uint8_t byte_idx = 0;
	uint8_t char_count = 0;
	uint8_t utf8_bytes = 0;
	uint8_t text_width = 0;
	uint8_t text_heigth = 0;
	uint16_t char_offsets[GLIV_LABEL_TEXT_MAX_LENGTH];

    while (byte_idx < text_len && char_count < GLIV_LABEL_TEXT_MAX_LENGTH)
	{
		uint16_t unicode = gliv_utf8_to_unicode(&label->text[byte_idx], &utf8_bytes);

		if (utf8_bytes != 0)
		{
			for (int i = 0; i < label->font->num_of_blocks; i++)
			{
				if (unicode >= label->font->blocks[i].first && unicode <= label->font->blocks[i].last)
				{
					char_offsets[char_count] = label->font->blocks[i].offset + (unicode - label->font->blocks[i].first);
					text_width += label->font->widths[char_offsets[char_count]];
					char_count++;
					break;
				}
			}
			
			byte_idx += utf8_bytes;
		}
		else
		{
			byte_idx++;
		}
        
    }

	if (char_count > 0)
	{
		text_width += char_count - 1; // account for a 1-pixel gap between characters
	}
	
	if (text_width > label->width)
	{
		text_width = label->width;
	}
	text_heigth = label->height > label->font->height ? label->font->height : label->height;

	// Fill label background if it is not transparent
	if (!label->bg_transparent)
	{
		gliv_color_t background_color = label->color == GLIV_COLOR_WHITE ? GLIV_COLOR_BLACK : GLIV_COLOR_WHITE;
		gliv_draw_rectangle(inst, &(gliv_rectangle_t){.x = label->x, .y = label->y, .width = label->width, .height = label->height, .color = background_color, .filled = GLIV_FILL_SOLID});
	}

    // Calculate initial x and y coordinates based on alignment
	uint8_t a_x = 0, a_y = 0;
	gliv_get_aligned_pos(label->x, label->y, label->width, label->height, text_width, text_heigth, label->align, &a_x, &a_y);	

    // Render each character of the text string
    uint8_t current_x = a_x;
	uint8_t max_x = label->x + label->width;
	uint8_t max_y = label->y + label->height;
	
	for (int i = 0; i < char_count; i++)
	{
		if (current_x >= max_x)
		{
			break;
		}

		uint8_t width = max_x - current_x; // image container width
		uint8_t height = max_y - a_y; // image container height

		gliv_image_t tmp_image = {
			.x   = current_x,
			.y   = a_y,
			.width = width,
			.height = height,
			.bg_transparent = label->bg_transparent,
			.color = label->color,
			.res = &(const gliv_image_res_t){
				.data   = label->font->data[char_offsets[i]],
				.width  = label->font->widths[char_offsets[i]],
				.height = label->font->height
			}
		};

		gliv_draw_image(inst, &tmp_image);

		current_x += label->font->widths[char_offsets[i]];
		current_x++; // add space between characters
    }
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
        case GLIV_ALIGN_TOP_LEFT:
        case GLIV_ALIGN_TOP_CENTER:
        case GLIV_ALIGN_TOP_RIGHT:
            *obj_y = area_y;
            break;
        case GLIV_ALIGN_MIDDLE_LEFT:
        case GLIV_ALIGN_MIDDLE_CENTER:
        case GLIV_ALIGN_MIDDLE_RIGHT:
            *obj_y = area_y + (area_h - obj_h) / 2;
            break;
        default: // BOTTOM
            *obj_y = area_y + (area_h - obj_h);
            break;
    }
}

// 4 bytes characters are not included
static uint16_t gliv_utf8_to_unicode(const char *text, uint8_t *utf8_bytes)
{	
    *utf8_bytes = 0;
    uint16_t unicode = 0;

    uint8_t b0 = (uint8_t)text[0];

    if ((b0 & 0x80) == 0) // 1 byte
    {
        unicode = b0;
        *utf8_bytes = 1;
    }
    else if ((b0 & 0xE0) == 0xC0) // 2 bytes
    {
        if (text[1] != '\0')
        {
			uint8_t b1 = (uint8_t)text[1];

            unicode = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
            *utf8_bytes = 2;
        }
    }
    else if ((b0 & 0xF0) == 0xE0) // 3 bytes
    {
        if (text[1] != '\0' && text[2] != '\0')
        {
			uint8_t b1 = (uint8_t)text[1];
			uint8_t b2 = (uint8_t)text[2];

            unicode = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            *utf8_bytes = 3;
        }
    }

    return unicode;
}
