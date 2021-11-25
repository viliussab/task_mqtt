CC:=gcc
BIN:=mqtt_subscriber
LDFLAGS:=-lmosquitto -largp -lubox -luci -lsqlite3 -ljson-c -lcurl
SRC:= $(shell find $(SOURCEDIR) -name '*.c')
OBJS:=$(SRC:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN) $(OBJS)
