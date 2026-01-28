CC = gcc
FLAGS = -Wall -Iinclude
SRC = ${shell find src -name '*.c'}
OBJ = ${SRC:src/%.c=build/%.o}
TARGET = bin/flow
ARGS = $(filter-out $@,$(MAKECMDGOALS))

all: ${TARGET}

${TARGET}: ${OBJ}
	@mkdir -p bin
	${CC} ${OBJ} -o ${TARGET}

build/%.o: src/%.c
	@mkdir -p ${dir $@}
	${CC} ${FLAGS} -c $< -o $@

clean:
	rm -rf build/* ${TARGET}
	
run: ${TARGET}
	./${TARGET}

test: clean all
	@:
	@if [ -z "$(ARGS)" ]; then \
		echo "Error: no test name provided, run using 'make test test_name'"; \
	else \
		clear; \
		./${TARGET} ./tests/$(ARGS).flow; \
		if [ $$? -eq 0 ]; then \
			gcc ./out.s -o ./out; \
			rm -f ./out.s; \
			if [ $$? -eq 0 ]; then \
				./out; \
				rm -f ./out; \
			fi; \
		fi; \
	fi
%:
	@:

.PHONY: all clean run test
