# WallpaperMaker

A Qt6 C++ application for creating custom wallpapers by cropping and scaling images to specific resolutions.

## Features

- **Drag-and-drop image loading** - Simply drag images into the application
- **Comprehensive format support** - Supports PNG, JPG/JPEG, JFIF, HEIF/HEIC, BMP, TIFF/TIF, GIF
- **Interactive image manipulation** - Pan with click-drag, zoom with scroll wheel
- **17 predefined resolutions** - From HD (1280×720) to 8K UHD (7680×4320) and ultrawide formats
- **Static crop overlay** - Crop frame stays fixed while you position the image underneath
- **Image adjustments** - Brightness, contrast, and saturation controls (coming soon, maybe)
- **Multiple output formats** - Save as PNG, JPEG, BMP, or TIFF
- **Quality control** - JPEG quality slider for size optimization
- **Smart file naming** - Automatically adds resolution suffix (e.g., `photo_1920x1080.png`)
- **Persistent settings** - Remembers your preferences and last used directory

## Supported Resolutions

- 1280 × 720 (HD)
- 1366 × 768 (WXGA)
- 1920 × 1080 (Full HD)
- 2560 × 1440 (QHD)
- 3840 × 2160 (4K UHD)
- 4096 × 2160 (Cinema 4K)
- 7680 × 4320 (8K UHD)
- 1920 × 1200 (WUXGA)
- 2560 × 1600 (WQXGA)
- 3840 × 2400 (WQUXGA)
- 2256 × 1504 (3:2 Tablet)
- 2880 × 1920 (3:2 High-res)
- 3000 × 2000 (3:2 Ultra)
- 2560 × 1080 (Ultrawide)
- 3440 × 1440 (Ultrawide QHD)
- 5120 × 2160 (5K Ultrawide)
- 5120 × 1440 (Super Ultrawide)

## Requirements

- Qt6 (Core, Widgets, Gui components)
- CMake 3.16 or later
- C++17 compatible compiler
- Linux/Unix system (tested on Linux, but should work on the BSDs)

## Installation

- use cmake

## Usage

1. **Load an image**:
   - Drag and drop an image file into the application, or
   - Use File → Open Image... (Ctrl+O)

2. **Select target resolution**:
   - Choose from the Resolution dropdown
   - The white crop frame shows the target aspect ratio

3. **Position the image**:
   - **Pan**: Click and drag to move the image
   - **Zoom**: Use mouse wheel to zoom in/out
   - The crop frame stays fixed while the image moves underneath

4. **Adjust the image** (optional):
   - Use Brightness, Contrast, and Saturation sliders
   - Changes are applied in real-time

5. **Choose output format**:
   - Select PNG, JPEG, BMP, or TIFF
   - For JPEG, adjust quality with the slider

6. **Save the wallpaper**:
   - Click Save Wallpaper or use Ctrl+S
   - The file is saved with automatic resolution suffix
   
## License

MIT
