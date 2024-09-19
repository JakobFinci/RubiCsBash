#include "rubiks_start.h"

#include <stdint.h>  // uint16_t
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// time

#include "constants.h"

adjacent_face* make_adjacent_face(int face_num,
                                  const int squares[N_SQUARES_PER_SIDE]) {
  adjacent_face* new_adjacent_face =
      (adjacent_face*)malloc(sizeof(adjacent_face));
  new_adjacent_face->face = face_num;
  for (int ind = 0; ind < N_SQUARES_PER_SIDE; ind++) {
    new_adjacent_face->edge_squares[ind] = squares[ind];
  }
  return new_adjacent_face;
}

void free_adjacent_face(adjacent_face* adjacent_face_) { free(adjacent_face_); }

void initialize_adjacent_faces(
    face* new_face, int* adjacent_faces,
    int adjacent_face_edge_squares[N_FACES][N_SQUARES_PER_SIDE]) {
  for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
    new_face->adjacent_faces[ind] = make_adjacent_face(
        adjacent_faces[ind], adjacent_face_edge_squares[adjacent_faces[ind]]);
  }
}

face* make_face(int face_num) {
  face* new_face = (face*)malloc(sizeof(face));
  for (int ind = 0; ind < N_SQUARES_PER_FACE; ind++) {
    new_face->colors[ind] = face_num;
  }

  switch (face_num) {
    case FACE_FRONT:;
      int adjacent_faces_0[N_EDGES_PER_SIDE] = {FACE_TOP, FACE_RIGHT,
                                                FACE_BOTTOM, FACE_LEFT};
      int adjacent_face_edge_squares_0[N_FACES][N_SQUARES_PER_SIDE] = {
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_UL, SQUARE_ML, SQUARE_LL},
          {SQUARE_UL, SQUARE_ML, SQUARE_LL},
          {SQUARE_UL, SQUARE_ML, SQUARE_LL},
          {SQUARE_LR, SQUARE_MR, SQUARE_UR},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED}};
      initialize_adjacent_faces(new_face, adjacent_faces_0,
                                adjacent_face_edge_squares_0);
      break;
    case FACE_RIGHT:;
      int adjacent_faces_1[N_EDGES_PER_SIDE] = {FACE_FRONT, FACE_TOP, FACE_BACK,
                                                FACE_BOTTOM};
      int adjacent_face_edge_squares_1[N_FACES][N_SQUARES_PER_SIDE] = {
          {SQUARE_LR, SQUARE_MR, SQUARE_UR},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_LL, SQUARE_LM, SQUARE_LR},
          {SQUARE_UR, SQUARE_UM, SQUARE_UL},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_UL, SQUARE_ML, SQUARE_LL}};
      initialize_adjacent_faces(new_face, adjacent_faces_1,
                                adjacent_face_edge_squares_1);
      break;
    case FACE_TOP:;
      int adjacent_faces_2[N_EDGES_PER_SIDE] = {FACE_FRONT, FACE_LEFT,
                                                FACE_BACK, FACE_RIGHT};
      int adjacent_face_edge_squares_2[N_FACES][N_SQUARES_PER_SIDE] = {
          {SQUARE_UR, SQUARE_UM, SQUARE_UL},
          {SQUARE_UR, SQUARE_UM, SQUARE_UL},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_UR, SQUARE_UM, SQUARE_UL},
          {SQUARE_UR, SQUARE_UM, SQUARE_UL}};
      initialize_adjacent_faces(new_face, adjacent_faces_2,
                                adjacent_face_edge_squares_2);
      break;

    case FACE_BOTTOM:;
      int adjacent_faces_3[N_EDGES_PER_SIDE] = {FACE_FRONT, FACE_RIGHT,
                                                FACE_BACK, FACE_LEFT};
      int adjacent_face_edge_squares_3[N_FACES][N_SQUARES_PER_SIDE] = {
          {SQUARE_LL, SQUARE_LM, SQUARE_LR},
          {SQUARE_LL, SQUARE_LM, SQUARE_LR},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_LL, SQUARE_LM, SQUARE_LR},
          {SQUARE_LL, SQUARE_LM, SQUARE_LR}};
      initialize_adjacent_faces(new_face, adjacent_faces_3,
                                adjacent_face_edge_squares_3);
      break;

    case FACE_LEFT:;
      int adjacent_faces_4[N_EDGES_PER_SIDE] = {FACE_FRONT, FACE_BOTTOM,
                                                FACE_BACK, FACE_TOP};
      int adjacent_face_edge_squares_4[N_FACES][N_SQUARES_PER_SIDE] = {
          {SQUARE_UL, SQUARE_ML, SQUARE_LL},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_UR, SQUARE_UM, SQUARE_UL},
          {SQUARE_LL, SQUARE_LM, SQUARE_LR},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_LR, SQUARE_MR, SQUARE_UR}};
      initialize_adjacent_faces(new_face, adjacent_faces_4,
                                adjacent_face_edge_squares_4);
      break;

    case FACE_BACK:;
      int adjacent_faces_5[N_EDGES_PER_SIDE] = {FACE_RIGHT, FACE_TOP, FACE_LEFT,
                                                FACE_BOTTOM};
      int adjacent_face_edge_squares_5[N_FACES][N_SQUARES_PER_SIDE] = {
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
          {SQUARE_LR, SQUARE_MR, SQUARE_UR},
          {SQUARE_LR, SQUARE_MR, SQUARE_UR},
          {SQUARE_LR, SQUARE_MR, SQUARE_UR},
          {SQUARE_UL, SQUARE_ML, SQUARE_LL},
          {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED}};
      initialize_adjacent_faces(new_face, adjacent_faces_5,
                                adjacent_face_edge_squares_5);
      break;
    default:
      // error_and_exit("Only cases 0-5 are valid.");
      break;
  }
  return new_face;
}

