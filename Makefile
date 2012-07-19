
.PHONY: all
all:


OUT = out

TARGETS = grinch test

define target
 OUT.$1 = out/obj.$1
 SOURCE_DIRS.$1 = src/$1 src/shared
 SOURCES.$1 := $$(foreach i,$$(SOURCE_DIRS.$1),$$(wildcard $$i/*.cpp))
 FILES.$1 := $$(foreach i,$$(SOURCE_DIRS.$1),$$(wildcard $$i/*))
 FILES.HASH.$1 := $$(shell python -c 'import sys; print(hash("\n".join(sorted(set(sys.argv[1:])))))' $$(FILES.$1))
 FILES.HASH.TIMESTAMP.$1 = $$(OUT.$1)/files.$$(FILES.HASH.$1).timestamp
 $$(FILES.HASH.TIMESTAMP.$1):
	mkdir -p $$(OUT.$1)
	rm -f $$(OUT.$1)/files.*.timestamp
	touch $$@
 BINARY.$1 = $$(OUT)/$1
 $$(BINARY.$1): $$(FILES.$1) $$(FILES.HASH.TIMESTAMP.$1)
	g++ -g -o $$@ $$(foreach i,$$(SOURCE_DIRS.$1),-I$$i) $$(SOURCES.$1) -lsfml-graphics -lsfml-window
 all: $$(BINARY.$1)
endef

$(foreach i,$(TARGETS),$(eval $(call target,$i)))

