.PHONY: all

all: src/toml.c src/toml.h

CFLAGS+= -std=c11
CXXFLAGS+= -std=c++17


src/toml.c: src/toml.mjs
	/data/vendor/node-12.3.1/bin/node --experimental-modules src/toml.mjs

src/toml.c.o: src/toml.c
	${CXX} -x c ${CFLAGS} -c -o $@ $^

src/toml.cpp.o: src/toml.cpp
	${CXX} ${CXXFLAGS} -c -o $@ $^

test/%: src/%.c.o src/%.cpp.o test/%.cpp
# test/toml: src/toml.o test/toml.cpp
	${CXX} ${CXXFLAGS} -o $@ $^