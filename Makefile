GBDK_HOME ?= $(CURDIR)/.tools/gbdk
PYTHON ?= $(CURDIR)/.tools/venv/bin/python
LCC = $(GBDK_HOME)/bin/lcc
ROM = build/rollback.gbc

.PHONY: all setup play test package clean content
all: $(ROM)
setup:
	python3 tools/setup.py
build/assets.c: tools/assets.py
	@mkdir -p build
	$(PYTHON) tools/assets.py
build/assets.h: build/assets.c
	@test -f $@
build/campaign.stamp: tools/campaign.py data/campaign.json content src/campaign.h
	@mkdir -p build
	$(PYTHON) tools/campaign.py
	@touch $@
build/portraits.stamp: tools/portraits.py tools/campaign.py
	@mkdir -p build
	$(PYTHON) tools/portraits.py
	@touch $@
$(ROM): $(wildcard src/*.c) src/game.h src/campaign.h build/assets.c build/assets.h build/campaign.stamp build/portraits.stamp
	$(PYTHON) tools/build.py "$(LCC)" "$@"
play: $(ROM)
	$(PYTHON) tools/play.py $(ROM)
test: $(ROM)
	$(PYTHON) tools/verify_campaign.py $(ROM)
package: test
	$(PYTHON) tools/package.py
clean:
	$(PYTHON) -c "import shutil; shutil.rmtree('build', ignore_errors=True)"
