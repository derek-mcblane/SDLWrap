cmake_minimum_required(VERSION 3.25.0)

project(SDLWrap VERSION 0.1.0)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(SDLWrap_WARNING_OPTIONS
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

add_subdirectory(source)

export(PACKAGE SDLWrap)
export(EXPORT SDLWrapTargets
    NAMESPACE SDLWrap::
)
install(EXPORT SDLWrapTargets
    NAMESPACE SDLWrap::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SDLWrap
)
option(SDLWRAP_BUNDLE_SDL2_SHARED_LIBS OFF "bundle SDL2 shared libraries")
if (SDLWRAP_BUNDLE_SDL_SHARED_LIBRARIES)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        cmake_path(RELATIVE_PATH CMAKE_FULL_INSTALL_LIBDIR
            BASE_DIRECTORY ${CMAKE_FULL_INSTALL_BIN_DIR}
            OUTPUT_VARIABLE lib_relative_to_bin
        )
        set(CMAKE_INSTALL_RPATH $ORIGIN/${lib_relative_to_bin})
    elseif(CMAKE_SYTEM_NAME STREQUAL "Darwin")
        set(CMAKE_INSTALL_RPATH @loader_path/${CMAKE_INSTALL_LIBDIR})
    endif()
    install(IMPORTED_RUNTIME_ARTIFACTS
        SDL2::SDL2
        SDL2_image::SDL2_image
    )
endif()

set(SDLWrap_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/SDLWrap)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SDLWrapConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion 
)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SDLWrapConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SDLWrapConfig.cmake
    INSTALL_DESTINATION ${SDLWrap_CMAKEDIR}
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SDLWrapConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SDLWrapConfigVersion.cmake
    DESTINATION ${SDLWrap_CMAKEDIR}
)

include(CPack)
