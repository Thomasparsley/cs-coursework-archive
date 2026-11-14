# Tests for the os2 assignments. Sources are never modified; the library-style
# files are #included into the test programs, pgmtoascii is tested as a
# black box. Works with clang (macOS) and gcc (Linux).
#
#   make test            run everything
#   make test-sanitize   run with AddressSanitizer + UBSan
#   make test-tsan       run the pipe tests with ThreadSanitizer
#   make clean

CC ?= cc
CSTD = -std=c11 -Wall -Wextra -pthread
BUILD = tests/build

# petrzela-tomas-1-lin.c uses `#pragma region`, which the compiler does not know
PIPE_WARN = -Wno-unknown-pragmas

SAN = -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=undefined -g
TSAN = -fsanitize=thread -g -O1

.PHONY: test test-sanitize test-tsan clean

test: $(BUILD)/test_pipe $(BUILD)/test_lalloc $(BUILD)/pgmtoascii
	$(BUILD)/test_pipe
	$(BUILD)/test_lalloc
	sh tests/test_pgmtoascii.sh $(BUILD)/pgmtoascii

test-sanitize: $(BUILD)/san_pipe $(BUILD)/san_lalloc $(BUILD)/san_pgmtoascii
	$(BUILD)/san_pipe
	$(BUILD)/san_lalloc
	# error paths exit(1) without freeing, so leak checking is off here
	ASAN_OPTIONS=detect_leaks=0 sh tests/test_pgmtoascii.sh $(BUILD)/san_pgmtoascii

test-tsan: $(BUILD)/tsan_pipe
	$(BUILD)/tsan_pipe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/test_pipe: tests/test_pipe.c tests/mini_test.h petrzela-tomas-1-lin.c | $(BUILD)
	$(CC) $(CSTD) $(PIPE_WARN) -O1 -o $@ tests/test_pipe.c

$(BUILD)/test_lalloc: tests/test_lalloc.c tests/mini_test.h petrzela-tomas-3-lin.c | $(BUILD)
	$(CC) $(CSTD) -O1 -o $@ tests/test_lalloc.c

# The program itself is built with its own feature macros, only a plain
# warning level is used (it is third-party-style code that is not modified).
$(BUILD)/pgmtoascii: petrzela-tomas-2-any.c | $(BUILD)
	$(CC) -std=c11 -Wall -Wextra -O1 -o $@ petrzela-tomas-2-any.c

$(BUILD)/san_pipe: tests/test_pipe.c tests/mini_test.h petrzela-tomas-1-lin.c | $(BUILD)
	$(CC) $(CSTD) $(PIPE_WARN) $(SAN) -o $@ tests/test_pipe.c

$(BUILD)/san_lalloc: tests/test_lalloc.c tests/mini_test.h petrzela-tomas-3-lin.c | $(BUILD)
	$(CC) $(CSTD) $(SAN) -o $@ tests/test_lalloc.c

$(BUILD)/san_pgmtoascii: petrzela-tomas-2-any.c | $(BUILD)
	$(CC) -std=c11 -Wall -Wextra $(SAN) -o $@ petrzela-tomas-2-any.c

$(BUILD)/tsan_pipe: tests/test_pipe.c tests/mini_test.h petrzela-tomas-1-lin.c | $(BUILD)
	$(CC) $(CSTD) $(PIPE_WARN) $(TSAN) -o $@ tests/test_pipe.c

clean:
	rm -rf $(BUILD)
