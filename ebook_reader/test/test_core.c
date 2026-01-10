#include <stdio.h>
#include <unity.h>

#include "conftest.h"
#include "core/core.h"
#include "core/core_internal.h"
#include "utils/error.h"

ebk_core_t core;

void setUp(void) {
  core = NULL;
  enable_ebk_display_init_mock = true;
  enable_ebk_display_show_boot_img_mock = true;
  enable_ebk_display_destroy_mock = true;
  enable_ebk_gui_init_mock = true;
  enable_ebk_gui_tick_mock = true;
  enable_ebk_gui_destroy_mock = true;
}

void tearDown() { ebk_core_destroy(&core); }

void test_ebk_core_init_success(void) {
  ebk_errno = ebk_core_init(&core);
  TEST_ASSERT_EQUAL(0, ebk_errno);

  TEST_ASSERT_EQUAL_STRING(ebk_core_edump(ebk_CoreEventEnum_NONE),
                           ebk_core_edump(core->ev_data.event));
  TEST_ASSERT_EQUAL_STRING(ebk_core_sdump(ebk_CoreStateEnum_MENU),
                           ebk_core_sdump(core->state));

  TEST_ASSERT_TRUE(core->ctx.display != NULL);
  TEST_ASSERT_TRUE(core->ctx.gui != NULL);

  for (int i = ebk_CoreStateEnum_MENU; i <= ebk_CoreStateEnum_ERROR; i++) {
    printf("%d\n", i);
    TEST_ASSERT_TRUE(core->modules[i].open != NULL);
    TEST_ASSERT_TRUE(core->modules[i].close != NULL);
    TEST_ASSERT_TRUE(core->modules[i].destroy != NULL);
  }
}


