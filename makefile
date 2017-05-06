CC=g++
CFLAGS=-lm -lpthread -lX11

imgify: imgify.cpp
	$(CC) imgify.cpp -o imgify $(CFLAGS)

clean:
	rm imgify
