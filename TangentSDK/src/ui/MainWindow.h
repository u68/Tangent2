#pragma once
#include <QMainWindow>
#include <QCloseEvent>

class TabEditorWidget;
class ConsoleWidget;
class ProjectExplorer;
class QDockWidget;
class DiscordRPCManager;
class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onFileClicked(const QString& filePath);
    void onSaveFile();
    void onSaveAllFiles();
    void onOpenWorkspace();
    void onCreateFile();
    void onCreateFolder();
    void onNewProject();
    void onBuildProject();
    void onOpenSettings();
    void onRunOnEmulator();
    void onLoadOntoPicoEase();
    void onDownloadFromPicoEase();
    void onStartRP2Ease();
    void updateWindowTitle();
    void onCurrentFileChanged(const QString& filePath);
    void onColorsChanged();
    void onDiscordSettingsChanged();

private:
    TabEditorWidget* tabEditor;
    ConsoleWidget* console;
    ProjectExplorer* projectExplorer;
    QDockWidget* projectDock;
    QDockWidget* consoleDock;
    QString m_workspacePath;
    DiscordRPCManager* m_discordRPC;

    void setupDocking();
    void setupMenus();
    QString getWorkspacePath(bool forceDialog = false);
    bool promptSaveAll();
    void saveWindowState();
    void restoreWindowState();
    void applyColors();
    QString getPasswordHexString() const;
    QString getClockspeedHexString() const;
};
