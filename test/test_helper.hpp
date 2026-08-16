#ifndef __TEST_HELPER_HPP__
#define __TEST_HELPER_HPP__

/* test RBT */
void test_insert_search();
void test_remove();
void test_insert_emplace();
void test_heterogeneous_remove();
void test_invariant();
void test_get_successor();
void fuzz_test();

/* test L3_LOB */
void test_query();
void test_order();
void testspend();

/* test L2_LOB */
void test_apply_snapshot();

#endif
