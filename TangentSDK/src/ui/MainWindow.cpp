#include "MainWindow.h"
#include "TabEditorWidget.h"
#include "EditorWidget.h"
#include "ConsoleWidget.h"
#include "ProjectExplorer.h"
#include "TitleBarWidget.h"
#include "SettingsDialog.h"
#include "RomBuilderDialog.h"
#include "../app/DiscordRPCManager.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDirIterator>
#include <QCoreApplication>

MainWindow::MainWindow() {
    setWindowTitle("TangentSDK");
    resize(1400, 900);

    tabEditor = new TabEditorWidget(this);
    setCentralWidget(tabEditor);

    m_workspacePath = getWorkspacePath();
    
    // Initialize Discord RPC
    m_discordRPC = new DiscordRPCManager(this);
    QSettings settings("TangentSDK", "TangentSDK");
    if (settings.value("discord/enabled", false).toBool()) {
        m_discordRPC->initialize();
    }
    
    setupDocking();
    setupMenus();

    // Set the workspace path for project explorer
    projectExplorer->setRootPath(m_workspacePath);
    
    // Give project explorer access to tab editor for unsaved file checks
    projectExplorer->setTabEditor(tabEditor);

    // Connect project explorer file clicks to open files
    connect(projectExplorer, &ProjectExplorer::fileClicked, 
            this, &MainWindow::onFileClicked);

    // Connect tab changes to update window title and Discord RPC
    connect(tabEditor, &TabEditorWidget::currentFileChanged,
            this, &MainWindow::updateWindowTitle);
    connect(tabEditor, &TabEditorWidget::currentFileChanged,
            this, &MainWindow::onCurrentFileChanged);
    connect(tabEditor, &TabEditorWidget::currentEditorModificationChanged,
            this, [this](bool) { updateWindowTitle(); });

    updateWindowTitle();

    // Apply saved colors
    applyColors();

    // Bring window to front and maximize
    showMaximized();
    raise();
    activateWindow();
}

QString MainWindow::getWorkspacePath(bool forceDialog) {
    QSettings settings("TangentSDK", "TangentSDK");
    QString savedPath = settings.value("workspacePath").toString();
    bool dontShowAgain = settings.value("dontShowWorkspaceDialog", false).toBool();
    
    if (!forceDialog && dontShowAgain && !savedPath.isEmpty() && QDir(savedPath).exists()) {
        return savedPath;
    }

    // Default workspace path
    QString defaultPath = savedPath.isEmpty() 
        ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/TangentSDK/workspace"
        : savedPath;

    // Create dialog without parent so it appears on top
    QDialog dialog(nullptr, Qt::WindowStaysOnTopHint);
    dialog.setWindowTitle("Select Workspace Folder");
    dialog.setMinimumWidth(500);
    dialog.setWindowModality(Qt::ApplicationModal);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* label = new QLabel("Select the workspace folder for your projects:");
    layout->addWidget(label);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    QLineEdit* pathEdit = new QLineEdit(defaultPath);
    QPushButton* browseBtn = new QPushButton("Browse...");
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseBtn);
    layout->addLayout(pathLayout);

    QCheckBox* dontShowCheck = new QCheckBox("Don't show this again");
    dontShowCheck->setChecked(dontShowAgain);
    layout->addWidget(dontShowCheck);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(browseBtn, &QPushButton::clicked, &dialog, [&]() {
        QString dir = QFileDialog::getExistingDirectory(&dialog, "Select Workspace Folder", 
                                                         pathEdit->text());
        if (!dir.isEmpty()) {
            pathEdit->setText(dir);
        }
    });

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Show and bring to front
    dialog.show();
    dialog.raise();
    dialog.activateWindow();

    QString workspacePath = defaultPath;
    if (dialog.exec() == QDialog::Accepted) {
        workspacePath = pathEdit->text();
        settings.setValue("dontShowWorkspaceDialog", dontShowCheck->isChecked());
    }

    // Create folder if it doesn't exist
    QDir dir(workspacePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Save the path
    settings.setValue("workspacePath", workspacePath);

    return workspacePath;
}

bool MainWindow::promptSaveAll() {
    return tabEditor->promptSaveAllChanges();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // Prompt to save unsaved changes
    if (!promptSaveAll()) {
        event->ignore();
        return;
    }
    
    // Shutdown Discord RPC
    if (m_discordRPC) {
        m_discordRPC->shutdown();
    }
    
    saveWindowState();
    event->accept();
}

