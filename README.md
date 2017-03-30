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
    sudo apt-get install libglew-dev libassimp-dev libsdl2-dev libopenexr-dev
    
    # I have never tested it with qt 5.5
    # It would be better to install the newest qt version from the [qt website](https://www.qt.io/download/).
    # But maybe the native qt package will also work? I have never tested it.
    sudo apt-get install qtbase5-dev libqt5svg5-dev
    
### Compile

    mkdir build
    cd build
    cmake ..
    make
    
### Voxelize

In order to (re)voxelize, blender >= 2.78 needs to be installed. The directory containing the blender executable is allowed to be located in the `PATH` environment variable. In order to avoid ambiguity, you can also create a symling to the 2.78 blender executable with the name `blender-newest`.
