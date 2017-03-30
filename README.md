# gl-research-toolkit
A small collection of tools thought for researching realtime graphic techniques using OpenGL

## Known issues

- Sometime starts with GL_OUT_OF_MEMORY or completely rubbish images. In case that happens, just close and restart.  
    Note to myself: I will never use OpenGL extensions again which aren't alreada well established and tested (I'm looking at you, NV_command_list)


## Install

Install instructions for Ubuntu 16.04

### System Requirenments

- Ubuntu 16.04 LTS or newer
- NVidia GTX 960 or newer
- OpenGL 4.5 or newer

### Install dependencies

    sudo apt-get install git cmake g++ 
    sudo apt-get install qtbase5-dev libqt5svg5-dev
    sudo apt-get install libglew-dev libassimp-dev libsdl2-dev
    
### Compile

    mkdir build
    cd build
    cmake ..
    make
    
### Voxelize

In order to (re)voxelize, blender >= 2.78 needs to be installed with the blender >= 2.78 executable in the `PATH` environment variable.
