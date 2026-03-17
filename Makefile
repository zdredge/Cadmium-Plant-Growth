# Compiler settings
CXX          = g++
CXXFLAGS     = -std=c++17 -Wall -Wextra

# Include paths
# CADMIUM_INCLUDE  = /home/zdredge/cadmium_v2/include # Update this path to your local Cadmium include directory
CADMIUM_INCLUDE = $(CADMIUM)
# PROJECT_INCLUDE  = main/include
INCLUDES         = -I$(CADMIUM_INCLUDE) -Iatomics -Icoupled

# Directories
BIN_DIR = bin

# Header dependencies
HEADERS = $(wildcard atomics/*.hpp) $(wildcard coupled/*.hpp)

# Default target
all: test_nature test_soil test_stem test_basil_plant greenhouse

# Build test_nature
test_nature: test/test_nature.cpp $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/test_nature test/test_nature.cpp

# Build test_soil
test_soil: test/test_soil.cpp $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/test_soil test/test_soil.cpp

# Build test_stem
test_stem: test/test_stem.cpp $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/test_stem test/test_stem.cpp

# Build test_basil_plant
test_basil_plant: test/test_basil_plant.cpp $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/test_basil_plant test/test_basil_plant.cpp

# Build greenhouse
greenhouse: top_model/main.cpp $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_DIR)/greenhouse top_model/main.cpp

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Run targets
run_nature: test_nature
	./$(BIN_DIR)/test_nature

run_soil: test_soil
	./$(BIN_DIR)/test_soil

run_stem: test_stem
	./$(BIN_DIR)/test_stem

run_basil_plant: test_basil_plant
	./$(BIN_DIR)/test_basil_plant

run_greenhouse: greenhouse
	./$(BIN_DIR)/greenhouse

# Clean build artifacts
clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean run run_soil run_stem run_basil_plant run_greenhouse
