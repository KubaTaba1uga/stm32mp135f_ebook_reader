#include <stdint.h>
#include <unity.h>

#include "turtle_horizontal.h"
#include "turtle_vertical.h"

int rotate(unsigned char *buf, uint32_t buf_len) { return 0; }

void test_rotate(void) {
  int err = rotate(turtle_horizontal, sizeof(turtle_horizontal));
  TEST_ASSERT_EQUAL(0, err);

  TEST_ASSERT_EQUAL_CHAR_ARRAY(turtle_vertical, turtle_horizontal,
                               sizeof(turtle_vertical));
}
