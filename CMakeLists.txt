cmake_minimum_required(VERSION 3.00)

project(EventSimulator CXX)

set(CMAKE_CXX_STANDARD 11)

# Enable optimizations for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Optimization flags for different compilers
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # For GCC or Clang
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")  # Optimization level 3
        # Add other optimization flags if needed
    elseif(MSVC)
        # For MSVC
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")  # Optimization level 2
        # Add other optimization flags if needed
    endif()
endif()

add_library(event_sim SHARED src/simulator.cpp)
target_include_directories(event_sim PUBLIC include)

add_subdirectory(examples)
