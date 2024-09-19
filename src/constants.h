#pragma once

#include <stdlib.h>

typedef enum {
  N_FACES = 6,
  N_SQUARES_PER_SIDE = 3,
  N_EDGES_PER_SIDE = 4,
  N_SQUARES_PER_FACE = 9,
  N_CHARACTERS_TO_ENCODE_CUBE = 56,
  N_CHARACTERS_TO_ENCODE_MOVE = 4,
} cube_parameters;

typedef enum {
  FACE_FRONT = 0,
  FACE_RIGHT = 1,
  FACE_TOP = 2,
  FACE_BOTTOM = 3,
  FACE_LEFT = 4,
  FACE_BACK = 5,
} face_ids;

typedef struct {
  int face;
  int edge_squares[N_SQUARES_PER_SIDE];
} adjacent_face;

typedef struct {
  int colors[N_SQUARES_PER_SIDE * N_SQUARES_PER_SIDE];
  adjacent_face* adjacent_faces[N_EDGES_PER_SIDE];
} face;

typedef struct {
  face* faces[N_FACES];
} cube;

union intToChar {
  int i;
  char c[4];
};

typedef enum {
  SQUARE_UL = 0,
  SQUARE_UM = 1,
  SQUARE_UR = 2,
  SQUARE_ML = 3,
  SQUARE_MM = 4,
  SQUARE_MR = 5,
  SQUARE_LL = 6,
  SQUARE_LM = 7,
  SQUARE_LR = 8,
  SQUARE_UNDEFINED = -1,
} square_ids;

typedef enum {
  MIN_DEPTH = 20,
  MAX_DEPTH = 30,
} shuffle_depths;
