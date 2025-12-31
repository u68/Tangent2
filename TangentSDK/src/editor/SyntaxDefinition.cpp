#include "SyntaxDefinition.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextStream>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDebug>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>

SyntaxDefinition& SyntaxDefinition::instance() {
    static SyntaxDefinition inst;
    return inst;
}

SyntaxDefinition::SyntaxDefinition() {
    load();
}

QString SyntaxDefinition::getDefaultFilePath() const {
    // Default file is shipped with the application in resources
    QString appDir = QCoreApplication::applicationDirPath();
    return appDir + "/syntax/default.json";
}

QString SyntaxDefinition::getEditedFilePath() const {
    // User's customizations file in app data directory
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath + "/syntax");
    return dataPath + "/syntax/edited.json";
}

QString SyntaxDefinition::getExtensionsFolder() const {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString extPath = dataPath + "/syntax/extensions";
    QDir().mkpath(extPath);
    return extPath;
}

void SyntaxDefinition::loadExtensionList() {
    m_extensions.clear();
    
    QSettings settings("TangentSDK", "TangentSDK");
    
    // Load saved priorities for default and edited
    int defaultPriority = settings.value("syntax/defaultPriority", 0).toInt();
    int editedPriority = settings.value("syntax/editedPriority", 1).toInt();
    bool defaultEnabled = settings.value("syntax/defaultEnabled", true).toBool();
    bool editedEnabled = settings.value("syntax/editedEnabled", true).toBool();
    
    // Always add default extension
    SyntaxExtension defaultExt;
    defaultExt.id = "default";
    defaultExt.name = "Default";
    defaultExt.filePath = getDefaultFilePath();
    defaultExt.enabled = defaultEnabled;
    defaultExt.isDefault = true;
    defaultExt.isEdited = false;
    defaultExt.priority = defaultPriority;
    m_extensions.append(defaultExt);
    
    // Always add edited extension (user's settings)
    SyntaxExtension editedExt;
    editedExt.id = "edited";
    editedExt.name = "Editor Settings";
    editedExt.filePath = getEditedFilePath();
    editedExt.enabled = editedEnabled;
    editedExt.isDefault = false;
    editedExt.isEdited = true;
    editedExt.priority = editedPriority;
    m_extensions.append(editedExt);
    
    // Ensure edited.json file exists so users can edit it directly
    if (!QFile::exists(editedExt.filePath)) {
        QDir().mkpath(QFileInfo(editedExt.filePath).absolutePath());
        QFile file(editedExt.filePath);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonObject root;
            root["name"] = "Editor Settings";
            root["languages"] = QJsonObject();
            root["common"] = QJsonObject();
            root["console"] = QJsonObject();
            QJsonDocument doc(root);
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
        }
    }
    
    // Load custom extensions from settings
    int extCount = settings.beginReadArray("syntax/extensions");
    for (int i = 0; i < extCount; ++i) {
        settings.setArrayIndex(i);
        SyntaxExtension ext;
        ext.id = settings.value("id").toString();
        ext.name = settings.value("name").toString();
        ext.filePath = settings.value("filePath").toString();
        ext.enabled = settings.value("enabled", true).toBool();
        ext.isDefault = false;
        ext.isEdited = false;
        ext.priority = settings.value("priority", i + 2).toInt();
        
        // Skip if it's somehow a duplicate of default or edited
        if (ext.id != "default" && ext.id != "edited" && !ext.filePath.isEmpty()) {
            m_extensions.append(ext);
        }
    }
    settings.endArray();
    
    // Sort by priority
    std::sort(m_extensions.begin(), m_extensions.end(), 
              [](const SyntaxExtension& a, const SyntaxExtension& b) {
                  return a.priority < b.priority;
              });
}

