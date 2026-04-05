CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS = 

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BUILD_DIR)/liborderbook.a

.PHONY: all clean compile_commands

all: $(BUILD_DIR) $(TARGET) compile_commands.json

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	ar rcs $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile_commands.json: $(SRCS)
	@echo '[' > $@
	@first=true; \
	for src in $(SRCS); do \
		obj=$$(echo $$src | sed 's|$(SRC_DIR)/|$(BUILD_DIR)/|' | sed 's|\.cpp|.o|'); \
		if [ "$$first" = true ]; then first=false; else echo ',' >> $@; fi; \
		echo '  {' >> $@; \
		echo '    "directory": "'$(CURDIR)'",' >> $@; \
		echo '    "command": "$(CXX) $(CXXFLAGS) -c '$$src' -o '$$obj'",' >> $@; \
		echo '    "file": "'$$src'",' >> $@; \
		echo '    "output": "'$$obj'"' >> $@; \
		echo -n '  }' >> $@; \
	done; \
	echo '' >> $@; \
	echo ']' >> $@

clean:
	rm -rf $(BUILD_DIR) *.o compile_commands.json

.PHONY: test
test:
	@echo "Tests not implemented yet"
