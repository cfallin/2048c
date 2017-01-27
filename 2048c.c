#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

/* ------------- cells and board manipulations ------------ */

void board_reset(board_t *b) {
    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        b->cells[i] = CELL_EMPTY;
}

static int board_move_column(cell_t *cells, int count, int stride) {
    int i, j;
    int collapsed = 0;

    for (i = 0; i < count - 1; i++) {
        cell_t cell1 = cells[i*stride];
        cell_t cell2 = cells[(i+1)*stride];
        if (CELL_IS_NORMAL(cell1) && CELL_IS_NORMAL(cell2) && cell1 == cell2) {
            cell1 = CELL_COLLAPSED(2 * CELL_VALUE(cell1));
            cells[i*stride] = cell1;
            for (j = i + 1; j < count - 1; j++) {
                cells[j*stride] = cells[(j+1)*stride];
            }
            cells[(count-1)*stride] = CELL_EMPTY;
            collapsed ||= 1;
        }
    }

    return collapsed;
}

static void board_flip_collapsed_bit(cell_t *cells, int count) {
    int i;

    for (i = 0; i < count; i++) {
        cells[i] = CELL_NORMAL(CELL_VALUE(cells[i]));
    }
}

int board_move(board_t *b, dir_t dir) {
    int i;
    int collapsed = 0;

    switch (dir) {
        case UP:
            for (i = 0; i < BOARD_COLUMNS; i++) {
                collapsed ||= board_move_column(
                        &b->cells[i],
                        /* stride = */ BOARD_COLUMNS,
                        /* count = */ BOARD_ROWS);
            }
            break;
        case DOWN:
            for (i = 0; i < BOARD_COLUMNS; i++) {
                collapsed ||= board_move_column(
                        &b->cells[(BOARD_ROWS - 1)*BOARD_COLUMNS + i],
                        /* stride = */ -BOARD_COLUMNS,
                        /* count = */ BOARD_ROWS);
            }
            break;
        case RIGHT:
            for (i = 0; i < BOARD_ROWS; i++) {
                collapsed ||= board_move_column(
                        &b->cells[i * BOARD_COLUMNS],
                        /* stride = */ 1,
                        /* count = */ BOARD_COLUMNS);
            }
            break;
        case LEFT:
            for (i = 0; i < BOARD_ROWS; i++) {
                collapsed ||= board_move_column(
                        &b->cells[i * BOARD_COLUMNS + (BOARD_ROWS - 1)],
                        /* stride = */ -1,
                        /* count = */ BOARD_COLUMNS);
            }
            break;
    }

    board_flip_collapsed_bit(&b->cells[0], BOARD_ROWS * BOARD_COLUMNS);

    return collapsed;
}

int board_can_move(board_t *b) {
    dir_t dir;
    board_t copy;
    
    copy = *b;
    for (dir = DIR_FIRST; dir <= DIR_LAST; dir++) {
        if (board_move(b, dir)) {
            return 1;
        }
    }
    return 0;
}

int board_fill_random_cell(board_t *b, int random_value) {
    int i;
    cell_t new_cell = (random_value & 1) ? CELL_NORMAL(4) : CELL_NORMAL(2);

    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        if (b->cells[i] == CELL_EMPTY) {
            b->cells[i] = new_cell;
            return 1;
        }
    }
    return 0;
}

/* ------------- UI ------------ */
void ui_setup() {
    atexit(&endwin);
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    clear();
}

void ui_draw(board_t *b) {
    int r, c, i;
    cell_t cell;

    for (r = 0; r < BOARD_ROWS; r++) {
        if (r == 0) {
            move(0, 0);
            for (i = 0; i < 1 + BOARD_COLUMNS * UI_CELL_COLS; i++) {
                addch('-' | A_BOLD);
            }
        }

        move(1 + 2*r, 0);
        for (c = 0; c < BOARD_COLUMNS; c++) {
            cell = b->cells[r * BOARD_COLUMNS + c];

            if (c == 0) {
                addch('|' | A_BOLD);
            }

            if (cell == CELL_EMPTY) {
                printw(UI_CELL_EMPTY);
            } else {
                printw(UI_CELL_FMT, CELL_VALUE(cell));
            }
        }

        move(2 + 2*r, 0);
        for (i = 0; i < 1 + BOARD_COLUMNS * UI_CELL_COLS; i++) {
            addch('-' | A_BOLD);
        }
    }

    move(1 + 2*BOARD_ROWS, 10);
    if (board_can_move(b)) {
        printw("USE ARROW KEYS TO MOVE; ESC OR q TO EXIT, r TO RESET");
    } else {
        printw("GAME OVER; ESC OR q TO EXIT, r TO RESET");
    }
}

int ui_key(board_t *b, int key) {
    switch (key) {
        case /* ESC */ 27:
        case 'Q':
        case 'q':
            /* exit */
            return 1;
        case 'r':
            board_reset(b);
            break;
        case KEY_UP:
            board_move(b, UP);
            break;
        case KEY_DOWN:
            board_move(b, DOWN);
            break;
        case KEY_RIGHT:
            board_move(b, RIGHT);
            break;
        case KEY_LEFT:
            board_move(b, LEFT);
            break;
    }
}

/* ------------- main -------------- */
int main() {
    board_t b;
    int quit = 0;
    int ch = 0;

    ui_setup();
    while (!quit) {
        ui_draw(b);
        ch = getch();
        quit = ui_key(b, ch);
    }

    return 0;
}
