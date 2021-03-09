SHELL:=$(or $(CONFIG_SHELL),bash)
BSB := ./node_modules/.bin/bsb.exe
BSB_ARGS:= -make-world
SOURCE_DIRS_JSON := lib/bs/.sourcedirs.json
TARGET := $(or $(TARGET),web)
BACKEND_DIR := ./src/Server

serve: copy_bsconfig
	@make -j 2 serve_frontend serve_backend

copy_bsconfig:
	rm -rf bsconfig.json
	cp "bsconfig.$(TARGET).json" bsconfig.json

serve_frontend:
	trap 'kill %1' INT TERM
    # BuckleScript doesn't like being run first.
	yarn serve & $(MAKE) watch

serve_backend:
	trap 'kill %1' INT TERM
    # BuckleScript doesn't like being run first.
	cd $(BACKEND_DIR); yarn start:dev & bsb -make-world -w

$(SOURCE_DIRS_JSON): bsconfig.json
	$(BSB) -install

bs:
	$(BSB) $(BSB_ARGS)

watch_frontend: $(SOURCE_DIRS_JSON)
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
