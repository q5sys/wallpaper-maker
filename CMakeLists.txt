cmake_minimum_required(VERSION 3.16)
project(WallpaperMaker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Enable Qt's MOC (Meta-Object Compiler)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set up source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ImageViewer.cpp
    src/ImageProcessor.cpp
    src/SettingsDialog.cpp
)

set(HEADERS
    src/MainWindow.h
    src/ImageViewer.h
    src/ImageProcessor.h
    src/SettingsDialog.h
)

# Create the executable
add_executable(WallpaperMaker ${SOURCES} ${HEADERS})

# Link Qt6 libraries
target_link_libraries(WallpaperMaker Qt6::Core Qt6::Widgets Qt6::Gui)


# Platform-specific settings
if(WIN32)
    set_target_properties(WallpaperMaker PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Install target
install(TARGETS WallpaperMaker
    RUNTIME DESTINATION bin
)
