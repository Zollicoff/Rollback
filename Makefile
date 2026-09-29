GBDK_HOME ?= $(CURDIR)/.tools/gbdk
PYTHON ?= $(CURDIR)/.tools/venv/bin/python
LCC = $(GBDK_HOME)/bin/lcc
ROM = build/rollback-e1-training.gbc
SOURCES = src/main.c src/game.c src/video.c src/audio.c build/assets.c build/missions.c

.PHONY: all setup play test package clean
all: $(ROM)
setup:
	python3 tools/setup.py
build/assets.c: tools/assets.py
	@mkdir -p build
	$(PYTHON) tools/assets.py
build/assets.h: build/assets.c
	@test -f $@
build/missions.c: data/training.json tools/content.py src/game.h
	@mkdir -p build
	$(PYTHON) tools/content.py
$(ROM): $(SOURCES) src/game.h build/assets.h
	$(LCC) -Isrc -Ibuild -Wm-yC -Wm-yt0x19 -Wm-yo4 -Wm-ynROLLBACK -Wl-m -Wl-j -o $@ $(SOURCES)
play: $(ROM)
	$(PYTHON) tools/play.py $(ROM)
test: $(ROM)
	$(PYTHON) tools/verify.py $(ROM)
package: test
	$(PYTHON) tools/package.py
clean:
	$(PYTHON) -c "import shutil; shutil.rmtree('build', ignore_errors=True)"
