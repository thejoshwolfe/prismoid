
.PHONY: all
all:

SRC = src
OUT = out
SHARED = shared

OUTPUTS := $(filter-out $(SHARED),$(shell ls $(SRC)))

define hash
 $(shell echo -n $1 | md5sum | cut -d' ' -f1)
endef

define hash-timestamp
 $1.HASH := $2.$$(strip $$(call hash,$3)).timestamp
 $$($1.HASH):
	@mkdir -p $$(dir $2)
	@rm -f $2.*.timestamp
	@touch $$@
endef

define object
 $1: $2
	@mkdir -p $$(dir $1)
	g++ -o $1 -c -g $$(addprefix -I,$3) $2 -MMD -MP -MF $1.d
 -include $1.d
endef

define binary
 $1.OUT = out/obj.$1
 $1.SOURCE_DIRS = $$(addprefix $$(SRC)/,$1 $$(SHARED))
 $1.SOURCES := $$(foreach i,$$($1.SOURCE_DIRS),$$(wildcard $$i/*.cpp))
 $1.OBJECTS = $$(foreach i,$$($1.SOURCES),$$($1.OUT)/$$i.o)
 #$$(eval $$(call object,out/obj.grinch/src/grinch/main.cpp.o,src/grinch/main.cpp,b))
 $$(foreach i,$$($1.SOURCES),$$(eval $$(call object,$$($1.OUT)/$$i.o,$$i,$$($1.SOURCE_DIRS))))
 $1.BINARY = $$($1.OUT)/$1
 $$($1.BINARY): $$($1.OBJECTS)
	g++ -o $$@ $$($1.OBJECTS) -lsfml-graphics -lsfml-window
 all: $$($1.BINARY)
endef

#$(eval $(call object,out/obj.grinch/src/grinch/main.cpp.o,src/grinch/main.cpp,b))
#all: out/obj.grinch/src/grinch/main.cpp.o
$(foreach i,$(OUTPUTS),$(eval $(call binary,$i)))

clean:
	rm -rf $(OUT)
