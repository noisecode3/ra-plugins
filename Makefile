#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: dgl plugins gen

# --------------------------------------------------------------

PREFIX  ?= /home/$(USER)
DESTDIR ?=

# --------------------------------------------------------------

dgl:
ifeq ($(HAVE_CAIRO_OR_OPENGL),true)
	$(MAKE) -C dpf/dgl
endif

plugins: dgl
	# Plugins
	$(MAKE) all -C plugins/RobotMoogFilter
	$(MAKE) all -C plugins/RobotHexedFilter
	$(MAKE) all -C plugins/RobotBarkCompressor

gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator

	# Plugins
	$(MAKE) clean -C plugins/RobotMoogFilter
	$(MAKE) clean -C plugins/RobotHexedFilter
	$(MAKE) clean -C plugins/RobotBarkCompressor

# --------------------------------------------------------------

install:
	install -d $(DESTDIR)$(PREFIX)/.ladspa/
	#install -d $(DESTDIR)$(PREFIX)/.dssi/
	install -d $(DESTDIR)$(PREFIX)/.lv2/
	install -d $(DESTDIR)$(PREFIX)/.vst/
	#install -d $(DESTDIR)$(PREFIX)/.local/bin/

	install -m 644 bin/*-ladspa.* $(DESTDIR)$(PREFIX)/.ladspa/
	#install -m 644 bin/*-dssi.*   $(DESTDIR)$(PREFIX)/.dssi/
	install -m 644 bin/*-vst.*    $(DESTDIR)$(PREFIX)/.vst/
	cp -r bin/*.lv2   $(DESTDIR)$(PREFIX)/.lv2/

ifeq ($(HAVE_JACK),true)
	#install -m 755 bin/RobotMoogFilter$(APP_EXT)       $(DESTDIR)$(PREFIX)/.local/bin/
	#install -m 755 bin/RobotDexedFilter$(APP_EXT)      $(DESTDIR)$(PREFIX)/.local/bin/
endif # HAVE_JACK

# --------------------------------------------------------------

.PHONY: plugins

