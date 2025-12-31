#pragma once
#include <QColor>
#include <QString>
#include <QMap>
#include <QSettings>
#include <QFont>

// Central place for all syntax highlighting colors and settings
class SyntaxTheme {
public:
    struct ColorEntry {
        QColor color;
        bool bold = false;
        
        ColorEntry() = default;
        ColorEntry(const QColor& c, bool b = false) : color(c), bold(b) {}
    };
    
    static SyntaxTheme& instance();
    
    // Color access
    ColorEntry getColor(const QString& key) const;
    void setColor(const QString& key, const ColorEntry& entry);
    void setColor(const QString& key, const QColor& color, bool bold);
    
    // Font settings
    QString fontFamily() const;
    void setFontFamily(const QString& family);
    
    int fontSize() const;
    void setFontSize(int size);
    
    // All color keys and their display names
    static QMap<QString, QString> colorKeys();
    static QMap<QString, QColor> defaultColors();
    static QMap<QString, bool> defaultBold();
    
    // Load/save from QSettings
    void load();
    void save();
    
    // Reset to defaults
    void resetToDefaults();
    
    // Get all current colors
    QMap<QString, ColorEntry> allColors() const;
    void setAllColors(const QMap<QString, ColorEntry>& colors);

private:
    SyntaxTheme();
    ~SyntaxTheme() = default;
    
    QMap<QString, ColorEntry> m_colors;
    QString m_fontFamily;
    int m_fontSize = 11;
    
    static QMap<QString, QColor> s_defaultColors;
    static QMap<QString, bool> s_defaultBold;
    static QMap<QString, QString> s_colorKeys;
};
