cmake_minimum_required(VERSION 3.22)
project(WebServer VERSION 0.0.1 LANGUAGES CXX)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_BINARY_DIR}/build/Release/generators/conan_toolchain.cmake)

# Add these lines to use the new Conan CMake integration
find_package(fmt CONFIG REQUIRED) 

find_package(uwebsockets REQUIRED)

# Find Boost libraries required for Boost.Asio and Boost.Beast
find_package(Boost 1.78 REQUIRED COMPONENTS system thread)

# Find Catch2 for testing
find_package(Catch2 3 REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

# Gather all source files for the main project
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
)

# Add the main executable
add_executable(WebServer ${PROJECT_SOURCES})

# Link Boost libraries to the main executable
target_link_libraries(WebServer PRIVATE Boost::system Boost::thread uwebsockets::uwebsockets)

# Enable compiler features and warnings (optional but recommended)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(WebServer PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
    target_compile_options(WebServer PRIVATE /W4 /WX)
endif()

# Testing Configuration
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)

add_executable(UnitTests ${TEST_SOURCES})

include_directories(uSockets/src)
# Link Catch2 with tests and Boost libraries
target_link_libraries(UnitTests PRIVATE Catch2::Catch2WithMain Boost::system Boost::thread)

# Register tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(UnitTests)

# Installation (Optional)
install(TARGETS WebServer
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