void MainWindow::setupDocking() {
    // Set tabbed dock widgets to show tabs at top
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    
    // Style the dock tabs
    setStyleSheet(styleSheet() +
        "QTabBar::tab {"
        "    background-color: #2d2d2d;"
        "    color: #cccccc;"
        "    padding: 6px 16px;"
        "    margin-right: 2px;"
        "    border-top-left-radius: 4px;"
        "    border-top-right-radius: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #3a3a3a;"
        "}"
        "QTabBar::tab:hover {"
        "    background-color: #454545;"
        "}"
    );
    
    // Project Explorer dock
    projectDock = new QDockWidget(this);
    projectDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    projectDock->setWindowTitle("Project Explorer");  // For tabbed dock tab name
    projectDock->setTitleBarWidget(new TitleBarWidget("Project Explorer", projectDock));
    projectDock->setObjectName("ProjectExplorer");
    projectExplorer = new ProjectExplorer();
    projectDock->setWidget(projectExplorer);
    addDockWidget(Qt::LeftDockWidgetArea, projectDock);

    // Console dock
    consoleDock = new QDockWidget(this);
    consoleDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    consoleDock->setWindowTitle("Console");  // For tabbed dock tab name
    consoleDock->setTitleBarWidget(new TitleBarWidget("Console", consoleDock));
    consoleDock->setObjectName("Console");
    console = new ConsoleWidget();
    consoleDock->setWidget(console);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);
    
    // Restore saved window state (dock positions/visibility)
    restoreWindowState();
}

void MainWindow::saveWindowState() {
    QSettings settings("TangentSDK", "TangentSDK");
    settings.setValue("windowGeometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::restoreWindowState() {
    QSettings settings("TangentSDK", "TangentSDK");
    if (settings.contains("windowGeometry")) {
        restoreGeometry(settings.value("windowGeometry").toByteArray());
    }
    if (settings.contains("windowState")) {
        restoreState(settings.value("windowState").toByteArray());
    }
}

void MainWindow::setupMenus() {
    QMenu* file = menuBar()->addMenu("File");
    
    QAction* newFileAction = file->addAction("New File");
    newFileAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(newFileAction, &QAction::triggered, this, &MainWindow::onCreateFile);

    QAction* newFolderAction = file->addAction("New Folder");
    newFolderAction->setShortcut(QKeySequence("Ctrl+Shift+N"));
    connect(newFolderAction, &QAction::triggered, this, &MainWindow::onCreateFolder);

    file->addSeparator();

    QAction* openAction = file->addAction("Open File");
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openAction, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File");
        if (!filePath.isEmpty()) {
            tabEditor->openFile(filePath);
        }
    });

    QAction* openWorkspaceAction = file->addAction("Open Workspace...");
    connect(openWorkspaceAction, &QAction::triggered, this, &MainWindow::onOpenWorkspace);

    file->addSeparator();

    QAction* saveAction = file->addAction("Save");
    saveAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);

    QAction* saveAllAction = file->addAction("Save All");
    saveAllAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(saveAllAction, &QAction::triggered, this, &MainWindow::onSaveAllFiles);

    file->addSeparator();
    
    QAction* settingsAction = file->addAction("Settings...");
    settingsAction->setShortcut(QKeySequence("Ctrl+,"));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onOpenSettings);

    // Project menu
    QMenu* project = menuBar()->addMenu("Project");
    
    QAction* newProjectAction = project->addAction("New Project...");
    connect(newProjectAction, &QAction::triggered, this, &MainWindow::onNewProject);
    
    QAction* buildProjectAction = project->addAction("Build Project");
    buildProjectAction->setShortcut(QKeySequence("F5"));
    connect(buildProjectAction, &QAction::triggered, this, &MainWindow::onBuildProject);
    
    QAction* buildRomAction = project->addAction("Build ROM...");
    buildRomAction->setShortcut(QKeySequence("F7"));
    connect(buildRomAction, &QAction::triggered, this, &MainWindow::onBuildRom);

    // Run/Debug menu
    QMenu* runDebug = menuBar()->addMenu("Run/Debug");
    
    QAction* runOnEmulatorAction = runDebug->addAction("Run on Emulator");
    runOnEmulatorAction->setShortcut(QKeySequence("F6"));
    connect(runOnEmulatorAction, &QAction::triggered, this, &MainWindow::onRunOnEmulator);
    
    runDebug->addSeparator();
    
    QAction* loadOntoPicoEaseAction = runDebug->addAction("Load onto PicoEase");
    connect(loadOntoPicoEaseAction, &QAction::triggered, this, &MainWindow::onLoadOntoPicoEase);
    
    QAction* downloadFromPicoEaseAction = runDebug->addAction("Download ROM from PicoEase");
    connect(downloadFromPicoEaseAction, &QAction::triggered, this, &MainWindow::onDownloadFromPicoEase);
    
    QAction* startRP2EaseAction = runDebug->addAction("Start RP2Ease");
    connect(startRP2EaseAction, &QAction::triggered, this, &MainWindow::onStartRP2Ease);

    QMenu* view = menuBar()->addMenu("View");
    
    // Use toggleViewAction() which properly handles minimize/restore
    QAction* toggleProjectExplorer = projectDock->toggleViewAction();
    toggleProjectExplorer->setText("Project Explorer");
    view->addAction(toggleProjectExplorer);

    QAction* toggleConsole = consoleDock->toggleViewAction();
    toggleConsole->setText("Console");
    view->addAction(toggleConsole);
}

