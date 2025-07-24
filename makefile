CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinc
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN := NISTCrypto

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: release

release: CXXFLAGS += -O2
release: $(BIN)

debug: CXXFLAGS += -g -O0
debug: $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: all release debug clean

clean:
	rm -rf $(OBJ_DIR) $(BIN)