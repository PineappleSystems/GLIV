#include <stdio.h>
#include <stdbool.h>
#include "minifb.h"
#include "gliv.h"

// images
#include "km.h"
#include "logo.h"

// fonts
#include "arial_bold_39_num.h"

#define TEST_TAG         "pages_example"
#define GLIV_PAGES_COUNT 5

typedef struct app_state_s {
    int current_page;
    bool need_close;
    bool need_page_change;
    const char *title;
} app_state_t;

static void draw_page_0(gliv_t *gliv_display);
static void draw_page_1(gliv_t *gliv_display);
static void draw_page_2(gliv_t *gliv_display);
static void draw_page_3(gliv_t *gliv_display);
static void draw_page_4(gliv_t *gliv_display);

static void(*draw_page[GLIV_PAGES_COUNT])(gliv_t *gliv_display) = {
    draw_page_0, draw_page_1, draw_page_2, draw_page_3, draw_page_4
};

static void keyboard(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool is_pressed);

int main() {
    app_state_t app = {
        .current_page = 0,
        .need_close = false,
        .need_page_change = true,
        .title = "GLIV Example - ESC to exit"
    };

    struct mfb_window *window = mfb_open_ex(app.title, GLIV_MAX_WIDTH * 5, GLIV_MAX_HEIGHT * 5, MFB_WF_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "Failed to open minifb window\n");
        return -1;
    }

    mfb_set_user_data(window, (void *)&app);
    mfb_set_keyboard_callback(window, keyboard);    

    gliv_t gliv_display = {0}; 
    uint32_t buffer[GLIV_MAX_WIDTH * GLIV_MAX_HEIGHT] = {0}; 
    mfb_update_state state;    

    gliv_init(&gliv_display, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT);

    while (!app.need_close) {
        if (app.need_page_change == true) {
            app.need_page_change = false;
            draw_page[app.current_page](&gliv_display);
            
            for (int y = 0; y < GLIV_MAX_HEIGHT; y++) {
                for (int x = 0; x < GLIV_MAX_WIDTH; x++) {
                    uint8_t pixel = gliv_get_pixel(&gliv_display, x, y); 
                    if (pixel) {
                        buffer[y * GLIV_MAX_WIDTH + x] = 0x34b4eb;
                    } else {
                        buffer[y * GLIV_MAX_WIDTH + x] = 0x000000;
                    }
                }
            }
        }

        state = mfb_update_ex(window, buffer, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT);
        
        if (state != MFB_STATE_OK) {
            break;
        }

        if (!mfb_wait_sync(window)) {
            break;
        }
    }

    mfb_close(window);
    window = NULL;

    return 0;
}

static void draw_page_0(gliv_t *gliv_display) {
    gliv_fill(gliv_display, GLIV_COLOR_WHITE);
    gliv_draw_pixel(gliv_display, 1, 0, GLIV_COLOR_BLACK);

    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 0, .y1 = 63, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 63, .y1 = 0, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 127, .y1 = 0, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 127, .y1 = 32, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 63, .x1 = 127, .y1 = 63, .color = GLIV_COLOR_BLACK});
}

static void draw_page_1(gliv_t *gliv_display) {
    gliv_fill(gliv_display, GLIV_COLOR_BLACK);
    gliv_draw_image(gliv_display, &(gliv_image_t){.x = (gliv_display->width - 52) / 2, .y = (gliv_display->height - 46) / 2, .res = &logo_image_res});
}

static void draw_page_2(gliv_t *gliv_display) {
    gliv_fill(gliv_display, GLIV_COLOR_BLACK);
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = gliv_display->height / 2, .x1 = gliv_display->width, .y1 = gliv_display->height / 2, .color = GLIV_COLOR_WHITE});
}

static void draw_page_3(gliv_t *gliv_display) {    
    gliv_fill(gliv_display, GLIV_COLOR_BLACK);
    gliv_draw_label(gliv_display, &(gliv_label_t){.x = 0, .y = 0, .font = &arial_bold_39_num, .width = 128, .height = 64, .align = GLIV_ALIGN_MIDDLE_CENTER, .text = "278"});
}

static void draw_page_4(gliv_t *gliv_display) {
    gliv_fill(gliv_display, GLIV_COLOR_WHITE);
    gliv_draw_rectangle(gliv_display, &(gliv_rectangle_t){.x = 0, .y = 7, .width = 128, .height = 56, .color = GLIV_COLOR_BLACK, .filled = GLIV_FILL_SOLID});
    gliv_draw_image(gliv_display, &(gliv_image_t){.x = (gliv_display->width - 14) / 2, .y = 7 + (56 - 7) / 2, .res = &km_image_res});
    gliv_draw_rectangle(gliv_display, &(gliv_rectangle_t){.x = 0, .y = 56, .width = 128, .height = 7, .color = GLIV_COLOR_WHITE, .filled = GLIV_FILL_NONE});
}

static void keyboard(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool is_pressed) {

    app_state_t *app = (app_state_t *) mfb_get_user_data(window);
    
    if (!app) return;

    MFB_LOGI(TEST_TAG, "%s > keyboard: key: %s (pressed: %d) [key_mod: %x]", app->title, mfb_get_key_name(key), is_pressed, mod);

    if (is_pressed == false) {
        if (key == MFB_KB_KEY_RIGHT) {
            app->current_page++;
            if (app->current_page >= GLIV_PAGES_COUNT) app->current_page = 0;
            app->need_page_change = true;
        }

        if (key == MFB_KB_KEY_LEFT) {
            app->current_page--;
            if (app->current_page < 0) app->current_page = GLIV_PAGES_COUNT - 1;
            app->need_page_change = true;
        }

        if (key == MFB_KB_KEY_ESCAPE) {
            app->need_close = true;
        }
    }
}