void MainWindow::onFileClicked(const QString& filePath) {
    // Determine the project path from the file path
    QString projectPath = projectExplorer->getCurrentProjectPath();
    if (!projectPath.isEmpty()) {
        tabEditor->setProjectPath(projectPath);
    }
    tabEditor->openFile(filePath);
}

void MainWindow::onSaveFile() {
    tabEditor->saveCurrentFile();
}

void MainWindow::onSaveAllFiles() {
    tabEditor->saveAllFiles();
}

void MainWindow::onOpenWorkspace() {
    if (!promptSaveAll()) {
        return;
    }

    // Close all tabs
    tabEditor->closeAllTabs();

    // Get new workspace path
    m_workspacePath = getWorkspacePath(true);
    projectExplorer->setRootPath(m_workspacePath);
    updateWindowTitle();
}

void MainWindow::onCreateFile() {
    projectExplorer->createNewFileInRoot();
}

void MainWindow::onCreateFolder() {
    projectExplorer->createNewFolderInRoot();
}

void MainWindow::onNewProject() {
    bool ok;
    QString projectName = QInputDialog::getText(this, "New Project",
                                                  "Project name:", QLineEdit::Normal,
                                                  "MyProject", &ok);
    if (!ok || projectName.isEmpty()) {
        return;
    }

    // Create project folder
    QString projectPath = m_workspacePath + "/" + projectName;
    QDir dir;
    if (dir.exists(projectPath)) {
        QMessageBox::warning(this, "Error", "A project with this name already exists.");
        return;
    }
    
    dir.mkpath(projectPath);

    // Create main.tml
    QFile tmlFile(projectPath + "/main.tml");
    if (tmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tmlFile);
        out << "; Tangent Markup Language main file\n";
        out << "; Project: " << projectName << "\n";
        tmlFile.close();
    }

    // Create main.tasm
    QFile tasmFile(projectPath + "/main.tasm");
    if (tasmFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tasmFile);
        out << "; Tangent Assembly main file\n";
        out << "; Project: " << projectName << "\n";
        tasmFile.close();
    }

    // Expand to show the new project
    projectExplorer->expandToPath(projectPath);
}

