cmake_minimum_required(VERSION 3.10)
project(dsa_project)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the Asio and Crow libraries from the paths set during the build
find_path(ASIO_INCLUDE_DIR NAMES asio.hpp PATHS "${CMAKE_PREFIX_PATH}")
find_path(CROW_INCLUDE_DIR NAMES crow.h PATHS "${CMAKE_PREFIX_PATH}")

# Add the executable
add_executable(dsa_project main.cpp src/helper.cpp)

# Include directories
target_include_directories(dsa_project PUBLIC ${ASIO_INCLUDE_DIR} ${CROW_INCLUDE_DIR} include)

# Link libraries if necessary (e.g., for filesystem on some compilers)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(dsa_project PRIVATE stdc++fs)
endif()