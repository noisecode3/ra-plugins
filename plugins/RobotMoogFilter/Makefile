#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = RobotMoogFilter

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	RobotMoogFilterPlugin.cpp

#FILES_UI  = \
#	RobotMoogFilterArtwork.cpp \
#	RobotMoogFilterUI.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = clap vst2 vst3 lv2_sep dssi ladspa

all: $(TARGETS)

# --------------------------------------------------------------

