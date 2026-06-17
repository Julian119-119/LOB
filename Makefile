CXX ?= g++

CPP_FLAGS := -Iinclude -std=c++17 -Wall -Wextra
OUT_DIR := out
SRC_DIR := src
OBJ_DIR := obj
TEST_DIR := test
BENCHMARK_DIR := benchmark
TEST_OBJ := $(OBJ_DIR)/test_BST_RBT.o $(OBJ_DIR)/test_LOB_order.o $(OBJ_DIR)/test_LOB_query.o $(OBJ_DIR)/test_main.o

$(OBJ_DIR):
	mkdir -p $@

$(OUT_DIR):
	mkdir -p $@

$(OBJ_DIR)/test_BST_RBT.o: $(TEST_DIR)/test_BST_RBT.cpp | $(OBJ_DIR)
	$(CXX) $(CPP_FLAGS) -c $< -o $@

$(OBJ_DIR)/test_LOB_order.o: $(TEST_DIR)/test_LOB_order.cpp | $(OBJ_DIR)
	$(CXX) $(CPP_FLAGS) -c $< -o $@

$(OBJ_DIR)/test_LOB_query.o: $(TEST_DIR)/test_LOB_query.cpp | $(OBJ_DIR)
	$(CXX) $(CPP_FLAGS) -c $< -o $@

$(OBJ_DIR)/test_main.o: $(TEST_DIR)/test_main.cpp | $(OBJ_DIR)
	$(CXX) $(CPP_FLAGS) -c $< -o $@

$(OBJ_DIR)/LOB_type.o: $(SRC_DIR)/LOB_type.cpp | $(OBJ_DIR)
	$(CXX) $(CPP_FLAGS) -c $< -o $@

$(OBJ_DIR)/benchmark.o: $(BENCHMARK_DIR)/benchmark.cpp | $(OBJ_DIR)
	$(CXX) $(CPP_FLAGS) -c $< -o $@

all: mkdir test_program benchmark_program


test_program: $(TEST_OBJ) $(OBJ_DIR)/LOB_type.o | $(OUT_DIR)
	$(CXX) $(CPP_FLAGS) $^ -o $(OUT_DIR)/$@
benchmark_program: $(OBJ_DIR)/benchmark.o $(OBJ_DIR)/LOB_type.o | $(OUT_DIR)
	$(CXX) $(CPP_FLAGS) $^ -o $(OUT_DIR)/$@
.PHONY: test clean mkdir run_benchmark all

test: mkdir test_program
	$(OUT_DIR)/test_program

run_benchmark: mkdir benchmark_program 
	$(OUT_DIR)/benchmark_program

mkdir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)
