# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds
![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Hexed filter
This is a modified version of Dexed synth filter, its also a low-pass filter with 4 modes and can be steep and the resonance is now extremely ringy.
I like it and it reminds me of old Cabaret Voltaire. I will probably change the sound and especially the tone.
With filter duck and wet parameters I could get some really beautiful tone out of it. its not completely balanced yet.

## Moog filter
This is a low-pass filter with resonance that boost cut-off frequency. It tries to be the original ladder.
this is not completed I gonna change some small things here.

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
