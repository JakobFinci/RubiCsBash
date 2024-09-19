#pragma once

#include "constants.h"

/**
 * Allocate memory for an adjacent_face struct and fill it with provided values.
 *
 * Given an integer representing the value of the adjacent face and an array of
 * the indexes of the squares on that face sharing an edge, create storage for
 * an adjacent face on the heap. The caller is responsible for ensuring that the
 * memory is cleaned up after use.
 *
 * @param face_num An integer representing the value of the adjacent face (0-5).
 * @param squares An array of integers containing the indexes of the squares
 * adjacent to the edge.
 * @return A pointer to the new adjacent_face struct.
 */
adjacent_face* make_adjacent_face(int face_num,
                                  const int squares[N_SQUARES_PER_SIDE]);

/**
 * Free memory for an adjacent_face struct.
 *
 * @param adjacent_face_ A pointer to the adjacent_face struct to free.
 */
void free_adjacent_face(adjacent_face* adjacent_face_);

/**
 * Initialize the adjacent faces of a face struct.
 *
 * Given a face and its adjacency information, create a new adjacent_face struct
 * for each edge of the face. The memory for the adjacent_face structs is
 * dynamically allocated and managed by the face struct (i.e., the face will
 * take care of cleaning it up).
 *
 * @param new_face The face struct to create adjacency_faces for
 * @param adjacent_faces An array of integers containing the faces adjacent to
 * the edges of new_face (going clockwise along the perimeter).
 * @param adjacent_face_edge_squares An array containing arrays of integers that
 * contain the adjacent edge squares for each face of the cube. The arrays are
 * ordered based on the index of that face of the cube.
 */
void initialize_adjacent_faces(
    face* new_face, int* adjacent_faces,
    int adjacent_face_edge_squares[N_FACES][N_SQUARES_PER_SIDE]);

/**
 * Allocate memory for a face struct of the given color.
 *
 * Given an integer representing the value of the face, create storage for
 * the face on the heap. The face will automatically initialize its adjacent
 * face information based on its face value. The caller is responsible for
 * ensuring that the memory is cleaned up after use.
 *
 * @param face_num An integer representing the face index of the face to be
 * created (0-5).
 * @return A pointer to the new face struct.
 */
face* make_face(int face_num);

/**
 * Free memory for a face struct.
 *
 * @param face_ A pointer to the face struct to free.
 */
void free_face(face* face_);

/**
 * Allocate memory for a Rubik's cube.
 *
 * Create storage for a cube on the heap. The cube will be created as a
 * representation of a solved Rubik's cube. The caller is responsible for
 * ensuring that the memory is cleaned up after use.
 *
 * @return A pointer to the new cube struct.
 */
cube* make_cube();

/**
 * Free memory for a cube struct.
 *
 * @param cube_ A pointer to the cube struct to free.
 */
void free_cube(cube* cube_);

/**
 * Print a text representation of the cube to standard output.
 *
 * @param cube_ A pointer to the cube to print.
 */
void print_cube(cube* cube_);

/**
 * Perform a deep copy of an adjacent_face struct onto another adjacent_face
 * struct. The addresses of the given adjacent_face structs remain unchanged.
 *
 * @param original_face A pointer to the adjacent_face struct to be copied.
 * @param new_face A pointer to the adjacent_face struct that original_face is
 * copied to.
 */
void copy_adjacent_face(adjacent_face* original_face, adjacent_face* new_face);

/**
 * Perform a deep copy of a face struct onto another face struct. The addresses
 * of the given face structs remain unchanged.
 *
 * @param original_face A pointer to the face struct to be copied.
 * @param new_face A pointer to the face struct that original_face is copied to.
 */
void copy_face(face* original_face, face* new_face);

/**
 * Perform a deep copy of a cube struct onto another cube struct. The addresses
 * of the given cube structs remain unchanged.
 *
 * @param original_cube A pointer to the cube struct to be copied.
 * @param new_cube A pointer to the cube struct that original_cube is copied to.
 */
void copy_cube(cube* original_cube, cube* new_cube);

/**
 * Rotate the given face of a Rubik's cube in the given direction. This function
 * imitates the physical action of rotating a Rubik's cube in all aspects.
 *
 * @param cube_ A pointer to the cube struct to rotate a face of
 * @param face_num An integer representing the face index of the face to be
 * rotated (0-5).
 * @param direction An integer (0 or 1) representing the direction of the
 * rotation. 0 results in the face rotating clockwise and 1 results in the face
 * rotating counter-clockwise.
 */
void rotate(cube* cube_, int face_num, int direction);

/**
 * Shuffle the given Rubik's cube using pseudo-random values.
 *
 * @param cube_ A pointer to the cube struct to shuffle.
 * @param min_depth An integer representing the minimum number of times to turn
 * the cube during the shuffle.
 * @param max_depth An integer representing the maximum number of times to turn
 * the cube during the shuffle.
 */
void shuffle(cube* cube_, int min_depth, int max_depth);

/**
 * Checks if the given Rubik's cube is solved.
 *
 * @param cube_ A pointer to the cube struct to check.
 * @return 1 if the cube is solved and 0 if it is not.
 */
int is_solved(cube* cube_);
