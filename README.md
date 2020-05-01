
Register3D is a 3D point cloud registration/alignment tool using the
Iterative Closest Point (ICP) algorithm. This software was written by me
during my PhD studies but modified for general consumption.

This program is released under the GPL license because it uses GNU GSL.

 * Copyright (C) 2010, 2011 Nghia Ho
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

Email: nghiaho12@yahoo.com
Website: http://nghiaho.com

COMPILING
===============================================================================
Tested on Ubuntu 18.04.

The following libraries need to be installed
- libgsl (GNU Scientific Library)
- libGLEW
- wxWidgets

On Ubuntu, you can run:
sudo apt-get install libwxgtk3.0-dev
sudo apt-get install libglew-dev

To build the binary using CMake:

mkdir build
cd build
cmake ..
make
./register3d


