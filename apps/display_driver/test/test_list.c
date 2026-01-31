#include <unity.h>
#include <stdbool.h>
#include <stdint.h>

#include "utils/list.h"
#include "utils/err.h"

static int int_ptr_cmp(void *node_data, void *data) {
  int a = *(int *)node_data;
  int b = *(int *)data;
  return (a == b) ? 0 : 1;
}

static void noop_destroy(void *data) {
  (void)data;
}

static void counter_destroy(void *data) {
  int *counter = (int *)data;
  (*counter)++;
}

static struct dd_ListNode *mk_node(void *data, struct dd_ListNode *next) {
  struct dd_ListNode *n = dd_malloc(sizeof(*n));
  TEST_ASSERT_NOT_NULL(n);
  n->data = data;
  n->next = next;
  return n;
}

static struct dd_List list;

void setUp(void) {
  dd_errno = 0;
  list = DD_LIST_INITIALIZER;
}

void tearDown(void) {
  dd_list_destroy(&list, NULL);
}

void test_dd_list_is_empty_fresh_initializer(void) {
  TEST_ASSERT_TRUE(dd_list_is_empty(&list));
  TEST_ASSERT_NULL(list.head);
}

void test_dd_list_append_null_list_sets_errno(void) {
  dd_error_t err = dd_list_append(NULL, (void *)0x1234);
  TEST_ASSERT_NOT_EQUAL(0, err);
  TEST_ASSERT_NOT_EQUAL(0, dd_errno);
}

void test_dd_list_append_appends_to_tail_when_head_exists(void) {
  int a = 1, b = 2;

  list.head = mk_node(&a, NULL);

  dd_error_t err = dd_list_append(&list, &b);
  TEST_ASSERT_EQUAL(0, err);

  TEST_ASSERT_EQUAL_PTR(&a, list.head->data);
  TEST_ASSERT_NOT_NULL(list.head->next);
  TEST_ASSERT_EQUAL_PTR(&b, list.head->next->data);
  TEST_ASSERT_NULL(list.head->next->next);
}

void test_dd_list_get_value_empty_returns_null(void) {
  int key = 123;
  void *out = dd_list_get_value(&list, &key, int_ptr_cmp);
  TEST_ASSERT_NULL(out);
}

void test_dd_list_get_value_finds_matching_value(void) {
  int a = 1, b = 2, c = 3;

  list.head = mk_node(&a, mk_node(&b, mk_node(&c, NULL)));

  int key = 2;
  void *out = dd_list_get_value(&list, &key, int_ptr_cmp);
  TEST_ASSERT_EQUAL_PTR(&b, out);
}

void test_dd_list_get_value_not_found_returns_null(void) {
  int a = 1, b = 2;

  list.head = mk_node(&a, mk_node(&b, NULL));

  int key = 999;
  void *out = dd_list_get_value(&list, &key, int_ptr_cmp);
  TEST_ASSERT_NULL(out);
}

void test_dd_list_pop_nc_empty_returns_null(void) {
  int key = 1;
  void *out = dd_list_pop_nc(&list, &key, int_ptr_cmp);
  TEST_ASSERT_NULL(out);
}

void test_dd_list_pop_nc_removes_head_node_and_returns_value(void) {
  int a = 1, b = 2, c = 3;

  list.head = mk_node(&a, mk_node(&b, mk_node(&c, NULL)));

  int key = 1;
  void *out = dd_list_pop_nc(&list, &key, int_ptr_cmp);

  TEST_ASSERT_EQUAL_PTR(&a, out);

  /* new head must be b */
  TEST_ASSERT_NOT_NULL(list.head);
  TEST_ASSERT_EQUAL_PTR(&b, list.head->data);
  TEST_ASSERT_NOT_NULL(list.head->next);
  TEST_ASSERT_EQUAL_PTR(&c, list.head->next->data);
  TEST_ASSERT_NULL(list.head->next->next);
}

void test_dd_list_pop_nc_removes_middle_node_and_returns_value(void) {
  int a = 1, b = 2, c = 3;

  list.head = mk_node(&a, mk_node(&b, mk_node(&c, NULL)));

  int key = 2;
  void *out = dd_list_pop_nc(&list, &key, int_ptr_cmp);

  TEST_ASSERT_EQUAL_PTR(&b, out);

  /* list should now be a -> c */
  TEST_ASSERT_EQUAL_PTR(&a, list.head->data);
  TEST_ASSERT_NOT_NULL(list.head->next);
  TEST_ASSERT_EQUAL_PTR(&c, list.head->next->data);
  TEST_ASSERT_NULL(list.head->next->next);
}

void test_dd_list_pop_nc_removes_last_node_and_returns_value(void) {
  int a = 1, b = 2, c = 3;

  list.head = mk_node(&a, mk_node(&b, mk_node(&c, NULL)));

  int key = 3;
  void *out = dd_list_pop_nc(&list, &key, int_ptr_cmp);

  TEST_ASSERT_EQUAL_PTR(&c, out);

  /* list should now be a -> b */
  TEST_ASSERT_EQUAL_PTR(&a, list.head->data);
  TEST_ASSERT_NOT_NULL(list.head->next);
  TEST_ASSERT_EQUAL_PTR(&b, list.head->next->data);
  TEST_ASSERT_NULL(list.head->next->next);
}

void test_dd_list_destroy_empty_keeps_initializer(void) {
  dd_list_destroy(&list, noop_destroy);
  TEST_ASSERT_TRUE(dd_list_is_empty(&list));
  TEST_ASSERT_NULL(list.head);
}

void test_dd_list_destroy_resets_list_and_calls_func_for_each_node(void) {
  int a = 0, b = 0, c = 0;

  list.head = mk_node(&a, mk_node(&b, mk_node(&c, NULL)));

  dd_list_destroy(&list, counter_destroy);

  /* each node's int incremented once */
  TEST_ASSERT_EQUAL(1, a);
  TEST_ASSERT_EQUAL(1, b);
  TEST_ASSERT_EQUAL(1, c);

  TEST_ASSERT_TRUE(dd_list_is_empty(&list));
  TEST_ASSERT_NULL(list.head);
}

void test_dd_list_destroy_accepts_null_func(void) {
  int a = 1;
  list.head = mk_node(&a, NULL);

  dd_list_destroy(&list, NULL);

  TEST_ASSERT_TRUE(dd_list_is_empty(&list));
  TEST_ASSERT_NULL(list.head);
}
