cmake_minimum_required(VERSION 2.8)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

project("GP Prime" C CXX)

file (GLOB source
tree.cpp
)



#raytracer executable
add_executable(Primer main.cpp ${source} tree.h parameters.h )

