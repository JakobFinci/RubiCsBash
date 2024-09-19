#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stdio.h>

#include "../src/rubiks_start.h"
#include "../src/server/encoding.h"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

// encode function is causing tests to crash
Test(encode, encode_properly) {
  char result[6 * 9];
  cube* original = make_cube();
  original->faces[0]->colors[6] = 9;
  cube* decoded = make_cube();

  encode(original, result);
  const char* to_decode = result;
  decode(decoded, to_decode);

  for (int ind = 0; ind < 9; ind++) {
    cr_assert(eq(int, original->faces[4]->colors[ind],
                 decoded->faces[4]->colors[ind]));
  }
}

Test(decode, decode_properly) {
  char result[6 * 9];
  cube* original = make_cube();
  original->faces[5]->colors[2] = 9;
  cube* decoded = make_cube();

  encode(original, result);
  const char* to_decode = result;

  decode(decoded, to_decode);

  for (int ind = 0; ind < 9; ind++) {
    cr_assert(eq(int, original->faces[5]->colors[ind],
                 decoded->faces[5]->colors[ind]));
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
