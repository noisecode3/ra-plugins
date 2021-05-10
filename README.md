# Robot Audio Plugins
This is a work in progress!

## Moog filter
This is a classic filter. It can be used to boost frequencies with the resonance knob and wet knob to blend it in and more like a low pass or base boost filter with only the wet and frequency knob.

# Prepare
You need to have mesa headers and basic build tools from you're distribution. make install will install in you home folder, in the future I will make root installation.

installation:
=============

    git clone https://github.com/noisecode3/ra-plugins
    cd ra-plugins
    git submodule update --init
    make
    make install