void SyntaxDefinition::saveExtensionList() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    // Save default and edited priorities/enabled state
    for (const auto& ext : m_extensions) {
        if (ext.isDefault) {
            settings.setValue("syntax/defaultPriority", ext.priority);
            settings.setValue("syntax/defaultEnabled", ext.enabled);
        } else if (ext.isEdited) {
            settings.setValue("syntax/editedPriority", ext.priority);
            settings.setValue("syntax/editedEnabled", ext.enabled);
        }
    }
    
    // Save custom extensions (not default or edited)
    QList<SyntaxExtension> customExts;
    for (const auto& ext : m_extensions) {
        if (!ext.isDefault && !ext.isEdited) {
            customExts.append(ext);
        }
    }
    
    settings.beginWriteArray("syntax/extensions", customExts.size());
    for (int i = 0; i < customExts.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("id", customExts[i].id);
        settings.setValue("name", customExts[i].name);
        settings.setValue("filePath", customExts[i].filePath);
        settings.setValue("enabled", customExts[i].enabled);
        settings.setValue("priority", customExts[i].priority);
    }
    settings.endArray();
}

void SyntaxDefinition::load() {
    m_languages.clear();
    m_extensionMap.clear();
    m_commonRules.clear();
    m_consoleRules.clear();
    m_customizations = QJsonObject();
    
    loadExtensionList();
    loadAllExtensions();
}

void SyntaxDefinition::loadAllExtensions() {
    // Load extensions in priority order (lower priority first, so higher priority overrides)
    for (const auto& ext : m_extensions) {
        if (!ext.enabled) continue;
        
        QFile file(ext.filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();
            if (!doc.isNull() && doc.isObject()) {
                bool isOverride = (ext.priority > 0);  // Anything after default is an override
                loadFromJson(doc.object(), isOverride);
                
                // Keep track of edited customizations separately
                if (ext.isEdited) {
                    m_customizations = doc.object();
                }
            }
        }
    }
}

void SyntaxDefinition::loadFromJson(const QJsonObject& root, bool isOverride) {
    // Load languages
    QJsonObject languages = root["languages"].toObject();
    for (auto it = languages.begin(); it != languages.end(); ++it) {
        QString langKey = it.key();
        QJsonObject langObj = it.value().toObject();
        
        LanguageDefinition& lang = m_languages[langKey];
        
        if (!isOverride || !m_languages.contains(langKey)) {
            // Set basic info from first definition
            if (langObj.contains("name")) {
                lang.name = langObj["name"].toString();
            }
            
            QJsonArray extArray = langObj["extensions"].toArray();
            for (const auto& ext : extArray) {
                QString extension = ext.toString();
                if (!lang.extensions.contains(extension)) {
                    lang.extensions.append(extension);
                    m_extensionMap[extension] = langKey;
                }
            }
            
            // Load completions (merge with existing)
            QJsonObject completions = langObj["completions"].toObject();
            for (auto cit = completions.begin(); cit != completions.end(); ++cit) {
                QStringList items;
                QJsonArray arr = cit.value().toArray();
                for (const auto& item : arr) {
                    items.append(item.toString());
                }
                // Merge with existing completions
                QStringList& existing = lang.completions[cit.key()];
                for (const QString& item : items) {
                    if (!existing.contains(item)) {
                        existing.append(item);
                    }
                }
            }
        }
        
        // Load rules (can be overridden)
        QJsonObject rules = langObj["rules"].toObject();
        for (auto rit = rules.begin(); rit != rules.end(); ++rit) {
            QString ruleKey = rit.key();
            QJsonObject ruleObj = rit.value().toObject();
            
            if (isOverride && lang.rules.contains(ruleKey)) {
                // Merge override into existing rule
                SyntaxRule& existing = lang.rules[ruleKey];
                if (ruleObj.contains("color")) {
                    existing.color = QColor(ruleObj["color"].toString());
                }
                if (ruleObj.contains("bold")) {
                    existing.bold = ruleObj["bold"].toBool();
                }
                if (ruleObj.contains("pattern")) {
                    existing.pattern = ruleObj["pattern"].toString();
                }
                if (ruleObj.contains("patterns")) {
                    existing.patterns.clear();
                    QJsonArray arr = ruleObj["patterns"].toArray();
                    for (const auto& p : arr) {
                        existing.patterns.append(p.toString());
                    }
                }
                if (ruleObj.contains("keywords")) {
                    existing.keywords.clear();
                    QJsonArray arr = ruleObj["keywords"].toArray();
                    for (const auto& k : arr) {
                        existing.keywords.append(k.toString());
                    }
                }
            } else {
                lang.rules[ruleKey] = parseRule(ruleObj);
            }
        }
    }
    
    // Load common rules
    QJsonObject common = root["common"].toObject();
    QJsonObject commonRules = common["rules"].toObject();
    for (auto it = commonRules.begin(); it != commonRules.end(); ++it) {
        QString ruleKey = it.key();
        QJsonObject ruleObj = it.value().toObject();
        
        if (isOverride && m_commonRules.contains(ruleKey)) {
            SyntaxRule& existing = m_commonRules[ruleKey];
            if (ruleObj.contains("color")) {
                existing.color = QColor(ruleObj["color"].toString());
            }
            if (ruleObj.contains("bold")) {
                existing.bold = ruleObj["bold"].toBool();
            }
        } else if (!isOverride || !m_commonRules.contains(ruleKey)) {
            m_commonRules[ruleKey] = parseRule(ruleObj);
        }
    }
    
    // Load console rules
    QJsonObject console = root["console"].toObject();
    for (auto it = console.begin(); it != console.end(); ++it) {
        QString ruleKey = it.key();
        QJsonObject ruleObj = it.value().toObject();
        
        if (isOverride && m_consoleRules.contains(ruleKey)) {
            SyntaxRule& existing = m_consoleRules[ruleKey];
            if (ruleObj.contains("color")) {
                existing.color = QColor(ruleObj["color"].toString());
            }
            if (ruleObj.contains("bold")) {
                existing.bold = ruleObj["bold"].toBool();
            }
        } else if (!isOverride || !m_consoleRules.contains(ruleKey)) {
            m_consoleRules[ruleKey] = parseRule(ruleObj);
        }
    }
}

