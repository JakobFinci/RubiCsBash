#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stdio.h>

#include "../src/constants.h"
#include "../src/rubiks_start.h"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

// Test that make_adjacent_face returns the correct face number
Test(make_adjacent_face, face_1_int) {
  int face1 = 1;
  const int squares[N_SQUARES_PER_SIDE] = {1, 2, 3};
  adjacent_face* test = make_adjacent_face(face1, squares);
  cr_assert(eq(int, test->face, face1));
}
// Test that make_adjacent_face returns the correct edge squares
Test(make_adjacent_face, face_1_adjacent_faces) {
  int face1 = 1;
  const int squares[N_SQUARES_PER_SIDE] = {1, 2, 3};
  adjacent_face* test = make_adjacent_face(face1, squares);
  for (int ind = 0; ind < 3; ind++) {
    cr_assert(eq(int, test->edge_squares[ind], squares[ind]));
  }
}

// Use Valgrind instead of testing free_adjacent_face

// Test that initialize_adjacent_faces properly updates adjacent faces
Test(initialize_adjacent_faces, update_adjacent_faces) {
  face* new_face = (face*)malloc(sizeof(face));
  int adjacent_faces[N_EDGES_PER_SIDE] = {FACE_TOP, FACE_RIGHT, FACE_BOTTOM,
                                          FACE_LEFT};
  int adjacent_face_edge_squares[N_FACES][N_SQUARES_PER_SIDE] = {
      {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_LR, SQUARE_MR, SQUARE_UR},
      {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED}};
  initialize_adjacent_faces(new_face, adjacent_faces,
                            adjacent_face_edge_squares);

  cr_assert(eq(int, new_face->adjacent_faces[0]->face, 2));
  cr_assert(eq(int, new_face->adjacent_faces[1]->face, 1));
  cr_assert(eq(int, new_face->adjacent_faces[2]->face, 3));
  cr_assert(eq(int, new_face->adjacent_faces[3]->face, 4));
}

// Test that initialize_adjacency_lists properly updates first adjacent edge
// squares

Test(initialize_adjacent_faces, update_adjacent_edge_squares) {
  face* new_face = (face*)malloc(sizeof(face));
  int adjacent_faces[N_EDGES_PER_SIDE] = {FACE_TOP, FACE_RIGHT, FACE_BOTTOM,
                                          FACE_LEFT};
  int adjacent_face_edge_squares[N_FACES][N_SQUARES_PER_SIDE] = {
      {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_LR, SQUARE_MR, SQUARE_UR},
      {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED}};
  initialize_adjacent_faces(new_face, adjacent_faces,
                            adjacent_face_edge_squares);
  int edge_squares[3] = {0, 3, SQUARE_LL};
  cr_assert(
      eq(int, new_face->adjacent_faces[0]->edge_squares[0], edge_squares[0]));
  cr_assert(
      eq(int, new_face->adjacent_faces[0]->edge_squares[1], edge_squares[1]));
  cr_assert(
      eq(int, new_face->adjacent_faces[0]->edge_squares[2], edge_squares[2]));
}

// test that make face defines the correct colors

Test(make_face, face_4_colors) {
  int face_num = 4;
  face* face_four = make_face(face_num);
  cr_assert(eq(int, face_four->colors[0], face_num));
}

// test that make face defines the correct adjacent faces

