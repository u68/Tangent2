#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QColor>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    enum FileType { Unknown, TASM, TML };
    
    SyntaxHighlighter(QTextDocument* doc);
    
    void setFileType(const QString& filePath);
    void setFileTypeFromExtension(const QString& extension);
    void setProjectPath(const QString& projectPath);
    void refreshProjectLabels();
    
    // Load colors from settings and re-apply
    void reloadColorsFromSettings();

protected:
    void highlightBlock(const QString& text) override;

private:
    FileType m_fileType = Unknown;
    QString m_fileExtension;
    QString m_projectPath;
    QStringList m_projectLabels;
    
    // Cached formats for current language
    QMap<QString, QTextCharFormat> m_formats;
    
    // Cached compiled patterns
    struct HighlightRule {
        QRegularExpression pattern;
        QString formatKey;
    };
    QList<HighlightRule> m_rules;
    
    // Bracket formats for TML
    QTextCharFormat m_bracketFormats[3];
    
    void highlightTASM(const QString& text);
    void highlightTML(const QString& text);
    void setupFormats();
    void buildHighlightRules();
};
