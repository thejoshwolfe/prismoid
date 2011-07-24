
BINARY = grinch
IMAGE = sample.png

$(BINARY): main.cpp $(IMAGE)
	g++ -o $@ -lsfml-graphics $<

$(IMAGE):
	wget http://bak.ndla.no/cgi-bin/trac.cgi/export/333/modules/3rdparty/drupal/imagecache-6.x-2.0-beta10/sample.png

.PHONEY: all
all: $(BINARY)

.PHONEY: run
run: all
	./$(BINARY)

.PHONEY: clean
clean:
	rm -f $(BINARY) $(IMAGE)