Test(make_face, face_4_adjacent_faces) {
  int face_num = 4;
  face* face_four = make_face(face_num);
  face* test_face_four = (face*)malloc(sizeof(face));
  int adjacent_faces_4[N_EDGES_PER_SIDE] = {FACE_FRONT, FACE_BOTTOM, FACE_BACK,
                                            FACE_TOP};
  int adjacent_face_edge_squares_4[N_FACES][N_SQUARES_PER_SIDE] = {
      {SQUARE_UL, SQUARE_ML, SQUARE_LL},
      {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
      {SQUARE_UR, SQUARE_UM, SQUARE_UL},
      {SQUARE_LL, SQUARE_LM, SQUARE_LR},
      {SQUARE_UNDEFINED, SQUARE_UNDEFINED, SQUARE_UNDEFINED},
      {SQUARE_LR, SQUARE_MR, SQUARE_UR}};
  initialize_adjacent_faces(test_face_four, adjacent_faces_4,
                            adjacent_face_edge_squares_4);
  for (int ind = 0; ind < 4; ind++) {
    for (int jind = 0; jind < 3; jind++) {
      cr_assert(eq(int, face_four->adjacent_faces[ind]->edge_squares[jind],
                   test_face_four->adjacent_faces[ind]->edge_squares[jind]));
    }
  }
}

// test make cube
// not sure how best to test this one - ask team in class

// Use Valgrind instead of testing free_face

// Use Valgrind instead of testing free_cube

// test copy functions:
// test copy_adjacent_face:

// test that face of adjacent face is copied correctly
Test(copy_adjacent_face, copy_adjacent_face_check_face) {
  int face_three = 3;
  const int squares[N_SQUARES_PER_SIDE] = {0, 3, 6};
  adjacent_face* original = make_adjacent_face(face_three, squares);
  adjacent_face* copy = (adjacent_face*)malloc(sizeof(adjacent_face));
  copy_adjacent_face(original, copy);
  cr_assert(eq(int, original->face, copy->face));
}
// test that edge squares of adjacent face is copied correctly
Test(copy_adjacent_face, copy_adjacent_face_check_edge_squares) {
  int face_three = 3;
  const int squares[N_SQUARES_PER_SIDE] = {0, 3, 6};
  adjacent_face* original = make_adjacent_face(face_three, squares);
  adjacent_face* copy = (adjacent_face*)malloc(sizeof(adjacent_face));
  copy_adjacent_face(original, copy);
  for (int ind = 0; ind < 3; ind++) {
    cr_assert(eq(int, original->edge_squares[ind], copy->edge_squares[ind]));
  }
}

// test copy_face:
// test that colors are correctly copied
Test(copy_face, copy_face_check_colors) {
  int face_five = 3;
  face* original = make_face(face_five);
  face* copy = make_face(0);
  copy_face(original, copy);

  for (int ind = 0; ind < N_SQUARES_PER_FACE; ind++) {
    cr_assert(eq(int, original->colors[ind], copy->colors[ind]));
  }
}
// test that face adjacent faces are correctly copied
Test(copy_face, copy_face_check_adjacent_face_nums) {
  int face_five = 1;
  face* original = make_face(face_five);
  face* copy = make_face(0);
  copy_face(original, copy);

  for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
    cr_assert(eq(int, original->adjacent_faces[ind]->face,
                 copy->adjacent_faces[ind]->face));
  }
}
// test that face adjacent faces' edge squares are correctly copied
Test(copy_face, copy_face_check_adjacent_face_edge_squares) {
  int face_num = 2;
  face* original = make_face(face_num);
  face* copy = make_face(0);
  copy_face(original, copy);

  for (int ind = 0; ind < 4; ind++) {
    for (int jind = 0; jind < 3; jind++) {
      cr_assert(eq(int, original->adjacent_faces[ind]->edge_squares[jind],
                   copy->adjacent_faces[ind]->edge_squares[jind]));
    }
  }
}

// Test copy_cube:
// test that faces of cube have their adjacent faces edge squares correctly
// copied
Test(copy_cube, copy_cube_check_faces_adjacent_faces_edge_squares) {
  cube* original = make_cube();
  cube* copy = make_cube();

  for (int jind = 0; jind < N_FACES; jind++) {
    for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
      for (int kind = 0; kind < N_SQUARES_PER_SIDE; kind++) {
        // give impossible number to check that copy brings it back to normal
        copy->faces[jind]->adjacent_faces[ind]->edge_squares[kind] = 9;
      }
    }
  }

  copy_cube(original, copy);

  for (int jind = 0; jind < N_FACES; jind++) {
    for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
      for (int kind = 0; kind < N_SQUARES_PER_SIDE; kind++) {
        cr_assert(
            eq(int, copy->faces[jind]->adjacent_faces[ind]->edge_squares[kind],
               original->faces[jind]->adjacent_faces[ind]->edge_squares[kind]));
      }
    }
  }
}

// test that faces of cube have their adjacent faces face nums correctly
// copied
Test(copy_cube, copy_cube_check_faces_adjacent_faces_face_nums) {
  cube* original = make_cube();
  cube* copy = make_cube();
  copy_cube(original, copy);

  for (int jind = 0; jind < N_FACES; jind++) {
    for (int ind = 0; ind < N_EDGES_PER_SIDE; ind++) {
      // give impossible number to check that copy brings it back to normal
      copy->faces[jind]->adjacent_faces[ind]->face = 9;
    }
  }
  for (int ind = 0; ind < N_FACES; ind++) {
    for (int jind = 0; jind < N_SQUARES_PER_FACE; jind++) {
      cr_assert(eq(int, original->faces[ind]->colors[jind],
                   copy->faces[ind]->colors[jind]));
    }
  }
}

Test(copy_cube, copy_cube_check_faces_colors) {
  cube* original = make_cube();
  cube* copy = make_cube();
  for (int ind = 0; ind < N_SQUARES_PER_FACE; ind++) {
    // give impossible number to check that copy brings it back to normal
    copy->faces[0]->colors[ind] = 9;
  }
  copy_cube(original, copy);

  for (int ind = 0; ind < N_FACES; ind++) {
    for (int jind = 0; jind < N_SQUARES_PER_FACE; jind++) {
      cr_assert(eq(int, original->faces[ind]->colors[jind],
                   copy->faces[ind]->colors[jind]));
    }
  }
}

