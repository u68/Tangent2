#include "SyntaxTheme.h"
#include <QFontDatabase>

// Static member initialization
QMap<QString, QColor> SyntaxTheme::s_defaultColors = {
    // TASM syntax colors
    {"syntax.tasm.keyword", QColor("#C1443F")},
    {"syntax.tasm.register", QColor("#9CDCFE")},
    {"syntax.tasm.label", QColor("#FFD700")},
    {"syntax.tasm.comment", QColor("#6A9955")},
    {"syntax.tasm.address", QColor("#27b397")},
    {"syntax.tasm.directive", QColor("#C586C0")},
    
    // TML syntax colors
    {"syntax.tml.keyword", QColor("#4EC9B0")},
    {"syntax.tml.field", QColor("#9CDCFE")},
    {"syntax.tml.labelRef", QColor("#FFD700")},
    {"syntax.tml.bracket0", QColor("#FFD700")},
    {"syntax.tml.bracket1", QColor("#DA70D6")},
    {"syntax.tml.bracket2", QColor("#179FFF")},
    
    // Common syntax colors
    {"syntax.number", QColor("#B5CEA8")},
    {"syntax.string", QColor("#CE9178")},
    
    // Console colors
    {"console.error", QColor("#ff6b6b")},
    {"console.warning", QColor("#ffd93d")},
    {"console.success", QColor("#6bcf6b")},
    {"console.normal", QColor("#cccccc")}
};

QMap<QString, bool> SyntaxTheme::s_defaultBold = {
    // TASM - keywords and directives are bold by default
    {"syntax.tasm.keyword", true},
    {"syntax.tasm.register", false},
    {"syntax.tasm.label", false},
    {"syntax.tasm.comment", false},
    {"syntax.tasm.address", false},
    {"syntax.tasm.directive", true},
    
    // TML - keywords are bold by default
    {"syntax.tml.keyword", true},
    {"syntax.tml.field", false},
    {"syntax.tml.labelRef", false},
    {"syntax.tml.bracket0", false},
    {"syntax.tml.bracket1", false},
    {"syntax.tml.bracket2", false},
    
    // Common
    {"syntax.number", false},
    {"syntax.string", false},
    
    // Console
    {"console.error", true},
    {"console.warning", false},
    {"console.success", false},
    {"console.normal", false}
};

QMap<QString, QString> SyntaxTheme::s_colorKeys = {
    // TASM
    {"syntax.tasm.keyword", "Keywords"},
    {"syntax.tasm.register", "Registers"},
    {"syntax.tasm.label", "Labels"},
    {"syntax.tasm.comment", "Comments"},
    {"syntax.tasm.address", "Addresses"},
    {"syntax.tasm.directive", "Directives (@)"},
    
    // TML
    {"syntax.tml.keyword", "Keywords"},
    {"syntax.tml.field", "Fields"},
    {"syntax.tml.labelRef", "Label References"},
    {"syntax.tml.bracket0", "Brackets (Level 1)"},
    {"syntax.tml.bracket1", "Brackets (Level 2)"},
    {"syntax.tml.bracket2", "Brackets (Level 3)"},
    
    // Common
    {"syntax.number", "Numbers"},
    {"syntax.string", "Strings"},
    
    // Console
    {"console.error", "Error Text"},
    {"console.warning", "Warning Text"},
    {"console.success", "Success Text"},
    {"console.normal", "Normal Text"}
};

SyntaxTheme& SyntaxTheme::instance() {
    static SyntaxTheme instance;
    return instance;
}

SyntaxTheme::SyntaxTheme() {
    // Set default font
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_fontFamily = monoFont.family();
    m_fontSize = 11;
    
    // Initialize with defaults
    resetToDefaults();
    
    // Then load saved settings (will override defaults)
    load();
}

SyntaxTheme::ColorEntry SyntaxTheme::getColor(const QString& key) const {
    return m_colors.value(key, ColorEntry(QColor("#FFFFFF"), false));
}

void SyntaxTheme::setColor(const QString& key, const ColorEntry& entry) {
    m_colors[key] = entry;
}

void SyntaxTheme::setColor(const QString& key, const QColor& color, bool bold) {
    m_colors[key] = ColorEntry(color, bold);
}

QString SyntaxTheme::fontFamily() const {
    return m_fontFamily;
}

void SyntaxTheme::setFontFamily(const QString& family) {
    m_fontFamily = family;
}

int SyntaxTheme::fontSize() const {
    return m_fontSize;
}

void SyntaxTheme::setFontSize(int size) {
    m_fontSize = size;
}

QMap<QString, QString> SyntaxTheme::colorKeys() {
    return s_colorKeys;
}

QMap<QString, QColor> SyntaxTheme::defaultColors() {
    return s_defaultColors;
}

QMap<QString, bool> SyntaxTheme::defaultBold() {
    return s_defaultBold;
}

void SyntaxTheme::load() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    // Load font settings
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_fontFamily = settings.value("editor/fontFamily", monoFont.family()).toString();
    m_fontSize = settings.value("editor/fontSize", 11).toInt();
    
    // Load colors
    settings.beginGroup("colors");
    for (auto it = s_defaultColors.begin(); it != s_defaultColors.end(); ++it) {
        QString colorStr = settings.value(it.key(), it.value().name()).toString();
        bool bold = settings.value(it.key() + ".bold", s_defaultBold.value(it.key(), false)).toBool();
        m_colors[it.key()] = ColorEntry(QColor(colorStr), bold);
    }
    settings.endGroup();
}

void SyntaxTheme::save() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    // Save font settings
    settings.setValue("editor/fontFamily", m_fontFamily);
    settings.setValue("editor/fontSize", m_fontSize);
    
    // Save colors
    settings.beginGroup("colors");
    for (auto it = m_colors.begin(); it != m_colors.end(); ++it) {
        settings.setValue(it.key(), it.value().color.name());
        settings.setValue(it.key() + ".bold", it.value().bold);
    }
    settings.endGroup();
}

void SyntaxTheme::resetToDefaults() {
    m_colors.clear();
    for (auto it = s_defaultColors.begin(); it != s_defaultColors.end(); ++it) {
        m_colors[it.key()] = ColorEntry(it.value(), s_defaultBold.value(it.key(), false));
    }
    
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_fontFamily = monoFont.family();
    m_fontSize = 11;
}

QMap<QString, SyntaxTheme::ColorEntry> SyntaxTheme::allColors() const {
    return m_colors;
}

void SyntaxTheme::setAllColors(const QMap<QString, ColorEntry>& colors) {
    m_colors = colors;
}
