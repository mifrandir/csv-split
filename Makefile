CC          = cc
LD          = cc
CFLAG       = -Wall
PROG_NAME   = csv-split

FILES_EXIST = ./scripts/files_exist.sh

SRC_DIR     = ./src
BUILD_DIR   = ./build
BIN_DIR     = ./bin
INSTALL_DIR = /usr/local/bin
LIST = $(SRC_LIST:%.c=%)
SRC_LIST = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJ_LIST = $(addprefix $(BUILD_DIR)/,$(LIST:%=%.o))

.PHONY: all
all: $(PROG_NAME)

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
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: check
check:
	$(FILES_EXIST) $(OBJ_LIST)
	$(FILES_EXIST) $(BIN_DIR)/$(PROG_NAME)

.PHONY: install
install: $(PROG_NAME)
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

TEST_DIR=./src/test

.PHONY: test
test: test-build

.PHONY: test-build
test-build: mkdir bin/compare-files test-compile

.PHONY: test-compile
test-compile:

bin/compare-files: mkdir
	$(CC) $(CFLAG) -o $@ $(TEST_DIR)/compare_files.c