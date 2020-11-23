CC          = gcc
LD          = gcc
CFLAG       = -Wall -O3
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
BENCH_SRC_DIR  = $(SRC_DIR)/benchmark
BENCH_BUILD_DIR= $(BUILD_DIR)/benchmark
BENCH_LIST     = $(BENCH_SRC_LIST:%.c=%)
BENCH_SRC_LIST = $(notdir $(wildcard $(BENCH_SRC_DIR)/*.c))
BENCH_OBJ_LIST = $(addprefix $(BENCH_BUILD_DIR)/,$(BENCH_LIST:%=%.o))
INSTALL_DIR    = /usr/local/bin
DATA_DIR       = ./data
TEMP_DIR       = ./temp
VERBOSE 			 = 0

ifeq ($(VERBOSE),0)
	V := @
else
	V :=
endif

.PHONY: all
all: build comparisons

.PHONY: version
version:
	$(V) $(CC) --version
	$(V) sh -c "if [ $(CC) != $(LD) ]; then $(LD) --version; fi"

$(BUILD_DIR)/%.o:
	@echo "Compiling $@"
	$(V) $(CC) -c $(CFLAG) -o $@ $(SRC_DIR)/$(notdir $(@:%.o=%.c))

.PHONY: compile
compile: $(OBJ_LIST)

.PHONY: mkdir
mkdir:
	$(V) mkdir -p $(BUILD_DIR)
	$(V) mkdir -p $(BIN_DIR)

.PHONY: build
build: mkdir compile
	@echo "Linking $(BIN_DIR)/$(PROG_NAME)"
	$(V) $(LD) $(OBJ_LIST) -o $(BIN_DIR)/$(PROG_NAME)

.PHONY: clean
clean: test-clean bench-clean
	$(V) -rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: check
check:
	@echo "Checking object files"
	$(V) $(FILES_EXIST) $(OBJ_LIST)
	@echo "Checking binaries"
	$(V) $(FILES_EXIST) $(BIN_DIR)/$(PROG_NAME)

.PHONY: install
install: build
	$(V) mkdir -p $(INSTALL_DIR)
	$(V) cp $(BIN_DIR)/$(PROG_NAME) $(INSTALL_DIR)/$(PROG_NAME)

.PHONY: uninstall
uninstall:
	$(V) rm -rf $(INSTALL_DIR)/$(PROG_NAME)

.PHONY: install-local
install-local: $(PROG_NAME)
	$(V) mkdir -p $(DESTDIR)
	$(V) cp $(BIN_DIR)/$(PROG_NAME) $(DESTDIR)/$(PROG_NAME)

.PHONY: uninstall-local
uninstall-local:
	# make uninstall-local
	$(V) rm -rf $(DESTDIR)/$(PROG_NAME)

.PHONY: test
test: clean build check test-build
	@echo "Testing test utils"
	$(V) $(BIN_DIR)/compare-files $(BIN_DIR)/compare-files $(BIN_DIR)/compare-files
	$(V) $(BIN_DIR)/compare-dirs $(BIN_DIR) $(BIN_DIR)
	@echo "Initialising tests"
	$(V) ./scripts/init_test.sh
	@echo "----- Running tests -----"
	$(V) $(BIN_DIR)/tests $(BIN_DIR)/$(PROG_NAME) $(DATA_DIR)/test $(TEMP_DIR)/test
	@echo "-------------------------"

.PHONY: test-build
test-build: test-mkdir comparisons test-compile

.PHONY: test-mkdir
test-mkdir:
	$(V) mkdir -p $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%.o:
	@echo "Compiling $@"
	$(V) $(CC) -c $(CFLAG) -o $@ $(TEST_SRC_DIR)/$(notdir $(@:%.o=%.c))

.PHONY: test-compile
test-compile: $(TEST_OBJ_LIST)
	@echo "Linking $(BIN_DIR)/tests"
	$(V) $(LD) -o $(BIN_DIR)/tests $(TEST_OBJ_LIST)

.PHONY: test-clean
test-clean:
	$(V) -rm -rf $(TEMP_DIR) $(TEST_BUILD_DIR)

.PHONY: comparisons
comparisons: mkdir test-mkdir
	@echo "Compiling $(TEST_BUILD_DIR)/compare_files.o"
	$(V) $(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/compare_files.o $(TEST_SRC_DIR)/compare_files.c
	@echo "Compiling $(TEST_BUILD_DIR)/compare_directories.o"
	$(V) $(CC) -c $(CFLAG) -o $(TEST_BUILD_DIR)/compare_directories.o $(TEST_SRC_DIR)/compare_directories.c
	@echo "Linking $(BIN_DIR)/compare-files"
	$(V) $(LD) -o $(BIN_DIR)/compare-files $(TEST_BUILD_DIR)/compare_files.o
	@echo "Linking $(BIN_DIR)/compasre-dirs"
	$(V) $(LD) -o $(BIN_DIR)/compare-dirs $(TEST_BUILD_DIR)/compare_directories.o

.PHONY: benchmark
benchmark: bench-build $(DATA_DIR)/benchmark

.PHONY: bench-mkdir
bench-mkdir: mkdir
	$(V) mkdir -p $(BENCH_BUILD_DIR)

.PHONY: bench-clean
bench-clean:
	$(V) -rm -rf $(BENCH_BUILD_DIR) $(DATA_DIR)/benchmark

.PHONY: bench-build
bench-build: bench-mkdir bench-compile

.PHONY: bench-compile
bench-compile: $(BENCH_OBJ_LIST)
	@echo "Linking $(BIN_DIR)/generate"
	$(V) $(LD) $(CFLAG) -o $(BIN_DIR)/generate $(BENCH_OBJ_LIST)

$(BENCH_BUILD_DIR)/%.o:
	@echo "Compiling $@"
	$(V) $(CC) -c $(CFLAG) -o $@ $(BENCH_SRC_DIR)/$(notdir $(@:%.o=%.c))

$(DATA_DIR)/benchmark:
	@echo "Generating data/benchmark"
	$(V) scripts/generate_bench_data.py $(BIN_DIR)/generate $(DATA_DIR)/benchmark


$(TEST_BUILD_DIR)/%.o:
	@echo "Compiling $@"
	$(V) $(CC) -c $(CFLAG) -o $@ $(TEST_SRC_DIR)/$(notdir $(@:%.o=%.c))

$(BIN_DIR)/imartingraham:
	git clone https://github.com/imartingraham/csv-split $(BIN_DIR)/imartingraham