CC          = cc
LD          = cc
CFLAG       = -Wall
PROG_NAME   = csv-split

FILES_EXIST = ./scripts/files_exist.sh

SRC_DIR     = ./src
TEST_SRC_DIR=./src/test
BUILD_DIR   = ./build
TEST_BUILD_DIR = $(BUILD_DIR)/test
BIN_DIR     = ./bin
INSTALL_DIR = /usr/local/bin
DATA_DIR = ./data
TEMP_DIR = ./temp
LIST = $(SRC_LIST:%.c=%)
SRC_LIST = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJ_LIST = $(addprefix $(BUILD_DIR)/,$(LIST:%=%.o))

.PHONY: all
all: build comparisons

$(BUILD_DIR)/%.o:
	$(CC) -c $(CFLAG) -o $@ $(SRC_DIR)/$(notdir $(@:%.o=%.c))

.PHONY: compile
compile: $(OBJ_LIST)

.PHONY: mkdir
mkdir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

.PHONY: build
build: mkdir compile
	$(LD) $(OBJ_LIST) -o $(BIN_DIR)/$(PROG_NAME)

.PHONY: clean
clean: test-clean
	-rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: check
check:
	$(FILES_EXIST) $(OBJ_LIST)
	$(FILES_EXIST) $(BIN_DIR)/$(PROG_NAME)

.PHONY: install
install: build
	mkdir -p $(INSTALL_DIR)
	cp $(BIN_DIR)/$(PROG_NAME) $(INSTALL_DIR)/$(PROG_NAME)

.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_DIR)/$(PROG_NAME)

.PHONY: install-local
install-local: $(PROG_NAME)
	mkdir -p $(DESTDIR)
	cp $(BIN_DIR)/$(PROG_NAME) $(DESTDIR)/$(PROG_NAME)

.PHONY: uninstall-local
uninstall-local:
	rm -rf $(DESTDIR)/$(PROG_NAME)

.PHONY: test
test: test-clean build test-build
	# checking whether compare-files works
	$(BIN_DIR)/compare-files $(BIN_DIR)/compare-files $(BIN_DIR)/compare-files
	$(BIN_DIR)/compare-dirs $(BIN_DIR) $(BIN_DIR)
	$(BIN_DIR)/test $(BIN_DIR)/$(PROG_NAME) $(DATA_DIR)/test $(TEMP_DIR)/test

.PHONY: test-build
test-build: mkdir test-mkdir comparisons test-compile

.PHONY: test-mkdir
test-mkdir:
	mkdir -p $(TEST_BUILD_DIR)

.PHONY: test-compile
test-compile:
	$(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/test.o $(TEST_SRC_DIR)/test.c
	$(LD) -o $(BIN_DIR)/test $(TEST_BUILD_DIR)/test.o

.PHONY: test-clean
test-clean:
	-rm -rf $(TEMP_DIR) $(TEST_BUILD_DIR)

.PHONY: comparisons
comparisons: mkdir test-mkdir
	$(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/compare_files.o $(TEST_SRC_DIR)/compare_files.c
	$(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/compare_directories.o $(TEST_SRC_DIR)/compare_directories.c
	$(LD) -o $(BIN_DIR)/compare-files $(TEST_BUILD_DIR)/compare_files.o
	$(LD) -o $(BIN_DIR)/compare-dirs $(TEST_BUILD_DIR)/compare_directories.o