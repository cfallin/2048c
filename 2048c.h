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

typedef struct board_t board_t;
struct board_t {
    cell_t cells[BOARD_ROWS * BOARD_COLUMNS];
};

void board_reset(board_t *b);

static void board_set(board_t *b, int row, int column, cell_t value) {
    b->cells[row*BOARD_COLUMNS + column] = value;
}
static cell_t board_get(board_t *b, int row, int column) {
    return b->cells[row*BOARD_COLUMNS + column];
}

enum dir_t {
    UP = 1,
    DIR_FIRST = UP,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4,
    DIR_LAST = RIGHT,
};

/* returns `1` if two or more cells merged. */ 
int board_move(board_t *b, dir_t dir);
/* returns `1` if a move in any direction could cause a merge. */
int board_can_move(board_t *b);
/* fills a random cell with a new `2` or `4` tile, unless no empty
 * spaces are left. Returns `1` if tile could be inserted. Requires
 * an arbitrary integer value `random_value` with which the random
 * choice is made. */
int board_fill_random_cell(board_t *b, int random_value);

/* ------------- UI ------------ */

#define UI_CELL_COLS 7
#define UI_CELL_FMT "%6d "
#define UI_CELL_EMPTY "       "

void ui_setup();
void ui_draw(board_t *b);
/* returns `1` to indicate exit was requested. */
int ui_key(board_t *b, int key);

#endif  // _2048C_H_
