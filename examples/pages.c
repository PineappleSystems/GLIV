#include <stdio.h>
#include "minifb.h"
#include "gliv.h"

// images
#include "km.h"
#include "logo.h"

// fonts
#include "arial_14.h"
#include "arial_bold_39_num.h"

#define TEST_TAG         "pages_example"
#define GLIV_PAGES_COUNT 5

typedef struct app_state_s {
    int current_page;
    bool need_close;
    bool need_page_change;
    const char *title;
} app_state_t;

static void draw_page_1(gliv_t *gliv_display);
static void draw_page_2(gliv_t *gliv_display);
static void draw_page_3(gliv_t *gliv_display);
static void draw_page_4(gliv_t *gliv_display);
static void draw_page_5(gliv_t *gliv_display);

static void(*draw_page[GLIV_PAGES_COUNT])(gliv_t *gliv_display) = {
    draw_page_1, draw_page_2, draw_page_3, draw_page_4, draw_page_5
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

static void draw_page_1(gliv_t *gliv_display) {
    // -1 due to zero-based indexing
    const uint8_t max_x = gliv_display->width - 1;
    const uint8_t max_y = gliv_display->height - 1;

    gliv_fill(gliv_display, GLIV_COLOR_WHITE);
    gliv_draw_pixel(gliv_display, 1, 0, GLIV_COLOR_BLACK);

    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = 0, .x1 = 0, .y1 = max_y, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = max_y, .x1 = max_x / 2, .y1 = 0, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = max_y, .x1 = max_x, .y1 = 0, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = max_y, .x1 = max_x, .y1 = max_y / 2, .color = GLIV_COLOR_BLACK});
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = max_y, .x1 = max_x, .y1 = max_y, .color = GLIV_COLOR_BLACK});
}

static void draw_page_2(gliv_t *gliv_display) {
    gliv_fill(gliv_display, GLIV_COLOR_WHITE);
    gliv_draw_image(gliv_display, &(gliv_image_t){
        .x = (gliv_display->width - logo_image_res.width) / 2,
        .y = (gliv_display->height - logo_image_res.height) / 2,
        .bg_transparent = true,
        .color = GLIV_COLOR_BLACK,
        .res = &logo_image_res
    });
}

static void draw_page_3(gliv_t *gliv_display) {
    // -1 due to zero-based indexing
    const uint8_t max_x = gliv_display->width - 1;
    const uint8_t max_y = gliv_display->height - 1;

    const uint8_t line_y = max_y / 2;

    gliv_fill(gliv_display, GLIV_COLOR_BLACK);
    gliv_draw_line(gliv_display, &(gliv_line_t){.x0 = 0, .y0 = line_y, .x1 = max_x, .y1 = line_y, .color = GLIV_COLOR_WHITE});
    gliv_draw_label(gliv_display, &(gliv_label_t){
        .x = 0,
        .y = 0,
        .font = &arial_14,
        .width = gliv_display->width,
        .height = line_y, // without overlapping the line
        .bg_transparent = true,
        .color = GLIV_COLOR_WHITE,
        .align = GLIV_ALIGN_MIDDLE_CENTER,
        .text = "page 3"
    });
    gliv_draw_label(gliv_display, &(gliv_label_t){
        .x = 0,
        .y = line_y + 1,
        .font = &arial_14,
        .width = gliv_display->width,
        .height = gliv_display->height - line_y,
        .bg_transparent = true,
        .color = GLIV_COLOR_WHITE,
        .align = GLIV_ALIGN_MIDDLE_CENTER,
        .text = "страница 3"
    });
}

static void draw_page_4(gliv_t *gliv_display) {    
    gliv_fill(gliv_display, GLIV_COLOR_BLACK);
    gliv_draw_label(gliv_display, &(gliv_label_t){
        .x = 0,
        .y = 0,
        .font = &arial_bold_39_num,
        .width = gliv_display->width,
        .height = gliv_display->height,
        .bg_transparent = true,
        .color = GLIV_COLOR_WHITE,
        .align = GLIV_ALIGN_MIDDLE_CENTER,
        .text = "278"
    });
}

static void draw_page_5(gliv_t *gliv_display) {
    // -1 due to zero-based indexing
    const uint8_t max_x = gliv_display->width - 1;
    const uint8_t max_y = gliv_display->height - 1;

    const uint8_t rec1_y = 7;
    const uint8_t rec1_w = gliv_display->width;
    const uint8_t rec1_h = gliv_display->height - rec1_y;

    const uint8_t rec2_y = 56;
    const uint8_t rec2_w = gliv_display->width;
    const uint8_t rec2_h = gliv_display->height - rec2_y;

    gliv_fill(gliv_display, GLIV_COLOR_WHITE);
    gliv_draw_rectangle(gliv_display, &(gliv_rectangle_t){.x = 0, .y = rec1_y, .width = rec1_w, .height = rec1_h, .color = GLIV_COLOR_BLACK, .fill_mode = GLIV_FILL_SOLID});
    gliv_draw_image(gliv_display, &(gliv_image_t){
        .x = (gliv_display->width - km_image_res.width) / 2,
        .y = rec1_y + (rec1_h - km_image_res.height) / 2,
        .bg_transparent = true,
        .color = GLIV_COLOR_WHITE,
        .res = &km_image_res
    });
    gliv_draw_rectangle(gliv_display, &(gliv_rectangle_t){.x = 0, .y = rec2_y, .width = rec2_w, .height = rec2_h, .color = GLIV_COLOR_WHITE, .fill_mode = GLIV_FILL_NONE});
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
