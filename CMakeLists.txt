cmake_minimum_required(VERSION 3.10)
project(Anim-Paint VERSION 1.0)

add_executable(${PROJECT_NAME} 
    Anim-Paint/src/main.cpp
    Anim-Paint/ico/app.rc
)

# SOURCES FILES
set(SOURCES
    Anim-Paint/src/Window.cpp
    Anim-Paint/src/DebugLog.cpp    
    Anim-Paint/src/Textures.cpp
    Anim-Paint/src/Theme.cpp
    Anim-Paint/src/Cursor.cpp
    Anim-Paint/src/Time.cpp
    Anim-Paint/src/ElementGUI/ElementGUI.cpp
    Anim-Paint/src/ElementGUI/Button.cpp
    Anim-Paint/src/ElementGUI/Checkbox.cpp
    Anim-Paint/src/ElementGUI/Slider.cpp
    Anim-Paint/src/ElementGUI/Scrollbar.cpp
    Anim-Paint/src/ElementGUI/TextInput.cpp
    Anim-Paint/src/ElementGUI/NumberInput.cpp
    Anim-Paint/src/WorldToTileConverter.cpp
    Anim-Paint/src/Clipboard.cpp
    Anim-Paint/src/Filters.cpp
    Anim-Paint/src/Animation/Layer.cpp
    Anim-Paint/src/Animation/Frame.cpp
    Anim-Paint/src/Animation/Animation.cpp
    Anim-Paint/src/Dialogs/Dialog.cpp
    Anim-Paint/src/Dialogs/FileDialog.cpp
    Anim-Paint/src/Dialogs/Dialog_Save_Project.cpp
    Anim-Paint/src/Dialogs/Dialog_Load_Project.cpp
    Anim-Paint/src/Dialogs/Dialog_Import.cpp
    Anim-Paint/src/Dialogs/Dialog_Load_SpriteSheet.cpp
    Anim-Paint/src/Dialogs/Dialog_Export.cpp
    Anim-Paint/src/Dialogs/FramesDialog.cpp
    Anim-Paint/src/Dialogs/LayersDialog.cpp
    Anim-Paint/src/Dialogs/AnimationsDialog.cpp
    Anim-Paint/src/Dialogs/Palette.cpp
    Anim-Paint/src/Dialogs/Dialog_Rotation.cpp
    Anim-Paint/src/Dialogs/Dialog_Brightness_Contrast.cpp
    Anim-Paint/src/Dialogs/Dialog_Saturation.cpp
    Anim-Paint/src/Dialogs/Dialog_Sepia.cpp
    Anim-Paint/src/Dialogs/Dialog_Outline.cpp
    Anim-Paint/src/Dialogs/Dialog_Resize.cpp
    Anim-Paint/src/Dialogs/Dialog_Chessboard.cpp
    Anim-Paint/src/MainMenu.cpp
    Anim-Paint/src/BottomBar.cpp
    Anim-Paint/src/Tools/Selection.cpp
    Anim-Paint/src/Tools/Brush.cpp
    Anim-Paint/src/Tools/Line.cpp
    Anim-Paint/src/Tools/Toolbar.cpp
    Anim-Paint/src/Canvas.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

source_group(TREE ${CMAKE_SOURCE_DIR}/Anim-Paint/src
             PREFIX "Source Files"
             FILES ${SOURCES})

# HEADERS FILES
set(HEADERS
    Anim-Paint/include/Window.hpp
    Anim-Paint/include/DebugLog.hpp
    Anim-Paint/include/Textures.hpp
    Anim-Paint/include/Theme.hpp
    Anim-Paint/include/Cursor.hpp
    Anim-Paint/include/Time.hpp
    Anim-Paint/include/ElementGUI/ElementGUI.hpp
    Anim-Paint/include/ElementGUI/Button.hpp
    Anim-Paint/include/ElementGUI/Checkbox.hpp
    Anim-Paint/include/ElementGUI/Slider.hpp
    Anim-Paint/include/ElementGUI/Scrollbar.hpp
    Anim-Paint/include/ElementGUI/TextInput.hpp
    Anim-Paint/include/ElementGUI/NumberInput.hpp
    Anim-Paint/include/WorldToTileConverter.hpp
    Anim-Paint/include/Clipboard.hpp
    Anim-Paint/include/Filters.hpp
    Anim-Paint/include/Animation/Layer.hpp
    Anim-Paint/include/Animation/Frame.hpp
    Anim-Paint/include/Animation/Animation.hpp
    Anim-Paint/include/Dialogs/Dialog.hpp
    Anim-Paint/include/Dialogs/FileDialog.hpp
    Anim-Paint/include/Dialogs/Dialog_Save_Project.hpp
    Anim-Paint/include/Dialogs/Dialog_Load_Project.hpp
    Anim-Paint/include/Dialogs/Dialog_Import.hpp
    Anim-Paint/include/Dialogs/Dialog_Load_SpriteSheet.hpp
    Anim-Paint/include/Dialogs/Dialog_Export.hpp
    Anim-Paint/include/Dialogs/FramesDialog.hpp
    Anim-Paint/include/Dialogs/LayersDialog.hpp
    Anim-Paint/include/Dialogs/AnimationsDialog.hpp
    Anim-Paint/include/Dialogs/Palette.hpp
    Anim-Paint/include/Dialogs/Dialog_Rotation.hpp
    Anim-Paint/include/Dialogs/Dialog_Brightness_Contrast.hpp
    Anim-Paint/include/Dialogs/Dialog_Saturation.hpp
    Anim-Paint/include/Dialogs/Dialog_Sepia.hpp
    Anim-Paint/include/Dialogs/Dialog_Outline.hpp
    Anim-Paint/include/Dialogs/Dialog_Resize.hpp
    Anim-Paint/include/Dialogs/Dialog_Chessboard.hpp
    Anim-Paint/include/MainMenu.hpp
    Anim-Paint/include/BottomBar.hpp
    Anim-Paint/include/Tools/Selection.hpp
    Anim-Paint/include/Tools/Brush.hpp
    Anim-Paint/include/Tools/Line.hpp
    Anim-Paint/include/Tools/Toolbar.hpp
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
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# set the SFML lib
set(SFML_DIR "C:/SFML-3.0.2/lib/cmake/SFML")

# set the packages SFML
find_package(SFML 3.0.2 COMPONENTS Graphics Window System Audio REQUIRED)
target_link_libraries(${PROJECT_NAME} 
	SFML::Graphics
        SFML::Window
        SFML::System
	SFML::Audio
)

# set the working directory (assets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Anim-Paint"
    VS_DEBUGGER_WORKING_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Anim-Paint"
)

get_target_property(WD_DEBUG   ${PROJECT_NAME} VS_DEBUGGER_WORKING_DIRECTORY_DEBUG)
get_target_property(WD_RELEASE ${PROJECT_NAME} VS_DEBUGGER_WORKING_DIRECTORY_RELEASE)

message(STATUS "WD DEBUG   = ${WD_DEBUG}")
message(STATUS "WD RELEASE = ${WD_RELEASE}")

# set the path for SFML bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;C:\\SFML-3.0.2\\bin"
)