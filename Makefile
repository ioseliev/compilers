CC := gcc
LD := ${CC}

CFLAGS := -g -O2 -I./

TARGET = lexer
OBJS := $(patsubst %.c, %.o, $(wildcard *.c))


all: ${TARGET}

${TARGET}: ${OBJS}
	${LD} -o $@ $^

%.o: %.c
	${CC} -c ${CFLAGS} -o $@ $<

.PHONY: run
run: ${TARGET}
	./${TARGET}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJS}
