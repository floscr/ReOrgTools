BSB := ./node_modules/.bin/bsb.exe
BSB_ARGS:= -make-world
SOURCE_DIRS_JSON := lib/bs/.sourcedirs.json
TARGET := $(or $(TARGET),web)
BACKEND_DIR := ./src/Server

#############
# Development
#############

dev: copy_bsconfig
    # Kill any leftover processes before starting the server
	ps -xa | grep $(pwd) | grep node | grep -v grep | awk {'print $1'} | xargs --no-run-if-empty kill -9
	@make -j 10 serve_frontend serve_backend

serve_frontend: copy_bsconfig
	trap 'kill %1' INT TERM
	$(MAKE) start_frontend & $(MAKE) watch_bsb_frontend

serve_backend:
	trap 'kill %1' INT TERM
	@make -j 2 start_backend watch_bsb_backend

start_frontend:
	yarn serve

start_backend:
	cd $(BACKEND_DIR); yarn start:dev

watch_bsb_frontend: $(SOURCE_DIRS_JSON)
    # `entr` exits when the directory contents change, so we restart it to pick
    # up updated files
	while true; do \
		trap 'break' INT; \
		find -L $$(jq -r 'include "./dirs"; dirs' $(SOURCE_DIRS_JSON)) -maxdepth 1 \
			-type f -iregex ".*\.\(re\|ml\)i?" | \
		entr -nd $(BSB) $(BSB_ARGS); \
	done

watch_bsb_backend:
	while true; do \
		trap 'break' INT; \
		cd $(BACKEND_DIR); find -L $$(jq -r 'include "./dirs"; dirs' $(SOURCE_DIRS_JSON)) -maxdepth 1 \
			-type f -iregex ".*\.\(re\|ml\)i?" | \
		entr -nd $(BSB) $(BSB_ARGS); \
	done

# Testing
test_backend:
	cd ./src/Server; yarn run test

##########
# Building
##########

build_frontend_reorga:
	cd ./src/ReOrga; bsb -make-world

build_frontend: copy_bsconfig build_frontend_reorga build
	yarn build:webpack

build_esbuild: build
	yarn build:esbuild

build:
	$(BSB) $(BSB_ARGS)

#####
# CLI
#####

generate_theme:
	cd ./src/ThemeGeneration; bsb -make-world
	node ./src/ThemeGeneration/GenerateTheme.bs.js

#######
# Tools
#######

copy_bsconfig:
	rm -rf bsconfig.json
	cp "bsconfig.$(TARGET).json" bsconfig.json

$(SOURCE_DIRS_JSON): bsconfig.json
	$(BSB) -install

bs:
	$(BSB) $(BSB_ARGS)

print-%: ; @echo $*=$($*)

clean:
	$(BSB) -clean-world

.PHONY: bs bsdirs all clean
