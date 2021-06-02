# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds

![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Hexed filter
This is a little bit modified version of Dexed synth filter, its alow-pass filter with 1-4 pole modes and bit metallic tone

 - To do: remove clicks, balance the amplitude, make 5-8 presets to for quick demonstration


## Moog filter
This is a low-pass Moog like filter

 - To do: make resonance in percent, should cutoff also be in percent?


# Prepare
You need to have mesa headers and basic build tools from you're distribution(I don't have a GUI lol). make install will install in you're home folder, in the future I will make root installation.

installation:
=============

    git clone https://github.com/noisecode3/ra-plugins
    cd ra-plugins
    git submodule update --init
    make
    make install

## Plugins Todo
 - TVDistortion (dynamic distortion)
 - Bark (MIDI trigger/sidechain compressor)
 - Burn (saturation)
 - LFO (MIDI CC controller LFO, inspired by Renoise LFO)
 - RandomMIDICC (done, but need rework/review the code)
