cmake_minimum_required(VERSION 3.14)

project(DendenDesktop CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
add_executable(logicalsweeper guimain.cpp ai.cpp board.cpp boardview.cpp boardgenerationprogress.cpp boardconfigview.cpp)
target_link_libraries(logicalsweeper ${wxWidgets_LIBRARIES})
