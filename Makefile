.PHONY: all var clean install

ENVIRON?=stage
VERSION=1.0.1

CXX?=g++
AR?=ar
NODE?=/data/server/node/bin/node
CFLAGS?= -g -O0
CFLAGS+= -fPIC -std=c11
CXXFLAGS?= -g -O0
CXXFLAGS+= -fPIC -std=c++17

TESTORS_SOURCES := $(wildcard ./test/*.cpp)
TARGETS := $(patsubst ./test/%.cpp,%,${TESTORS_SOURCES})
TESTORS := $(patsubst ./test/%.cpp,./${ENVIRON}/%.test,${TESTORS_SOURCES})

all: ${TESTORS} ./${ENVIRON}/liblltoml.a | ${ENVIRON}

${ENVIRON}:
	mkdir -p ./${ENVIRON}
	for name in ${TARGETS} ; \
	do \
		mkdir -p "./${ENVIRON}/$$name"; \
	done

SOURCES := $(wildcard $(foreach target,${TARGETS},./src/${target}/*.cpp))
DEFINES := $(foreach target,${TARGETS},./src/${target}/parser.mjs)
PARSERS := $(foreach target,${TARGETS},./src/${target}/parser.c)
OBJECTS := $(patsubst ./src/%.c,./${ENVIRON}/%.c.o, ${PARSERS}) $(patsubst ./src/%.cpp,./${ENVIRON}/%.cpp.o, ${SOURCES}) 
.PHONY: ${ENVIRON} var clean install 

var:
	@echo TESTORS = [${TESTORS}]
	@echo SOURCES = [${SOURCES}]
	@echo DEFINES = [${DEFINES}]
	@echo PARSERS = [${PARSERS}]
	@echo OBJECTS = [${OBJECTS}]

clean:
	rm -rf ./${ENVIRON}

install:
	mkdir -p /data/vendor/lltoml-${VERSION}/include/toml
	mkdir -p /data/vendor/lltoml-${VERSION}/lib
	cp ./src/toml/*.hpp /data/vendor/lltoml-${VERSION}/include/toml
	cp ./src/toml/*.h /data/vendor/lltoml-${VERSION}/include/toml
	cp ./${ENVIRON}/liblltoml.a /data/vendor/lltoml-${VERSION}/lib

./${ENVIRON}/%.c.o: ./src/%.c | ${ENVIRON}
	${CXX} -x c ${CFLAGS} -c $^ -o $@
./${ENVIRON}/%.cpp.o: ./src/%.cpp | ${ENVIRON}
	${CXX} ${CXXFLAGS} -c $^ -o $@
./src/%.c: ./src/%.mjs
	${NODE} --experimental-modules $^

./${ENVIRON}/%.test: ./test/%.cpp ${OBJECTS} | ${ENVIRON}
	${CXX} ${CXXFLAGS} -o $@ $^

./${ENVIRON}/liblltoml.a: ${OBJECTS} | ${ENVIRON}
	${AR} rcs $@ $^
