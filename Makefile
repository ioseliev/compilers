CXX := g++
LD := ${CXX}

CXXFLAGS := -pipe -fPIC -Wall -Wextra -Wpedantic -g -O2

TARGET = exec
OBJS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))


all: ${TARGET}

${TARGET}: ${OBJS}
	${LD} -o $@ $^

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} -o $@ $<

.PHONY: run
run: ${TARGET}
	./${TARGET}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJS}
