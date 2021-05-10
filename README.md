# Robot Audio Plugins
This is a work in progress! I had a repository with a distortion plugin I called disto::tv or TVDistortion. It was extremely experimental and I took it to different directions and renamed some parameters about 5 times. Will finish that and put the source code here. Also think its better I keep my experiments to myself and put plugins here that I think is usable for others, that have a clear direction and whole idea. I believe in modular plugins that do one thing and do it good. I like noisy and heavy music mostly and will probably be influenced by those sounds.

## Moog filter
This is a low-pass filter with resonance that boost at cut off frequency.

# Prepare
You need to have mesa headers and basic build tools from you're distribution. make install will install in you home folder, in the future I will make root installation.

installation:
=============

    git clone https://github.com/noisecode3/ra-plugins
    cd ra-plugins
    git submodule update --init
    make
    make install
