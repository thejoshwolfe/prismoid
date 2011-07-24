
BINARY = grinch

$(BINARY): main.cpp
	g++ -o $@ -lsfml-graphics $<

.PHONEY: all
all: $(BINARY)

.PHONEY: run
run: all
	./$(BINARY)

.PHONEY: clean
clean:
	rm -f $(BINARY)