void free_face(face* face_) {
  for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
    free_adjacent_face((face_->adjacent_faces)[ind]);
  }
  free(face_);
}

cube* make_cube() {
  cube* new_cube = (cube*)malloc(sizeof(cube));
  for (int face_num = 0; face_num < N_FACES; face_num++) {
    new_cube->faces[face_num] = make_face(face_num);
  }
  return new_cube;
}

void free_cube(cube* cube_) {
  for (int face_num = 0; face_num < N_FACES; face_num++) {
    free_face((cube_->faces)[face_num]);
  }
  free(cube_);
}

void print_cube(cube* cube_) {
  printf("Cube with faces:\n");
  for (size_t face_id = 0; face_id < N_FACES; face_id++) {
    printf("  Face %lu:\n", face_id);
    for (size_t square = 0; square < N_SQUARES_PER_FACE; square++) {
      printf(" %d", cube_->faces[face_id]->colors[square]);
    }
  }
}

void copy_adjacent_face(adjacent_face* original_face, adjacent_face* new_face) {
  new_face->face = original_face->face;
  for (int ind = 0; ind < N_SQUARES_PER_SIDE; ind++) {
    new_face->edge_squares[ind] = original_face->edge_squares[ind];
  }
}

void copy_face(face* original_face, face* new_face) {
  // copy the square colors
  for (int square_num = 0; square_num < N_SQUARES_PER_FACE; square_num++) {
    new_face->colors[square_num] = original_face->colors[square_num];
  }
  // copy the list of adjacent faces
  for (int adjacent_face_num = 0; adjacent_face_num < N_EDGES_PER_SIDE;
       adjacent_face_num++) {
    copy_adjacent_face(original_face->adjacent_faces[adjacent_face_num],
                       new_face->adjacent_faces[adjacent_face_num]);
  }
}

void copy_cube(cube* original_cube, cube* new_cube) {
  for (int face_num = 0; face_num < N_FACES; face_num++) {
    copy_face(original_cube->faces[face_num], new_cube->faces[face_num]);
  }
}

static void transpose_face_clockwise(face* original_face,
                                     face* new_face_to_populate) {
  /* Rotate a face in a clockwise manner by moving the squares of the
  original face onto their new location on the new face. */
  new_face_to_populate->colors[SQUARE_UL] = original_face->colors[SQUARE_LL];
  new_face_to_populate->colors[SQUARE_UM] = original_face->colors[SQUARE_ML];
  new_face_to_populate->colors[SQUARE_UR] = original_face->colors[SQUARE_UL];
  new_face_to_populate->colors[SQUARE_ML] = original_face->colors[SQUARE_LM];
  new_face_to_populate->colors[SQUARE_MM] = original_face->colors[SQUARE_MM];
  new_face_to_populate->colors[SQUARE_MR] = original_face->colors[SQUARE_UM];
  new_face_to_populate->colors[SQUARE_LL] = original_face->colors[SQUARE_LR];
  new_face_to_populate->colors[SQUARE_LM] = original_face->colors[SQUARE_MR];
  new_face_to_populate->colors[SQUARE_LR] = original_face->colors[SQUARE_UR];
}

