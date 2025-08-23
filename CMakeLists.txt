cmake_minimum_required(VERSION 3.10)
project(Anim-Paint VERSION 1.0)

add_executable(${PROJECT_NAME} Anim-Paint/main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
    Anim-Paint/Window.hpp
    Anim-Paint/Textures.hpp
    Anim-Paint/Theme.hpp
    Anim-Paint/Mouse.hpp
    Anim-Paint/Time.hpp
    Anim-Paint/ElementGUI.hpp
    Anim-Paint/Button.hpp
    Anim-Paint/Checkbox.hpp
    Anim-Paint/Dialog.hpp
    
    Anim-Paint/WorldToTileConverter.hpp
    Anim-Paint/Clipboard.hpp
    Anim-Paint/Filters.hpp

    Anim-Paint/Layer.hpp
    Anim-Paint/Frame.hpp
    
    Anim-Paint/MainMenu.hpp
    Anim-Paint/Selection.hpp
    Anim-Paint/Brush.hpp
    Anim-Paint/Tools.hpp
    Anim-Paint/FramesDialog.hpp
    Anim-Paint/LayersDialog.hpp
    Anim-Paint/Canvas.hpp
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