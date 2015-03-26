# Minimum required CMake version
cmake_minimum_required(VERSION 2.8)

# Project name
project(calib_tool)

# Find the OpenCV installation
find_package(OpenCV REQUIRED)

# Find the Boost installation, specifically the components 'system' and 'filesystem'
find_package(Boost COMPONENTS system filesystem REQUIRED)

# set path to the images
set(DATA_FOLDER "/home/samarth/Pictures/Webcam/logitech_640x480")
set(IMAGE_FOLDER "${DATA_FOLDER}/images/")

# set the configuration input file to ${PROJECT_SOURCE_DIR}/Config.h.in and the includable header file holding configuration information to ${PROJECT_SOURCE_DIR}/include/Config.h
configure_file("${PROJECT_SOURCE_DIR}/Config.h.in" "${PROJECT_SOURCE_DIR}/include/Config.h")

# Other directories where header files for linked libraries can be found
include_directories(${OpenCV_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})

# executable produced as a result of compilation
add_executable(calib_tool src/calib_tool.cpp)
# libraries to be linked with this executable - OpenCV
target_link_libraries(calib_tool ${OpenCV_LIBS} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
