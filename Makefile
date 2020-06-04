CC          = gcc
LD          = gcc
CFLAG       = -Wall
PROG_NAME   = csv-split
INC         = -I inc

SRC_DIR     = ./src
BUILD_DIR   = ./build
BIN_DIR     = ./bin
LIST = $(notdir $(wildcard $(SRC_DIR)/*.c))
SRC_LIST = $(addprefix $(SRC_DIR)/,$(LIST))
OBJ_LIST = $(addprefix $(BUILD_DIR)/,$(LIST:.c=.o))

.PHONY: all clean $(PROG_NAME) compile

all: $(PROG_NAME)

compile: 
	$(foreach f, $(LIST:%.c=%), $(CC) -c $(CFLAG) -o $(BUILD_DIR)/$(f).o $(SRC_DIR)/$(f).c;)

mkdir:
	mkdir $(BUILD_DIR)
	mkdir $(BIN_DIR)

$(PROG_NAME): mkdir compile
	$(LD) $(OBJ_LIST) -o $(BIN_DIR)/$@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)