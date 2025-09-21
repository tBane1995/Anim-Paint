cmake_minimum_required(VERSION 3.10)
project(Anim-Paint VERSION 1.0)

add_executable(${PROJECT_NAME} Anim-Paint/src/main.cpp)

# SOURCES FILES
set(SOURCES
    Anim-Paint/src/Window.cpp
    Anim-Paint/src/Textures.cpp
    Anim-Paint/src/Theme.cpp
    Anim-Paint/src/Mouse.cpp
    Anim-Paint/src/Time.cpp
    Anim-Paint/src/ElementGUI.cpp
    Anim-Paint/src/Button.cpp
    Anim-Paint/src/Checkbox.cpp
    Anim-Paint/src/Slider.cpp
    Anim-Paint/src/Dialog.cpp
    Anim-Paint/src/WorldToTileConverter.cpp
    Anim-Paint/src/Clipboard.cpp
    Anim-Paint/src/Filters.cpp
    Anim-Paint/src/Layer.cpp
    Anim-Paint/src/Frame.cpp
    Anim-Paint/src/Animation.cpp
    Anim-Paint/src/FramesDialog.cpp
    Anim-Paint/src/LayersDialog.cpp
    Anim-Paint/src/dialogs/Dialog_Image_Rotation.cpp
    Anim-Paint/src/dialogs/Dialog_Image_Brightness_Contrast.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

source_group(TREE ${CMAKE_SOURCE_DIR}/Anim-Paint/src
             PREFIX "Source Files"
             FILES ${SOURCES})

# HEADERS FILES
set(HEADERS
    Anim-Paint/include/Window.hpp
    Anim-Paint/include/Textures.hpp
    Anim-Paint/include/Theme.hpp
    Anim-Paint/include/Mouse.hpp
    Anim-Paint/include/Time.hpp
    Anim-Paint/include/ElementGUI.hpp
    Anim-Paint/include/Button.hpp
    Anim-Paint/include/Checkbox.hpp
    Anim-Paint/include/Slider.hpp
    Anim-Paint/include/Dialog.hpp
    Anim-Paint/include/WorldToTileConverter.hpp
    Anim-Paint/include/Clipboard.hpp
    Anim-Paint/include/Filters.hpp
    Anim-Paint/include/Layer.hpp
    Anim-Paint/include/Frame.hpp
    Anim-Paint/include/Animation.hpp
    Anim-Paint/include/FramesDialog.hpp
    Anim-Paint/include/LayersDialog.hpp
    Anim-Paint/include/dialogs/Dialog_Image_Rotation.hpp
    Anim-Paint/include/dialogs/Dialog_Image_Brightness_Contrast.hpp
    Anim-Paint/include/dialogs/Dialog_Image_Saturation.hpp
    Anim-Paint/include/dialogs/Dialog_Image_Sepia.hpp
    Anim-Paint/include/MainMenu.hpp
    Anim-Paint/include/Selection.hpp
    Anim-Paint/include/Lasso.hpp
    Anim-Paint/include/Brush.hpp
    Anim-Paint/include/Tools.hpp
    Anim-Paint/include/Canvas.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${HEADERS})

source_group(TREE ${CMAKE_SOURCE_DIR}/Anim-Paint/include
             PREFIX "Header Files"
             FILES ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Anim-Paint/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# set the SFML lib
set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")

# set the packages SFML
find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# set the working directory (assets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Anim-Paint
)

# set the path for SFML bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;C:\\SFML-2.6.2\\bin"
)