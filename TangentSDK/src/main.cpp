#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Global styles
    app.setStyleSheet(
        // Dock widget title bars - thinner
        "QDockWidget::title {"
        "    padding: 2px;"
        "    background-color: #2d2d2d;"
        "}"
        "QDockWidget {"
        "    titlebar-close-icon: url(none);"
        "    titlebar-normal-icon: url(none);"
        "}"
        // All buttons - hover effect
        "QPushButton:hover {"
        "    background-color: #4a4a4a;"
        "}"
        // Menu bar items - consistent padding for all states
        "QMenuBar::item {"
        "    padding: 4px 8px;"
        "    background: transparent;"
        "}"
        "QMenuBar::item:selected, QMenuBar::item:hover, QMenuBar::item:pressed {"
        "    padding: 4px 8px;"
        "    background-color: #4a4a4a;"
        "}"
        // Menu items
        "QMenu::item {"
        "    padding: 4px 20px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #4a4a4a;"
        "}"
    );

    MainWindow win;
    win.show();

    return app.exec();
}
