cmake_minimum_required(VERSION 2.8)
project (register3D)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(wxWidgets COMPONENTS core base gl richtext html REQUIRED)

include(${wxWidgets_USE_FILE})

add_library(libANN
    ann_1.1.2/src/ANN.cpp
    ann_1.1.2/src/bd_fix_rad_search.cpp
    ann_1.1.2/src/bd_pr_search.cpp
    ann_1.1.2/src/bd_search.cpp
    ann_1.1.2/src/bd_tree.cpp
    ann_1.1.2/src/bd_tree.h
    ann_1.1.2/src/brute.cpp
    ann_1.1.2/src/kd_dump.cpp
    ann_1.1.2/src/kd_fix_rad_search.cpp
    ann_1.1.2/src/kd_fix_rad_search.h
    ann_1.1.2/src/kd_pr_search.cpp
    ann_1.1.2/src/kd_pr_search.h
    ann_1.1.2/src/kd_search.cpp
    ann_1.1.2/src/kd_search.h
    ann_1.1.2/src/kd_split.cpp
    ann_1.1.2/src/kd_split.h
    ann_1.1.2/src/kd_tree.cpp
    ann_1.1.2/src/kd_tree.h
    ann_1.1.2/src/kd_util.cpp
    ann_1.1.2/src/kd_util.h
    ann_1.1.2/src/lib
    ann_1.1.2/src/perf.cpp
    ann_1.1.2/src/pr_queue.h
    ann_1.1.2/src/pr_queue_k.h)

target_include_directories(libANN PRIVATE ann_1.1.2/include)

add_library(lib
    src/About.cc
    src/About.h
    src/Cylinder.cc
    src/Cylinder.h
    src/GLCanvas.cc
    src/GLCanvas.h
    src/Global.h
    src/Help.cc
    src/Help.h
    src/ICP.cc
    src/ICPDialog.cc
    src/ICPDialog.h
    src/ICP.h
    src/ICPPoint.h
    src/MainWindow.cc
    src/MainWindow.h
    src/Math2.cc
    src/Math2.h
    src/Matrix.cc
    src/Matrix.h
    src/Misc.cc
    src/Misc.h
    src/MyANN.cc
    src/MyANN.h
    src/OGLWrapper.cc
    src/OGLWrapper.h
    src/Point.cc
    src/Point.h
    src/PointOP.cc
    src/PointOP.h
    src/PLYReader.cc
    src/PLYReader.h
    src/Sphere.h
    src/ZPR.cc
    src/ZPR.h
)

target_include_directories(lib PRIVATE ann_1.1.2/include)

add_executable(register3d src/main.cc)
target_link_libraries(register3d lib libANN z GL GLU GLEW gsl gslcblas ${wxWidgets_LIBRARIES})
target_include_directories(register3d PRIVATE ann_1.1.2/include)
