# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds.
![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Moog filter
This is a low-pass filter with resonance that boost cut-off frequency. It tries to be the original ladder.

## Hexed filter
This is a modified version Dexed synth filter, its it's similar to Moog but different algorithm and sound.

# Prepare
You need to have mesa headers and basic build tools from you're distribution. make install will install in you home folder, in the future I will make root installation.

installation:
=============

    git clone https://github.com/noisecode3/ra-plugins
    cd ra-plugins
    git submodule update --init
    make
    make install

## Plugins Todo
 - TVdistortion (dynamic distortion)
 - Bark (MIDI trigger/sidechain compressor)
 - Burn (saturation)
 - LFO (MIDI CC controller LFO, inspired by Renoise LFO)
 - RandomMIDICC (done, but need rework/review the code)
