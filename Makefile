CC = gcc
FLAGS = -Wall -Iinclude
SRC = ${shell find src -name '*.c'}
OBJ = ${SRC:src/%.c=build/%.o}
TARGET = bin/flow

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
	@if [ -z "${TEST}" ]; then \
		echo "Error: no arguments provided, run using 'make test TEST=test_name'"; \
	else \
		./${TARGET} ./tests/${TEST}.flow; \
		if [ $$? -eq 0 ]; then \
			clang -arch x86_64 -o ./tests/${TEST} ./tests/${TEST}.s; \
			if [ $$? -eq 0 ]; then \
				echo ""; \
				./tests/${TEST}; \
			fi; \
		fi; \
	fi
.PHONY: all clean run test
