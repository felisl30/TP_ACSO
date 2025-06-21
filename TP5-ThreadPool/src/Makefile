# Compiler settings - Can change to clang++ if preferred
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -g

# Build targets
TARGET = threadpool
# SRC = thread-pool.cc Semaphore.cc main.cc
TEST_TARGET = tpcustomtest
SRC = thread-pool.cc Semaphore.cc main.cc
TEST_SRC = thread-pool.cc Semaphore.cc tptest.cc

# # Link the target with object files
# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# Default target
all: $(TARGET) $(TEST_TARGET)

# # Link the target with object files
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# # Link the target with object files
$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

custom:
	$(CXX) $(CXXFLAGS) -o $(TARGET) thread-pool.cc Semaphore.cc test_custom.cc


# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)
