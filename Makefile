EXECS=CompareNQueens main

.PHONY: all
all: $(EXECS)
CC = g++

% : %.cpp
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(EXECS)
