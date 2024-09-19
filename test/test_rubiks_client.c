#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stdio.h>

#include "../src/constants.h"
#include "../src/server/rubiks_client.h"

// test encoding user input
Test(encode_user_input, encodes_properly) {
  int face_num = 2;
  int input = 'a';
  char encoded[4];
  encode_user_input(input, face_num, encoded);
  cr_assert(eq(chr, encoded[0], '2'));
  cr_assert(eq(chr, encoded[1], '1'));
}
