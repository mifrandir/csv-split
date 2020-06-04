CC          = cc
LD          = cc
CFLAG       = -Wall
PROG_NAME   = csv-split

SRC_DIR     = ./src
BUILD_DIR   = ./build
BIN_DIR     = ./bin
INSTALL_DIR = /usr/local/bin
LIST = $(SRC_LIST:%.c=%)
SRC_LIST = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJ_LIST = $(addprefix $(BUILD_DIR)/,$(LIST:%=%.o))

.PHONY: all clean $(PROG_NAME) compile install install-local uninstall uninstall-local

all: $(PROG_NAME)

$(BUILD_DIR)/%.o:
	$(CC) -c $(CFLAG) -o $@ $(SRC_DIR)/$(notdir $(@:%.o=%.c))

compile: $(OBJ_LIST)

mkdir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

build: mkdir compile
	$(LD) $(OBJ_LIST) -o $(BIN_DIR)/$(PROG_NAME)

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

check:

install: $(PROG_NAME)
	mkdir -p $(INSTALL_DIR)
	cp $(BIN_DIR)/$(PROG_NAME) $(INSTALL_DIR)/$(PROG_NAME)

uninstall:
	rm -rf $(INSTALL_DIR)/$(PROG_NAME)

install-local: $(PROG_NAME)
	mkdir -p $(DESTDIR)
	cp $(BIN_DIR)/$(PROG_NAME) $(DESTDIR)/$(PROG_NAME)

uninstall-local:
	rm -rf $(DESTDIR)/$(PROG_NAME)