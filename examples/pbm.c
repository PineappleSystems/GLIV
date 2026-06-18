// This file shows an example of exporting a library-generated image to monochrome PBM format.

#include <stdio.h>
#include <string.h>
#include "gliv.h"

// Generating a buffer with PBM binary data
// It was moved to a separate function specifically so that it could be used not only
// for writing to a file, but, for example, for transferring over a network or easy porting
// Without dynamic memory allocation
size_t generate_pbm_data(gliv_t *gliv_instance, uint8_t *out_data) {
    const int bytes_per_row = (GLIV_MAX_WIDTH + 7) / 8;
    size_t total_bytes = 0;
    char header[64]; // Buffer size must be sufficient
    size_t header_size = snprintf(header, sizeof(header), "P4\n%d %d\n", GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT); // PBM Header (P4 = binary black and white)
    memcpy(out_data, header, header_size); // Copy header
    total_bytes = bytes_per_row * GLIV_MAX_HEIGHT + header_size; // Calculating the total file size
    
    // Generating pixel data
    uint8_t* pixel_data = out_data + header_size;
    
    for (int y = 0; y < GLIV_MAX_HEIGHT; y++) {
        for (int byte_x = 0; byte_x < bytes_per_row; byte_x++) {
            uint8_t byte = 0;
            for (int bit = 0; bit < 8; bit++) {
                int x = byte_x * 8 + bit;
                if (x >= GLIV_MAX_WIDTH) break;
                
                gliv_color_t pixel = gliv_get_pixel(gliv_instance, x, y);
                if (pixel == GLIV_COLOR_WHITE) {
                    byte |= (1 << (7 - bit)); // 1 = black in pbm (light emitting pixel)
                }
            }
            pixel_data[y * bytes_per_row + byte_x] = byte;
        }
    }
    return total_bytes;
}

// Function for saving PBM data to a file in the current directory
void save_pbm_file(gliv_t *gliv_instance, const char* filename) {
    size_t data_size;
    uint8_t pbm_data[1500] = {0}; // Buffer size must be sufficient
    data_size = generate_pbm_data(gliv_instance, pbm_data);

    FILE* file = fopen(filename, "wb"); // Open file in binary mode 
    if (file) {
        fwrite(pbm_data, 1, data_size, file);
        fclose(file);
        printf("File saved in the current directory");
    } else {
        printf("Failed to create file\n");
    }
}

int main() {
    gliv_t gliv_display = {0}; 
    gliv_init(&gliv_display, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT);
    gliv_fill(&gliv_display, GLIV_COLOR_WHITE);
    gliv_draw_pixel(&gliv_display, 1, 0, GLIV_COLOR_BLACK);
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 0, .y1 = 63, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 63, .y1 = 0, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 127, .y1 = 0, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 127, .y1 = 32, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 127, .y1 = 63, .color = GLIV_COLOR_BLACK});
    save_pbm_file(&gliv_display, "gliv.pbm");
    return 0;
}
