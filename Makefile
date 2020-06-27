EXECUTABLE_NAME = doge48
BUILD_DIRECTORY = bin
BUILD_PATH = $(BUILD_DIRECTORY)/$(EXECUTABLE_NAME)
COMPILER_FLAGS = -Wall -Wextra -Werror -Wpedantic
LINKED_LIBS = -lSDL2 -lSDL2_image
STANDARD_VERSION = c++17

.PHONY: make_build_dir
make_build_dir:
	mkdir -p $(BUILD_DIRECTORY)

.PHONY: format
format:
	clang-format -i *.cpp

.PHONY: build
build: make_build_dir
	g++ -std=$(STANDARD_VERSION) $(COMPILER_FLAGS) *.cpp $(LINKED_LIBS) -o $(BUILD_PATH)

.PHONY: build_release
build_release: make_build_dir
	g++ -std=$(STANDARD_VERSION) $(COMPILER_FLAGS) *.cpp $(LINKED_LIBS) -O3 -o $(BUILD_PATH)

.PHONY: run
run: build
	$(BUILD_PATH)

.PHONY: run_release
run_release: build_release
	$(BUILD_PATH)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIRECTORY)
