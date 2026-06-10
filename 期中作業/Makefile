CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -g
TARGETS  = myshell

.PHONY: all clean run test

all: $(TARGETS)

myshell: myshell.c
	$(CC) $(CFLAGS) -o $@ $<

test: myshell
	@echo "=== Test 1: built-in help ==="
	@echo "help\nexit" | ./myshell
	@echo ""
	@echo "=== Test 2: built-in pwd ==="
	@echo "pwd\nexit" | ./myshell
	@echo ""
	@echo "=== Test 3: external command ==="
	@echo "whoami\nexit" | ./myshell
	@echo ""
	@echo "=== Test 4: output redirection ==="
	@echo "echo hello redirect test > /tmp/myshell_test.txt" | ./myshell 2>&1
	@cat /tmp/myshell_test.txt
	@rm -f /tmp/myshell_test.txt
	@echo ""
	@echo "=== Test 5: pipe ==="
	@echo "ls -la | wc -l" | ./myshell 2>&1

clean:
	rm -f $(TARGETS) redirect_out.txt redirect_err.txt

run: myshell
	./myshell
