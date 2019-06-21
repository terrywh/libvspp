.PHONY: all var clean

ENVIRON?=stage

CXX?=g++
AR?=ar
CFLAGS?= -g -O0
CFLAGS+= -std=c11
CXXFLAGS?= -g -O0
CXXFLAGS+= -std=c++17

TESTORS_SOURCES := $(wildcard ./test/*.cpp)
TARGETS := $(patsubst ./test/%.cpp,%,${TESTORS_SOURCES})
TESTORS := $(patsubst ./test/%.cpp,./${ENVIRON}/%.test,${TESTORS_SOURCES})

all: ${TESTORS} ./${ENVIRON}/liblltoml.a | ${ENVIRON}

${ENVIRON}:
	mkdir -p ./bin
	mkdir -p ./${ENVIRON}
	for name in ${TARGETS} ; \
	do \
		mkdir -p "./${ENVIRON}/$$name"; \
	done

SOURCES := $(wildcard $(foreach target,${TARGETS},./src/${target}/*.cpp))
DEFINES := $(foreach target,${TARGETS},./src/${target}/parser.mjs)
PARSERS := $(foreach target,${TARGETS},./src/${target}/parser.c)
OBJECTS := $(patsubst ./src/%.c,./${ENVIRON}/%.c.o, ${PARSERS}) $(patsubst ./src/%.cpp,./${ENVIRON}/%.cpp.o, ${SOURCES}) 


var:
	@echo TESTORS = [${TESTORS}]
	@echo SOURCES = [${SOURCES}]
	@echo DEFINES = [${DEFINES}]
	@echo PARSERS = [${PARSERS}]
	@echo OBJECTS = [${OBJECTS}]
	@echo FILTERS = [$(filter ./${ENVIRON}/toml/%, ${OBJECTS})]

clean:
	rm -rf ./${ENVIRON}

./${ENVIRON}/%.c.o: ./src/%.c | ${ENVIRON}
	${CXX} -x c ${CFLAGS} -c -o $@ $^
./${ENVIRON}/%.cpp.o: ./src/%.cpp | ${ENVIRON}
	${CXX} ${CXXFLAGS} -c -o $@ $^
./src/%.c: ./src/%.mjs
	/data/vendor/node-12.3.1/bin/node --experimental-modules $^

./${ENVIRON}/%.test: ./test/%.cpp ${OBJECTS} | ${ENVIRON}
	${CXX} ${CXXFLAGS} -o $@ $^
./${ENVIRON}/liblltoml.a: ${OBJECTS} | ${ENVIRON}
