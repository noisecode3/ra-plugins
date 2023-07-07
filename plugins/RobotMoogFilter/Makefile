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


# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BUILD_FLAGS_ALL = \
        -I../../include

BUILD_C_FLAGS   += $(BUILD_FLAGS_ALL)
BUILD_CXX_FLAGS += $(BUILD_FLAGS_ALL)
# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = lv2_dsp clap vst2 vst3 ladspa
all: $(TARGETS)
# --------------------------------------------------------------
