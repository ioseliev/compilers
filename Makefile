CXX := g++
LD := ${CXX}

CXXFLAGS := -pipe -fPIC -g -O2 -I./

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
