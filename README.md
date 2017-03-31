# gl-research-toolkit
A small collection of tools thought for researching realtime graphic techniques using OpenGL

## Known issues

- Sometime starts with GL_OUT_OF_MEMORY or completely rubbish images. In case that happens, just close and restart.  
    Note to myself: I will never use OpenGL extensions again which aren't alreada well established and tested (I'm looking at you, NV_command_list)
- When compiling with g++, compiler warnings appear
- When quitting the software, a warning appears: *process 10332: arguments to dbus_connection_unref() were incorrect, assertion "connection->generation == _dbus_current_generation" failed in file ../../dbus/dbus-connection.c line 2822.*
This is normally a bug in some application using the D-Bus library.


## Install

Install instructions for Ubuntu 16.04

### System Requirenments

- Ubuntu 16.04 LTS or newer
- Recommended:  NVidia GTX 980 or newer (minimum: NVidia GTX 960)
- OpenGL 4.5 or newer
- Recommended: Qt 5.8 or newer
    - in the ubuntu package system, the packages `qtbase5-dev` and `libqt5svg5-dev` are available, which install 5.5 for ubuntu 16.04.  
        While this software should be compatible to 5.5, I recommend using 5.8 or newer, as this software was developed and tested using 5.8

### Install dependencies

    sudo apt-get install git cmake g++ 
    sudo apt-get install libglew-dev libassimp-dev libsdl2-dev libopenexr-dev
    
    
### Compile

    mkdir build
    cd build
    cmake ..
    make
    
### Voxelize

If the already voxelized files exist, (for example */external/assets/advanced/sponza/sponza-floor{12f33bff-74aa-54df-b3ca-2d3003780cb7}.mesh.signed-distance-field.texture*) no further voxelization is necessary.

If they don't exist, the meshes need to be voxelized first. In order to voxelize, blender >= 2.78 needs to be installed. The directory containing the blender executable is expected to be located in the `PATH` environment variable. In order to avoid ambiguity, you can also create a symlink to the 2.78 blender executable with the name `blender-newest`.

**Warning** revoxelizing needs a modern NVidia card (GTX 960 or better) and may block your computer for a few minutes while doing so. If already existing, I recommend to just use the already voxelized files in the asset directories.

## Usage

Run the *deferred-renderer* executable for the best experience using the software. In order to have all debugging options available, execute the *forward-renderer*.

Navigation with WASD while holding the right mouse button down. The Toolbar on the lower right allows toggling the different dialogs.

