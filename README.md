# Robot Audio Plugins
This is a work in progress! I like noisy and heavy music mostly and are interested in those sounds
![screenshot](https://raw.githubusercontent.com/noisecode3/ra-plugins/main/extra/RobotAudioLogoTransparent.png "Beta Logo")

## Hexed filter
This is a modified version of Dexed synth filter, its alow-pass filter with 1-4 pole modes(1 means 4 pole now..will change) its a small waveshaping/distortion on the resonance tone that subjectively to me sounds like underwater bubbles or whale song on lower frequencies with small reverb, or wind blowing on fast drums on low frequency with a lot of resonance. It has a bit metallic brightness. Still have to remove clicks from changing pole mode.

## Moog filter
This is a low-pass filter with resonance that boost cut-off frequency. It tries to be the original ladder.
This is not completed I gonna change some small things. But the sound is mostly complete.

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
 - TVdistortion (dynamic distortion)
 - Bark (MIDI trigger/sidechain compressor)
 - Burn (saturation)
 - LFO (MIDI CC controller LFO, inspired by Renoise LFO)
 - RandomMIDICC (done, but need rework/review the code)
