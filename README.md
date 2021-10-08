# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds. I make my plugins with lots of love.
Im will fix all the bugs I test them a lot usually in carla (or Ardour or Renoise)

![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Bark Compressor
MIDI/audio triggered peak compressor, I now think like 2 modes, 1 normal that get trigger by audio, 2 gets triggered by midi Note on and off and threshold

 - coming soon, don't use, won't build by default
 - https://en.wikipedia.org/wiki/Bark_scale
 - https://github.com/micknoise/Maximilian/blob/master/cpp/openFrameworks/ofxMaxim/libs/maxiBark.h
 - To do: CV try Bark scale and tube emulation
 - To do: scale parameters
 - To do: midi

## Hexed Filter
Modified version of Dexed synth filter, low-pass filter with 1-4 pole modes

 - To do: make 5-8 presets to for quick demonstration
 - To do: make UI (I like big knobs and I can not lie)
 - To do: remove dsp instructions that are not needed (when parameters not changed)
 - To do: change parameter smooth strategy for bigger buffers and optimize parameter smoothing
 - To do: clean up

## Moog Filter
Low-pass Moog like filter

 - To do: make 5-8 presets to for quick demonstration
 - To do: Make UI (I like big knobs and I can not lie)
 - To do: remove dsp instructions that are not needed (when parameters not changed)
 - To do: change parameter smooth strategy for bigger buffers and optimize parameter smoothing
 - To do: clean up


# Prepare
You need to have mesa headers and basic build tools and git from you're distribution(I don't have a GUI lol, dpf needs it).
make install will install in you're home folder, in the future I will make root installation.

installation Guide:
=============

    git clone https://github.com/noisecode3/ra-plugins
    cd ra-plugins
    git submodule update --init
    make
    make install

COPY and PASTE ME to install:
=============

    git clone https://github.com/noisecode3/ra-plugins; cd ra-plugins; git submodule update --init; make; make install;

## Plugins Todo
 - TVDistortion (dynamic distortion)
 - Burn (saturation)
 - LFO (MIDI CC controller LFO, inspired by Renoise LFO)
 - RandomMIDICC (done, but need rework/review the code)

## Resources
 - https://www.dsprelated.com/freebooks/filters/