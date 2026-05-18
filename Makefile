.PHONY: all clean platform run

ifeq ($(OS),Windows_NT)
    PLATFORM := WINDOWS
else
    UNAME_S := $(shell uname -s)
    PLATFORM := $(shell \
        if [ "$(UNAME_S)" = "Linux" ]; then echo LINUX; \
        elif [ "$(UNAME_S)" = "Darwin" ]; then echo MACOS; \
        elif echo "$(UNAME_S)" | grep -q MINGW; then echo MINGW; \
        elif echo "$(UNAME_S)" | grep -q CYGWIN; then echo CYGWIN; \
        else echo UNKNOWN; fi)
endif

CXX := g++
CXXFLAGS := -Isrc/include

OUT_DIR := out
SRC_DIR := src
OBJ_DIR := obj


all: platform clean mkdir main

mkdir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

platform: CXX := $(if $(filter MACOS,$(PLATFORM)),clang++,g++)
platform: CXXFLAGS := -Isrc/include $(if $(filter MACOS,$(PLATFORM)),-std=c++11)
platform:
	@echo "Detected PLATFORM: $(PLATFORM)"
	@echo "Using compiler: $(CXX)"


CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $(OUT_DIR)/$@

run: all
	$(OUT_DIR)/main
