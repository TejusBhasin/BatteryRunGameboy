#include <gb/gb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAP_W 20
#define MAP_H 16
#define PLAY_TOP 2
#define MAX_CELLS 6
#define MAX_ENEMIES 3

/* Background tiles: 0 floor, 1 wall, 2 exit. */
const unsigned char bg_tiles[] = {
    /* 0: floor */
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* 1: wall */
    0xFF,0xFF,0x81,0x81,0xBD,0xBD,0xA5,0xA5,
    0xA5,0xA5,0xBD,0xBD,0x81,0x81,0xFF,0xFF,
    /* 2: exit / door */
    0x7E,0x7E,0x42,0x42,0x5A,0x5A,0x5A,0x5A,
    0x5A,0x5A,0x5A,0x5A,0x42,0x42,0x7E,0x7E
};

/* Sprite tiles 8,9,10,11: player, battery, drone, shield. */
const unsigned char sprite_tiles[] = {
    /* 8: player */
    0x18,0x18,0x3C,0x3C,0x7E,0x7E,0x5A,0x5A,
    0x7E,0x7E,0x3C,0x3C,0x24,0x24,0x66,0x66,
    /* 9: battery */
    0x18,0x18,0x3C,0x3C,0x7E,0x7E,0x5A,0x5A,
    0x7E,0x7E,0x7E,0x7E,0x3C,0x3C,0x18,0x18,
    /* 10: drone */
    0x00,0x00,0x66,0x66,0xFF,0xFF,0xBD,0xBD,
    0xFF,0xFF,0x3C,0x3C,0x18,0x18,0x24,0x24,
    /* 11: shield */
    0x18,0x18,0x3C,0x3C,0x7E,0x7E,0xFF,0xFF,
    0xFF,0xFF,0x7E,0x7E,0x3C,0x3C,0x18,0x18
};

