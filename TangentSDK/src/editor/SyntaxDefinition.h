#pragma once
#include <QString>
#include <QColor>
#include <QStringList>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

// Represents a single syntax rule with color and styling
struct SyntaxRule {
    QString displayName;
    QColor color;
    bool bold = false;
    QString pattern;           // Single regex pattern
    QStringList patterns;      // Multiple regex patterns (e.g., for registers)
    QStringList keywords;      // Keywords list (converted to pattern automatically)
    bool dynamic = false;      // If true, patterns are loaded from project (e.g., labels)
    
    bool isValid() const { return color.isValid(); }
};

// Represents a language definition
struct LanguageDefinition {
    QString name;
    QStringList extensions;
    QMap<QString, SyntaxRule> rules;
    QMap<QString, QStringList> completions;  // category -> list of completions
};

// Represents a syntax extension file
struct SyntaxExtension {
    QString id;           // Unique identifier (filename without .json)
    QString name;         // Display name
    QString filePath;     // Full path to the JSON file
    bool enabled = true;  // Whether the extension is active
    bool isDefault = false;  // True for default.json (cannot be deleted)
    bool isEdited = false;   // True for user's settings file (cannot be deleted)
    int priority = 0;     // Lower = loaded first, higher = takes precedence
};

class SyntaxDefinition {
public:
    static SyntaxDefinition& instance();
    
    // Load definitions from files
    void load();
    void save();
    
    // Get language by extension
    LanguageDefinition* getLanguageByExtension(const QString& extension);
    QString getLanguageNameByExtension(const QString& extension) const;
    
    // Get all language names
    QStringList getLanguageNames() const;
    
    // Get rule for a specific language and rule key
    SyntaxRule getRule(const QString& languageKey, const QString& ruleKey) const;
    
    // Get common rules (numbers, strings)
    SyntaxRule getCommonRule(const QString& ruleKey) const;
    
    // Get console colors
    SyntaxRule getConsoleRule(const QString& ruleKey) const;
    
    // Get all rule keys for a language (for settings UI)
    QStringList getRuleKeys(const QString& languageKey) const;
    QStringList getCommonRuleKeys() const;
    QStringList getConsoleRuleKeys() const;
    
    // Get completions for a language
    QStringList getCompletions(const QString& languageKey, const QString& category) const;
    QStringList getAllCompletions(const QString& languageKey) const;
    
    // Modify a rule (saves to edited.json)
    void setRuleColor(const QString& languageKey, const QString& ruleKey, const QColor& color);
    void setRuleBold(const QString& languageKey, const QString& ruleKey, bool bold);
    void setCommonRuleColor(const QString& ruleKey, const QColor& color);
    void setCommonRuleBold(const QString& ruleKey, bool bold);
    void setConsoleRuleColor(const QString& ruleKey, const QColor& color);
    void setConsoleRuleBold(const QString& ruleKey, bool bold);
    
    // Reset a specific rule to default
    void resetRule(const QString& languageKey, const QString& ruleKey);
    void resetCommonRule(const QString& ruleKey);
    void resetConsoleRule(const QString& ruleKey);
    
    // Reset all customizations
    void resetAllToDefaults();
    
    // Build regex pattern from keywords list
    static QString buildKeywordPattern(const QStringList& keywords);
    
    // Get project labels with their source file names (for completion popup)
    static QMap<QString, QString> getProjectLabelsWithFiles(const QString& projectPath);
    
    // Extension management
    QList<SyntaxExtension> getExtensions() const;
    void setExtensions(const QList<SyntaxExtension>& extensions);
    void addExtension(const QString& filePath);
    void removeExtension(const QString& id);
    void setExtensionEnabled(const QString& id, bool enabled);
    void setExtensionPriority(const QString& id, int priority);
    QString createNewExtension(const QString& name);
    QString getExtensionPath(const QString& id) const;
    QString getDefaultFilePath() const;
    QString getEditedFilePath() const;
    QString getExtensionsFolder() const;
    
    // Open extension file for editing
    static void openExtensionInEditor(const QString& filePath);
    
private:
    SyntaxDefinition();
    ~SyntaxDefinition() = default;
    SyntaxDefinition(const SyntaxDefinition&) = delete;
    SyntaxDefinition& operator=(const SyntaxDefinition&) = delete;
    
    void loadFromJson(const QJsonObject& root, bool isOverride);
    SyntaxRule parseRule(const QJsonObject& ruleObj) const;
    QJsonObject ruleToJson(const SyntaxRule& rule) const;
    
    void loadExtensionList();
    void saveExtensionList();
    void loadAllExtensions();
    
    QMap<QString, LanguageDefinition> m_languages;  // key -> language
    QMap<QString, QString> m_extensionMap;          // extension -> language key
    QMap<QString, SyntaxRule> m_commonRules;
    QMap<QString, SyntaxRule> m_consoleRules;
    
    // Extension system
    QList<SyntaxExtension> m_extensions;
    
    // Track which rules have been customized (for edited.json)
    QJsonObject m_customizations;
};
