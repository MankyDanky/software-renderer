cmake_minimum_required(VERSION 3.11)
project(SoftwareRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output binaries to a "bin" folder inside build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Dependency: Raylib ---
# 1. Check if user has placed raylib in vendor/raylib
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/raylib/CMakeLists.txt")
    message(STATUS "Found local raylib in vendor/raylib")
    add_subdirectory(vendor/raylib)
# 2. If not, download it automatically
else()
    message(STATUS "Raylib not found in vendor/. Downloading via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(raylib)
endif()

# --- Source ---
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# --- Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)