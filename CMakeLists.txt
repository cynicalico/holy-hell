cmake_minimum_required(VERSION 3.20)
project(holy-hell LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.0.0
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.2.28
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
        GIT_TAG release-3.2.4
)
FetchContent_MakeAvailable(SDL3_image)

add_library(libholyhell)
target_compile_features(libholyhell PRIVATE cxx_std_23)
target_sources(libholyhell
        PRIVATE
        "src/board.cpp"
        "src/engine.cpp"

        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
        "include/board.hpp"
        "include/engine.hpp"
        "include/piece.hpp"
)
target_link_libraries(libholyhell PUBLIC fmt::fmt SDL3::SDL3 SDL3_image::SDL3_image)

add_executable(holy-hell main.cpp)
target_compile_features(holy-hell PRIVATE cxx_std_23)
target_link_libraries(holy-hell PRIVATE libholyhell)