void MainWindow::onBuildProject() {
    // Get the currently selected project folder from the project explorer
    QString projectPath = projectExplorer->getCurrentProjectPath();
    
    if (projectPath.isEmpty()) {
        QMessageBox::warning(this, "Build Error", "No project selected. Please select a project folder in the Project Explorer.");
        return;
    }
    
    // Save all files first
    tabEditor->saveAllFiles();
    
    // Get buildtools path (relative to executable)
    QString appDir = QCoreApplication::applicationDirPath();
    QString buildToolsPath = appDir + "/../buildtools";
    
    QString tasExe = buildToolsPath + "/tas-t16.exe";
    QString tmlcExe = buildToolsPath + "/tmlc-t16.exe";
    QString tlExe = buildToolsPath + "/tl-t16.exe";
    
    // Get project name for display
    QString projectName = QDir(projectPath).dirName();
    
    // Clear console and start build output
    console->clear();
    console->appendSuccess("=== Building Project: " + projectName + " ===\n\n");
    
    // Delete and recreate build folder
    QString buildPath = projectPath + "/build";
    QDir buildDir(buildPath);
    if (buildDir.exists()) {
        console->appendOutput("Cleaning build folder...\n");
        buildDir.removeRecursively();
    }
    buildDir.mkpath(".");
    
    // Collect all .tasm and .tml files
    QStringList tasmFiles;
    QStringList tmlFiles;
    
    QDirIterator it(projectPath, QStringList() << "*.tasm" << "*.tml", 
                    QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        // Skip files in build folder
        if (filePath.contains("/build/")) continue;
        
        if (filePath.endsWith(".tasm")) {
            tasmFiles << filePath;
        } else if (filePath.endsWith(".tml")) {
            tmlFiles << filePath;
        }
    }
    
    bool success = true;
    QStringList toFiles;
    
    // Helper to make paths relative to project
    auto relativePath = [&projectPath](const QString& fullPath) -> QString {
        if (fullPath.startsWith(projectPath)) {
            return fullPath.mid(projectPath.length() + 1);
        }
        return QFileInfo(fullPath).fileName();
    };
    
    // Process .tasm files with tas-t16.exe
    for (const QString& tasmFile : tasmFiles) {
        QFileInfo fi(tasmFile);
        QString outputFile = buildPath + "/" + fi.baseName() + ".tl";
        
        QStringList args;
        args << tasmFile << "-o" << outputFile;
        
        // Display with relative paths
        console->appendOutput("$ tas-t16.exe " + relativePath(tasmFile) + 
                            " -o build/" + fi.baseName() + ".tl\n");
        
        QProcess process;
        process.start(tasExe, args);
        process.waitForFinished();
        
        QString output = process.readAllStandardOutput();
        QString errors = process.readAllStandardError();
        
        if (!output.isEmpty()) console->appendOutput(output);
        if (!errors.isEmpty()) {
            console->appendError(errors);
        }
        
        if (process.exitCode() != 0) {
            console->appendError("  -> FAILED (exit code " + QString::number(process.exitCode()) + ")\n\n");
            console->appendError("=== Build Failed ===\n");
            success = false;
            return;  // Stop on first error
        } else {
            console->appendSuccess("  -> build/" + fi.baseName() + ".tl\n\n");
        }
    }
    
    // Process .tml files with tmlc-t16.exe
    for (const QString& tmlFile : tmlFiles) {
        QFileInfo fi(tmlFile);
        QString outputFile = buildPath + "/" + fi.baseName() + ".to";
        
        QStringList args;
        args << tmlFile << "-o" << outputFile;
        
        // Display with relative paths
        console->appendOutput("$ tmlc-t16.exe " + relativePath(tmlFile) + 
                            " -o build/" + fi.baseName() + ".to\n");
        
        QProcess process;
        process.start(tmlcExe, args);
        process.waitForFinished();
        
        QString output = process.readAllStandardOutput();
        QString errors = process.readAllStandardError();
        
        if (!output.isEmpty()) console->appendOutput(output);
        if (!errors.isEmpty()) {
            console->appendError(errors);
        }
        
        if (process.exitCode() != 0) {
            console->appendError("  -> FAILED (exit code " + QString::number(process.exitCode()) + ")\n\n");
            console->appendError("=== Build Failed ===\n");
            success = false;
            return;  // Stop on first error
        } else {
            console->appendSuccess("  -> build/" + fi.baseName() + ".to\n\n");
            toFiles << outputFile;
        }
    }
    
    // Link all .to files with tl-t16.exe
    if (success && !toFiles.isEmpty()) {
        // Build relative path list for display
        QStringList relativeToFiles;
        for (const QString& f : toFiles) {
            relativeToFiles << "build/" + QFileInfo(f).fileName();
        }
        
        // Output is now a .tp (Tangent Program) file
        QString outputFile = projectName + ".tp";
        
        QStringList args = toFiles;
        args << "-o" << outputFile;
        
        console->appendOutput("$ tl-t16.exe " + relativeToFiles.join(" ") + " -o " + outputFile + "\n");
        
        QProcess process;
        process.setWorkingDirectory(buildPath);
        process.start(tlExe, args);
        process.waitForFinished();
        
        QString output = process.readAllStandardOutput();
        QString errors = process.readAllStandardError();
        
        if (!output.isEmpty()) console->appendOutput(output);
        if (!errors.isEmpty()) {
            console->appendError(errors);
        }
        
        if (process.exitCode() != 0) {
            console->appendError("  -> FAILED (exit code " + QString::number(process.exitCode()) + ")\n\n");
            console->appendError("=== Build Failed ===\n");
            success = false;
        } else {
            console->appendSuccess("  -> build/" + outputFile + "\n\n");
        }
    }
    
    // Final status
    if (success) {
        console->appendSuccess("=== Build Successful ===\n");
    }
    
    // Refresh project explorer to show build folder
    projectExplorer->refresh();
}

