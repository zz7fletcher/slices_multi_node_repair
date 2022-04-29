#  dir base
HEADER_DIR := include
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build/bin
EXP_DIR := exp
EXP_BIN_DIR := $(EXP_DIR)/bin
EXP_OBJ_DIR := $(EXP_DIR)/obj
EXP_SRC_DIR := $(EXP_DIR)/src
TEST_DIR := test
TEST_SRC_DIR := $(TEST_DIR)/src
TEST_OBJ_DIR := $(TEST_DIR)/obj
TEST_BIN_DIR := $(TEST_DIR)/bin

#  set of all source files & object files
# SRC := $(wildcard $(SRC_DIR)/*.cc)
# OBJ := $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(SRC:.cc=.o))

#  start of the programe
all : node_main

#  compile utility $ compile options
CXX := g++
CXXFLAGS := -march=native -std=c++11 -o3 -pthread -I$(HEADER_DIR)#-mcmodel=medium


# $(OBJ_DIR)/write_worker.o : $(SRC_DIR)/write_worker.cc $(OBJ_DIR)/memory_pool.o /usr/lib/libisal.so
# 	$(CXX) $(CXXFLAGS) -c $<  -o $@

# $(OBJ_DIR)/tcp_node.o : $(OBJ_DIR)/tcp_server.o $(OBJ_DIR)/tcp_client.o
# 	$(CXX) $(CXXFLAGS) -c $<  -o $@

#  rule of compiling object with corresponding  source file
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cc
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXP_OBJ_DIR)/%.o : $(EXP_SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o : $(TEST_SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

#  clean obj % bin
clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/* $(TEST_OBJ_DIR)/* $(TEST_BIN_DIR)/* $(EXP_OBJ_DIR)/* $(EXP_BIN_DIR)/*

node_main : $(OBJ_DIR)/node_main.o $(OBJ_DIR)/tcp_node.o $(OBJ_DIR)/tcp_server.o $(OBJ_DIR)/tcp_client.o /usr/local/lib/libsockpp.so $(OBJ_DIR)/memory_pool.o  /usr/lib/libisal.so 
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@
