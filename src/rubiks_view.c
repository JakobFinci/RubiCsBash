#include <ncurses.h>

#include "constants.h"
#include "rubiks_start.h"

int main(void) {
  const short COLOR_ORANGE = 8;
  const short RGB_MAX = 1000;
  const int CURSOR_SPACES = 30;
  const int CURSOR_NEW_LINES = CURSOR_SPACES / 2;
  int input = 0;

  // Create cube
  cube* rubiks_cube = make_cube();
  shuffle(rubiks_cube, MIN_DEPTH, MAX_DEPTH);
  int current_face = 0;

  // Start curses
  initscr();
  noecho();
  start_color();
  clear();
  refresh();

  // Initialize colors
  init_color(COLOR_ORANGE, RGB_MAX, (short)(RGB_MAX / 2), 0);
  init_pair(FACE_FRONT + 1, COLOR_BLACK, COLOR_WHITE);
  init_pair(FACE_RIGHT + 1, COLOR_BLACK, COLOR_RED);
  init_pair(FACE_TOP + 1, COLOR_BLACK, COLOR_BLUE);
  init_pair(FACE_BOTTOM + 1, COLOR_BLACK, COLOR_GREEN);
  init_pair(FACE_LEFT + 1, COLOR_BLACK, COLOR_ORANGE);
  init_pair(FACE_BACK + 1, COLOR_BLACK, COLOR_YELLOW);

  while (1) {
    // Display cube face
    for (int row = 0; row < N_SQUARES_PER_SIDE; row++) {
      for (int col = 0; col < N_SQUARES_PER_SIDE; col++) {
        attron(COLOR_PAIR(rubiks_cube->faces[current_face]
                              ->colors[row * N_SQUARES_PER_SIDE + col] +
                          1));
        for (int current_line = 0; current_line < CURSOR_NEW_LINES - 1;
             current_line++) {
          move(row * CURSOR_NEW_LINES + current_line, col * CURSOR_SPACES);
          for (int current_char = 0; current_char < CURSOR_SPACES - 1;
               current_char++) {
            printw(" ");
          }
          printw("\n");
        }
      }
    }

    // Get input
    input = getch();  // curses call to input from keyboard
    switch (input) {
      case 'a':
        rotate(rubiks_cube, current_face, 1);
        break;
      case 'd':
        rotate(rubiks_cube, current_face, 0);
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
        current_face = input - '0';
        break;
      case ' ':
        if (is_solved(rubiks_cube)) {
          shuffle(rubiks_cube, MIN_DEPTH, MAX_DEPTH);
        } else {
          free_cube(rubiks_cube);
          rubiks_cube = make_cube();
        }
        break;
      default:
        break;
    }
    if (input == 'q' || input == 'Q') {
      break;
    }
  }

  // Close window
  endwin();

  // Clean-up
  free_cube(rubiks_cube);
  return 0;
}
