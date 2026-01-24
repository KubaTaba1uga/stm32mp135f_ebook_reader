#include <unity.h>
#include <stdbool.h>
#include <stdint.h>

#include "utils/zlist.h"
#include "utils/err.h"
#include "utils/mem.h"   /* for mem_malloc / mem_free */

static zlist_node_t mk_node(void) {
  zlist_node_t n = mem_malloc(sizeof(*n));
  TEST_ASSERT_NOT_NULL(n);
  n->next = NULL;
  return n;
}

static zlist_t mk_list(void) {
  zlist_t l = mem_malloc(sizeof(*l));
  TEST_ASSERT_NOT_NULL(l);
  l->head = NULL;
  l->len = 0;
  return l;
}

static int count_nodes(zlist_t l) {
  int c = 0;
  for (zlist_node_t n = l->head; n; n = n->next) c++;
  return c;
}

static zlist_t list;

void setUp(void) {
  err_o = (err_t){0};
  list = mk_list();
}

void tearDown(void) {
  zlist_node_t n = list->head;
  while (n) {
    zlist_node_t next = n->next;
    mem_free(n);
    n = next;
  }
  mem_free(list);
}

void test_zlist_append_null_head_sets_error_and_returns_minus1(void) {
  zlist_node_t n = mk_node();

  int rc = zlist_append(NULL, n);
  TEST_ASSERT_EQUAL(-1, rc);

  /* adapt these two lines to your err_t fields */
  TEST_ASSERT_EQUAL(EINVAL, err_o->code);
  TEST_ASSERT_NOT_NULL(err_o->msg);

  mem_free(n);
}

void test_zlist_append_null_node_sets_error_and_returns_minus1(void) {
  int rc = zlist_append(list, NULL);
  TEST_ASSERT_EQUAL(-1, rc);

  /* adapt these two lines to your err_t fields */
  TEST_ASSERT_EQUAL(EINVAL, err_o->code);
  TEST_ASSERT_NOT_NULL(err_o->msg);
}

void test_zlist_append_to_empty_sets_head_len_and_next_null(void) {
  zlist_node_t a = mk_node();

  TEST_ASSERT_NULL(list->head);
  TEST_ASSERT_EQUAL(0, list->len);

  int rc = zlist_append(list, a);
  TEST_ASSERT_EQUAL(0, rc);

  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_NULL(a->next);
  TEST_ASSERT_EQUAL(1, list->len);
  TEST_ASSERT_EQUAL(1, count_nodes(list));
}

void test_zlist_append_appends_to_tail_and_increments_len(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();
  zlist_node_t c = mk_node();

  TEST_ASSERT_EQUAL(0, zlist_append(list, a));
  TEST_ASSERT_EQUAL(0, zlist_append(list, b));
  TEST_ASSERT_EQUAL(0, zlist_append(list, c));

  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_EQUAL_PTR(b, list->head->next);
  TEST_ASSERT_EQUAL_PTR(c, list->head->next->next);
  TEST_ASSERT_NULL(c->next);

  TEST_ASSERT_EQUAL(3, list->len);
  TEST_ASSERT_EQUAL(3, count_nodes(list));
}

void test_zlist_append_overwrites_node_next_to_null(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();
  zlist_node_t junk = mk_node();

  b->next = junk;

  TEST_ASSERT_EQUAL(0, zlist_append(list, a));
  TEST_ASSERT_EQUAL(0, zlist_append(list, b));

  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_EQUAL_PTR(b, list->head->next);
  TEST_ASSERT_NULL(b->next); 

  mem_free(junk);
}

void test_zlist_get_empty_returns_null(void) {
  TEST_ASSERT_NULL(list->head);
  TEST_ASSERT_NULL(zlist_get(list, 0));
  TEST_ASSERT_NULL(zlist_get(list, 1));
  TEST_ASSERT_NULL(zlist_get(list, -1));
}

void test_zlist_get_index0_returns_head(void) {
  zlist_node_t a = mk_node();
  zlist_append(list, a);

  TEST_ASSERT_EQUAL_PTR(a, zlist_get(list, 0));
}

void test_zlist_get_middle_index_returns_expected_node(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();
  zlist_node_t c = mk_node();

  zlist_append(list, a);
  zlist_append(list, b);
  zlist_append(list, c);

  TEST_ASSERT_EQUAL_PTR(b, zlist_get(list, 1));
}

void test_zlist_get_out_of_range_returns_null(void) {
  zlist_node_t a = mk_node();
  zlist_append(list, a);

  TEST_ASSERT_NULL(zlist_get(list, 1));
  TEST_ASSERT_NULL(zlist_get(list, 2));
}

