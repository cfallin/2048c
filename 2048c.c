/*
 * 2048c -- a console implementation of 2048
 *
 * Copyright (c) 2017, Chris Fallin <cfallin@c1f.net>
 *
 * Released under the MIT License.
 */

#include <assert.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "2048c.h"

/* ------------- cells and board manipulations ------------ */

void board_reset(board_t *b) {
    int i;
    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        b->cells[i] = CELL_EMPTY;
    }
}

int board_move_column(cell_t *cells, int stride, int count) {
    int i, j;
    int collapsed = 0;

    /* first, shift down into empty spots */
    for (i = 0, j = 0; i < count; i++) {
        cell_t value = cells[i*stride];
        if (value != CELL_EMPTY) {
            cells[j*stride] = value;
            j++;
        }
    }
    for (; j < count; j++) {
        cells[j*stride] = CELL_EMPTY;
    }

    /* second, attempt merges */
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
            collapsed |= 1;
        }
    }

    return collapsed;
}

void board_flip_collapsed_bit(cell_t *cells, int count) {
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
                collapsed |= board_move_column(
                        &b->cells[i],
                        /* stride = */ BOARD_COLUMNS,
                        /* count = */ BOARD_ROWS);
            }
            break;
        case DOWN:
            for (i = 0; i < BOARD_COLUMNS; i++) {
                collapsed |= board_move_column(
                        &b->cells[(BOARD_ROWS - 1)*BOARD_COLUMNS + i],
                        /* stride = */ -BOARD_COLUMNS,
                        /* count = */ BOARD_ROWS);
            }
            break;
        case LEFT:
            for (i = 0; i < BOARD_ROWS; i++) {
                collapsed |= board_move_column(
                        &b->cells[i * BOARD_COLUMNS],
                        /* stride = */ 1,
                        /* count = */ BOARD_COLUMNS);
            }
            break;
        case RIGHT:
            for (i = 0; i < BOARD_ROWS; i++) {
                collapsed |= board_move_column(
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
    board_t copy;
    dir_t dir;
    int i;

    /* any empty cell means we can move */
    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        if (b->cells[i] == CELL_EMPTY) {
            return 1;
        }
    }

    /* no empty cells, but a merge may still be possible */
    copy = *b;
    for (dir = DIR_FIRST; dir <= DIR_LAST; dir++) {
        if (board_move(&copy, dir)) {
            return 1;
        }
    }

    return 0;
}

int board_is_win(board_t *b) {
    int i;

    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        if (b->cells[i] == CELL_NORMAL(2048)) {
            return 1;
        }
    }
    return 0;
}

int board_fill_random_cell(board_t *b, int random_value) {
    int i;
    int empties;
    int cell_idx;
    cell_t new_cell;

    /* compute a new cell value (always 2 or 4) */
    new_cell = (random_value & 1) ? CELL_NORMAL(4) : CELL_NORMAL(2);

    /* find a place to insert new cell, randomly: first count empty
     * cells, then pick an index, then find that cell */
    empties = 0;
    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        if (b->cells[i] == CELL_EMPTY) {
            empties++;
        }
    }
    if (empties == 0) {
        return 0;
    }

    cell_idx = (random_value >> 1) % empties;
    empties = 0;
    for (i = 0; i < BOARD_ROWS * BOARD_COLUMNS; i++) {
        if (b->cells[i] == CELL_EMPTY) {
            if (empties == cell_idx) {
                b->cells[i] = new_cell;
                return 1;
            } else {
                empties++;
            }
        }
    }

    /* shouldn't happen */
    assert(false);
}

/* ------------- UI ------------ */
void ui_teardown() {
    endwin();
}

void ui_setup() {
    atexit(&ui_teardown);
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
            for (i = 0; i < UI_ROW_WIDTH; i++) {
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

            addch('|' | A_BOLD);
        }

        move(2 + 2*r, 0);
        for (i = 0; i < UI_ROW_WIDTH; i++) {
            addch('-' | A_BOLD);
        }
    }

    move(1 + 2*BOARD_ROWS, 1);
    if (board_is_win(b)) {
        printw("YOU'VE WON! KEEP PLAYING IF YOU'D LIKE.");
    } else if (!board_can_move(b)) {
        printw("GAME OVER!");
    }
    move(3 + 2*BOARD_ROWS, 1);
    printw("ESC OR q TO EXIT, r TO RESET");
    move(5 + 2*BOARD_ROWS, 1);
    printw("2048c v0.1");
    move(6 + 2*BOARD_ROWS, 1);
    printw("https://cfallin.github.io/2048c/");
    move(7 + 2*BOARD_ROWS, 1);
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
    return 0;
}

/* ------------- main -------------- */
int main() {
    board_t b;
    int quit = 0;
    int ch = 0;

    srand((int)time(NULL));

    board_reset(&b);
    ui_setup();
    while (!quit) {
        board_fill_random_cell(&b, rand());
        ui_draw(&b);
        ch = getch();
        quit = ui_key(&b, ch);
    }

    return 0;
}
