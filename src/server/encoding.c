#include "encoding.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "rubiks_start.h"

int encode(cube* to_encode, char* result) {
  for (int i = 0; i < N_FACES; ++i) {
    for (int j = 0; j < N_SQUARES_PER_FACE; ++j) {
      int index = i * N_SQUARES_PER_FACE + j;
      result[index] = (char)('0' + to_encode->faces[i]->colors[j]);
    }
  }
  result[N_CHARACTERS_TO_ENCODE_CUBE - 2] = '\n';
  result[N_CHARACTERS_TO_ENCODE_CUBE - 1] = '\0';
  printf("Inside of encode, the result is ");
  puts(result);
  return 0;
}

void decode(cube* result, const char* to_decode) {
  for (int i = 0; i < N_FACES; i++) {
    for (int j = 0; j < N_SQUARES_PER_FACE; j++) {
      int index = i * N_SQUARES_PER_FACE + j;
      result->faces[i]->colors[j] = to_decode[index] - '0';
    }
  }
}
