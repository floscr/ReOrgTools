SHELL:=$(or $(CONFIG_SHELL),bash)
BSB := ./node_modules/.bin/bsb.exe
BSB_ARGS:= -make-world
SOURCE_DIRS_JSON := lib/bs/.sourcedirs.json
TARGET := $(or $(TARGET),web)

all:
	serve

org_clock:
	TARGET=node $(MAKE) copy_bsconfig
	trap 'kill %1' int term
	yarn
	$(MAKE) watch & $(MAKE) org_clock_watch

copy_bsconfig:
    # Copy bsconfig for either web or node
	rm -rf bsconfig.json
	cp "bsconfig.$(TARGET).json" bsconfig.json

org_clock_watch:
	while true; do \
		trap 'break' int; \
		find src -name '*.js' | \
		entr -nd node ./src/OrgClock/OrgClock.bs.js; \
	done

serve:
	trap 'kill %1' INT TERM
    # BuckleScript doesn't like being run first.
	yarn serve & $(MAKE) watch

$(SOURCE_DIRS_JSON): bsconfig.json
	$(BSB) -install

bs:
	$(BSB) $(BSB_ARGS)

watch: $(SOURCE_DIRS_JSON)
    # `entr` exits when the directory contents change, so we restart it to pick
    # up updated files
	while true; do \
		trap 'break' INT; \
		find -L $$(jq -r 'include "./dirs"; dirs' $(SOURCE_DIRS_JSON)) -maxdepth 1 \
			-type f -iregex ".*\.\(re\|ml\)i?" | \
		entr -nd $(BSB) $(BSB_ARGS); \
	done

build:
	$(BSB) $(BSB_ARGS)

print-%: ; @echo $*=$($*)

clean:
	$(BSB) -clean-world

.PHONY: bs bsdirs all clean
