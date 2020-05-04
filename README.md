Register3D is a 3D point cloud registration tool. Given two point clouds and a set of user selected registration points it will iteratively find an optimal transform between the two sets, such that the Euclidean distance the first set and all nearest neighbours in the second set are minimized. The output transform matrix is a typical 4x4 matrix of the form

```
|R0 R1 R2 TX|
|R3 R4 R5 TY|
|R6 R7 R8 TZ|
|0  0  0  1 |
```

where R is the rotation matrix and T the translation.

This software was written during my PhD (circa 2005) for aligning 3D point clouds obtained from a laser scanner. It has been recently updated to be a more general purpose tool. At the moment it only supports PLY files (ascii or binary).

Email: nghiaho12@yahoo.com

Website: http://nghiaho.com

# Compiling on Linux
Reigster3D has been tested on Ubuntu 18.04. It should work on Windows/MacOS since it uses cross platform libraries, but the CMakeLists.txt will probably need modification.

You'll need the following libraries installed
- Eigen3
- libGLEW
- wxWidgets

On Ubuntu 18.04 you can install them via

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
make -j
./register3d
```

# Typical workflow
Here is an example of how to use Register3D to register two point cloud PLY files.

## Load PLY files
Load the two PLY files via **File -> Open first point cloud** and
**File -> Open second point cloud**. Register3D will find a transformation from the first to the second point cloud.

## Navigation
Once the point clouds are loaded you can navigate the 3D view with the following keyboard/mouse shortcuts

- **Left mouse down** - rotate the 3D view
- **Right mouse down** - pan the 3D view
- **Mouse scroll** - zoom in/out the 3D view
- **G** - centre the view on the origin (very useful if you get lost)
- **1,2,3** - switch view (top, side, back)
- **CTRL + LEFT click** - move the rotation origin (the 3D axis that is rendered)
- **SHIFT + LEFT click** - add a registration point
- **ESC** - delete the last added registration point
- **C** - toggle false colouring of the point cloud
- **D** - toggle assisstive highlighting for localising the registration point in the second point cloud
- **S** - toggle assisstive highlighting using transparent spheres for localising the registration point in the second point cloud

## Add registration points
You will need to add 4 registration points betwen the first and second point cloud. Navigate the two views so you can see the common 3D point. Add the registration by SHIFT + LEFT clicking on the point in the first and second point cloud. If at anytime you make a mistake you can undo the added point via the ESC key. Once you have 4 registration points the **Register** button will enable.

![](https://github.com/nghiaho12/Register3D/blob/master/images/register3d_1.jpg "Adding registration points")

## Register
Click the Register button. You'll be presented with some options for the registration algorithm, which uses iterative closest point (ICP). Here is what each of them do

- **Maximum point for ICP** - Maximum number of points for ICP. This is limited by available memory and time you are willing to wait. More points will be improve accuracy, but there is a diminishing return.
- **Initial outlier distance** - Discard any points in the first point cloud that are too far away from the second point cloud, after applying the initial transform. The aim is to keep only the overlapping points. The units are absolute.
- **Least Trimmed Square percentage** - Amount of inliers we expect, even after discarding non-overlapping points. This allows some robustness to noisy points.
- **Maximum number of iterations** - Maximum ICP iterations.
- **Minimum relative MSE** - ICP will terminate early if the relative improvement in mean square error (MSE) falls bellow this threshold.
- **Nearest neighbour search error** - libANN is used under the hood to provided fast nearest neighbour search. The library allows tuning a trade off between speed and accuracy of the searching.

Click OK when you are happy with the settings.

## After registration
When ICP has finished, the 3D view will automatically switch to a merged view showing both point clouds in the same world. You can switch to false colour via the C key to see how well the registration is. To go back to the split view click the **Toggle view** button at the top.

![](https://github.com/nghiaho12/Register3D/blob/master/images/register3d_2.jpg "Merged view with false colouring")

You can now save the result of the registration via **File -> Save first point cloud and transformation matrix** or **File -> Save transformation matrix**. The first option will save both PLY and matrix file, the latter only saves the matrix transform. The transformation matrix is saved out as a text file as row major.

# TODO
- Unit tests!
- Update to modern OpenGL 3+