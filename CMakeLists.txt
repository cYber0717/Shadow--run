cmake_minimum_required(VERSION 3.20)
project(ShadowRun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics REQUIRED)

add_executable(ShadowRun src/main.cpp)
target_link_libraries(ShadowRun PRIVATE SFML::Graphics)

# copy the assets folder next to the exe after every build
add_custom_command(TARGET ShadowRun POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ShadowRun>/assets)