void test_zlist_get_negative_index_returns_null(void) {
  zlist_node_t a = mk_node();
  zlist_append(list, a);

  TEST_ASSERT_NULL(zlist_get(list, -1));
}

void _test_zlist_pop_empty_returns_null_and_len_unchanged(void) {
  TEST_ASSERT_NULL(list->head);
  TEST_ASSERT_EQUAL(0, list->len);

  zlist_node_t out = zlist_pop(list, 0);
  TEST_ASSERT_NULL(out);
  TEST_ASSERT_EQUAL(0, list->len);
  TEST_ASSERT_NULL(list->head);
}

void _test_zlist_pop_out_of_range_returns_null_and_list_unchanged(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();

  zlist_append(list, a);
  zlist_append(list, b);

  TEST_ASSERT_EQUAL(2, list->len);

  zlist_node_t out = zlist_pop(list, 2);
  TEST_ASSERT_NULL(out);

  TEST_ASSERT_EQUAL(2, list->len);
  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_EQUAL_PTR(b, list->head->next);
  TEST_ASSERT_NULL(b->next);
  TEST_ASSERT_EQUAL(2, count_nodes(list));
}

void _test_zlist_pop_negative_index_returns_null_and_list_unchanged(void) {
  zlist_node_t a = mk_node();
  zlist_append(list, a);

  zlist_node_t out = zlist_pop(list, -1);
  TEST_ASSERT_NULL(out);

  TEST_ASSERT_EQUAL(1, list->len);
  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_NULL(a->next);
}

void test_zlist_pop_index0_removes_head_and_returns_node(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();
  zlist_node_t c = mk_node();

  zlist_append(list, a);
  zlist_append(list, b);
  zlist_append(list, c);

  TEST_ASSERT_EQUAL(3, list->len);

  zlist_node_t out = zlist_pop(list, 0);
  TEST_ASSERT_EQUAL_PTR(a, out);

  /* popped node should be detached */
  TEST_ASSERT_NULL(out->next);

  /* list should now start at b */
  TEST_ASSERT_EQUAL_PTR(b, list->head);
  TEST_ASSERT_EQUAL_PTR(c, list->head->next);
  TEST_ASSERT_NULL(c->next);

  TEST_ASSERT_EQUAL(2, list->len);
  TEST_ASSERT_EQUAL(2, count_nodes(list));

  mem_free(out);
}

void test_zlist_pop_middle_removes_node_and_returns_it(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();
  zlist_node_t c = mk_node();

  zlist_append(list, a);
  zlist_append(list, b);
  zlist_append(list, c);

  zlist_node_t out = zlist_pop(list, 1);
  TEST_ASSERT_EQUAL_PTR(b, out);

  /* list should be a -> c */
  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_EQUAL_PTR(c, list->head->next);
  TEST_ASSERT_NULL(c->next);

  TEST_ASSERT_NULL(out->next);
  TEST_ASSERT_EQUAL(2, list->len);
  TEST_ASSERT_EQUAL(2, count_nodes(list));

  mem_free(out);
}

void test_zlist_pop_last_removes_tail_and_returns_it(void) {
  zlist_node_t a = mk_node();
  zlist_node_t b = mk_node();
  zlist_node_t c = mk_node();

  zlist_append(list, a);
  zlist_append(list, b);
  zlist_append(list, c);

  zlist_node_t out = zlist_pop(list, 2);
  TEST_ASSERT_EQUAL_PTR(c, out);

  /* list should be a -> b */
  TEST_ASSERT_EQUAL_PTR(a, list->head);
  TEST_ASSERT_EQUAL_PTR(b, list->head->next);
  TEST_ASSERT_NULL(b->next);

  TEST_ASSERT_NULL(out->next);
  TEST_ASSERT_EQUAL(2, list->len);
  TEST_ASSERT_EQUAL(2, count_nodes(list));

  mem_free(out);
}

void test_zlist_pop_singleton_index0_results_in_empty_list(void) {
  zlist_node_t a = mk_node();
  zlist_append(list, a);

  TEST_ASSERT_EQUAL(1, list->len);
  TEST_ASSERT_EQUAL_PTR(a, list->head);

  zlist_node_t out = zlist_pop(list, 0);
  TEST_ASSERT_EQUAL_PTR(a, out);

  TEST_ASSERT_NULL(out->next);
  TEST_ASSERT_NULL(list->head);
  TEST_ASSERT_EQUAL(0, list->len);

  mem_free(out);
}