void MainWindow::onBuildRom() {
    // Save all files first
    tabEditor->saveAllFiles();
    
    // Open the ROM builder dialog
    RomBuilderDialog dialog(m_workspacePath, this);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    // Get the selected programs and settings
    QStringList selectedPrograms = dialog.getSelectedPrograms();
    QString romPath = dialog.getSelectedRomPath();
    QString injectAddress = dialog.getInjectAddress();
    QString outputName = dialog.getOutputName();
    
    if (selectedPrograms.isEmpty()) {
        QMessageBox::warning(this, "Build ROM Error", "No programs selected.");
        return;
    }
    
    // Get buildtools path
    QString appDir = QCoreApplication::applicationDirPath();
    QString buildToolsPath = appDir + "/../buildtools";
    QString combinerExe = buildToolsPath + "/combiner-t16.exe";
    
    // Get password and clockspeed from settings
    QString password = getPasswordHexString();
    QString clockspeed = getClockspeedHexString();
    
    // Clear console and start build output
    console->clear();
    console->appendSuccess("=== Building ROM: " + outputName + " ===\n\n");
    
    // Build the combiner arguments
    QStringList args = selectedPrograms;
    args << "-bh";
    args << "-o" << outputName;
    args << "-p" << password;
    args << "-c" << clockspeed;
    args << "-i" << injectAddress;
    
    // Display command
    QStringList displayArgs;
    for (const QString& prog : selectedPrograms) {
        displayArgs << QFileInfo(prog).fileName();
    }
    console->appendOutput("$ combiner-t16.exe " + displayArgs.join(" ") + 
                         " -bh -o " + outputName + 
                         " -p " + password + 
                         " -c " + clockspeed + 
                         " -i " + injectAddress + "\n");
    
    // Run the combiner
    QProcess process;
    process.start(combinerExe, args);
    process.waitForFinished();
    
    QString output = process.readAllStandardOutput();
    QString errors = process.readAllStandardError();
    
    if (!output.isEmpty()) console->appendOutput(output);
    if (!errors.isEmpty()) {
        console->appendError(errors);
    }
    
    if (process.exitCode() != 0) {
        console->appendError("  -> FAILED (exit code " + QString::number(process.exitCode()) + ")\n\n");
        console->appendError("=== ROM Build Failed ===\n");
    } else {
        console->appendSuccess("  -> " + outputName + ".bin\n");
        console->appendSuccess("  -> " + outputName + ".hex\n\n");
        console->appendSuccess("=== ROM Build Successful ===\n");
    }
    
    // Refresh project explorer
    projectExplorer->refresh();
}

void MainWindow::updateWindowTitle() {
    EditorWidget* editor = tabEditor->currentEditor();
    QString title = "TangentSDK";
    
    if (editor) {
        QString fileName = editor->fileName();
        if (editor->isModified()) {
            fileName = "* " + fileName;
        }
        title = fileName + " - TangentSDK";
    }
    
    setWindowTitle(title);
}

void MainWindow::onOpenSettings() {
    SettingsDialog dialog(this);
    
    connect(&dialog, &SettingsDialog::workspacePathChanged, this, [this](const QString& newPath) {
        if (!promptSaveAll()) {
            return;
        }
        tabEditor->closeAllTabs();
        m_workspacePath = newPath;
        projectExplorer->setRootPath(m_workspacePath);
        updateWindowTitle();
    });
    
    connect(&dialog, &SettingsDialog::colorsChanged, this, &MainWindow::onColorsChanged);
    connect(&dialog, &SettingsDialog::discordSettingsChanged, this, &MainWindow::onDiscordSettingsChanged);
    
    dialog.exec();
}

