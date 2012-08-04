
.PHONY: all
all:

SRC = src
OUT = out
SHARED = shared

OUTPUTS := $(filter-out $(SHARED),$(shell ls $(SRC)))
LAZY_DIRS =

define lazy-create-dir
 $1: | $$(dir $1)
 LAZY_DIRS += $$(dir $1)
endef

define hash
 $(shell echo -n $1 | md5sum | cut -d' ' -f1)
endef

define hash-timestamp
 $1.HASH := $2.$$(strip $$(call hash,$3)).timestamp
 $$(eval $$(call lazy-create-dir,$$($1.HASH)))
 $$($1.HASH):
	rm -f $2.*.timestamp && touch $$@
endef

define object
 $$(eval $$(call lazy-create-dir,$1))
 $1.COMMAND = g++ -o $1 -c -g $$(addprefix -I,$3) $2 -MMD -MP -MF $1.d
 $$(eval $$(call hash-timestamp,$1.COMMAND,$1.command,$$($1.COMMAND)))
 $1: $2 $$($1.COMMAND.HASH)
	$$($1.COMMAND)
 -include $1.d
endef

define stray-object
 .PHONY: $1.cleanup
 $1.cleanup:
	rm -f $1 $1.d $1.command.*.timestamp
 all: $1.cleanup
endef

define binary
 $1.OUT = out/obj.$1

 $1.SOURCE_DIRS = $$(sort $$(addprefix $$(SRC)/,$1 $$(SHARED)))
 $1.SOURCES := $$(foreach i,$$($1.SOURCE_DIRS),$$(wildcard $$i/*.cpp))
 $1.OBJECTS = $$(sort $$(foreach i,$$($1.SOURCES),$$($1.OUT)/$$i.o))
 $$(foreach i,$$($1.SOURCES),$$(eval $$(call object,$$($1.OUT)/$$i.o,$$i,$$($1.SOURCE_DIRS))))
 $1.STRAY_OBJECTS := $$(filter-out $$($1.OBJECTS),$$(shell find $$($1.OUT) -name \*.o 2>/dev/null))
 $$(foreach i,$$($1.STRAY_OBJECTS),$$(eval $$(call stray-object,$$i)))

 $1.BINARY = $$(OUT)/$1
 $1.BINARY.COMMAND = g++ -o $$@ $$($1.OBJECTS) -l sfml-graphics -l sfml-window
 $$(eval $$(call hash-timestamp,$1.BINARY.COMMAND,$$($1.OUT)/@.command,$$($1.BINARY.COMMAND)))
 $$($1.BINARY): $$($1.OBJECTS) $$($1.BINARY.COMMAND.HASH)
	$$($1.BINARY.COMMAND)
 all: $$($1.BINARY)
endef

$(foreach i,$(OUTPUTS),$(eval $(call binary,$i)))

UNIQUE_LAZY_DIRS = $(sort $(LAZY_DIRS))
$(UNIQUE_LAZY_DIRS):
	mkdir -p $@

clean:
	rm -rf $(OUT)
