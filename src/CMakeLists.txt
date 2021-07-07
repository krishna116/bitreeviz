set(LIBRARY_NAME "bitreevizLib")

add_library(${LIBRARY_NAME}
    STATIC
        BiTreeFileParser.cpp
        BiTreeLayouter.cpp
        BiTreeParser.cpp
        BiTreePropertyManager.cpp
        BiTreeRenderer.cpp
        BiTreeViz.cpp
)

# If build in windows with visual studio generator, 
# find_package(PkgConfig REQUIRED) may do not work correctly,
# You can install vcpkg, install the cairomm-1.16 library by vcpkg 
# and then specify these library-paths explicitly.
find_package(PkgConfig REQUIRED)                                    ## if using MSYS2
pkg_check_modules(cairomm REQUIRED cairomm-1.16)                    ## if using MSYS2

target_link_libraries(${LIBRARY_NAME}
    PRIVATE
       ${cairomm_LIBRARIES}                                         ## if using MSYS2
#        D:/temp/vcpkg/installed/x86-windows/lib/cairomm-1.16.lib   ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/lib/sigc-3.0.lib       ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/lib/cairo.lib          ## if using vcpkg
)

target_include_directories(${LIBRARY_NAME}  
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}             # for config.h
    PRIVATE
       ${cairomm_INCLUDE_DIRS}                                          ## if using MSYS2
#        D:/temp/vcpkg/installed/x86-windows/include/cairomm-1.16       ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/lib/cairomm-1.16/include   ## if using vcpkg 
#        D:/temp/vcpkg/installed/x86-windows/include/sigc++-3.0         ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/lib/sigc++-3.0/include     ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/include/cairo              ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/include/lzo                ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/include                    ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/include/libpng16           ## if using vcpkg
#        D:/temp/vcpkg/installed/x86-windows/include/pixman-1           ## if using vcpkg
)