// test rotate functions:

// test that transpose_face_clockwise correctly transposes a face
Test(rotate, test_rotate_clockwise_front_face) {
  cube* original = make_cube();
  cube* example = make_cube();
  int face_num = 0;
  original->faces[0]->colors[0] = 9;
  example->faces[0]->colors[2] = 9;
  rotate(original, face_num, 0);
  for (int ind = 0; ind < N_SQUARES_PER_FACE; ind++) {
    cr_assert(eq(int, original->faces[0]->colors[ind],
                 example->faces[0]->colors[ind]));
  }
}

// test that transpose_face_counterclockwise correctly transposes a face
Test(rotate, test_rotate_counterclockwise_front_face) {
  cube* original = make_cube();
  cube* example = make_cube();
  int face_num = 0;
  original->faces[0]->colors[0] = 9;
  example->faces[0]->colors[6] = 9;
  rotate(original, face_num, 1);
  for (int ind = 0; ind < 9; ind++) {
    cr_assert(eq(int, original->faces[0]->colors[ind],
                 example->faces[0]->colors[ind]));
  }
}

// test that rotate correctly affects adjacent faces, clockwise, from last to
// first
Test(rotate, test_rotate_clockwise_other_faces) {
  cube* original = make_cube();
  cube* example = make_cube();
  int face_num = 1;
  original->faces[3]->colors[2] = 9;
  example->faces[0]->colors[8] = 9;
  example->faces[0]->colors[5] = 3;
  example->faces[0]->colors[2] = 3;
  rotate(original, face_num, 0);
  for (int ind = 8; ind > 0; ind -= 3) {
    cr_assert(eq(int, original->faces[0]->colors[ind],
                 example->faces[0]->colors[ind]));
  }
}

// test that rotate correctly affects adjacent faces, counterclockwise

Test(rotate, test_rotate_counterclockwise_other_faces) {
  cube* original = make_cube();
  cube* example = make_cube();
  int face_num = 1;
  original->faces[5]->colors[0] = 9;
  example->faces[2]->colors[6] = 9;
  example->faces[2]->colors[7] = 5;
  example->faces[2]->colors[8] = 5;
  rotate(original, face_num, 1);
  for (int ind = 6; ind < 9; ind++) {
    cr_assert(eq(int, original->faces[2]->colors[ind],
                 example->faces[2]->colors[ind]));
  }
}

// test that rotate correctly affects adjacent faces, counterclockwise, from
// first to last adjacent face
Test(rotate, test_rotate_counterclockwise_other_faces_first_to_last) {
  cube* original = make_cube();
  cube* example = make_cube();
  int face_num = 3;
  original->faces[0]->colors[6] = 9;
  example->faces[4]->colors[6] = 9;
  example->faces[4]->colors[7] = 0;
  example->faces[4]->colors[8] = 0;
  rotate(original, face_num, 1);
  for (int ind = 6; ind < 9; ind++) {
    cr_assert(eq(int, original->faces[4]->colors[ind],
                 example->faces[4]->colors[ind]));
  }
}

// test shuffle functions:
// since this function uses random numbers, there is not a lot we can test.

// test that shuffle doesn't change a cube when min_depth and max_depth are 0.
Test(shuffle, test_shuffle_min_max_zero) {
  cube* original = make_cube();
  cube* example = make_cube();
  shuffle(original, 0, 0);
  for (int ind = 0; ind < N_FACES; ind++) {
    for (int jind = 0; jind < N_SQUARES_PER_FACE; jind++) {
      cr_assert(eq(int, original->faces[ind]->colors[jind],
                   example->faces[ind]->colors[jind]));
    }
  }
}

// test is_solved functions:

// test that a cube that is solved returns 1.
Test(is_solved, test_solved_cube) {
  cube* cube_ = make_cube();
  cr_assert(eq(int, is_solved(cube_), 1));
}

// test that a cube with one incorrect square returns 0.
Test(is_solved, test_one_wrong_square) {
  cube* cube_ = make_cube();
  cube_->faces[4]->colors[7] = 5;
  cr_assert(eq(int, is_solved(cube_), 0));
}

// test that a cube that has been rotated returns 0.
Test(is_solved, rotated) {
  cube* cube_ = make_cube();
  rotate(cube_, 0, 0);
  cr_assert(eq(int, is_solved(cube_), 0));
}

// test that a shuffled cube returns 0 (there is a very unlikely chance that the
// shuffling undoes itself and the test will fail in that case).
Test(is_solved, shuffled) {
  cube* cube_ = make_cube();
  shuffle(cube_, 20, 20);
  cr_assert(eq(int, is_solved(cube_), 0));
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
