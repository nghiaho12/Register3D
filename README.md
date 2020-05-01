Register3D is a 3D point cloud registration tool using the Iterative Closest Point (ICP) algorithm. This software was written during my PhD (circa 2010). It has been recently updated to be a more general purpose tool.

Email: nghiaho12@yahoo.com
Website: http://nghiaho.com

COMPILING ON LINUX
===============================================================================
This software has been tested on Ubuntu 18.04.

You'll need the following libraries installed
- Eigen3
- libGLEW
- wxWidgets

On Ubuntu you can install via

```
sudo apt install libeigen3-dev
sudo apt install libglew-dev
sudo apt install libwxgtk3.0-dev
```

To build the binary via CMake

```
mkdir build
cd build
cmake ..
make
./register3d
```

