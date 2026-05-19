#include <stdio.h>
#include "minifb.h"
#include "gliv.h"

int main() {

    struct mfb_window *window = mfb_open_ex("GLIV Example - ESC to exit", GLIV_MAX_WIDTH * 5, GLIV_MAX_HEIGHT * 5, MFB_WF_RESIZABLE); // * 5 to make the image more visible
    if (window == NULL) {
        fprintf(stderr, "Failed to open minifb window\n");
        return -1;
    }

    gliv_t gliv_display = {}; // gliv instance
    uint32_t buffer[GLIV_MAX_WIDTH * GLIV_MAX_HEIGHT * sizeof(uint32_t)] = {}; // mfb buffer
    mfb_update_state state;

    // gliv work block
    gliv_init(&gliv_display, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT);
    gliv_fill(&gliv_display, GLIV_COLOR_WHITE);
    gliv_fill_pixel(&gliv_display, 1, 63, GLIV_COLOR_BLACK);    
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 0, .y1 = 63, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 63, .y1 = 63, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 127, .y1 = 63, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 127, .y1 = 31, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(&gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 127, .y1 = 0, .color = GLIV_COLOR_BLACK});

    // example of getting an image from a library
    // get all the pixels
    for (int y = 0; y < GLIV_MAX_HEIGHT; y++) {
        for (int x = 0; x < GLIV_MAX_WIDTH; x++) {
            uint8_t pixel = gliv_get_pixel(&gliv_display, x, GLIV_MAX_HEIGHT - 1 - y); // invert the Y coordinate for correct display
            if (pixel) {
                buffer[y * GLIV_MAX_WIDTH + x] = 0x34b4eb;
            } else {
                buffer[y * GLIV_MAX_WIDTH + x] = 0x000000;
            }
        }
    }

    do {
        state = mfb_update_ex(window, buffer, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT);
        if (state != MFB_STATE_OK)
            break;

    } while(mfb_wait_sync(window));

    window = NULL;

    return 0;
}