CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDLIBS = -lpulse -lpulse-simple -lm

basstap: src/main.c src/analysis.c
	$(CC) $(CFLAGS) src/main.c src/analysis.c -o basstap $(LDLIBS) 

clean:
	rm -f basstap

.PHONY: clean