#pragma once
#include <QObject>
#include <QString>
#include <QTimer>
#include <QDateTime>

class DiscordRPCManager : public QObject {
    Q_OBJECT
public:
    explicit DiscordRPCManager(QObject* parent = nullptr);
    ~DiscordRPCManager();
    
    // Initialize Discord RPC
    bool initialize();
    void shutdown();
    
    // Check if connected
    bool isConnected() const { return m_connected; }
    
    // Update presence
    void updatePresence(const QString& fileName, const QString& projectName);
    void clearPresence();
    
    // Settings
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
    void setCustomTextEnabled(bool enabled) { m_customTextEnabled = enabled; }
    void setDetailsFormat(const QString& format) { m_detailsFormat = format; }
    void setStateFormat(const QString& format) { m_stateFormat = format; }
    
    // Elapsed time reset modes: 0=startup, 1=project, 2=file
    void setElapsedResetMode(int mode) { m_elapsedResetMode = mode; }
    void resetElapsedTime();
    
    // Update for project/file changes
    void onProjectChanged(const QString& projectName);
    void onFileChanged(const QString& fileName);
    
signals:
    void connectionStatusChanged(bool connected);
    void error(const QString& message);

private slots:
    void onUpdateTimer();

private:
    QString formatString(const QString& format, const QString& fileName, const QString& projectName);
    QString getLanguageFromFile(const QString& fileName);
    
    bool m_enabled = false;
    bool m_connected = false;
    bool m_customTextEnabled = false;
    
    QString m_detailsFormat = "Editing ${file}";
    QString m_stateFormat = "Working on ${project}";
    int m_elapsedResetMode = 0;
    
    QString m_currentFile;
    QString m_currentProject;
    
    QDateTime m_startTime;
    QTimer* m_updateTimer;
    
    // Discord Application ID (you would register your app at discord.com/developers)
    static const QString APPLICATION_ID;
};
