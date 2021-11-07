# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds. I make my plugins with lots of love.
Im will fix all the bugs I test them a lot usually in carla (or Ardour or Renoise)

![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Bark Compressor
Bark is a peak compressor that filters the compressed part of signal with the Bark scale and controls the compressed voltage with tube emulation.
Bark has both MIDI and audio side chain modes.

 - don't use in serious projects yet, I have not finished the calculations, parts of the plugin is missing
 - https://en.wikipedia.org/wiki/Bark_scale


## Hexed Filter
Modified version of Dexed synth filter, low-pass filter with 1-4 pole modes

## Moog Filter
Low-pass Moog like filter

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