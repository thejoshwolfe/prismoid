
BINARY = grinch

$(BINARY): main.cpp
	g++ -o $@ -lsfml-window -lsfml-system -lGLU $<

.PHONEY: all
all: $(BINARY)

.PHONEY: run
run: all
	./$(BINARY)

.PHONEY: clean
clean:
	rm -f $(BINARY)