SyntaxRule SyntaxDefinition::parseRule(const QJsonObject& ruleObj) const {
    SyntaxRule rule;
    rule.displayName = ruleObj["displayName"].toString();
    rule.color = QColor(ruleObj["color"].toString());
    rule.bold = ruleObj["bold"].toBool(false);
    rule.pattern = ruleObj["pattern"].toString();
    rule.dynamic = ruleObj["dynamic"].toBool(false);
    
    // Parse patterns array
    if (ruleObj.contains("patterns")) {
        QJsonArray patternsArr = ruleObj["patterns"].toArray();
        for (const auto& p : patternsArr) {
            rule.patterns.append(p.toString());
        }
    }
    
    // Parse keywords array
    if (ruleObj.contains("keywords")) {
        QJsonArray keywordsArr = ruleObj["keywords"].toArray();
        for (const auto& k : keywordsArr) {
            rule.keywords.append(k.toString());
        }
    }
    
    return rule;
}

QJsonObject SyntaxDefinition::ruleToJson(const SyntaxRule& rule) const {
    QJsonObject obj;
    obj["color"] = rule.color.name();
    obj["bold"] = rule.bold;
    return obj;
}

void SyntaxDefinition::save() {
    // Save edited.json - always create the file so users can edit it directly
    QString editedPath = getEditedFilePath();
    QDir().mkpath(QFileInfo(editedPath).absolutePath());
    
    QFile file(editedPath);
    if (file.open(QIODevice::WriteOnly)) {
        // If no customizations, create an empty but valid structure
        QJsonObject toSave = m_customizations;
        if (toSave.isEmpty()) {
            toSave["name"] = "Editor Settings";
            toSave["languages"] = QJsonObject();
            toSave["common"] = QJsonObject();
            toSave["console"] = QJsonObject();
        }
        QJsonDocument doc(toSave);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
    
    // Save extension list
    saveExtensionList();
}

// Extension management
QList<SyntaxExtension> SyntaxDefinition::getExtensions() const {
    return m_extensions;
}

void SyntaxDefinition::setExtensions(const QList<SyntaxExtension>& extensions) {
    m_extensions = extensions;
    
    // Recalculate priorities based on order
    for (int i = 0; i < m_extensions.size(); ++i) {
        m_extensions[i].priority = i;
    }
    
    saveExtensionList();
    load();  // Reload with new order
}

void SyntaxDefinition::addExtension(const QString& filePath) {
    QFileInfo info(filePath);
    if (!info.exists() || info.suffix().toLower() != "json") return;
    
    // Copy to extensions folder
    QString destPath = getExtensionsFolder() + "/" + info.fileName();
    
    // Handle name collision
    int counter = 1;
    while (QFile::exists(destPath)) {
        destPath = getExtensionsFolder() + "/" + info.baseName() + "_" + QString::number(counter++) + ".json";
    }
    
    QFile::copy(filePath, destPath);
    
    // Create extension entry
    SyntaxExtension ext;
    ext.id = QFileInfo(destPath).baseName();
    ext.name = ext.id;  // Use filename as name, can be changed later
    ext.filePath = destPath;
    ext.enabled = true;
    ext.isDefault = false;
    ext.isEdited = false;
    ext.priority = m_extensions.size();
    
    // Try to get name from JSON
    QFile file(destPath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (!doc.isNull() && doc.isObject()) {
            QString name = doc.object()["name"].toString();
            if (!name.isEmpty()) {
                ext.name = name;
            }
        }
    }
    
    m_extensions.append(ext);
    saveExtensionList();
    load();
}

void SyntaxDefinition::removeExtension(const QString& id) {
    for (int i = 0; i < m_extensions.size(); ++i) {
        if (m_extensions[i].id == id) {
            // Don't allow removing default or edited
            if (m_extensions[i].isDefault || m_extensions[i].isEdited) return;
            
            // Delete the file
            QFile::remove(m_extensions[i].filePath);
            m_extensions.removeAt(i);
            
            // Recalculate priorities
            for (int j = 0; j < m_extensions.size(); ++j) {
                m_extensions[j].priority = j;
            }
            
            saveExtensionList();
            load();
            return;
        }
    }
}

void SyntaxDefinition::setExtensionEnabled(const QString& id, bool enabled) {
    for (auto& ext : m_extensions) {
        if (ext.id == id) {
            ext.enabled = enabled;
            saveExtensionList();
            load();
            return;
        }
    }
}

void SyntaxDefinition::setExtensionPriority(const QString& id, int priority) {
    for (auto& ext : m_extensions) {
        if (ext.id == id) {
            ext.priority = priority;
            break;
        }
    }
    
    // Re-sort by priority
    std::sort(m_extensions.begin(), m_extensions.end(),
              [](const SyntaxExtension& a, const SyntaxExtension& b) {
                  return a.priority < b.priority;
              });
    
    // Normalize priorities
    for (int i = 0; i < m_extensions.size(); ++i) {
        m_extensions[i].priority = i;
    }
    
    saveExtensionList();
    load();
}

QString SyntaxDefinition::createNewExtension(const QString& name) {
    QString safeName = name;
    safeName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (safeName.isEmpty()) safeName = "extension";
    
    QString filePath = getExtensionsFolder() + "/" + safeName + ".json";
    
    // Handle name collision
    int counter = 1;
    while (QFile::exists(filePath)) {
        filePath = getExtensionsFolder() + "/" + safeName + "_" + QString::number(counter++) + ".json";
    }
    
    // Create a minimal template with empty structure
    QJsonObject root;
    root["name"] = name;
    root["languages"] = QJsonObject();
    root["common"] = QJsonObject();
    root["console"] = QJsonObject();
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
    
    // Add to extensions
    SyntaxExtension ext;
    ext.id = QFileInfo(filePath).baseName();
    ext.name = name;
    ext.filePath = filePath;
    ext.enabled = true;
    ext.isDefault = false;
    ext.isEdited = false;
    ext.priority = m_extensions.size();
    
    m_extensions.append(ext);
    saveExtensionList();
    
    return filePath;
}

QString SyntaxDefinition::getExtensionPath(const QString& id) const {
    for (const auto& ext : m_extensions) {
        if (ext.id == id) {
            return ext.filePath;
        }
    }
    return QString();
}

void SyntaxDefinition::openExtensionInEditor(const QString& filePath) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

// Getter implementations
LanguageDefinition* SyntaxDefinition::getLanguageByExtension(const QString& extension) {
    QString langKey = m_extensionMap.value(extension.toLower());
    if (langKey.isEmpty()) return nullptr;
    return &m_languages[langKey];
}

QString SyntaxDefinition::getLanguageNameByExtension(const QString& extension) const {
    QString langKey = m_extensionMap.value(extension.toLower());
    if (langKey.isEmpty()) return QString();
    return m_languages.value(langKey).name;
}

QStringList SyntaxDefinition::getLanguageNames() const {
    QStringList names;
    for (const auto& lang : m_languages) {
        names.append(lang.name);
    }
    return names;
}

SyntaxRule SyntaxDefinition::getRule(const QString& languageKey, const QString& ruleKey) const {
    if (!m_languages.contains(languageKey)) return SyntaxRule();
    return m_languages[languageKey].rules.value(ruleKey);
}

SyntaxRule SyntaxDefinition::getCommonRule(const QString& ruleKey) const {
    return m_commonRules.value(ruleKey);
}

SyntaxRule SyntaxDefinition::getConsoleRule(const QString& ruleKey) const {
    return m_consoleRules.value(ruleKey);
}

QStringList SyntaxDefinition::getRuleKeys(const QString& languageKey) const {
    if (!m_languages.contains(languageKey)) return QStringList();
    return m_languages[languageKey].rules.keys();
}

QStringList SyntaxDefinition::getCommonRuleKeys() const {
    return m_commonRules.keys();
}

QStringList SyntaxDefinition::getConsoleRuleKeys() const {
    return m_consoleRules.keys();
}

QStringList SyntaxDefinition::getCompletions(const QString& languageKey, const QString& category) const {
    if (!m_languages.contains(languageKey)) return QStringList();
    return m_languages[languageKey].completions.value(category);
}

QStringList SyntaxDefinition::getAllCompletions(const QString& languageKey) const {
    QStringList all;
    if (!m_languages.contains(languageKey)) return all;
    
    const auto& completions = m_languages[languageKey].completions;
    for (auto it = completions.begin(); it != completions.end(); ++it) {
        all.append(it.value());
    }
    return all;
}

// Rule modification (saves to edited.json)
void SyntaxDefinition::setRuleColor(const QString& languageKey, const QString& ruleKey, const QColor& color) {
    if (!m_languages.contains(languageKey)) return;
    m_languages[languageKey].rules[ruleKey].color = color;
    
    // Update customizations
    QJsonObject languages = m_customizations["languages"].toObject();
    QJsonObject langObj = languages[languageKey].toObject();
    QJsonObject rules = langObj["rules"].toObject();
    QJsonObject ruleObj = rules[ruleKey].toObject();
    ruleObj["color"] = color.name();
    rules[ruleKey] = ruleObj;
    langObj["rules"] = rules;
    languages[languageKey] = langObj;
    m_customizations["languages"] = languages;
}

void SyntaxDefinition::setRuleBold(const QString& languageKey, const QString& ruleKey, bool bold) {
    if (!m_languages.contains(languageKey)) return;
    m_languages[languageKey].rules[ruleKey].bold = bold;
    
    // Update customizations
    QJsonObject languages = m_customizations["languages"].toObject();
    QJsonObject langObj = languages[languageKey].toObject();
    QJsonObject rules = langObj["rules"].toObject();
    QJsonObject ruleObj = rules[ruleKey].toObject();
    ruleObj["bold"] = bold;
    rules[ruleKey] = ruleObj;
    langObj["rules"] = rules;
    languages[languageKey] = langObj;
    m_customizations["languages"] = languages;
}

void SyntaxDefinition::setCommonRuleColor(const QString& ruleKey, const QColor& color) {
    m_commonRules[ruleKey].color = color;
    
    QJsonObject common = m_customizations["common"].toObject();
    QJsonObject rules = common["rules"].toObject();
    QJsonObject ruleObj = rules[ruleKey].toObject();
    ruleObj["color"] = color.name();
    rules[ruleKey] = ruleObj;
    common["rules"] = rules;
    m_customizations["common"] = common;
}

void SyntaxDefinition::setCommonRuleBold(const QString& ruleKey, bool bold) {
    m_commonRules[ruleKey].bold = bold;
    
    QJsonObject common = m_customizations["common"].toObject();
    QJsonObject rules = common["rules"].toObject();
    QJsonObject ruleObj = rules[ruleKey].toObject();
    ruleObj["bold"] = bold;
    rules[ruleKey] = ruleObj;
    common["rules"] = rules;
    m_customizations["common"] = common;
}

void SyntaxDefinition::setConsoleRuleColor(const QString& ruleKey, const QColor& color) {
    m_consoleRules[ruleKey].color = color;
    
    QJsonObject console = m_customizations["console"].toObject();
    QJsonObject ruleObj = console[ruleKey].toObject();
    ruleObj["color"] = color.name();
    console[ruleKey] = ruleObj;
    m_customizations["console"] = console;
}

void SyntaxDefinition::setConsoleRuleBold(const QString& ruleKey, bool bold) {
    m_consoleRules[ruleKey].bold = bold;
    
    QJsonObject console = m_customizations["console"].toObject();
    QJsonObject ruleObj = console[ruleKey].toObject();
    ruleObj["bold"] = bold;
    console[ruleKey] = ruleObj;
    m_customizations["console"] = console;
}

void SyntaxDefinition::resetRule(const QString& languageKey, const QString& ruleKey) {
    QJsonObject languages = m_customizations["languages"].toObject();
    QJsonObject langObj = languages[languageKey].toObject();
    QJsonObject rules = langObj["rules"].toObject();
    rules.remove(ruleKey);
    
    if (rules.isEmpty()) {
        langObj.remove("rules");
    } else {
        langObj["rules"] = rules;
    }
    
    if (langObj.isEmpty()) {
        languages.remove(languageKey);
    } else {
        languages[languageKey] = langObj;
    }
    
    if (languages.isEmpty()) {
        m_customizations.remove("languages");
    } else {
        m_customizations["languages"] = languages;
    }
    
    // Reload to get defaults
    load();
}

void SyntaxDefinition::resetCommonRule(const QString& ruleKey) {
    QJsonObject common = m_customizations["common"].toObject();
    QJsonObject rules = common["rules"].toObject();
    rules.remove(ruleKey);
    
    if (rules.isEmpty()) {
        common.remove("rules");
    } else {
        common["rules"] = rules;
    }
    
    if (common.isEmpty()) {
        m_customizations.remove("common");
    } else {
        m_customizations["common"] = common;
    }
    
    load();
}

void SyntaxDefinition::resetConsoleRule(const QString& ruleKey) {
    QJsonObject console = m_customizations["console"].toObject();
    console.remove(ruleKey);
    
    if (console.isEmpty()) {
        m_customizations.remove("console");
    } else {
        m_customizations["console"] = console;
    }
    
    load();
}

void SyntaxDefinition::resetAllToDefaults() {
    m_customizations = QJsonObject();
    QFile::remove(getEditedFilePath());
    load();
}

QString SyntaxDefinition::buildKeywordPattern(const QStringList& keywords) {
    if (keywords.isEmpty()) return QString();
    
    QStringList escaped;
    for (const QString& kw : keywords) {
        escaped.append(QRegularExpression::escape(kw));
    }
    return "\\b(" + escaped.join("|") + ")\\b";
}

QMap<QString, QString> SyntaxDefinition::getProjectLabelsWithFiles(const QString& projectPath) {
    QMap<QString, QString> labelMap;
    
    if (projectPath.isEmpty()) return labelMap;
    
    QDirIterator it(projectPath, QStringList() << "*.tasm", 
                    QDir::Files, QDirIterator::Subdirectories);
    QRegularExpression labelExpr("^\\s*([a-zA-Z_][a-zA-Z0-9_]*):");
    
    while (it.hasNext()) {
        QString filePath = it.next();
        // Skip files in build folder
        if (filePath.contains("/build/") || filePath.contains("\\build\\")) continue;
        
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QRegularExpressionMatch match = labelExpr.match(line);
                if (match.hasMatch()) {
                    QString label = match.captured(1);
                    // If label already exists from another file, append the file name
                    if (labelMap.contains(label)) {
                        QString existing = labelMap[label];
                        if (!existing.contains(fileName)) {
                            labelMap[label] = existing + ", " + fileName;
                        }
                    } else {
                        labelMap[label] = fileName;
                    }
                }
            }
            file.close();
        }
    }
    
    return labelMap;
}
