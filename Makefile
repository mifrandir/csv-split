CC          = cc
LD          = cc
CFLAG       = -Wall
PROG_NAME   = csv-split

FILES_EXIST = ./scripts/files_exist.sh

BIN_DIR        = ./bin
SRC_DIR        = ./src
BUILD_DIR      = ./build
LIST           = $(SRC_LIST:%.c=%)
SRC_LIST       = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJ_LIST       = $(addprefix $(BUILD_DIR)/,$(LIST:%=%.o))
TEST_SRC_DIR   =./src/test
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_LIST      = $(TEST_SRC_LIST:%.c=%)
TEST_SRC_LIST  = $(notdir $(wildcard $(TEST_SRC_DIR)/*test*.c))
TEST_OBJ_LIST  = $(addprefix $(TEST_BUILD_DIR)/,$(TEST_LIST:%=%.o))
INSTALL_DIR    = /usr/local/bin
DATA_DIR       = ./data
TEMP_DIR       = ./temp

.PHONY: version
version:
	# make version
	@$(CC) --version
	@sh -c "if [ $(CC) != $(LD) ]; then $(LD) --version; fi"

.PHONY: all
all: version build comparisons

$(BUILD_DIR)/%.o:
	$(CC) -c $(CFLAG) -o $@ $(SRC_DIR)/$(notdir $(@:%.o=%.c))

.PHONY: compile
compile: $(OBJ_LIST)

.PHONY: mkdir
mkdir:
	# make mkdir
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

.PHONY: build
build: mkdir compile
	# make build
	$(LD) $(OBJ_LIST) -o $(BIN_DIR)/$(PROG_NAME)

.PHONY: clean
clean: test-clean
	# make clean
	-rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: check
check:
	# make check
	$(FILES_EXIST) $(OBJ_LIST)
	$(FILES_EXIST) $(BIN_DIR)/$(PROG_NAME)

.PHONY: install
install: build
	# make install
	mkdir -p $(INSTALL_DIR)
	cp $(BIN_DIR)/$(PROG_NAME) $(INSTALL_DIR)/$(PROG_NAME)

.PHONY: uninstall
uninstall:
	# make uninstall
	rm -rf $(INSTALL_DIR)/$(PROG_NAME)

.PHONY: install-local
install-local: $(PROG_NAME)
	# make install-local
	mkdir -p $(DESTDIR)
	cp $(BIN_DIR)/$(PROG_NAME) $(DESTDIR)/$(PROG_NAME)

.PHONY: uninstall-local
uninstall-local:
	# make uninstall-local
	rm -rf $(DESTDIR)/$(PROG_NAME)

.PHONY: test
test: version clean build check test-build
	# make test
	# checking whether comparisons work
	$(BIN_DIR)/compare-files $(BIN_DIR)/compare-files $(BIN_DIR)/compare-files
	$(BIN_DIR)/compare-dirs $(BIN_DIR) $(BIN_DIR)
	# preparing data directory
	./scripts/init_test.sh
	$(BIN_DIR)/tests $(BIN_DIR)/$(PROG_NAME) $(DATA_DIR)/test $(TEMP_DIR)/test

.PHONY: test-build
test-build: test-mkdir comparisons test-compile

.PHONY: test-mkdir
test-mkdir:
	# make test-mkdir
	mkdir -p $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%.o:
	$(CC) -c $(CFLAG) -o $@ $(TEST_SRC_DIR)/$(notdir $(@:%.o=%.c))

.PHONY: test-compile
test-compile: $(TEST_OBJ_LIST)
	# make test-compile
	$(LD) -o $(BIN_DIR)/tests $(TEST_OBJ_LIST)

.PHONY: test-clean
test-clean:
	# make test-clean
	-rm -rf $(TEMP_DIR) $(TEST_BUILD_DIR)

.PHONY: comparisons
comparisons: mkdir test-mkdir
	# make comparisons
	$(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/compare_files.o $(TEST_SRC_DIR)/compare_files.c
	$(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/compare_directories.o $(TEST_SRC_DIR)/compare_directories.c
	$(LD) -o $(BIN_DIR)/compare-files $(TEST_BUILD_DIR)/compare_files.o
	$(LD) -o $(BIN_DIR)/compare-dirs $(TEST_BUILD_DIR)/compare_directories.o