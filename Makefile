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
	$(MAKE) -C dpf/dgl opengl3

plugins: dgl
	# Plugins
	$(MAKE) all -C plugins/RobotMoogFilter
	$(MAKE) all -C plugins/RobotHexedFilter
	#$(MAKE) all -C plugins/RobotBarkCompressor

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

	find lib -name "*cpp.d" -exec rm {} \;
	find lib -name "*cpp.o" -exec rm {} \;
	rm -fr bin
	rm -fr build

# --------------------------------------------------------------

install:
	install -d $(DESTDIR)$(PREFIX)/.ladspa/
	install -d $(DESTDIR)$(PREFIX)/.dssi/
	install -d $(DESTDIR)$(PREFIX)/.lv2/
	install -d $(DESTDIR)$(PREFIX)/.vst3/
	install -d $(DESTDIR)$(PREFIX)/.vst/
	install -d $(DESTDIR)$(PREFIX)/.clap/
	install -d $(DESTDIR)$(PREFIX)/.local/bin/

	install -m 644 bin/*-ladspa.* $(DESTDIR)$(PREFIX)/.ladspa/
	install -m 644 bin/*-dssi.*   $(DESTDIR)$(PREFIX)/.dssi/
ifeq ($(HAVE_LIBLO),true)
	cp -r  bin/*-dssi $(DESTDIR)$(PREFIX)/.dssi/
endif # HAVE_LIBLO
	cp -rL bin/*.lv2  $(DESTDIR)$(PREFIX)/.lv2/
	cp -rL bin/*.vst3 $(DESTDIR)$(PREFIX)/.vst3/
	cp -rL bin/*.clap $(DESTDIR)$(PREFIX)/.clap/

	#install -m 755 bin/RobotMoogFilter$(APP_EXT)       $(DESTDIR)$(PREFIX)/.local/bin/
	install -m 755 bin/RobotHexedFilter$(APP_EXT)      $(DESTDIR)$(PREFIX)/.local/bin/

# --------------------------------------------------------------

.PHONY: plugins

