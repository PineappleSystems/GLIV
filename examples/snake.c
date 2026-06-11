// Let's have some fun and play snake. The snake moves only by turning left and right.
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "minifb.h"
#include "gliv.h"
#include "arial_bold_39_num.h"

#define BASE_GAME_INTERVAL_MS 100
#define CELL_SIZE 5  // Essentially scaling
#define MAX_SNAKE_LENGTH (GLIV_MAX_WIDTH * GLIV_MAX_HEIGHT)

typedef struct position_s {
    int x, y;
} position_t;

typedef struct snake_s {
    position_t body[MAX_SNAKE_LENGTH];
    int length;
    int direction;
    int next_direction;
} snake_t;

gliv_t gliv_display = {0}; // Display instance
snake_t snake = {0}; // Snake instance
position_t food = {0};
uint32_t buffer[GLIV_MAX_WIDTH * GLIV_MAX_HEIGHT * sizeof(uint32_t)] = {0}; // MiniFB frame buffer
int score = 0;
bool game_over = false;
bool escape = false;
int game_interval = BASE_GAME_INTERVAL_MS;

// Generate new position for food
void generate_food() {
    bool valid_position;
    do { // Check collisions
        valid_position = true;
        food.x = rand() % GLIV_MAX_WIDTH;
        food.y = rand() % GLIV_MAX_HEIGHT;
        
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.x && snake.body[i].y == food.y) {
                valid_position = false;
                break;
            }
        }
    } while (!valid_position);
}

void update_game() {
    snake.direction = snake.next_direction;
    position_t new_head = snake.body[0];
    switch (snake.direction)
    {
        case 0: new_head.y--; break;
        case 1: new_head.x--; break;
        case 2: new_head.y++; break;
        case 3: new_head.x++; break;
    }

    // Teleport!
    if (new_head.x < 0) new_head.x = GLIV_MAX_WIDTH - 1;
    if (new_head.x >= GLIV_MAX_WIDTH) new_head.x = 0;
    if (new_head.y < 0) new_head.y = GLIV_MAX_HEIGHT - 1;
    if (new_head.y >= GLIV_MAX_HEIGHT) new_head.y = 0;

    bool ate_food = (new_head.x == food.x && new_head.y == food.y);
    for (int i = snake.length; i > 0; i--) {
        snake.body[i] = snake.body[i-1];
    }
    snake.body[0] = new_head;

    if (ate_food) {
        snake.length++;
        score++;
        game_interval--;
        if (game_interval < 1) game_interval = 1; // I don't think it's possible, but just in case
        
        if (snake.length >= MAX_SNAKE_LENGTH) {
            game_over = true;
            return;
        }
        
        generate_food();
    }

    // Checking for a collision with itself after all recalculations
    for (int i = 1; i < snake.length; i++) {
        if (snake.body[i].x == new_head.x && snake.body[i].y == new_head.y) {
            game_over = true;
            return;
        }
    }
}

void init_game() {
    // Init snake
    snake.length = 3;
    snake.direction = 0;
    snake.next_direction = 0;
    for (int i = 0; i < snake.length; i++) { // generate start position
        snake.body[i].x = GLIV_MAX_WIDTH / 2;
        snake.body[i].y = GLIV_MAX_HEIGHT / 2 + i;
    }
    generate_food(); // Init food
    // Reset values for restart
    score = 0;
    game_over = false;
    game_interval = BASE_GAME_INTERVAL_MS;
}

void render_game() {
    // Render from GLIV to MiniFB
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

// MFB Handle keys
static void keyboard(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool is_pressed) {
    if (is_pressed == false) {
        if (key == MFB_KB_KEY_RIGHT) {
            snake.next_direction = snake.direction - 1;
            if (snake.next_direction < 0) snake.next_direction = 3;
        }
        else if (key == MFB_KB_KEY_LEFT) {
            snake.next_direction = snake.direction + 1;
            if (snake.next_direction > 3) snake.next_direction = 0;
        }
        if (key == MFB_KB_KEY_SPACE) {
            if (game_over) {
                init_game();
            } else {
                game_over = true;
            }
        }
        if (key == MFB_KB_KEY_ESCAPE) {
            escape = true;
        }
    }
}

int main() {
    srand(time(NULL)); // Random init
    struct mfb_window *window = mfb_open_ex("GLIV Snake - LEFT, RIGHT, SPACE - restart", GLIV_MAX_WIDTH * 5, GLIV_MAX_HEIGHT * 5, MFB_WF_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "Failed to open minifb window\n");
        return -1;
    }
    mfb_set_keyboard_callback(window, keyboard);    
    gliv_init(&gliv_display, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT);
    init_game();

    DWORD last_update = GetTickCount(); // Init update time

    while(!escape) {
        DWORD current_time = GetTickCount();

        if (current_time - last_update >= game_interval) {
            if (!game_over) {
                update_game();
                last_update = current_time;
                gliv_fill(&gliv_display, GLIV_COLOR_BLACK);
                // Draw snake
                for (int i = 0; i < snake.length; i++) {
                    gliv_draw_pixel(&gliv_display, snake.body[i].x, snake.body[i].y, GLIV_COLOR_WHITE);
                }
                // Draw food
                gliv_draw_pixel(&gliv_display, food.x, food.y, GLIV_COLOR_WHITE);
            } else {
                gliv_fill(&gliv_display, GLIV_COLOR_BLACK);
                char score_string[6];
                gliv_label_t lbl_score = {.x = 0, .y = 0, .font = &arial_bold_39_num, .width = 128, .height = 64, .color = GLIV_COLOR_WHITE, .align = GLIV_ALIGN_MIDDLE_CENTER, .bg_transparent = true, .text = score_string};
                snprintf(score_string, sizeof(score_string), "%d", score);
                gliv_draw_label(&gliv_display, &lbl_score);
            }
            render_game();
        }

        if (mfb_update_ex(window, buffer, GLIV_MAX_WIDTH, GLIV_MAX_HEIGHT) != MFB_STATE_OK) {
            break;
        }

        if (!mfb_wait_sync(window)) {
            break;
        }

        Sleep(10); // // A small delay to reduce CPU load
    } 

    mfb_close(window);
    window = NULL;
    return 0;
}
