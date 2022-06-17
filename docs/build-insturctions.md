# Build Instructions

## Dependencies

### ONNX Runtime 

Download latest release and extract to `src/RenderHost/Lib` directory, change `CMakeLists.txt` if necessary.

Official Page: https://github.com/microsoft/onnxruntime

## Platform

### Windows

Windows SDK is required since it ships the headers and the libraries required to build Windows Applications.

### Mac OS

To be implemented...

### Linux

#### Packages 

You need to install these packages to get the required C/C++ header files.

+ Midi
````
sudo apt-get install libasound2-dev
````

+ OpenGL
````
sudo apt-get install mesa-common-dev
````

+ X11
````
sudo apt install libqt5x11extras5-dev
````

#### Local input method

Qt doesn't work well with input programs, you may need to move some shared libraries to the `lib` directory to enable it in Qt Application.

## Build from source

There's no other libraries you need to compile in advance.