void MainWindow::onCurrentFileChanged(const QString& filePath) {
    if (!m_discordRPC) return;
    
    QString projectPath = projectExplorer->getCurrentProjectPath();
    QString projectName;
    if (!projectPath.isEmpty()) {
        projectName = QDir(projectPath).dirName();
    }
    
    if (!filePath.isEmpty()) {
        QString fileName = QFileInfo(filePath).fileName();
        m_discordRPC->onFileChanged(fileName);
        m_discordRPC->onProjectChanged(projectName);
    } else {
        m_discordRPC->updatePresence("", projectName);
    }
}

void MainWindow::onColorsChanged() {
    applyColors();
    // Reload syntax highlighting colors on all open editors
    tabEditor->reloadAllSyntaxColors();
    // Reload console colors
    console->reloadFromTheme();
}

void MainWindow::onDiscordSettingsChanged() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    bool enabled = settings.value("discord/enabled", false).toBool();
    m_discordRPC->setEnabled(enabled);
    
    if (enabled) {
        m_discordRPC->setCustomTextEnabled(settings.value("discord/customText", false).toBool());
        m_discordRPC->setDetailsFormat(settings.value("discord/details", "Editing ${file}").toString());
        m_discordRPC->setStateFormat(settings.value("discord/state", "Working on ${project}").toString());
        m_discordRPC->setElapsedResetMode(settings.value("discord/elapsedReset", 0).toInt());
        
        // Update presence with current file
        EditorWidget* editor = tabEditor->currentEditor();
        if (editor) {
            QString projectPath = projectExplorer->getCurrentProjectPath();
            QString projectName = projectPath.isEmpty() ? "" : QDir(projectPath).dirName();
            m_discordRPC->updatePresence(editor->fileName(), projectName);
        }
    }
}

void MainWindow::applyColors() {
    QSettings settings("TangentSDK", "TangentSDK");
    settings.beginGroup("colors");
    
    // Get colors with defaults
    auto getColor = [&settings](const QString& key, const QString& defaultColor) -> QString {
        return settings.value(key, defaultColor).toString();
    };
    
    // Build dynamic stylesheet
    QString styleSheet = QString(
        // Main window background
        "QMainWindow { background-color: %1; }"
        
        // Menu bar
        "QMenuBar { background-color: %2; color: %3; }"
        "QMenuBar::item:selected { background-color: %4; }"
        "QMenu { background-color: %2; color: %3; }"
        "QMenu::item:selected { background-color: %4; }"
        
        // Dock widgets
        "QDockWidget { background-color: %5; color: %6; }"
        "QDockWidget::title { background-color: %5; color: %6; padding: 4px; }"
        
        // Tree view (Project Explorer)
        "QTreeView { background-color: %7; color: %8; }"
        "QTreeView::item:hover { background-color: %9; }"
        "QTreeView::item:selected { background-color: %10; }"
        
        // Scrollbars
        "QScrollBar:vertical { background-color: %11; width: 12px; }"
        "QScrollBar::handle:vertical { background-color: %12; min-height: 20px; border-radius: 4px; margin: 2px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar:horizontal { background-color: %11; height: 12px; }"
        "QScrollBar::handle:horizontal { background-color: %12; min-width: 20px; border-radius: 4px; margin: 2px; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
    ).arg(
        getColor("ui.dockBackground", "#252526"),
        getColor("ui.menuBackground", "#2d2d2d"),
        getColor("ui.menuForeground", "#cccccc"),
        getColor("ui.menuHover", "#094771"),
        getColor("ui.dockBackground", "#252526"),
        getColor("ui.dockTitle", "#cccccc"),
        getColor("ui.treeBackground", "#252526"),
        getColor("ui.treeItem", "#cccccc"),
        getColor("ui.treeItemHover", "#2a2d2e"),
        getColor("ui.treeItemSelected", "#094771"),
        getColor("ui.scrollbarBackground", "#1e1e1e"),
        getColor("ui.scrollbarThumb", "#424242")
    );
    
    settings.endGroup();
    
    setStyleSheet(styleSheet);
}

