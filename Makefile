
BINARY = grinch
IMAGE = sample.png

$(BINARY): main.cpp $(IMAGE)
	g++ -o $@ -lsfml-graphics $<

$(IMAGE):
	wget -O $@ http://bak.ndla.no/cgi-bin/trac.cgi/export/333/modules/3rdparty/drupal/imagecache-6.x-2.0-beta10/sample.png
	@[ -n "$$(file $@ | grep PNG\ image\ data)" ] || (echo "$@ doesn't look like a png file!" && false)

.PHONEY: all
all: $(BINARY)

.PHONEY: run
run: all
	./$(BINARY)

.PHONEY: clean
clean:
	rm -f $(BINARY) $(IMAGE)

