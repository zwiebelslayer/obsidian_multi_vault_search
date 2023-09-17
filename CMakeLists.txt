cmake_minimum_required(VERSION 3.22)
project(obsidian_multi_vault_search)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

# add header path
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/misc/cpp
)

# set common source
file(GLOB SRC
        ./lib/imgui/*.h
        ./lib/imgui/*.cpp
        )
file (GLOB PLATFORM_SRC
        ./lib/imgui/backends/imgui_impl_win32.*
        ./lib/imgui/backends/imgui_impl_dx9.*
        ./lib/imgui/imgui_demo.cpp
        ./lib/imgui/imgui_draw.cpp
        ./lib/src/win/main.cpp
        ./lib/imgui/misc/cpp/imgui_stdlib.cpp
        )




add_executable(obsidian_multi_vault_search main.cpp imgui_test.cpp ${SRC} ${PLATFORM_SRC})
target_link_libraries(${PROJECT_NAME}  d3d9.lib)