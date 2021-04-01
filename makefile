CC = gcc
BUILD_DIR = build
SRC_DIR= src
SOURCES = $(shell find $(SRC_DIR)/ -name "*.c")
CFLAGS = -pthread -Wall
OBJS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PROG = race

default: $(PROG)

clean:
	rm $(OBJS) $(PROG)

.SECONDEXPANSION:

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $@
	
$(OBJS): $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) $< -c -o $@
	

