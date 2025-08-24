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

# Install .desktop file for Linux/FreeBSD desktop environments
if(UNIX AND NOT APPLE)
    install(FILES WallpaperMaker.desktop
        DESTINATION share/applications
    )
    
    # Install icon if it exists (optional)
    if(EXISTS "${CMAKE_SOURCE_DIR}/icons/wallpapermaker.png")
        install(FILES icons/wallpapermaker.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME wallpapermaker.png
        )
    endif()
    
    # Install smaller icon sizes if they exist
    foreach(size 16 22 24 32 48 64 128)
        if(EXISTS "${CMAKE_SOURCE_DIR}/icons/wallpapermaker-${size}.png")
            install(FILES icons/wallpapermaker-${size}.png
                DESTINATION share/icons/hicolor/${size}x${size}/apps
                RENAME wallpapermaker.png
            )
        endif()
    endforeach()
endif()
