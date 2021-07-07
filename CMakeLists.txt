cmake_minimum_required(VERSION 3.1.0)

project(krishna VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BITREEVIZ_APP_NAME "bitreeviz" CACHE INTERNAL "")

configure_file(config.h.in config.h)

add_subdirectory(src)
add_subdirectory(app)
# add_subdirectory(test)
