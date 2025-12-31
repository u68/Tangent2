#include "DiscordRPCManager.h"
#include "discord_rpc.h"
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <cstring>

// Discord Application ID
const QString DiscordRPCManager::APPLICATION_ID = "1451518123525013674";

// Static callback handlers
static void handleDiscordReady(const DiscordUser* user) {
    qDebug() << "Discord RPC Ready - User:" << user->username;
}

static void handleDiscordDisconnected(int errorCode, const char* message) {
    qDebug() << "Discord RPC Disconnected - Error:" << errorCode << message;
}

static void handleDiscordError(int errorCode, const char* message) {
    qDebug() << "Discord RPC Error:" << errorCode << message;
}

DiscordRPCManager::DiscordRPCManager(QObject* parent)
    : QObject(parent)
    , m_startTime(QDateTime::currentDateTime())
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(15000); // Update every 15 seconds
    connect(m_updateTimer, &QTimer::timeout, this, &DiscordRPCManager::onUpdateTimer);
    
    // Load settings
    QSettings settings("TangentSDK", "TangentSDK");
    m_enabled = settings.value("discord/enabled", false).toBool();
    m_customTextEnabled = settings.value("discord/customText", false).toBool();
    m_detailsFormat = settings.value("discord/details", "Editing ${file}").toString();
    m_stateFormat = settings.value("discord/state", "Working on ${project}").toString();
    m_elapsedResetMode = settings.value("discord/elapsedReset", 0).toInt();
}

DiscordRPCManager::~DiscordRPCManager() {
    shutdown();
}

bool DiscordRPCManager::initialize() {
    if (!m_enabled) {
        return false;
    }
    
    // Initialize Discord RPC
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.errored = handleDiscordError;
    
    Discord_Initialize(APPLICATION_ID.toUtf8().constData(), &handlers, 1, nullptr);
    
    m_connected = true;
    m_startTime = QDateTime::currentDateTime();
    m_updateTimer->start();
    
    emit connectionStatusChanged(true);
    qDebug() << "Discord RPC initialized";
    
    return true;
}

void DiscordRPCManager::shutdown() {
    if (m_connected) {
        m_updateTimer->stop();
        
        Discord_ClearPresence();
        Discord_Shutdown();
        
        m_connected = false;
        emit connectionStatusChanged(false);
        qDebug() << "Discord RPC shutdown";
    }
}

void DiscordRPCManager::setEnabled(bool enabled) {
    if (m_enabled == enabled) return;
    
    m_enabled = enabled;
    
    if (enabled) {
        initialize();
    } else {
        shutdown();
    }
}

void DiscordRPCManager::updatePresence(const QString& fileName, const QString& projectName) {
    if (!m_enabled || !m_connected) return;
    
    m_currentFile = fileName;
    m_currentProject = projectName;
    
    QString details;
    QString state;
    
    if (m_customTextEnabled) {
        details = formatString(m_detailsFormat, fileName, projectName);
        state = formatString(m_stateFormat, fileName, projectName);
    } else {
        // Default format
        if (!fileName.isEmpty()) {
            details = "Editing " + fileName;
        } else {
            details = "Idle";
        }
        
        if (!projectName.isEmpty()) {
            state = "Working on " + projectName;
        } else {
            state = "TangentSDK IDE";
        }
    }
    
    // Update Discord presence
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));
    
    // Store strings to keep them alive during the call
    QByteArray detailsBytes = details.toUtf8();
    QByteArray stateBytes = state.toUtf8();
    
    presence.details = detailsBytes.constData();
    presence.state = stateBytes.constData();
    presence.startTimestamp = m_startTime.toSecsSinceEpoch();
    
    // Note: Image keys must be uploaded to Discord Developer Portal first
    // Go to: https://discord.com/developers/applications/YOUR_APP_ID/rich-presence/assets
    // Upload images with these exact names, or comment these lines out
    // presence.largeImageKey = "tangentsdk_logo";
    // presence.largeImageText = "TangentSDK";
    
    Discord_UpdatePresence(&presence);
    
    qDebug() << "Discord presence updated - Details:" << details << "State:" << state;
}

void DiscordRPCManager::clearPresence() {
    if (!m_enabled || !m_connected) return;
    
    Discord_ClearPresence();
    
    m_currentFile.clear();
    m_currentProject.clear();
    
    qDebug() << "Discord presence cleared";
}

QString DiscordRPCManager::formatString(const QString& format, const QString& fileName, const QString& projectName) {
    QString result = format;
    
    QFileInfo fileInfo(fileName);
    
    result.replace("${file}", fileName);
    result.replace("${file.baseName}", fileInfo.completeBaseName());
    result.replace("${file.extension}", fileInfo.suffix());
    result.replace("${language}", getLanguageFromFile(fileName));
    result.replace("${project}", projectName.isEmpty() ? "Unknown Project" : projectName);
    
    return result;
}

QString DiscordRPCManager::getLanguageFromFile(const QString& fileName) {
    QFileInfo fileInfo(fileName);
    QString ext = fileInfo.suffix().toLower();
    
    if (ext == "tasm") {
        return "Tangent Assembly";
    } else if (ext == "tml") {
        return "Tangent Markup";
    }
    
    return "Unknown";
}

void DiscordRPCManager::resetElapsedTime() {
    m_startTime = QDateTime::currentDateTime();
    
    // Update presence with new timestamp
    if (!m_currentFile.isEmpty() || !m_currentProject.isEmpty()) {
        updatePresence(m_currentFile, m_currentProject);
    }
}

void DiscordRPCManager::onProjectChanged(const QString& projectName) {
    m_currentProject = projectName;
    
    if (m_elapsedResetMode == 1) { // Reset on new project
        resetElapsedTime();
    }
    
    updatePresence(m_currentFile, projectName);
}

void DiscordRPCManager::onFileChanged(const QString& fileName) {
    m_currentFile = fileName;
    
    if (m_elapsedResetMode == 2) { // Reset on new file
        resetElapsedTime();
    }
    
    updatePresence(fileName, m_currentProject);
}

void DiscordRPCManager::onUpdateTimer() {
    // Process Discord callbacks
    Discord_RunCallbacks();
    
    // Re-update presence periodically
    if (!m_currentFile.isEmpty() || !m_currentProject.isEmpty()) {
        updatePresence(m_currentFile, m_currentProject);
    }
}
