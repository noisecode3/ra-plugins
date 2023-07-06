#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = RobotHexedFilter

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
  RobotHexedFilterPlugin.cpp \
  RobotHexedFilterDSP.cpp

LIB = ../../lib
FILES_UI  = \
	RobotHexedFilterUI.cpp \
	$(LIB)/DPF-Widgets/opengl/DearImGui.cpp \
	$(LIB)/implot/implot.cpp \
  $(LIB)/implot/implot_items.cpp \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-paint.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-geometry.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-blend.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-rle.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-dash.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-ft-raster.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-ft-stroker.c \
  $(LIB)/lunasvg/3rdparty/plutovg/plutovg-ft-math.c \
  $(LIB)/lunasvg/lunasvg.cpp \
	$(LIB)/lunasvg/element.cpp \
  $(LIB)/lunasvg/property.cpp \
  $(LIB)/lunasvg/parser.cpp \
  $(LIB)/lunasvg/layoutcontext.cpp \
  $(LIB)/lunasvg/canvas.cpp \
  $(LIB)/lunasvg/clippathelement.cpp \
  $(LIB)/lunasvg/defselement.cpp \
  $(LIB)/lunasvg/gelement.cpp \
  $(LIB)/lunasvg/geometryelement.cpp \
  $(LIB)/lunasvg/graphicselement.cpp \
  $(LIB)/lunasvg/maskelement.cpp \
  $(LIB)/lunasvg/markerelement.cpp \
  $(LIB)/lunasvg/paintelement.cpp \
  $(LIB)/lunasvg/stopelement.cpp \
  $(LIB)/lunasvg/styledelement.cpp \
  $(LIB)/lunasvg/styleelement.cpp \
  $(LIB)/lunasvg/svgelement.cpp \
  $(LIB)/lunasvg/symbolelement.cpp \
  $(LIB)/lunasvg/useelement.cpp
# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk
BUILD_FLAGS_ALL = \
	-I../../include \
	-I../../include/lunasvg \
	-I../../include/lunasvg/3rdparty/stb \
	-I../../include/lunasvg/3rdparty/plutovg \
	-I../../include/implot \
	-I../../lib/DPF-Widgets/opengl \
	-I../../lib/DPF-Widgets/opengl/DearImGui \
	-I../../lib/DPF-Widgets/generic

BUILD_C_FLAGS   += $(BUILD_FLAGS_ALL)
BUILD_CXX_FLAGS += $(BUILD_FLAGS_ALL)
# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = jack clap lv2_sep vst2 vst3 dssi ladspa

all: $(TARGETS)

# --------------------------------------------------------------

