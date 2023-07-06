#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = RobotBarkCompressor

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	RobotBarkCompressorPlugin.cpp \
	../../include/FFTConvolver/AudioFFT.cpp \
	../../include/FFTConvolver/FFTConvolver.cpp \
	../../include/FFTConvolver/Utilities.cpp

#FILES_UI  = \
#	RobotBarkCompressorArtwork.cpp \
#	RobotBarkCompressorUI.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BUILD_C_FLAGS   += -I../../include
BUILD_CXX_FLAGS += -I../../include

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_JACK),true)
#TARGETS += jack
endif

#TARGETS += vst3

#ifeq ($(HAVE_DGL),true)
#ifeq ($(HAVE_LIBLO),true)
#TARGETS += dssi
#endif
#endif

#ifeq ($(HAVE_DGL),true)
#TARGETS += lv2_sep
#else
TARGETS += lv2_dsp
#endif

all: $(TARGETS)

# --------------------------------------------------------------

