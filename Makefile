CC = gcc
CFLAGS = -Wall -g -Iinclude/server -Iinclude/common $(shell pkg-config --cflags glib-2.0)
#CFLAGS = -Wall -O2 -Iinclude/server -Iinclude/common $(shell pkg-config --cflags glib-2.0)
LDFLAGS = $(shell pkg-config --libs glib-2.0)

# Find all .c files in src subdirectories
SRC_FILES := $(shell find src -type f -name "*.c")
SERVER_SRC_FILES := $(filter-out src/client/dclient.c, $(SRC_FILES))
CLIENT_SRC_FILES := $(filter-out $(wildcard src/server/*.c), $(SRC_FILES))

# Get list of .o files
OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(SRC_FILES))
SERVER_OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(SERVER_SRC_FILES))
CLIENT_OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(CLIENT_SRC_FILES))

all: dserver dclient

dserver: bin/dserver

dclient: bin/dclient

bin/dserver: $(SERVER_OBJ_FILES)
	$(CC) $(LDFLAGS) $^ -o $@

bin/dclient: $(CLIENT_OBJ_FILES)
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) bin/*