QString MainWindow::getPasswordHexString() const {
    QSettings settings("TangentSDK", "TangentSDK");
    
    int passwordPreset = settings.value("runDebug/passwordPreset", 0).toInt();
    
    if (passwordPreset == 3) {
        // Custom password - pad to 64 hex chars (32 bytes)
        QString password = settings.value("runDebug/customPasswordHex").toString().toUpper();
        while (password.length() < 64) {
            password += "0";
        }
        return password;
    } else if (passwordPreset == 1) {
        // 00 password
        return QString("00").repeated(32); // 32 bytes = 64 hex chars
    } else if (passwordPreset == 2) {
        // Casio password: 37 31 43 41 53 47 61 59 71 67 79 73 FF FF FF FF 07 23 13 11 19 17 29 23 FF FF FF FF FF FF FF FF
        return "373143415347615971677973FFFFFFFF0723131119172923FFFFFFFFFFFFFFFF";
    } else {
        // FF password (default)
        return QString("FF").repeated(32); // 32 bytes = 64 hex chars
    }
}

QString MainWindow::getClockspeedHexString() const {
    QSettings settings("TangentSDK", "TangentSDK");
    
    int clockPreset = settings.value("runDebug/clockPreset", 0).toInt();
    
    if (clockPreset == 2) {
        // Custom clockspeed - pad to 32 hex chars (16 bytes)
        QString clock = settings.value("runDebug/customClockHex").toString().toUpper();
        while (clock.length() < 32) {
            clock += "0";
        }
        return clock;
    } else if (clockPreset == 1) {
        // Overclock: 75 02 1E 0C 44 00 32 37 3B 21 10 00 7C 00 00 00
        return "75021E0C440032373B2110007C000000";
    } else {
        // Default (2MHz): EE FE 66 50 CE 75 25 F7 1F FF 64 58 7A 12 16 82
        return "EEFE6650CE7525F71FFF64587A121682";
    }
}

void MainWindow::onRunOnEmulator() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    QString emulatorPath = settings.value("runDebug/emulatorPath").toString();
    QString modelFolder = settings.value("runDebug/modelFolder").toString();
    
    if (emulatorPath.isEmpty()) {
        QMessageBox::warning(this, "Emulator Path Not Set",
            "Please set the path to CasioEmuMSVC.exe in Settings > Run/Debug.");
        onOpenSettings();
        return;
    }
    
    if (modelFolder.isEmpty()) {
        QMessageBox::warning(this, "Model Folder Not Set",
            "Please set the path to the model folder in Settings > Run/Debug.");
        onOpenSettings();
        return;
    }
    
    // Check if emulator exists
    if (!QFile::exists(emulatorPath)) {
        QMessageBox::warning(this, "Emulator Not Found",
            "The emulator executable was not found at:\n" + emulatorPath);
        return;
    }
    
    // Check if model folder exists
    if (!QDir(modelFolder).exists()) {
        QMessageBox::warning(this, "Model Folder Not Found",
            "The model folder was not found at:\n" + modelFolder);
        return;
    }
    
    // Run the emulator with model folder as argument
    QProcess* process = new QProcess(this);
    QStringList args;
    args << modelFolder;
    
    console->appendOutput("$ " + emulatorPath + " " + modelFolder + "\n");
    
    process->start(emulatorPath, args);
    
    if (!process->waitForStarted(3000)) {
        console->appendError("Failed to start emulator.\n");
        delete process;
    } else {
        console->appendSuccess("Emulator started.\n");
    }
}

void MainWindow::onLoadOntoPicoEase() {
    // TODO: Implement PicoEase loading
    console->appendOutput("Load onto PicoEase - Not yet implemented.\n");
}

void MainWindow::onDownloadFromPicoEase() {
    // TODO: Implement PicoEase download
    console->appendOutput("Download ROM from PicoEase - Not yet implemented.\n");
}

void MainWindow::onStartRP2Ease() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    QString rp2easePath = settings.value("runDebug/rp2easePath").toString();
    
    if (rp2easePath.isEmpty()) {
        QMessageBox::warning(this, "RP2Ease Path Not Set",
            "Please set the path to rp2ease.exe in Settings > Run/Debug.");
        onOpenSettings();
        return;
    }
    
    // Check if RP2Ease exists
    if (!QFile::exists(rp2easePath)) {
        QMessageBox::warning(this, "RP2Ease Not Found",
            "The RP2Ease executable was not found at:\n" + rp2easePath);
        return;
    }
    
    // Run RP2Ease
    QProcess* process = new QProcess(this);
    
    console->appendOutput("$ " + rp2easePath + "\n");
    
    process->start(rp2easePath);
    
    if (!process->waitForStarted(3000)) {
        console->appendError("Failed to start RP2Ease.\n");
        delete process;
    } else {
        console->appendSuccess("RP2Ease started.\n");
    }
}
