//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("WallpaperMaker");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("WallpaperMaker");
    app.setOrganizationDomain("wallpapermaker.local");
    
    // Set a modern style if available
    QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("Fusion")) {
        app.setStyle("Fusion");
    }
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}
