# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds. I make my plugins with lots of love.
Im trying to fix all the bugs I test them a lot usually in carla (or Ardour or Renoise)

![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Hexed filter
Modified version of Dexed synth filter, low-pass filter with 1-4 pole modes

 - To do: make 5-8 presets to for quick demonstration
 - To do: make UI (I like big knobs and I can not lie)
 - To do: clean up

## Moog filter
Low-pass Moog like filter

 - To do: make resonance and cutoff in percent
 - To do: remake presets
 - To do: Make UI
 - To do: clean up


# Prepare
You need to have mesa headers and basic build tools from you're distribution(I don't have a GUI lol).
make install will install in you're home folder, in the future I will make root installation.

installation:
=============

    git clone https://github.com/noisecode3/ra-plugins
    cd ra-plugins
    git submodule update --init
    make
    make install

The one line COPY and PASTE ME:
=============

    git clone https://github.com/noisecode3/ra-plugins; cd ra-plugins; git submodule update --init; make; make install;

## Plugins Todo
 - TVDistortion (dynamic distortion)
 - Bark (MIDI trigger/sidechain compressor)
 - Burn (saturation)
 - LFO (MIDI CC controller LFO, inspired by Renoise LFO)
 - RandomMIDICC (done, but need rework/review the code)

## Resources
 - https://www.dsprelated.com/freebooks/filters/