static void transpose_face_counterclockwise(face* original_face,
                                            face* new_face_to_populate) {
  /* Rotate a face in a counter-clockwise manner by moving the squares of the
  original face onto their new location on the new face. */
  new_face_to_populate->colors[SQUARE_UL] = original_face->colors[SQUARE_UR];
  new_face_to_populate->colors[SQUARE_UM] = original_face->colors[SQUARE_MR];
  new_face_to_populate->colors[SQUARE_UR] = original_face->colors[SQUARE_LR];
  new_face_to_populate->colors[SQUARE_ML] = original_face->colors[SQUARE_UM];
  new_face_to_populate->colors[SQUARE_MM] = original_face->colors[SQUARE_MM];
  new_face_to_populate->colors[SQUARE_MR] = original_face->colors[SQUARE_LM];
  new_face_to_populate->colors[SQUARE_LL] = original_face->colors[SQUARE_UL];
  new_face_to_populate->colors[SQUARE_LM] = original_face->colors[SQUARE_ML];
  new_face_to_populate->colors[SQUARE_LR] = original_face->colors[SQUARE_LL];
}

static void transpose_other_faces_clockwise(cube* cube_,
                                            cube* new_cube_to_populate,
                                            int face_num) {
  /* Rotate the squares adjacent to a face being rotated clockwise by writing
  the squares of the original face onto their new location on the new face */
  int old_color = 0;
  adjacent_face* current_adjacent_face = NULL;
  adjacent_face* next_adjacent_face = NULL;
  for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
    current_adjacent_face = cube_->faces[face_num]->adjacent_faces[ind];
    if (ind < N_EDGES_PER_SIDE - 1) {
      next_adjacent_face = cube_->faces[face_num]->adjacent_faces[ind + 1];
    } else {
      next_adjacent_face = cube_->faces[face_num]->adjacent_faces[0];
    }
    for (int jind = 0; jind < N_SQUARES_PER_SIDE; jind++) {
      old_color = cube_->faces[current_adjacent_face->face]
                      ->colors[current_adjacent_face->edge_squares[jind]];
      // set the color of the square to the color of the old one
      new_cube_to_populate->faces[next_adjacent_face->face]
          ->colors[next_adjacent_face->edge_squares[jind]] = old_color;
    }
  }
}

static void transpose_other_faces_counterclockwise(cube* cube_,
                                                   cube* new_cube_to_populate,
                                                   int face_num) {
  /* Rotate the squares adjacent to a face being rotated counter-clockwise by
  writing the squares of the original face onto their new location on the new
  face */
  int old_color = 0;
  adjacent_face* current_adjacent_face = NULL;
  adjacent_face* next_adjacent_face = NULL;
  for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
    current_adjacent_face = cube_->faces[face_num]->adjacent_faces[ind];
    if (ind < 1) {
      next_adjacent_face =
          cube_->faces[face_num]->adjacent_faces[N_EDGES_PER_SIDE - 1];
    } else {
      next_adjacent_face = cube_->faces[face_num]->adjacent_faces[ind - 1];
    }
    for (int jind = 0; jind < N_SQUARES_PER_SIDE; jind++) {
      old_color = cube_->faces[current_adjacent_face->face]
                      ->colors[current_adjacent_face->edge_squares[jind]];
      // set the color of the square to the color of the old one
      new_cube_to_populate->faces[next_adjacent_face->face]
          ->colors[next_adjacent_face->edge_squares[jind]] = old_color;
    }
  }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void rotate(cube* cube_, int face_num, int direction) {
  cube* cube_copy = make_cube();
  copy_cube(cube_, cube_copy);
  if (direction == 0) {
    transpose_face_clockwise(cube_->faces[face_num],
                             cube_copy->faces[face_num]);
    transpose_other_faces_clockwise(cube_, cube_copy, face_num);
  } else {
    transpose_face_counterclockwise(cube_->faces[face_num],
                                    cube_copy->faces[face_num]);
    transpose_other_faces_counterclockwise(cube_, cube_copy, face_num);
  }
  copy_cube(cube_copy, cube_);
  free_cube(cube_copy);
}

void shuffle(cube* cube_, int min_depth, int max_depth) {
  unsigned int seed = (unsigned int)time(NULL);
  int depth = rand_r(&seed) % (max_depth - min_depth + 1) + min_depth;
  for (int i = 0; i < depth; i++) {
    rotate(cube_, rand_r(&seed) % N_FACES, rand_r(&seed) % 2);
  }
}

int is_solved(cube* cube_) {
  for (int ind = 0; ind < N_FACES; ind++) {
    for (int jind = 0; jind < N_SQUARES_PER_FACE; jind++) {
      if (cube_->faces[ind]->colors[jind] != ind) {
        return 0;
      }
    }
  }
  return 1;
}
