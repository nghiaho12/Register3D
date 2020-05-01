Register3D_wx 1.1.1
===============================================================================
Register3D is a 3D point cloud stitching program using the 
Iterative Closest Point (ICP) algorithm. It can read text files exported from 
Riscan Pro or a custom binary format. This software was written by Nghia Ho 
during his  PhD studies at Monash University.

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

COMPILING ON LINUX
===============================================================================
To compile the Linux version of Register3D_wx you will need the following 
development libraries installed on your system:

- libgsl (GNU Scientific Library)
- libGLEW
- wxWidgets

Simply type 'make' to compile. And run the program by calling
'bin/Release/Register3D_wx'

The Makefile relies on wx-config and gsl-config to be present to find the libraries.

COMPILING ON WINDOWS
===============================================================================
The Windows version comes with a pre-compiled binary already in bin/Release.
I compiled using a 32bit version of MingW gcc 4.4.1. So it should work on a 
64bit version of Windows as well.

If you wish to compile on Windows you will need to install CodeBlock with MinGW.
To make it easier, all the necessary libraries have been included 
(GSL, GLEW, wxWidgets, zlib) though they might be out of date. So keep that in mind.
