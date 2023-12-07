find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2_image CONFIG REQUIRED COMPONENTS SDL2_image)

add_library(Core "")
add_library(${PROJECT_NAME}::Core ALIAS Core)
target_compile_features(Core PUBLIC cxx_std_20)
target_include_directories(Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_sources(Core
PUBLIC
FILE_SET HEADERS FILES
    sdlpp.h
PRIVATE
    sdlpp.cpp
)
install(TARGETS Core EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
    INCLUDES DESTINATIION ${CMAKE_INSTALL_INCLUDEDIR}
)
target_link_libraries(Core PUBLIC 
    Microsoft.GSL::GSL
    SDL2::SDL2
)

add_library(Image "")
add_library(${PROJECT_NAME}::Image ALIAS Image)
target_include_directories(Image PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_sources(Image
PUBLIC
FILE_SET HEADERS FILES
    sdlpp_image.h
PRIVATE
    sdlpp_image.cpp
)
install(TARGETS Image EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
    INCLUDES DESTINATIION ${CMAKE_INSTALL_INCLUDEDIR}
)
target_compile_features(Image PUBLIC cxx_std_20)
target_link_libraries(Image PUBLIC
    Microsoft.GSL::GSL
    SDL2::SDL2
    SDL2_image::SDL2_image
)
