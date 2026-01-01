cmake_minimum_required(VERSION 3.11)
project(SoftwareRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Dependency: Raylib ---
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/raylib/CMakeLists.txt")
    message(STATUS "Found local raylib in vendor/raylib")
    add_subdirectory(vendor/raylib)
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

# --- Multithreading Support ---
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# --- Source ---
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)


# --- Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)