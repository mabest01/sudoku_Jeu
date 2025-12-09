cmake_minimum_required(VERSION 3.10)
project(SudokuMaster C)

set(CMAKE_C_STANDARD 99)

# Find PkgConfig to handle GTK dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Include directories
include_directories(${GTK3_INCLUDE_DIRS} src)
link_directories(${GTK3_LIBRARY_DIRS})

# Compiler flags
add_definitions(${GTK3_CFLAGS_OTHER})

# Source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(SudokuMaster ${SOURCES})

# Link libraries
target_link_libraries(SudokuMaster ${GTK3_LIBRARIES})

# Copy style.css to the build directory so the app can find it
configure_file(${CMAKE_SOURCE_DIR}/style.css ${CMAKE_BINARY_DIR}/style.css COPYONLY)
