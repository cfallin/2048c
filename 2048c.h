/*
 * 2048c -- a console implementation of 2048
 *
 * Copyright (c) 2017, Chris Fallin <cfallin@c1f.net>
 *
 * Released under the MIT License.
 */

#ifndef _2048C_H_
#define _2048C_H_

/* ------------- cells and board manipulations ------------ */

typedef int cell_t;

#define CELL_EMPTY 0
#define CELL_VALUE(c) (((c) < 0) ? -(c) : (c))
#define CELL_IS_NORMAL(c) ((c) > 0)
#define CELL_IS_COLLAPSED(c) ((c) < 0)
#define CELL_NORMAL(c) (c)
#define CELL_COLLAPSED(c) (-(c))

#define BOARD_ROWS 4
#define BOARD_COLUMNS 4

typedef struct {
    cell_t cells[BOARD_ROWS * BOARD_COLUMNS];
} board_t;

typedef enum {
    UP = 1,
    DIR_FIRST = UP,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4,
    DIR_LAST = RIGHT,
} dir_t;

/* reset the board to empty */
void board_reset(board_t *b);
/* returns `1` if two or more cells merged. */ 
int board_move(board_t *b, dir_t dir);
/* returns `1` if a move in any direction is possible. */
int board_can_move(board_t *b);
/* returns `1` if the user has won (has a 2048 tile) */
int board_is_win(board_t *b);
/* fills a random cell with a new `2` or `4` tile, unless no empty
 * spaces are left. Returns `1` if tile could be inserted. Requires
 * an arbitrary integer value `random_value` with which the random
 * choice is made. */
int board_fill_random_cell(board_t *b, int random_value);

/* ------------- UI ------------ */

#define UI_CELL_COLS 8
#define UI_ROW_WIDTH ((BOARD_COLUMNS * UI_CELL_COLS) + 1)
#define UI_CELL_FMT "%6d "
#define UI_CELL_EMPTY "       "

void ui_setup();
void ui_draw(board_t *b);

typedef enum {
    UI_ACCEPTED,
    UI_IGNORE,
    UI_EXIT,
} ui_result_t;

ui_result_t ui_key(board_t *b, int key);

#endif  // _2048C_H_
