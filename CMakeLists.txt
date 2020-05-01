cmake_minimum_required(VERSION 2.8)
project (register3D)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(wxWidgets COMPONENTS core base gl richtext html REQUIRED)
find_package(Eigen3 3 REQUIRED NO_MODULE)

include(${wxWidgets_USE_FILE})

add_library(libANN
    src/ann_1.1.2/src/ANN.cpp
    src/ann_1.1.2/src/bd_fix_rad_search.cpp
    src/ann_1.1.2/src/bd_pr_search.cpp
    src/ann_1.1.2/src/bd_search.cpp
    src/ann_1.1.2/src/bd_tree.cpp
    src/ann_1.1.2/src/bd_tree.h
    src/ann_1.1.2/src/brute.cpp
    src/ann_1.1.2/src/kd_dump.cpp
    src/ann_1.1.2/src/kd_fix_rad_search.cpp
    src/ann_1.1.2/src/kd_fix_rad_search.h
    src/ann_1.1.2/src/kd_pr_search.cpp
    src/ann_1.1.2/src/kd_pr_search.h
    src/ann_1.1.2/src/kd_search.cpp
    src/ann_1.1.2/src/kd_search.h
    src/ann_1.1.2/src/kd_split.cpp
    src/ann_1.1.2/src/kd_split.h
    src/ann_1.1.2/src/kd_tree.cpp
    src/ann_1.1.2/src/kd_tree.h
    src/ann_1.1.2/src/kd_util.cpp
    src/ann_1.1.2/src/kd_util.h
    src/ann_1.1.2/src/lib
    src/ann_1.1.2/src/perf.cpp
    src/ann_1.1.2/src/pr_queue.h
    src/ann_1.1.2/src/pr_queue_k.h)

target_include_directories(libANN PRIVATE src/ann_1.1.2/include)

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
    src/PointReader.cc
    src/PointReader.h
    src/Sphere.h
    src/ZPR.cc
    src/ZPR.h
)

target_include_directories(lib PRIVATE src/ann_1.1.2/include)
target_link_libraries (lib Eigen3::Eigen)

add_executable(register3d src/main.cc)
target_link_libraries(register3d lib libANN z GL GLU GLEW gsl gslcblas ${wxWidgets_LIBRARIES})
target_include_directories(register3d PRIVATE src/ann_1.1.2/include)
