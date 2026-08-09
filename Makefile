CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude

SRC = src/main.c src/analysis.c src/fft.c

basstap: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o basstap -lpulse -lpulse-simple -lm

clean:
	rm -f basstap

.PHONY: clean