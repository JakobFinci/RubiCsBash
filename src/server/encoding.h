#pragma once
#include "../constants.h"

/**
 * Encode a cube to a string
 *
 * Encode a struct representing a full Rubik's cube to a character array, with
 * the first bytes containing information pertinent to decoding (num of faces
 * and num of cubes per face) and the latter bytes containing face information.
 *
 * @param to_encode a pointer to a cube struct to be encoded.
 * @param result a char pointer to have the result of encoding.
 * @return result A character array representing an encoded Rubik's cube.
 */
int encode(cube* to_encode, char* result);

/**
 * Decode a string to a cube
 *
 * Decode an encoded string representing a full Rubik's cube to a struct
 * representing that cube. Encoded string should be created with above encode
 * function.
 *
 * @param to_decode a encoded character array to be decoded.
 * @return A pointer to a struct representing a decoded cube.
 */
void decode(cube* result, const char* to_decode);
