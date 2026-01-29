#pragma once
#include <QMainWindow>
#include <QCloseEvent>
#include <QProcess>

class TabEditorWidget;
class ConsoleWidget;
class ProjectExplorer;
class QDockWidget;
class QAction;
class QMenu;
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
    void onBuildRom();
    void onOpenSettings();
    void onRunOnEmulator();
    void onLoadOntoPicoEase();
    void onDownloadFromPicoEase();
    void onStartRP2Ease();
    void updateWindowTitle();
    void onCurrentFileChanged(const QString& filePath);
    void onColorsChanged();
    void onDiscordSettingsChanged();
    void onOpenOsSourceToggled(bool checked);
    void onBuildOsSource(); 
    void onBuildOsProcessReadyStdout();
    void onBuildOsProcessReadyStderr();
    void onBuildOsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    TabEditorWidget* tabEditor;
    ConsoleWidget* console;
    ProjectExplorer* projectExplorer;
    QDockWidget* projectDock;
    QDockWidget* consoleDock;
    QString m_workspacePath;
    DiscordRPCManager* m_discordRPC;
    QAction* m_openOsSourceAction;
    QAction* m_buildOsSourceAction;
    QProcess* m_buildProcess = nullptr;
    QMenu* m_projectMenu = nullptr;
    QString m_previousWorkspacePath;

    void setupDocking();
    void setupMenus();
    QString getWorkspacePath(bool forceDialog = false);
    bool promptSaveAll();
    void saveWindowState();
    void restoreWindowState();
    void applyColors();
    QString getPasswordHexString() const;
    QString getClockspeedHexString() const;
    QString findTangent2SrcPath() const; 
};