const unsigned char map_data[MAP_W * MAP_H] = {
    1,0,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,0,2,1,
    1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,
    1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,0,1,
    1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,0,1,
    1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,0,1,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,1,
    1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,0,1,
    1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,
    1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1,
    1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,
    1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1,0,1,
    1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,
    1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,0,1,0,1,
    1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,
    1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

typedef struct {
    uint8_t x, y;
    uint8_t active;
} Actor;

Actor cells[MAX_CELLS] = {
    {2, 1, 1}, {12, 1, 1}, {16, 3, 1},
    {4, 7, 1}, {14, 11, 1}, {10, 14, 1}
};

Actor drones[MAX_ENEMIES] = {
    {17, 1, 1}, {3, 12, 1}, {15, 14, 1}
};

uint8_t player_x = 1;
uint8_t player_y = 1;
uint8_t collected = 0;
uint8_t shield = 0;
uint16_t frames = 0;
uint8_t state = 0; /* 0 title, 1 playing, 2 win, 3 lose */

static uint8_t blocked(uint8_t x, uint8_t y) {
    if (x >= MAP_W || y >= MAP_H) return 1;
    return map_data[y * MAP_W + x] == 1;
}

static void place_sprite(uint8_t id, uint8_t x, uint8_t y) {
    move_sprite(id, 8 + x * 8, 16 + (y + PLAY_TOP) * 8);
}

static void hide_sprite(uint8_t id) {
    move_sprite(id, 0, 0);
}

static void draw_hud(void) {
    gotoxy(0, 0);
    printf("CELLS %u/%u  SHIELD:%u", collected, MAX_CELLS, shield ? 1 : 0);
    gotoxy(0, 1);
    printf("A:SHIELD  B:RESET   ");
}

static void draw_world(void) {
    set_bkg_data(0, 3, bg_tiles);
    set_bkg_tiles(0, PLAY_TOP, MAP_W, MAP_H, map_data);
}

static void reset_game(void) {
    uint8_t i;

    player_x = 1;
    player_y = 0;
    collected = 0;
    shield = 0;
    frames = 0;

    cells[0].x=2;  cells[0].y=1;  cells[0].active=1;
    cells[1].x=12; cells[1].y=1; cells[1].active=1;
    cells[2].x=16; cells[2].y=3; cells[2].active=1;
    cells[3].x=4;  cells[3].y=7; cells[3].active=1;
    cells[4].x=14; cells[4].y=11;cells[4].active=1;
    cells[5].x=10; cells[5].y=14;cells[5].active=1;

    drones[0].x=17; drones[0].y=1;
    drones[1].x=3;  drones[1].y=12;
    drones[2].x=15; drones[2].y=14;

    place_sprite(0, player_x, player_y);
    for (i=0; i<MAX_CELLS; ++i) {
        place_sprite(1+i, cells[i].x, cells[i].y);
    }
    for (i=0; i<MAX_ENEMIES; ++i) {
        place_sprite(7+i, drones[i].x, drones[i].y);
    }
    hide_sprite(10);
    draw_hud();
}

static void title_screen(void) {
    cls();
    gotoxy(2, 4);  printf("BATTERY RUN");
    gotoxy(1, 6);  printf("THE GRID IS DYING.");
    gotoxy(1, 8);  printf("COLLECT 6 CELLS");
    gotoxy(1, 9);  printf("THEN REACH THE EXIT.");
    gotoxy(1, 11); printf("DODGE THE DRONES.");
    gotoxy(2, 14); printf("START = PLAY");
    gotoxy(2, 16); printf("OG GAME BOY");
    waitpad(J_START);
    cls();
    draw_world();
    reset_game();
    state = 1;
}

static void win_screen(void) {
    uint16_t seconds = frames / 60;
    cls();
    gotoxy(4, 5); printf("ESCAPED!");
    gotoxy(3, 8); printf("TIME %u SEC", seconds);
    gotoxy(2, 11); printf("THE GRID LIVES.");
    gotoxy(2, 14); printf("START = AGAIN");
    waitpad(J_START);
    cls();
    draw_world();
    reset_game();
    state = 1;
}

static void lose_screen(void) {
    cls();
    gotoxy(5, 5); printf("CAUGHT!");
    gotoxy(2, 8); printf("A DRONE GOT YOU.");
    gotoxy(2, 11); printf("B = RETRY");
    gotoxy(2, 14); printf("START = RETRY");
    waitpad(J_START | J_B);
    cls();
    draw_world();
    reset_game();
    state = 1;
}

static void collect_cells(void) {
    uint8_t i;
    for (i=0; i<MAX_CELLS; ++i) {
        if (cells[i].active && cells[i].x == player_x && cells[i].y == player_y) {
            cells[i].active = 0;
            collected++;
            hide_sprite(1+i);
        }
    }
}

static void move_player(uint8_t joy) {
    int8_t nx = player_x;
    int8_t ny = player_y;

    if (joy & J_LEFT)  nx--;
    if (joy & J_RIGHT) nx++;
    if (joy & J_UP)    ny--;
    if (joy & J_DOWN)  ny++;

    if (nx != player_x && ny != player_y) return;
    if (!blocked((uint8_t)nx, (uint8_t)ny)) {
        player_x = (uint8_t)nx;
        player_y = (uint8_t)ny;
        place_sprite(0, player_x, player_y);
    }
}

static void move_drones(void) {
    uint8_t i;
    for (i=0; i<MAX_ENEMIES; ++i) {
        int8_t dx = (int8_t)player_x - (int8_t)drones[i].x;
        int8_t dy = (int8_t)player_y - (int8_t)drones[i].y;
        int8_t nx = drones[i].x;
        int8_t ny = drones[i].y;

        /* Alternate horizontal/vertical pursuit so they feel less robotic. */
        if (((frames / 12) + i) & 1) {
            if (dx > 0) nx++; else if (dx < 0) nx--;
            if (blocked((uint8_t)nx, (uint8_t)ny)) {
                nx = drones[i].x;
                if (dy > 0) ny++; else if (dy < 0) ny--;
            }
        } else {
            if (dy > 0) ny++; else if (dy < 0) ny--;
            if (blocked((uint8_t)nx, (uint8_t)ny)) {
                ny = drones[i].y;
                if (dx > 0) nx++; else if (dx < 0) nx--;
            }
        }

        if (!blocked((uint8_t)nx, (uint8_t)ny)) {
            drones[i].x = (uint8_t)nx;
            drones[i].y = (uint8_t)ny;
            place_sprite(7+i, drones[i].x, drones[i].y);
        }
    }
}

static void check_drones(void) {
    uint8_t i;
    for (i=0; i<MAX_ENEMIES; ++i) {
        if (drones[i].x == player_x && drones[i].y == player_y) {
            if (shield) {
                shield = 0;
                /* Bounce the drone one tile toward the nearest wall-free space. */
                if (drones[i].x > 1 && !blocked(drones[i].x-1, drones[i].y))
                    drones[i].x--;
                else if (drones[i].x+1 < MAP_W && !blocked(drones[i].x+1, drones[i].y))
                    drones[i].x++;
                place_sprite(7+i, drones[i].x, drones[i].y);
            } else {
                state = 3;
            }
        }
    }
}

void main(void) {
    uint8_t joy, prev_joy = 0;
    uint8_t i;

    DISPLAY_ON;
    SHOW_SPRITES;
    SHOW_BKG;

    set_bkg_data(0, 3, bg_tiles);
    set_sprite_data(8, 4, sprite_tiles);
    for (i=0; i<MAX_CELLS; ++i) set_sprite_tile(1+i, 9);
    set_sprite_tile(0, 8);
    for (i=0; i<MAX_ENEMIES; ++i) set_sprite_tile(7+i, 10);
    set_sprite_tile(10, 11);

    /* DMG palette: four shades of gray. */
    BGP_REG = 0xE4;
    OBP0_REG = 0xE4;

    font_init();
    font_set(font_load(font_min));

    while (1) {
        if (state == 0) {
            title_screen();
            continue;
        }
        if (state == 2) {
            win_screen();
            continue;
        }
        if (state == 3) {
            lose_screen();
            continue;
        }

        frames++;
        joy = joypad();

        if ((joy & J_B) && !(prev_joy & J_B)) {
            reset_game();
        }

        if ((joy & J_A) && !(prev_joy & J_A)) {
            shield = 1;
            place_sprite(10, player_x, player_y);
        }

        if (joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN)) {
            if (!(prev_joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN))) {
                move_player(joy);
            }
        }

        collect_cells();

        /* A shield is consumed on a hit; otherwise it remains until used. */
        if ((frames % 12) == 0) {
            move_drones();
            check_drones();
        }

        if (collected == MAX_CELLS && player_x == 18 && player_y == 0) {
            state = 2;
        }

        if (shield) place_sprite(10, player_x, player_y);
        else hide_sprite(10);

        draw_hud();
        prev_joy = joy;
        wait_vbl_done();
    }
}